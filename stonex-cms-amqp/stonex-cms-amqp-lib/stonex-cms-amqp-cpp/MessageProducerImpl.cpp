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

#include "MessageProducerImpl.h"

#include <string_view>

#include <proton/sender.hpp>
#include <proton/session.hpp>
#include <proton/connection.hpp>
#include <proton/sender_options.hpp>
#include <proton/target_options.hpp>
#include <proton/work_queue.hpp>

#include "..\API\CMSMessage.h"
#include "..\API\CMSTextMessage.h"
#include "..\API\CMSBytesMessage.h"
#include "..\API\CMSQueue.h"
#include "..\API\CMSTopic.h"
#include "..\API\CMSTemporaryQueue.h"
#include "..\API\CMSTemporaryTopic.h"
#include "AMQPCMSMessageConverter.h"
#include "AMQPIDGenerator.h"

#include <fmt/format.h>

#include <LoggerFactory/LoggerFactory.h>


constexpr std::string_view QUEUE_CAPABILITY = "queue";
constexpr std::string_view TOPIC_CAPABILITY = "topic";
constexpr std::string_view TEMPORARY_QUEUE_CAPABILITY = "temporary-queue";
constexpr std::string_view TEMPORARY_TOPIC_CAPABILITY = "temporary-topic";

cms::amqp::MessageProducerImpl::MessageProducerImpl(const config::ProducerContext& context)
	:mLogger(LoggerFactory::getInstance().create("com.stonex.cms.amqp.MessageProducerImpl")),
	mContext(context)
{
	mContext.setLogger(mLogger);
	std::unique_lock lk(mMutex);
	auto opts = mContext.config();
	opts.second.handler(*this);
	if (mContext.mWorkQueue)
	{
		mContext.mWorkQueue->add([=]() {mContext.mSession.open_sender(opts.first,opts.second); });
		mCv.wait(lk, [this]() {return !mContext.checkState(ClientState::UNNINITIALIZED); });
	}
	else
	{
		throw CMSException("Session uninitiaized");
	}
}

cms::amqp::MessageProducerImpl::~MessageProducerImpl()
{
	close();
}

void cms::amqp::MessageProducerImpl::send(cms::Message* message)
{
	send(nullptr, message, mDeliveryMode, mPriority, mTTL, nullptr);
}

void cms::amqp::MessageProducerImpl::send(cms::Message* message, cms::AsyncCallback* onComplete)
{
	send(nullptr, message, mDeliveryMode, mPriority, mTTL, onComplete);
}

void cms::amqp::MessageProducerImpl::send(cms::Message* message, int deliveryMode, int priority, long long timeToLive)
{
	send(nullptr, message, deliveryMode, priority, timeToLive, nullptr);
}

void cms::amqp::MessageProducerImpl::send(cms::Message* message, int deliveryMode, int priority, long long timeToLive, cms::AsyncCallback* onComplete)
{
	send(nullptr, message, deliveryMode, priority, timeToLive, onComplete);
}

void cms::amqp::MessageProducerImpl::send(const cms::Destination* destination, cms::Message* message, int deliveryMode, int priority, long long timeToLive)
{
	send(destination, message, deliveryMode, priority, timeToLive, nullptr);
}

void cms::amqp::MessageProducerImpl::send(const cms::Destination* destination, cms::Message* message, int deliveryMode, int priority, long long timeToLive, cms::AsyncCallback* onComplete)
{
	auto message_copy = message->clone();
	std::unique_lock lk(mMutex);

	message_copy->setCMSDeliveryMode(deliveryMode);
	message_copy->setCMSPriority(priority);
	if (message_copy->getCMSDestination() == nullptr) [[unlikely]]
	{
		if (destination) [[unlikely]]
			message_copy->setCMSDestination(destination);
		else
		{
			auto sender_default_destination = AMQPCMSMessageConverter::createCMSDestination(&mContext.mSender);
			message_copy->setCMSDestination(sender_default_destination);
			delete sender_default_destination;
		}
	}

	//TO DO handle reply to and TTL
	message_copy->setCMSExpiration(timeToLive);


	mMessageIdSetter(message_copy);
	mTimestampSetter(message_copy);

	auto mess = mConverter.from_cms_message(message_copy);
	if(!mess)
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, "could not convert message to any of implemented types"));
	delete message_copy;
	
	if (mContext.mWorkQueue) //TO DO break by close called on send
	{
		mSendable.wait(lk, [this]() { return mCanSend; });
		mCanSend = false;
		if (onComplete) [[unlikely]]
			mContext.mWorkQueue->add([this, mess, onComplete] {mContext.mSender.send(*mess); onComplete->onSuccess(); });
		else
			mContext.mWorkQueue->add([this, mess] {mContext.mSender.send(*mess); });
	}
	else
	{
		throw CMSException("Session uninitiaized");
	}


	
	
}

void cms::amqp::MessageProducerImpl::close()
{

	mLogger->log(SEVERITY::LOG_TRACE, fmt::format("{}", __func__));

	if (mContext.checkState(ClientState::CLOSED))
	{
		mLogger->log(SEVERITY::LOG_WARNING, "trying to close producer that is allready closed");
		return;
	}

	if (mContext.mWorkQueue)
	{
		std::unique_lock lk(mMutex);
		mContext.mWorkQueue->add([=]() {mContext.mSender.close(); }); //check draining
		mCv.wait(lk, [this]() {return mContext.checkState(ClientState::CLOSED); });
	}
	else
	{
		throw CMSException("Session uninitiaized");
	}
}

void cms::amqp::MessageProducerImpl::on_sendable(proton::sender& sender)
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_TRACE, fmt::format("{}", __func__));
#endif
	mCanSend = true;
	mSendable.notify_one();
}

void cms::amqp::MessageProducerImpl::on_sender_open(proton::sender& sender)
{
	auto t1 = sender.target();

	if (auto err = sender.error(); err.empty())
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} address {} anonymous {} dynamic {} durable {} ", __func__, t1.address(), t1.anonymous(), t1.dynamic(), t1.durability_mode()));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));


	mContext.mSender = sender;
	mContext.mWorkQueue = &mContext.mSender.work_queue();
	mContext.mDestination = AMQPCMSMessageConverter::createCMSDestination(&mContext.mSender);
	if (mContext.checkState(ClientState::UNNINITIALIZED))
	{
		mContext.setState(ClientState::STOPPED);
	}
	else
	{
		mContext.setState(ClientState::STARTED);
	}
	mCv.notify_one();	
}

void cms::amqp::MessageProducerImpl::on_sender_error(proton::sender & sender)
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, sender.error().what()));
}

void cms::amqp::MessageProducerImpl::on_sender_close(proton::sender& sender)
{
	auto t1 = sender.target();
	if (auto err = sender.error(); err.empty())
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} address {} anonymous {} dynamic {} durable {} ", __func__, t1.address(), t1.anonymous(), t1.dynamic(), t1.durability_mode()));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} address {} anonymous {} dynamic {} durable {}  {}", __func__, t1.address(), t1.anonymous(), t1.dynamic(), t1.durability_mode(), err.what()));

	mContext.mWorkQueue = nullptr;
	mContext.setState(ClientState::CLOSED);
	mCv.notify_one();
}

void cms::amqp::MessageProducerImpl::send(const cms::Destination* destination, cms::Message* message)
{
	send(destination, message, mDeliveryMode, mPriority, mTTL, nullptr);
}

void cms::amqp::MessageProducerImpl::setDeliveryMode(int mode)
{
	switch (mode) 
	{
	case cms::DeliveryMode::PERSISTENT:
	case cms::DeliveryMode::NON_PERSISTENT:
		mDeliveryMode = (cms::DeliveryMode::DELIVERY_MODE)mode;
		break;
	default:
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("EXCEPTION {} {}", __func__, "Illegal delivery mode value"));
		throw cms::CMSException("Illegal delivery mode value");
	}
}

int cms::amqp::MessageProducerImpl::getDeliveryMode() const
{
	return mDeliveryMode;
}

void cms::amqp::MessageProducerImpl::setDisableMessageID(bool value)
{
	mMessageIdDisabed = value;
	if(mMessageIdDisabed)
		mMessageIdSetter = []([[maybe_unused]] const cms::Message*){};
	else
		mMessageIdSetter = [](cms::Message* message){
		if (message->getCMSMessageID().empty() == true)
		{
			message->setCMSMessageID(AMQPIDGenerator::generateMessageId());
		}
	};

}

bool cms::amqp::MessageProducerImpl::getDisableMessageID() const
{
	return mMessageIdDisabed;
}

void cms::amqp::MessageProducerImpl::setDisableMessageTimeStamp(bool value)
{
	mTimestampDisabed = value;
	if(mTimestampDisabed)
		mTimestampSetter = []([[maybe_unused]] const cms::Message*){};
	else
		mTimestampSetter = [](cms::Message* message){message->setCMSTimestamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()); };
	
}

bool cms::amqp::MessageProducerImpl::getDisableMessageTimeStamp() const
{
	return mTimestampDisabed;
}

void cms::amqp::MessageProducerImpl::setPriority(int priority)
{
	mPriority = priority;
}

int cms::amqp::MessageProducerImpl::getPriority() const
{
	return mPriority;
}

void cms::amqp::MessageProducerImpl::setTimeToLive(long long time)
{
	mTTL = time;
}

long long cms::amqp::MessageProducerImpl::getTimeToLive() const
{
	return mTTL;
}

std::shared_ptr <proton::message> cms::amqp::MessageProducerImpl::MessageConverter::from_cms_message(cms::Message* message)
{
	
	if (auto obj = dynamic_cast<CMSMessage*>(message))
	{
		return obj->mMessage;
	}
	else if (auto obj = dynamic_cast<CMSTextMessage*>(message))
	{
		return obj->mMessage;
	}
	else if (auto obj = dynamic_cast<CMSBytesMessage*>(message))
	{
		return obj->message();
	}
	//else if (auto obj = dynamic_cast<CMSMapMessage*>(message))
	//{
	//}
	//else if (auto obj = dynamic_cast<CMSStreamMessage*>(message))
	//{
	//}
	
	return nullptr;
}
