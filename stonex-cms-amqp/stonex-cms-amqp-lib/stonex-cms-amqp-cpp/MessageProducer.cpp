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

#include "Message.h"
#include "TextMessage.h"
#include "BytesMessage.h"
#include "Queue.h"
#include "Topic.h"
#include "TemporaryQueue.h"
#include "TemporaryTopic.h"

#include <fmt/format.h>


////
#include "TextMessage.h"


stonex::amqp::MessageProducer::MessageProducer(proton::session& session, const cms::Destination* destination)
:mSession(session)
{

	if (destination)
	{
		auto capabilities = internal::DestinationConverter::capabilities(destination);
		const auto address = internal::DestinationConverter::address(destination);

		mDestination.valid = true;
		mDestination.type = destination->getDestinationType();
		mDestination.address = address;

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
	if (destination)
	{
		message->setCMSDestination(destination);
	}
	else
	{
		message->setCMSDestination(internal::DestinationConverter::createCMSDestination(mDestination));
	}

	//JMSDeliveryMode

	if(!mMessageIdDisabed)
		message->setCMSMessageID(proton::uuid().random().str());

	if(!mTimestampDisabed)
		message->setCMSTimestamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

	message->setCMSExpiration(timeToLive);
	message->setCMSPriority(priority);

	auto castMessage = dynamic_cast<const stonex::amqp::BytesMessage*>(message);
	mWorkQueue->add([=] {mSender.send(castMessage->mMessage); });
	
	
}

void stonex::amqp::MessageProducer::close()
{
}

void stonex::amqp::MessageProducer::on_sendable(proton::sender& sender)
{
}

void stonex::amqp::MessageProducer::on_sender_open(proton::sender& sender)
{
	mWorkQueue = &sender.work_queue();
	mSender = sender;
	mCv.notify_all();
}

void stonex::amqp::MessageProducer::on_sender_error(proton::sender & sender)
{

}

void stonex::amqp::MessageProducer::on_sender_close(proton::sender& sender)
{

}


void  stonex::amqp::MessageProducer::on_sender_detach(proton::sender& sender)
{

}


void stonex::amqp::MessageProducer::on_error(const proton::error_condition& error)
{
}


void stonex::amqp::MessageProducer::on_tracker_accept(proton::tracker& tracker)
{
}

void stonex::amqp::MessageProducer::on_tracker_reject(proton::tracker& tracker)
{
}

void stonex::amqp::MessageProducer::on_tracker_release(proton::tracker& tracker)
{
}

void stonex::amqp::MessageProducer::on_tracker_settle(proton::tracker& tracker)
{
}

// void stonex::amqp::MessageProducer::send(const cms::Destination* destination, cms::Message* message)
// {
// 	send(destination, message, mDeliveryMode, mPriority, mTTL, nullptr);
// }

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