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

#include "MessageProducer.h"

#include <string_view>

#include <proton/sender.hpp>
#include <proton/session.hpp>
#include <proton/connection.hpp>
#include <proton/sender_options.hpp>
#include <proton/target_options.hpp>
#include <proton/work_queue.hpp>
#include <proton/message_id.hpp>
#include <proton/annotation_key.hpp>

#include "Message.h"
#include "TextMessage.h"
#include "BytesMessage.h"
#include "Queue.h"
#include "Topic.h"
#include "TemporaryQueue.h"
#include "TemporaryTopic.h"
#include "Protocol/utils.h"

namespace {
	void setMessageBody(proton::message& dest, cms::Message* src)
	{
		if (auto msg = dynamic_cast<cms::TextMessage*>(src))
		{
			dest.body(msg->getText());
			dest.message_annotations().put(stonex::amqp::internal::annotation::JMS_MESSAGE_TYPE, static_cast<int8_t>(stonex::amqp::internal::annotation::MESSAGE_TYPE::TEXT_MESSAGE));
		}
		else if (auto msg = dynamic_cast<cms::BytesMessage*>(src))
		{
			std::vector<unsigned char> buf;
			buf.reserve(msg->getBodyLength());

			auto body = msg->getBodyBytes();

			buf.insert(buf.begin(), body, body + msg->getBodyLength());
			delete body;

			dest.body(proton::binary(buf));
			dest.message_annotations().put(stonex::amqp::internal::annotation::JMS_MESSAGE_TYPE, static_cast<int8_t>(stonex::amqp::internal::annotation::MESSAGE_TYPE::BYTES_MESSAGE));
		}
	}

	void setMessageProperties(proton::message& dest, cms::Message* src)
	{
		for (const auto& propertyName : src->getPropertyNames())
		{
			switch (src->getPropertyValueType(propertyName))
			{
			case cms::Message::ValueType::NULL_TYPE:
				break;
			case cms::Message::ValueType::BOOLEAN_TYPE:
				dest.properties().put(propertyName, src->getBooleanProperty(propertyName));
				break;
			case cms::Message::ValueType::BYTE_TYPE:
				dest.properties().put(propertyName, src->getByteProperty(propertyName));
				break;
			case cms::Message::ValueType::CHAR_TYPE:
				dest.properties().put(propertyName, src->getByteProperty(propertyName));
				break;
			case cms::Message::ValueType::SHORT_TYPE:
				dest.properties().put(propertyName, src->getShortProperty(propertyName));
				break;
			case cms::Message::ValueType::INTEGER_TYPE:
				dest.properties().put(propertyName, src->getIntProperty(propertyName));
				break;
			case cms::Message::ValueType::LONG_TYPE:
				dest.properties().put(propertyName, src->getLongProperty(propertyName));
				break;
			case cms::Message::ValueType::DOUBLE_TYPE:
				dest.properties().put(propertyName, src->getDoubleProperty(propertyName));
				break;
			case cms::Message::ValueType::FLOAT_TYPE:
				dest.properties().put(propertyName, src->getFloatProperty(propertyName));
				break;
			case cms::Message::ValueType::STRING_TYPE:
				if (propertyName == stonex::amqp::internal::properties::JMSX_GROUP_ID || propertyName == stonex::amqp::internal::properties::CMSX_GROUP_ID)
					dest.group_id(src->getStringProperty(propertyName));

				dest.properties().put(propertyName, src->getStringProperty(propertyName));
				break;
			case cms::Message::ValueType::BYTE_ARRAY_TYPE:
				break;
			case cms::Message::ValueType::UNKNOWN_TYPE:
				break;
			}
		}
	}
}


stonex::amqp::MessageProducer::MessageProducer(proton::session& session, const cms::Destination* destination)
:mSession(session)
{
	if (destination)
	{
		mDefaultDestination.reset(stonex::amqp::internal::DestinationConverter::createProtonDestination(destination));
		auto capabilities = stonex::amqp::internal::DestinationConverter::capabilities(destination);
		const auto address = stonex::amqp::internal::DestinationConverter::address(destination);

		proton::sender_options opts;
		proton::target_options target_options;
		target_options.capabilities(capabilities);
		opts.target(target_options);
		opts.handler(*this);
		
		mSession.work_queue().add([this, address, opts]() {mSession.open_sender(address, opts); });
	}

	std::unique_lock lk(mMutex);
	mCv.wait(lk, [this]() { return mWorkQueue; });
}

stonex::amqp::MessageProducer::~MessageProducer()
{
	close();
}

void stonex::amqp::MessageProducer::send(cms::Message* message)
{
	send(nullptr, message, mDeliveryMode, mPriority, mTTL, nullptr);
}

void stonex::amqp::MessageProducer::send(cms::Message* message, cms::AsyncCallback* onComplete)
{
	send(nullptr, message, mDeliveryMode, mPriority, mTTL, onComplete);
}

void stonex::amqp::MessageProducer::send(cms::Message* message, int deliveryMode, int priority, long long timeToLive)
{
	send(nullptr, message, deliveryMode, priority, timeToLive, nullptr);
}

void stonex::amqp::MessageProducer::send(cms::Message* message, int deliveryMode, int priority, long long timeToLive, cms::AsyncCallback* onComplete)
{
	send(nullptr, message, deliveryMode, priority, timeToLive, onComplete);
}

void stonex::amqp::MessageProducer::send(const cms::Destination* destination, cms::Message* message)
{
	if(!destination)
		throw cms::CMSException("no destination specified");
	send(destination, message, mDeliveryMode, mPriority, mTTL, nullptr);
}

void stonex::amqp::MessageProducer::send(const cms::Destination* destination, cms::Message* message, cms::AsyncCallback* onComplete)
{
	if(!destination)
		throw cms::CMSException("no destination specified");
	send(destination, message, mDeliveryMode, mPriority, mTTL, onComplete);
}

void stonex::amqp::MessageProducer::send(const cms::Destination *destination, cms::Message *message, int deliveryMode, int priority, long long timeToLive)
{
	if(!destination)
		throw cms::CMSException("no destination specified");
	send(destination, message, mDeliveryMode, mPriority, mTTL, nullptr);
}

void stonex::amqp::MessageProducer::send(const cms::Destination* destination, cms::Message* message, int deliveryMode, int priority, long long timeToLive, cms::AsyncCallback* onComplete)
{
	std::unique_lock lk(mMutex);
	mReadyToSend = false;

	if(destination)
	{
		message->setCMSDestination(destination);
	}
	else
	{	
		auto dest = stonex::amqp::internal::DestinationConverter::createCMSDestination(*mDefaultDestination);
		message->setCMSDestination(dest);
		delete dest;
	}

	//JMSDeliveryMode

	if(!mMessageIdDisabed)
		message->setCMSMessageID(proton::uuid().random().str());

	if(!mTimestampDisabed)
		message->setCMSTimestamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

	message->setCMSExpiration(timeToLive);
	message->setCMSPriority(priority);

	proton::message protonMessage;

	setMessageProperties(protonMessage, message);

	if (!mMessageIdDisabed)
	{
		protonMessage.id(message->getCMSMessageID());
		protonMessage.properties().put(stonex::amqp::internal::properties::JMSX_MESSAGE_ID, message->getCMSMessageID());
		protonMessage.properties().put(stonex::amqp::internal::properties::AMQP_MESSAGE_ID, message->getCMSMessageID());
	}

	if (!mTimestampDisabed)
		protonMessage.creation_time(proton::timestamp(message->getCMSTimestamp()));

	if (const auto dest = message->getCMSDestination(); dest != nullptr)
	{
		protonMessage.to(stonex::amqp::internal::DestinationConverter::address(dest));
		protonMessage.address(stonex::amqp::internal::DestinationConverter::address(dest));
		protonMessage.message_annotations().put(stonex::amqp::internal::annotation::JMS_DESTINATION_TYPE, static_cast<int8_t>(stonex::amqp::internal::DestinationConverter::jmsDestinationType(dest)));
	}

	if (const auto dest = message->getCMSReplyTo(); dest != nullptr)
	{
		protonMessage.reply_to(stonex::amqp::internal::DestinationConverter::address(dest));
		protonMessage.message_annotations().put(stonex::amqp::internal::annotation::JMS_REPLY_TO_TYPE, static_cast<int8_t>(stonex::amqp::internal::DestinationConverter::jmsDestinationType(dest)));
	}

	setMessageBody(protonMessage, message);

	mWorkQueue->add([=] {mSender.send(protonMessage); });
	mCv.wait(lk, [this]() { return mReadyToSend; });
}

void stonex::amqp::MessageProducer::close()
{
	std::unique_lock lk(mMutex);
	mWorkQueue->add([=] {mSender.close(); });
	mCv.wait(lk, [this]() { return !mWorkQueue; });
}

void stonex::amqp::MessageProducer::on_sendable(proton::sender& sender)
{
	std::unique_lock lk(mMutex);
	mReadyToSend = true;
	if(sender.credit() == 0)
		LOG4CXX_INFO(mLogger, std::format("producer credits {}", sender.credit()));
	mCv.notify_all();
}

void stonex::amqp::MessageProducer::on_sender_open(proton::sender& sender)
{
	std::unique_lock lk(mMutex);
	mWorkQueue = &sender.work_queue();
	mSender = sender;
	LOG4CXX_INFO(mLogger, std::format("producer open credits {}", sender.credit()));
	mCv.notify_all();
}

void stonex::amqp::MessageProducer::on_sender_error(proton::sender & sender)
{
	LOG4CXX_ERROR(mLogger, std::format("producer error {}", sender.error().what()));
}

void stonex::amqp::MessageProducer::on_sender_close(proton::sender& sender)
{
	std::unique_lock lk(mMutex);
	mWorkQueue = nullptr;
	LOG4CXX_INFO(mLogger, std::format("producer close"));
	mCv.notify_all();
}

void stonex::amqp::MessageProducer::on_error(const proton::error_condition& error)
{
	LOG4CXX_ERROR(mLogger, std::format("producer error {}", error.what()));
}

void stonex::amqp::MessageProducer::setDeliveryMode(int mode)
{
	switch (mode) 
	{
	case cms::DeliveryMode::PERSISTENT:
	case cms::DeliveryMode::NON_PERSISTENT:
		mDeliveryMode = (cms::DeliveryMode::DELIVERY_MODE)mode;
		break;
	default:

		throw cms::CMSException("Illegal delivery mode value");
	}
}

int stonex::amqp::MessageProducer::getDeliveryMode() const
{
	return mDeliveryMode;
}

void stonex::amqp::MessageProducer::setDisableMessageID(bool value)
{
	mMessageIdDisabed = value;
}

bool stonex::amqp::MessageProducer::getDisableMessageID() const
{
	return mMessageIdDisabed;
}

void stonex::amqp::MessageProducer::setDisableMessageTimeStamp(bool value)
{
	mTimestampDisabed = value;
}

bool stonex::amqp::MessageProducer::getDisableMessageTimeStamp() const
{
	return mTimestampDisabed;
}

void stonex::amqp::MessageProducer::setPriority(int priority)
{
	mPriority = priority;
}

int stonex::amqp::MessageProducer::getPriority() const
{
	return mPriority;
}

void stonex::amqp::MessageProducer::setTimeToLive(long long time)
{
	mTTL = time;
}

long long stonex::amqp::MessageProducer::getTimeToLive() const
{
	return mTTL;
}

void stonex::amqp::MessageProducer::setMessageTransformer(cms::MessageTransformer* transformer)
{
	//TO DO implement message available listener management
};

cms::MessageTransformer* stonex::amqp::MessageProducer::getMessageTransformer() const
{
	//TO DO implement message available listener management
	return nullptr; 
};