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

#include <proton/error_condition.hpp>
#include <proton/messaging_handler.hpp>

#include <cms/Connection.h>

#include "ConnectionContext.h"
#include "ClientState.h"

#include <memory>
#include <logger/StoneXLogger.h>

namespace cms::amqp
{
	class SessionImpl;
	//!ProtonConnection
	/*!
	* Object representing CMS connection.
	* Message Handler for connection callbacks.
	* Proton connection is responsible for creating sessions for this connection by implementing proton::message_handler methods
	* Allow Metrics <TO DO>
	*/
	class ConnectionImpl : public proton::messaging_handler
	{
	public:
		ConnectionImpl(const config::ConnectionContext& context);

		ConnectionImpl(const ConnectionImpl&) = delete;
		ConnectionImpl(ConnectionImpl&&) = delete;

		ConnectionImpl& operator = (const ConnectionImpl&) = delete;
		ConnectionImpl& operator = (ConnectionImpl&&) = delete;

		~ConnectionImpl() override;
		void start();
		void stop();
		void close();

		std::string getClientID() const;
		void setClientID(const std::string& clientID);

		cms::ExceptionListener* getExceptionListener() const;
		void setExceptionListener(cms::ExceptionListener* listener);

		void setMessageTransformer(cms::MessageTransformer* transformer);
		cms::MessageTransformer* getMessageTransformer() const;

		void addSession(std::shared_ptr<SessionImpl> session);

		void on_transport_open(proton::transport& transport) override;
		void on_transport_close(proton::transport& transport) override;
		void on_transport_error(proton::transport& transport) override;
		void on_connection_open(proton::connection& connection) override;
		void on_connection_close(proton::connection& connection) override;
		void on_connection_error(proton::connection& connection) override;
	public:
		StonexLoggerPtr mLogger;
		const std::string mConnectionId;
		std::string mBrokerUrl;
		cms::ExceptionListener* mExceptionListener{ nullptr };
		config::ConnectionContext mContext;

	private:
		std::mutex mMutex;
		std::condition_variable mCv;
		std::vector<std::weak_ptr<SessionImpl>> mSessions;

	};

};
