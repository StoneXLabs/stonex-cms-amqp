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
#include <memory>
#include <cms/Session.h>

#include <LoggerFactory/LoggerFactory.h>

#include "ClientState.h"

#include "stonex-cms-amqp-lib-defines.h"

AMQP_DEFINES


	class ConnectionContext;
	class SessionImpl;
	class SessionContext;
	class CMSConnection; 
	class MessageConsumerImpl;
	class MessageProducerImpl;

	class CMS_API CMSSession : public ::cms::Session
	{

	public:
		explicit CMSSession(std::shared_ptr<SessionImpl> impl);
		virtual ~CMSSession();

		void close() override;
		void commit() override;
		void rollback() override;
		void recover() override;

		void start() override;
		void stop() override;


		::cms::MessageConsumer* createConsumer(const ::cms::Destination* destination) override;
		::cms::MessageConsumer* createConsumer(const ::cms::Destination* destination, const std::string& selector) override;
		::cms::MessageConsumer* createConsumer(const ::cms::Destination* destination, const std::string& selector, bool noLocal) override;
		::cms::MessageConsumer* createDurableConsumer(const ::cms::Topic* destination, const std::string& name, const std::string& selector, bool noLocal = false) override;

		::cms::MessageProducer* createProducer(const ::cms::Destination* destination) override;


		::cms::QueueBrowser* createBrowser(const ::cms::Queue* queue) override;;
		::cms::QueueBrowser* createBrowser(const ::cms::Queue* queue, const std::string& selector) override;;

		::cms::Queue* createQueue(const std::string& queueName) override;
		::cms::Topic* createTopic(const std::string& topicName) override;

		::cms::TemporaryQueue* createTemporaryQueue() override;
		::cms::TemporaryTopic* createTemporaryTopic() override;

		::cms::Message* createMessage() override;

		::cms::BytesMessage* createBytesMessage() override;
		::cms::BytesMessage* createBytesMessage(const unsigned char* bytes, int bytesSize) override;

		::cms::StreamMessage* createStreamMessage() override;

		::cms::TextMessage* createTextMessage() override;
		::cms::TextMessage* createTextMessage(const std::string& text) override;

		::cms::MapMessage* createMapMessage() override;

		::cms::Session::AcknowledgeMode getAcknowledgeMode() const override;

		bool isTransacted() const override;
		void unsubscribe(const std::string& name) override;
		void setMessageTransformer(::cms::MessageTransformer* transformer) override;
		::cms::MessageTransformer* getMessageTransformer() const override;

	private:
		StonexLoggerPtr mLogger;
		std::shared_ptr<SessionImpl> mPimpl;
	};


AMQP_DEFINES_CLOSE