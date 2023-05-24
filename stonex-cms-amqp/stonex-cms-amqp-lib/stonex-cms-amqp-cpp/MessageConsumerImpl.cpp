/*
 * Copyright 2022 StoneX Financial Ltd.
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MessageConsumerImpl.h"

#include <proton/source_options.hpp>
#include <proton/target_options.hpp>
#include <proton/connection.hpp>
#include <proton/work_queue.hpp>
#include <proton/annotation_key.hpp>

#include "../API/CMSTextMessage.h"
#include "../API/CMSBytesMessage.h"
#include "../API/CMSQueue.h"
#include "../API/CMSTopic.h"
#include "../API/CMSTemporaryQueue.h"
#include "../API/CMSTemporaryTopic.h"

#include "AMQPCMSMessageConverter.h"
#include "AsyncCallSynchronizer.h"

#include <fmt/format.h>

cms::amqp::MessageConsumerImpl::MessageConsumerImpl(const ::cms::Destination* destination, std::shared_ptr<proton::session> session, const std::string& selector, std::shared_ptr<StonexLogger> logger)
	:MessageConsumerImpl(destination, {}, session,false,false,true,selector,logger)
{

}

cms::amqp::MessageConsumerImpl::MessageConsumerImpl(const ::cms::Destination* destination, const std::string& name, std::shared_ptr<proton::session> session, bool durable, bool shared, bool autoAck, const std::string& selector, std::shared_ptr<StonexLogger> logger)
	:mEXHandler(logger)
{
	setLogger(logger);
	mRopts.handler(*this);
	proton::source_options sopts{};
	std::string address;

	switch (auto destType = destination->getDestinationType())
	{
	case ::cms::Destination::DestinationType::QUEUE:
		address = dynamic_cast<const CMSQueue*>(destination)->getQueueName();
		if (destAddressParser.isShared(address)/*shared*/)
		{
			sopts.capabilities(std::vector<proton::symbol> { "queue", "shared" });
		}
		else
			sopts.capabilities(std::vector<proton::symbol> { "queue" });
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		address = dynamic_cast<const CMSTopic*>(destination)->getTopicName();
		sopts.capabilities(std::vector<proton::symbol> { "topic" });
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		address = dynamic_cast<const CMSTemporaryQueue*>(destination)->getQueueName();
		sopts.capabilities(std::vector<proton::symbol> { "temporary-queue", "delete-on-close"});
		sopts.dynamic(true); //taret or source options
		sopts.expiry_policy(proton::terminus::expiry_policy::LINK_CLOSE); //according to documentation should be link detatch!!!!
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		address = dynamic_cast<const CMSTemporaryTopic*>(destination)->getTopicName();
		sopts.capabilities(std::vector<proton::symbol> { "temporary-topic", "delete-on-close"});
		sopts.dynamic(true); //taret or source options
		sopts.expiry_policy(proton::terminus::expiry_policy::LINK_CLOSE);
		break;

	}

	


	


	if (!name.empty()) 
		mRopts.name(name);

	if (durable) 
	{
		//configure durable subscription
		sopts.durability_mode(proton::source::UNSETTLED_STATE);
		sopts.expiry_policy(proton::source::NEVER);
	}





	if (!selector.empty())
	{
		proton::source::filter_map _filters;
		proton::symbol filter_key("selector");
		proton::value filter_value;
		// The value is a specific CMS "described type": binary string with symbolic descriptor
		proton::codec::encoder enc(filter_value);
		enc << proton::codec::start::described()
			<< proton::symbol("apache.org:selector-filter:string")
			<< selector
			<< proton::codec::finish();
		// In our case, the map has this one element
		_filters.put(filter_key, filter_value);
		sopts.filters(_filters);
	}


//	sopts.address(mAddress);
	
	mRopts.auto_accept(autoAck);
	mRopts.source(sopts);


	//waiting until all relevant resources is initialized
	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncStart, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), address, mRopts, session);
	setLogger(nullptr);
}

cms::amqp::MessageConsumerImpl::~MessageConsumerImpl()
{
	if(mState == ClientState::STARTED)
		close();
}

::cms::Message* cms::amqp::MessageConsumerImpl::receive()
{
	error("message consumer implementation", fmt::format("{} {}", __func__, "internal method not implemented"));
	return nullptr;
}

::cms::Message* cms::amqp::MessageConsumerImpl::receive(int milis)
{
	error("message consumer implementation", fmt::format("{} {}", __func__, "internal method not implemented"));
	return nullptr;
}

::cms::Message* cms::amqp::MessageConsumerImpl::receiveNoWait()
{
	error("message consumer implementation", fmt::format("{} {}", __func__, "internal method not implemented"));
	return nullptr;
}

void cms::amqp::MessageConsumerImpl::setMessageListener(::cms::MessageListener* listener)
{
	mListener = listener;
	if (!mListener)
		onMessageCallback = [](::cms::Message* message) ->void {delete message; };
	else
		onMessageCallback =  std::bind(&::cms::MessageListener::onMessage,listener,std::placeholders::_1);
}

cms::MessageListener* cms::amqp::MessageConsumerImpl::getMessageListener() const
{
	return mListener;
}


std::string cms::amqp::MessageConsumerImpl::getMessageSelector() const
{
	return std::string();
}

void cms::amqp::MessageConsumerImpl::setMessageAvailableListener(::cms::MessageAvailableListener* listener)
{
}

void cms::amqp::MessageConsumerImpl::start()
{
#if _DEBUG
	trace("message consumer implementation", fmt::format("{}", __func__));
#endif
	//ToDo check if allready started!!
	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncStart, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), mAddress, mRopts, std::make_shared<proton::session>(mProtonReceiver->session()));
}

void cms::amqp::MessageConsumerImpl::stop()
{
#if _DEBUG
	trace("message consumer implementation", fmt::format("{}", __func__));
#endif
	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncStop, this));
}

void cms::amqp::MessageConsumerImpl::close()
{
#if _DEBUG
	trace("message consumer implementation", fmt::format("{}", __func__));
#endif
	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncClose, this));
}

void cms::amqp::MessageConsumerImpl::on_receiver_open(proton::receiver& receiver)
{
#if _DEBUG
	trace("message consumer implementation", fmt::format("{} {}", __func__, receiver.error().what()));
#endif
	mProtonReceiver = std::make_unique<proton::receiver>(receiver);
	if (receiver.error().empty())
	{
		mDestination.reset(AMQPCMSMessageConverter::createCMSDestination(mProtonReceiver.get()));
		mState = ClientState::STARTED;
		mEXHandler.onResourceInitialized();
	}


}

void cms::amqp::MessageConsumerImpl::on_receiver_close(proton::receiver& receiver)
{
#if _DEBUG
	trace("message consumer implementation", fmt::format("{} {}", __func__, receiver.error().what()));
#endif
	mEXHandler.onResourceUninitialized(receiver.error());

	mState = ClientState::STOPPED;
}

void cms::amqp::MessageConsumerImpl::on_receiver_detach(proton::receiver& receiver)
{
#if _DEBUG
	trace("message consumer implementation", fmt::format("{} {}", __func__, receiver.error().what()));
#endif
	mEXHandler.onResourceInitialized();
	mState = ClientState::DETATCHED;
}

void cms::amqp::MessageConsumerImpl::on_receiver_error(proton::receiver& receiver)
{
	error("message consumer implementation", fmt::format("{} {}", __func__, receiver.error().what()));
}

void cms::amqp::MessageConsumerImpl::on_message(proton::delivery& delivery, proton::message& message)
{
	if (message.content_type() == "application/octet-stream")
	{
#if _DEBUG
		trace("message consumer implementation", fmt::format("{} {}", __func__, "bytes message"));
#endif
		onMessageCallback(new CMSBytesMessage(&message, &delivery, mProtonReceiver.get()));
	}
	else
	{
#if _DEBUG
		trace("message consumer implementation", fmt::format("{} {}", __func__, "text message"));
#endif
		onMessageCallback(new CMSTextMessage(&message, &delivery, mProtonReceiver.get()));
	}
	
}

const std::string cms::amqp::MessageConsumerImpl::getAddress() const
{
	return mProtonReceiver->source().address();
}

bool cms::amqp::MessageConsumerImpl::syncClose()
{
#if _DEBUG
	trace("message consumer implementation", fmt::format(" {}", __func__));
#endif

	if (!mProtonReceiver->closed() /* && mState == STATUS::OPEN*/) {
		return mProtonReceiver->connection().work_queue().add([=] {mProtonReceiver->close(); });
	}
	else
		return false;
}

bool cms::amqp::MessageConsumerImpl::syncStart(const std::string& address, const proton::receiver_options& options, std::shared_ptr<proton::session>  session)
{
#if _DEBUG
	trace("message consumer implementation", fmt::format(" {} address {}", __func__, address));
#endif

	return session->connection().work_queue().add([=] {session->open_receiver(address, mRopts); });
}

bool cms::amqp::MessageConsumerImpl::syncStop()
{
#if _DEBUG
	trace("message consumer implementation", fmt::format("{}", __func__));
#endif

	return 	mProtonReceiver->connection().work_queue().add([=] {mProtonReceiver->detach(); });
}
