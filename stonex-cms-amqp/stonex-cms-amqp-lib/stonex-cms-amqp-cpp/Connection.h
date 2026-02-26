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

//proton includes
#include <proton/messaging_handler.hpp>
#include <proton/connection_options.hpp>
#include <proton/connection.hpp>

//ActiveMq interface
#include "cms/Connection.h"

#include <condition_variable>


namespace stonex::amqp
{
	//!ProtonConnection
	/*!
	* Object representing CMS connection.
	* Message Handler for connection callbacks.
	* Proton connection is responsible for creating sessions for this connection by implementing proton::message_handler methods
	* Allow Metrics <TO DO>
	*/
	class Connection : public proton::messaging_handler, public cms::Connection
	{
	public:
		Connection(const std::string& primaryUrl, proton::connection_options& connectionOptions);

		Connection(const Connection&) = delete;
		Connection(Connection&&) = delete;

		Connection& operator = (const Connection&) = delete;
		Connection& operator = (Connection&&) = delete;

		~Connection() override;
		void start() override;
		void stop() override;
		void close() override;

        const cms::ConnectionMetaData* getMetaData() const override;
        cms::Session* createSession() override;
        cms::Session* createSession(cms::Session::AcknowledgeMode ackMode) override;
        std::string getClientID() const override;
        void setClientID(const std::string& clientID) override;
        cms::ExceptionListener* getExceptionListener() const override;
        void setExceptionListener(cms::ExceptionListener* listener) override;
        void setMessageTransformer(cms::MessageTransformer* transformer) override;
        cms::MessageTransformer* getMessageTransformer() const override;

		void on_transport_open(proton::transport& transport) override;
		void on_transport_close(proton::transport& transport) override;
		void on_transport_error(proton::transport& transport) override;
		void on_connection_open(proton::connection& connection) override;
		void on_connection_close(proton::connection& connection) override;
		void on_connection_error(proton::connection& connection) override;
	public:
		cms::ExceptionListener* mExceptionListener{ nullptr };
		cms::MessageTransformer* mMessageTransformer{ nullptr };

	private:
		std::mutex mMutex;
		std::condition_variable mCv;
		proton::connection mConnection;
		proton::work_queue* mWorkQueue{ nullptr };
		const std::string mPrimaryUrl;
		const proton::connection_options mConnectionOptions;
	};

};
