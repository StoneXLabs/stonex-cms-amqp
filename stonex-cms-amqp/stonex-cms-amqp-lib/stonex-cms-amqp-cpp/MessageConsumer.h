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
#include <proton/session.hpp>

#include <cms/MessageConsumer.h>
#include <cms/MessageListener.h>
#include <cms/Message.h>

#include <log4cxx/logger.h>

#include <regex>

namespace stonex::amqp
{
	class MessageConsumer : public proton::messaging_handler, public cms::MessageConsumer
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
		explicit MessageConsumer(proton::session& session, const cms::Destination* destination, const std::string& selector);

		~MessageConsumer();

		
		void start() override;
		void stop() override;
		void close() override;

		cms::Message* receive() override;
		cms::Message* receive(int milis) override;
		cms::Message* receiveNoWait() override;

		void setMessageListener(cms::MessageListener * listener) override;
		cms::MessageListener* getMessageListener() const override;;

		std::string getMessageSelector() const override;;

		void setMessageTransformer(cms::MessageTransformer* transformer) override;
		cms::MessageTransformer* getMessageTransformer() const override;

		void setMessageAvailableListener(cms::MessageAvailableListener* listener) override;
		cms::MessageAvailableListener* getMessageAvailableListener() const override;


		void on_receiver_open(proton::receiver& receiver) override;
		void on_receiver_close(proton::receiver& receiver) override;
		void on_receiver_detach(proton::receiver& receiver) override;
		void on_receiver_error(proton::receiver& receiver) override;
		void on_receiver_drain_finish(proton::receiver& receiver) override;

		void on_message(proton::delivery& delivery, proton::message& message) override;


	private:
		log4cxx::LoggerPtr mLogger{ log4cxx::Logger::getLogger("CMS") };
		proton::session mSession;
		proton::receiver mReceiver;
		cms::MessageListener *mListener{ nullptr };
		proton::work_queue* mWorkQueue{ nullptr };

		std::function<void(cms::Message*)> onMessageCallback;

	private:
		std::mutex mMutex;
		std::condition_variable mCv;
	};

};

