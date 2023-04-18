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

#pragma once

#include <condition_variable>
#include <mutex>

#include <proton/sender.hpp>
#include <proton/messaging_handler.hpp>

#include <cms/MessageProducer.h>
#include <cms/MessageListener.h>


#include "../API/CMSMessage.h"
#include "AsyncCallSynchronizer.h"
#include "AMQPIDGenerator.h"
#include "ClientState.h"

namespace cms::amqp
{

	class MessageProducerImpl : public proton::messaging_handler
	{
	private:
		class MessageConverter 
		{
		public:
			std::shared_ptr <proton::message> from_cms_message(::cms::Message* message);
		};
	public:
		MessageProducerImpl(const ::cms::Destination* destination, std::shared_ptr<proton::session> session);
		~MessageProducerImpl();
		void send(::cms::Message* message);
		void send(::cms::Message* message, ::cms::AsyncCallback* onComplete);
		void send(::cms::Message* message, int deliveryMode, int priority, long long timeToLive);
		void send(::cms::Message* message, int deliveryMode, int priority, long long timeToLive, ::cms::AsyncCallback* onComplete);

		/////////

        void send(const ::cms::Destination* destination, ::cms::Message* message, int deliveryMode, int priority, long long timeToLive);

		void send(const ::cms::Destination* destination, ::cms::Message* message, int deliveryMode,int priority, long long timeToLive, ::cms::AsyncCallback* onComplete);
		void send(const::cms::Destination*,::cms::Message*,::cms::AsyncCallback*) {};
		void send(const::cms::Destination* destination,::cms::Message* message);

		void setDeliveryMode(int mode);
		int getDeliveryMode() const;

		void setDisableMessageID(bool value);
		bool getDisableMessageID() const;

		void setDisableMessageTimeStamp(bool value);
		bool getDisableMessageTimeStamp() const;

		void setPriority(int priority);
		int getPriority() const;

		void setTimeToLive(long long time);
		long long getTimeToLive() const;

		void setMessageTransformer(::cms::MessageTransformer* transformer) { };
		::cms::MessageTransformer* getMessageTransformer() const { return nullptr; };
		
		//////////
		void close();

		void on_sendable(proton::sender& sender) override;
		void on_sender_open(proton::sender& sender) override;
		void on_sender_error(proton::sender& sender) override;
		void on_sender_close(proton::sender& sender) override;

	private:
		bool syncClose();
		bool syncStart(const std::string& address, const proton::sender_options& options, std::shared_ptr<proton::session>  session);
		bool syncStop();

	private:
		ClientState mState;
		std::unique_ptr<proton::sender> mProtonSender;
		cms::internal::AsyncCallSynchronizer mEXHandler;
		MessageConverter mConverter;

		::cms::DeliveryMode::DELIVERY_MODE mDeliveryMode = (::cms::DeliveryMode::DELIVERY_MODE)::cms::Message::DEFAULT_DELIVERY_MODE;
		bool mMessageIdDisabed{ false };
		bool mTimestampDisabed{ false };
		int mPriority = ::cms::Message::DEFAULT_MSG_PRIORITY;
		long long mTTL = ::cms::Message::DEFAULT_TIME_TO_LIVE;

		std::shared_ptr<::cms::Destination> mDestination{nullptr};

		std::function<void(::cms::Message*)> mMessageIdSetter = [](::cms::Message* message){
			if (message->getCMSMessageID().empty() == true)
			{
				message->setCMSMessageID(AMQPIDGenerator::generateMessageId());
			}
		};

		std::function<void(::cms::Message*)>mTimestampSetter = [](::cms::Message* message) {message->setCMSTimestamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()); };
	};

};