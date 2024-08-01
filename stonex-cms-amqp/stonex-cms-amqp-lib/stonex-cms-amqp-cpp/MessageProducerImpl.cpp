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

cms::amqp::MessageProducerImpl::MessageProducerImpl(const ProducerContext& context)
	:mLogger(LoggerFactory::getInstance().create("com.stonex.cms.amqp.MessageProducerImpl")),
	mEXHandler(mLogger),
	mContext(context)
{
	proton::sender_options opts = mContext.config();
	opts.handler(*this);
//	mEXHandler.SynchronizeCall(std::bind(&MessageProducerImpl::syncCreate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), address, opts, session);
}

cms::amqp::MessageProducerImpl::~MessageProducerImpl()
{
	close();
}

void cms::amqp::MessageProducerImpl::send(::cms::Message* message)
{
	send(nullptr, message, mDeliveryMode, mPriority, mTTL, nullptr);
}

void cms::amqp::MessageProducerImpl::send(::cms::Message* message, ::cms::AsyncCallback* onComplete)
{
	send(nullptr, message, mDeliveryMode, mPriority, mTTL, onComplete);
}

void cms::amqp::MessageProducerImpl::send(::cms::Message* message, int deliveryMode, int priority, long long timeToLive)
{
	send(nullptr, message, deliveryMode, priority, timeToLive, nullptr);
}

void cms::amqp::MessageProducerImpl::send(::cms::Message* message, int deliveryMode, int priority, long long timeToLive, ::cms::AsyncCallback* onComplete)
{
	send(nullptr, message, deliveryMode, priority, timeToLive, onComplete);
}

void cms::amqp::MessageProducerImpl::send(const::cms::Destination* destination, ::cms::Message* message, int deliveryMode, int priority, long long timeToLive)
{
	send(destination, message, deliveryMode, priority, timeToLive, nullptr);
}

void cms::amqp::MessageProducerImpl::send(const::cms::Destination* destination, ::cms::Message* message, int deliveryMode, int priority, long long timeToLive, ::cms::AsyncCallback* onComplete)
{
	auto message_copy = message->clone();

	mEXHandler.waitForResource();


	message_copy->setCMSDeliveryMode(deliveryMode);
	message_copy->setCMSPriority(priority);
	if (message_copy->getCMSDestination() == nullptr) [[unlikely]]
	{
		if (destination) [[unlikely]]
			message_copy->setCMSDestination(destination);
		else
		{
			auto sender_default_destination = AMQPCMSMessageConverter::createCMSDestination(mProtonSender.get());
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
	if(onComplete) [[unlikely]]
		mProtonSender->connection().work_queue().add([this, mess, onComplete] {mProtonSender->send(*mess); onComplete->onSuccess(); });
	else
		mProtonSender->connection().work_queue().add([this, mess] {mProtonSender->send(*mess); });

	
	
}

void cms::amqp::MessageProducerImpl::close()
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_TRACE, fmt::format("{}", __func__));
#endif
	if(mState == ClientState::STARTED)
		mEXHandler.SynchronizeCall(std::bind(&MessageProducerImpl::syncClose, this));
}

void cms::amqp::MessageProducerImpl::on_sendable(proton::sender& sender)
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_TRACE, fmt::format("{}", __func__));
#endif
	mEXHandler.onResourceInitialized();
}

void cms::amqp::MessageProducerImpl::on_sender_open(proton::sender& sender)
{
	auto t1 = sender.target();

	if (auto err = sender.error(); err.empty())
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} address {} anonymous {} dynamic {} durable {} ", __func__, t1.address(), t1.anonymous(), t1.dynamic(), t1.durability_mode()));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));

	mProtonSender = std::make_unique<proton::sender>(sender);
	if (sender.error().empty())
	{
		
		//add comment
		mDestination.reset(AMQPCMSMessageConverter::createCMSDestination(mProtonSender.get()));

		mState = ClientState::STARTED;
		mEXHandler.onResourceInitialized();
	}
	
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
	mState = ClientState::CLOSED;
	mEXHandler.onResourceUninitialized(sender.error());
}

bool cms::amqp::MessageProducerImpl::syncClose()
{
	if (mState == ClientState::STARTED)
		return  mProtonSender->session().connection().work_queue().add([this] {mProtonSender->close(); });
	else 
		return true;
}

bool cms::amqp::MessageProducerImpl::syncCreate(const std::string& address, const proton::sender_options& options, std::shared_ptr<proton::session> session)
{
	if (mState != ClientState::STARTED)
		return  session->connection().work_queue().add([session, address, options] {session->open_sender(address, options); });
	else
		return true;
	
}

void cms::amqp::MessageProducerImpl::send(const::cms::Destination* destination, ::cms::Message* message)
{
	send(destination, message, mDeliveryMode, mPriority, mTTL, nullptr);
}

void cms::amqp::MessageProducerImpl::setDeliveryMode(int mode)
{
	switch (mode) 
	{
	case ::cms::DeliveryMode::PERSISTENT:
	case ::cms::DeliveryMode::NON_PERSISTENT:
		mDeliveryMode = (::cms::DeliveryMode::DELIVERY_MODE)mode;
		break;
	default:
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("EXCEPTION {} {}", __func__, "Illegal delivery mode value"));
		throw ::cms::CMSException("Illegal delivery mode value");
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
		mMessageIdSetter = []([[maybe_unused]] const ::cms::Message*){};
	else
		mMessageIdSetter = [](::cms::Message* message){
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
		mTimestampSetter = []([[maybe_unused]] const ::cms::Message*){};
	else
		mTimestampSetter = [](::cms::Message* message){message->setCMSTimestamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()); };
	
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

std::shared_ptr <proton::message> cms::amqp::MessageProducerImpl::MessageConverter::from_cms_message(::cms::Message* message)
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
