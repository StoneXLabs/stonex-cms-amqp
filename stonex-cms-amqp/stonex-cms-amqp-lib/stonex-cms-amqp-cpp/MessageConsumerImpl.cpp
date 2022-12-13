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

cms::amqp::MessageConsumerImpl::MessageConsumerImpl(const ::cms::Destination* destination, std::shared_ptr<proton::session> session, const std::string& selector)
	:MessageConsumerImpl(destination, {}, session,false,false,true,selector)
{

}

cms::amqp::MessageConsumerImpl::MessageConsumerImpl(const ::cms::Destination* destination, const std::string& name, std::shared_ptr<proton::session> session, bool durable, bool shared, bool autoAck, const std::string& selector)
{
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
}

cms::amqp::MessageConsumerImpl::~MessageConsumerImpl()
{
	if(mState == ClientState::STARTED)
		close();
}

::cms::Message* cms::amqp::MessageConsumerImpl::receive()
{
	return nullptr;
}

::cms::Message* cms::amqp::MessageConsumerImpl::receive(int milis)
{
	return nullptr;
}

::cms::Message* cms::amqp::MessageConsumerImpl::receiveNoWait()
{
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
	//ToDo check if allready started!!
	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncStart, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), mAddress, mRopts, std::make_shared<proton::session>(mProtonReceiver->session()));
}

void cms::amqp::MessageConsumerImpl::stop()
{
	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncStop, this));
}

void cms::amqp::MessageConsumerImpl::close()
{
	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncClose, this));
}

void cms::amqp::MessageConsumerImpl::on_receiver_open(proton::receiver& receiver)
{
	mProtonReceiver = std::make_unique<proton::receiver>(receiver);
	if (receiver.error().empty())
	{
		mEXHandler.onResourceInitialized();
		mDestination.reset(AMQPCMSMessageConverter::createCMSDestination(mProtonReceiver.get()));
		mState = ClientState::STARTED;
	}
//	else
//	{
//		in case of error on_receiver_close(proton::receiver& receiver) handler is called and cleanup is made by library
//	}

		

	//add comment TO DO deadlock on consumer create error
//	mDestination.reset(AMQPCMSMessageConverter::createCMSDestination(mProtonReceiver.get()));

}

void cms::amqp::MessageConsumerImpl::on_receiver_close(proton::receiver& receiver)
{
	mEXHandler.onResourceUninitialized(receiver.error());

	mState = ClientState::STOPPED;
}

void cms::amqp::MessageConsumerImpl::on_receiver_detach(proton::receiver& receiver)
{

	mEXHandler.onResourceInitialized();
	mState = ClientState::DETATCHED;
}

void cms::amqp::MessageConsumerImpl::on_receiver_error(proton::receiver& receiver)
{
	//mEXHandler.onConsumerError(receiver.error().what());
}

void cms::amqp::MessageConsumerImpl::on_message(proton::delivery& delivery, proton::message& message)
{
	if (message.content_type() == "application/octet-stream")
	{
		onMessageCallback(new CMSBytesMessage(&message, &delivery, mProtonReceiver.get()));
	}
	else
	{
		onMessageCallback(new CMSTextMessage(&message, &delivery, mProtonReceiver.get()));
	}
	
}

const std::string cms::amqp::MessageConsumerImpl::getAddress() const
{
	return mProtonReceiver->source().address();
}

bool cms::amqp::MessageConsumerImpl::syncClose()
{
	if (!mProtonReceiver->closed()/* && mState == STATUS::OPEN*/)
		return mProtonReceiver->connection().work_queue().add([=] {mProtonReceiver->close(); });
	else
		return false;
}

bool cms::amqp::MessageConsumerImpl::syncStart(const std::string& address, const proton::receiver_options& options, std::shared_ptr<proton::session>  session)
{
	return session->connection().work_queue().add([=] {session->open_receiver(address, mRopts); });
}

bool cms::amqp::MessageConsumerImpl::syncStop()
{
	return 	mProtonReceiver->connection().work_queue().add([=] {mProtonReceiver->detach(); });
}
//
//
//::cms::Destination* cms::amqp::MessageConsumerImpl::initializeDestination()
//{
//
//	::cms::Destination* dest{ nullptr };
//
//	switch (capabilityToDestinationType(mProtonReceiver->source().capabilities()))
//	{
//	case ::cms::Destination::DestinationType::QUEUE:
//		dest = new cms::CMSQueue(mProtonReceiver->source().address());
//		break;
//	case ::cms::Destination::DestinationType::TOPIC:
//		dest = new cms::CMSTopic(mProtonReceiver->source().address());
//		break;
//	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
//		dest = new cms::CMSTemporaryQueue();
//		dynamic_cast<cms::CMSTemporaryQueue*>(dest)->mQueueName = mProtonReceiver->source().address();
//		break;
//	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
//		//		dynamic_cast<cms::CMSTemporaryTopic*>(dest)->mTopicName = mProtonReceiver->target().address();
//		break;
//	default:
//		throw ::cms::InvalidDestinationException("invalid destination type");
//		break;
//	}
//	return dest;
//}
//
//::cms::Destination::DestinationType cms::amqp::MessageConsumerImpl::capabilityToDestinationType(const std::vector<proton::symbol>& capabilities) const
//{
//	auto capability = std::find_if(std::cbegin(capabilities), std::cend(capabilities), [](const proton::symbol item) {
//		if (item == QUEUE_CAPABILITY)
//			return ::cms::Destination::DestinationType::QUEUE;
//		else if (item == TOPIC_CAPABILITY)
//			return ::cms::Destination::DestinationType::TOPIC;
//		else if (item == TEMPORARY_QUEUE_CAPABILITY)
//			return ::cms::Destination::DestinationType::TEMPORARY_QUEUE;
//		else if (item == TEMPORARY_TOPIC_CAPABILITY)
//			return ::cms::Destination::DestinationType::TEMPORARY_TOPIC;
//		});
//
//	if (capability == std::cend(capabilities))
//		throw::cms::InvalidDestinationException("missing capability type.");  //to do more explicit message
//
//
//	if (auto setting = *capability; setting == QUEUE_CAPABILITY)
//		return ::cms::Destination::DestinationType::QUEUE;
//	else if (setting == TOPIC_CAPABILITY)
//		return ::cms::Destination::DestinationType::TOPIC;
//	else if (setting == TEMPORARY_QUEUE_CAPABILITY)
//		return ::cms::Destination::DestinationType::TEMPORARY_QUEUE;
//	else if (setting == TEMPORARY_TOPIC_CAPABILITY)
//		return ::cms::Destination::DestinationType::TEMPORARY_TOPIC;
//	else
//		throw ::cms::InvalidDestinationException("illegal capability type.");  //to do more explicit message
//}