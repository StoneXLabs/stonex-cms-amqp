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

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/ConnectionMetaData.h>
#include <cms/ExceptionListener.h>

#include <logger/StonexLogSource.h>


#include "stonex-cms-amqp-lib-defines.h"


AMQP_DEFINES


	class ConnectionImpl;
	class FactoryContext;
	class ConnectionContext;

	class CMS_API CMSConnection : public ::cms::Connection, public StonexLogSource
	{
	public:
		explicit CMSConnection(std::shared_ptr<FactoryContext> context, std::shared_ptr<StonexLogger> logger = nullptr);
		CMSConnection(std::shared_ptr<FactoryContext> context, const std::string& username, const std::string& password, std::shared_ptr<StonexLogger> logger = nullptr);
		CMSConnection(std::shared_ptr<FactoryContext> context, const std::string& username, const std::string& password, const std::string& clientId, std::shared_ptr<StonexLogger> logger = nullptr);

		~CMSConnection() override = default;

		void close() override;
		void start() override;
		void stop() override;

		const ::cms::ConnectionMetaData* getMetaData() const override;


		//!createSession()
		/*! instantienate new session for connection
		* Session creation request is passed to connection work_queue, session
		* method returns immiediately but created object is blocked until session creation is confirmed by broker
		*/
		::cms::Session* createSession() override;
		::cms::Session* createSession(::cms::Session::AcknowledgeMode ackMode) override;

		std::string getClientID() const override;
		void setClientID(const std::string& clientID) override;

		::cms::ExceptionListener* getExceptionListener() const override;
		void setExceptionListener(::cms::ExceptionListener* listener) override;

		void setMessageTransformer(::cms::MessageTransformer* transformer) override;
		::cms::MessageTransformer* getMessageTransformer() const override;

		void setLogger(std::shared_ptr<StonexLogger> sink) override;

	protected:
		std::shared_ptr<ConnectionContext> connectionContext() const;

	private:
		std::shared_ptr<ConnectionImpl> mPimpl;

	};


AMQP_DEFINES_CLOSE