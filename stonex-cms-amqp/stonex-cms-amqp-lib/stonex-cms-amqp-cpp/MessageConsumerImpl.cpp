/*
 * Copyright 2022 - 2023 StoneX Financial Ltd.
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
#include <proton/delivery.hpp>


#include "../API/CMSTextMessage.h"
#include "../API/CMSBytesMessage.h"
#include "../API/CMSQueue.h"
#include "../API/CMSTopic.h"
#include "../API/CMSTemporaryQueue.h"
#include "../API/CMSTemporaryTopic.h"

#include "AMQPCMSMessageConverter.h"
#include "AsyncCallSynchronizer.h"

#include <fmt/format.h>

#include <LoggerFactory/LoggerFactory.h>

//cms::amqp::MessageConsumerImpl::MessageConsumerImpl(const ::cms::Destination* destination, std::shared_ptr<proton::session> session, const std::string& selector)
//	:MessageConsumerImpl(destination, {}, session,false,false,true,selector)
//{
//
//}

cms::amqp::MessageConsumerImpl::MessageConsumerImpl(const config::ConsumerContext& context)
	:mLogger(LoggerFactory::getInstance().create("com.stonex.cms.amqp.MessageConsumerImpl")),
	mContext(context)
{
	std::unique_lock lk(mMutex);
	proton::receiver_options ropts = mContext.config();
	ropts.handler(*this);
	if (mContext.mWorkQueue)
	{
		mContext.mWorkQueue->add([=]() {mContext.mReceiver.open(ropts); });
		mCv.wait(lk, [this]() {return mState != ClientState::UNNINITIALIZED; });
	}
	else
	{
		throw CMSException("Session uninitiaized");
	}

	//waiting until all relevant resources is initialized
//	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncCreate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), address, mRopts, session);
}

cms::amqp::MessageConsumerImpl::~MessageConsumerImpl()
{
	if(getState() == ClientState::STARTED)
		close();
}

::cms::Message* cms::amqp::MessageConsumerImpl::receive()
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, "internal method not implemented"));
	return nullptr;
}

::cms::Message* cms::amqp::MessageConsumerImpl::receive(int milis)
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, "internal method not implemented"));
	return nullptr;
}

::cms::Message* cms::amqp::MessageConsumerImpl::receiveNoWait()
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, "internal method not implemented"));
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

	mLogger->log(SEVERITY::LOG_INFO, fmt::format("{}", __func__));

	if (mContext.mWorkQueue)
	{
		std::unique_lock lk(mMutex);
		mContext.mWorkQueue->add([=]() {mContext.mReceiver.add_credit(10); });
		mCv.wait(lk, [this]() {return mState == ClientState::STOPPED; });
		setState(ClientState::STARTED);
	}
	else
	{
		throw CMSException("Session uninitiaized");
	}

}

void cms::amqp::MessageConsumerImpl::stop()
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("{}", __func__));

	if (mContext.mWorkQueue)
	{
		std::unique_lock lk(mMutex);
		mContext.mWorkQueue->add([=]() {mContext.mReceiver.drain(); }); //check draining
		mCv.wait(lk, [this]() {return mState == ClientState::STOPPED; });
		setState(ClientState::STARTED);
	}
	else
	{
		throw CMSException("Session uninitiaized");
	}
//	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncStop, this));
}

void cms::amqp::MessageConsumerImpl::close()
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("{}", __func__));

	if (mContext.mWorkQueue)
	{
		std::unique_lock lk(mMutex);
		mContext.mWorkQueue->add([=]() {mContext.mReceiver.close(); }); //check draining
		mCv.wait(lk, [this]() {return mState == ClientState::STOPPED; });
		setState(ClientState::STARTED);
	}
	else
	{
		throw CMSException("Session uninitiaized");
	}
//	mEXHandler.SynchronizeCall(std::bind(&MessageConsumerImpl::syncClose, this));
}

void cms::amqp::MessageConsumerImpl::on_receiver_open(proton::receiver& receiver)
{
	auto proton_source = receiver.source();

	if (auto err = receiver.error(); err.empty())
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} address {} anonymous {} dynamic {} durable {} ", __func__, proton_source.address(), proton_source.anonymous(), proton_source.dynamic(), proton_source.durability_mode()));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));


	mContext.mReceiver = receiver;
	mContext.mWorkQueue = &mContext.mReceiver.work_queue();
	mContext.mDestination = AMQPCMSMessageConverter::createCMSDestination(&mContext.mReceiver);
	setState(ClientState::STARTED); //check
	mCv.notify_one();
//	mProtonReceiver = std::make_unique<proton::receiver>(receiver);
	//if (receiver.error().empty())
	//{
	//	mDestination.reset(AMQPCMSMessageConverter::createCMSDestination(mProtonReceiver.get()));
	//	setState(ClientState::STARTED);
	//	mEXHandler.onResourceInitialized();
	//}


}

void cms::amqp::MessageConsumerImpl::on_receiver_close(proton::receiver& receiver)
{
#if _DEBUG
	if (auto err = receiver.error(); err.empty())
		mLogger->log(SEVERITY::LOG_TRACE, fmt::format("{}", __func__));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));
#endif/*
	mEXHandler.onResourceUninitialized(receiver.error());

	setState(ClientState::STOPPED);*/
	setState(ClientState::CLOSED);
	mCv.notify_one();
}

void cms::amqp::MessageConsumerImpl::on_receiver_detach(proton::receiver& receiver)
{
#if _DEBUG
	if (auto err = receiver.error(); err.empty())
		mLogger->log(SEVERITY::LOG_TRACE, fmt::format("{}", __func__));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));
#endif/*
	mEXHandler.onResourceInitialized();
	setState(ClientState::DETATCHED);*/

	setState(ClientState::DETATCHED);
	mCv.notify_one();
}

void cms::amqp::MessageConsumerImpl::on_receiver_error(proton::receiver& receiver)
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, receiver.error().what()));
}

void cms::amqp::MessageConsumerImpl::on_receiver_drain_finish(proton::receiver& receiver)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} draining done", __func__));
	//setState(ClientState::STOPPED);
	//mEXHandler.onResourceInitialized();

	setState(ClientState::STOPPED);
	mCv.notify_one();
}


void cms::amqp::MessageConsumerImpl::on_message(proton::delivery& delivery, proton::message& message)
{
	if (!delivery.receiver().draining())
	  delivery.receiver().add_credit(1);

	if (message.content_type() == "application/octet-stream")
	{
#if _DEBUG
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} {} credit {} draining {}", __func__, "bytes message", delivery.receiver().credit(), delivery.receiver().draining()));
#endif
		onMessageCallback(new CMSBytesMessage(&message, &delivery, &mContext.mReceiver));
	}
	else
	{
#if _DEBUG
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} {} credit {}, draining {}", __func__, "text message", delivery.receiver().credit(), delivery.receiver().draining()));
#endif
		onMessageCallback(new CMSTextMessage(&message, &delivery, &mContext.mReceiver));
	}
	
}

const std::string cms::amqp::MessageConsumerImpl::getAddress() const
{
	return  mContext.mReceiver.source().address();
}

cms::amqp::ClientState cms::amqp::MessageConsumerImpl::getState()
{
	return mState;
}

void cms::amqp::MessageConsumerImpl::setState(ClientState state)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} last state {} current state {}", __func__, mState, state));
	mState = state;
}

//bool cms::amqp::MessageConsumerImpl::syncCreate(const std::string& address, const proton::receiver_options& options, std::shared_ptr<proton::session> session)
//{
//#if _DEBUG
//	mLogger->log(SEVERITY::LOG_TRACE, fmt::format(" {} address {}", __func__, address));
//#endif
//
////	return session->connection().work_queue().add([=] {session->open_receiver(address, mRopts); });
//	return false;
//}
//
//bool cms::amqp::MessageConsumerImpl::syncClose()
//{
//#if _DEBUG
//	mLogger->log(SEVERITY::LOG_TRACE, fmt::format(" {}", __func__));
//#endif
//
//	if (!mProtonReceiver->closed() /* && mState == STATUS::OPEN*/) {
//		return mProtonReceiver->connection().work_queue().add([=] {mProtonReceiver->close(); });
//	}
//	else
//		return false;
//}

//bool cms::amqp::MessageConsumerImpl::syncStop()
//{
//#if _DEBUG
//	mLogger->log(SEVERITY::LOG_TRACE, fmt::format("{}", __func__));
//#endif
//
//	return 	mProtonReceiver->work_queue().add([=] {
//		if(!mProtonReceiver->draining())
//		  mProtonReceiver->drain();
//	});
////	return 	mProtonReceiver->connection().work_queue().add([=] {mProtonReceiver->detach(); });
//}
