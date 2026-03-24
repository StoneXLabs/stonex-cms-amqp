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

#include <cms/Session.h>
#include <proton/messaging_handler.hpp>
#include <proton/connection.hpp>

#include <condition_variable>
#include <mutex>

#include <log4cxx/logger.h>




namespace stonex::amqp
{
	class Session : public proton::messaging_handler, public cms::Session
	{
	public:
		explicit Session(proton::connection& connection, cms::Session::AcknowledgeMode ackMode);
		virtual ~Session();

		void start() override;
		void stop() override;
		void close() override;
		void commit() override;
		void rollback() override;
		void recover() override;
        cms::MessageConsumer* createConsumer(const cms::Destination* destination) override;
        cms::MessageConsumer* createConsumer(const cms::Destination* destination, const std::string& selector) override;
        cms::MessageConsumer* createConsumer(const cms::Destination* destination, const std::string& selector, bool noLocal) override;
        cms::MessageConsumer* createDurableConsumer(const cms::Topic* destination, const std::string& name, const std::string& selector, bool noLocal = false) override;
        cms::MessageProducer* createProducer(const cms::Destination* destination = nullptr) override;
        cms::QueueBrowser* createBrowser(const cms::Queue* queue) override;
        cms::QueueBrowser* createBrowser(const cms::Queue* queue, const std::string& selector) override;
        cms::Queue* createQueue(const std::string& queueName) override;
        cms::Topic* createTopic(const std::string& topicName) override;
        cms::TemporaryQueue* createTemporaryQueue() override;
        cms::TemporaryTopic* createTemporaryTopic() override;
        cms::Message* createMessage() override;
        cms::BytesMessage* createBytesMessage() override;
        cms::BytesMessage* createBytesMessage(const unsigned char* bytes, int bytesSize) override;
        cms::StreamMessage* createStreamMessage() override;
        cms::TextMessage* createTextMessage() override;
        cms::TextMessage* createTextMessage(const std::string& text) override;
        cms::MapMessage* createMapMessage() override;
        cms::Session::AcknowledgeMode getAcknowledgeMode() const override;
        bool isTransacted() const override;
        void unsubscribe(const std::string& name) override;
        void setMessageTransformer(cms::MessageTransformer* transformer) override;
        cms::MessageTransformer* getMessageTransformer() const override;

		void on_session_open(proton::session& session) override;
		void on_session_close(proton::session& session) override;
		void on_session_error(proton::session& session) override;

	private:
        log4cxx::LoggerPtr mLogger{ log4cxx::Logger::getLogger("com.stonex.cms.Session") };
		proton::connection mConnection;
		proton::session mSession;
		proton::work_queue* mWorkQueue{nullptr};
		std::mutex mMutex;
		std::condition_variable mCv;
	};

};

