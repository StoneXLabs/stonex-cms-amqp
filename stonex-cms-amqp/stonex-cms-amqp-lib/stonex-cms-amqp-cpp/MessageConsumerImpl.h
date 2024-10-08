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
#include <functional>

#include <proton/receiver.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/receiver_options.hpp>

#include <cms/MessageConsumer.h>
#include <cms/MessageListener.h>

#include <cms/Message.h>
#include <regex>
#include "ClientState.h"
#include <logger/StoneXLogger.h>
#include "ConsumerContext.h"

namespace cms::amqp
{
	class MessageConsumerImpl : public proton::messaging_handler
	{
	
		class DestinationParser
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="FQQN"></param>
			/// https://activemq.apache.org/components/artemis/migration-documentation/VirtualTopics.html
			/// "VirtualTopic.Orders::Consumer.A.VirtualTopic.Orders"
			/// <returns></returns>
			bool isShared(const std::string& FQQN)
			{
				return std::regex_match(FQQN, FQQN_regex);
			}

		private:

			const std::regex FQQN_regex{ "^VirtualTopic\\.[a-zA-Z0-9_-]+::Consumer(\\.[a-zA-Z0-9_-]+)+" };
		};
	public:
		explicit MessageConsumerImpl(const config::ConsumerContext& context);

		~MessageConsumerImpl();
		cms::Message* receive();
		cms::Message* receive(int milis);
		cms::Message* receiveNoWait();

		void setMessageListener(cms::MessageListener * listener);
		cms::MessageListener* getMessageListener() const;

		std::string getMessageSelector() const;

		void setMessageTransformer(cms::MessageTransformer* transformer) {};
		cms::MessageTransformer* getMessageTransformer() const { return nullptr; };

		void setMessageAvailableListener(cms::MessageAvailableListener* listener);
		cms::MessageAvailableListener* getMessageAvailableListener() const { return nullptr; };

		void start();
		void stop();
		void close();

		void on_receiver_open(proton::receiver& receiver) override;
		void on_receiver_close(proton::receiver& receiver) override;
		void on_receiver_detach(proton::receiver& receiver) override;
		void on_receiver_error(proton::receiver& receiver) override;
		void on_receiver_drain_finish(proton::receiver& receiver) override;

		void on_message(proton::delivery& delivery, proton::message& message) override;

		const std::string getAddress() const;

	private:
		StonexLoggerPtr mLogger;
		cms::MessageListener *mListener{ nullptr };
		std::string mAddress;

		std::function<void(cms::Message*)> onMessageCallback = [=](cms::Message* message) ->void { delete message; };

		DestinationParser destAddressParser;
		config::ConsumerContext mContext;
	private:
		std::mutex mMutex;
		std::condition_variable mCv;
	};

};

