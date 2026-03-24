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

#pragma once

#include <condition_variable>
#include <mutex>

#include <proton/sender.hpp>
#include <proton/session.hpp>
#include <proton/work_queue.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/tracker.hpp>

#include <log4cxx/logger.h>

#include <cms/MessageProducer.h>
#include "Message.h"
#include <functional>
#include <chrono>

#include "Protocol/utils.h"

namespace stonex::amqp
{

	class MessageProducer : public proton::messaging_handler, public cms::MessageProducer
	{
	public:
		MessageProducer(proton::session& session, const cms::Destination* destination);
		virtual ~MessageProducer();
		
		void send(cms::Message* message) override;
		void send(cms::Message* message, cms::AsyncCallback* onComplete) override;
		void send(cms::Message* message, int deliveryMode, int priority, long long timeToLive) override;
		void send(cms::Message* message, int deliveryMode, int priority, long long timeToLive, cms::AsyncCallback* onComplete) override;
        void send(const cms::Destination* destination, cms::Message* message) override;
        void send(const cms::Destination* destination, cms::Message* message, cms::AsyncCallback* onComplete) override;
        void send(const cms::Destination* destination, cms::Message* message,
                          int deliveryMode, int priority, long long timeToLive) override;
        void send(const cms::Destination* destination, cms::Message* message, int deliveryMode,
                          int priority, long long timeToLive, cms::AsyncCallback* onComplete) override;


		void setDeliveryMode(int mode) override;
		int getDeliveryMode() const override;

		void setDisableMessageID(bool value) override;
		bool getDisableMessageID() const override;

		void setDisableMessageTimeStamp(bool value) override;
		bool getDisableMessageTimeStamp() const override;

		void setPriority(int priority) override;
		int getPriority() const override;

		void setTimeToLive(long long time) override;
		long long getTimeToLive() const override;

		void setMessageTransformer(cms::MessageTransformer* transformer) override;
		cms::MessageTransformer* getMessageTransformer() const override;
		
		void close();
	
	private:
		void on_sendable(proton::sender& sender) override;
		void on_sender_open(proton::sender& sender) override;
		void on_sender_error(proton::sender& sender) override;
		void on_sender_close(proton::sender& sender) override;
	    void on_error(const proton::error_condition& error) override;

	private:
		log4cxx::LoggerPtr mLogger{ log4cxx::Logger::getLogger("com.stonex.cms.MessageProducer") };
		bool mReadyToSend{false};
		proton::session mSession;
		proton::sender mSender;
		proton::work_queue* mWorkQueue{ nullptr };
		std::mutex mMutex;
		std::condition_variable mCv;
		std::unique_ptr<internal::Destination> mDefaultDestination{ nullptr };

		cms::DeliveryMode::DELIVERY_MODE mDeliveryMode = static_cast<cms::DeliveryMode::DELIVERY_MODE>(cms::Message::DEFAULT_DELIVERY_MODE);
		bool mMessageIdDisabed{ false };
		bool mTimestampDisabed{ false };
		int mPriority = cms::Message::DEFAULT_MSG_PRIORITY;
		long long mTTL = cms::Message::DEFAULT_TIME_TO_LIVE;
	};
};