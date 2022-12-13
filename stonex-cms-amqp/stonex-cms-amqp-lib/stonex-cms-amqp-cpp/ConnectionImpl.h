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

#include <proton/error_condition.hpp>
#include <proton/messaging_handler.hpp>

#include <cms/Connection.h>

#include "../API/ConnectionContext.h"
#include "AsyncCallSynchronizer.h"
#include "ClientState.h"

namespace cms::amqp
{
	//!ProtonConnection
	/*!
	* Object representing CMS connection.
	* Message Handler for connection callbacks.
	* Proton connection is responsible for creating sessions for this connection by implementing proton::message_handler methods
	* Allow Metrics <TO DO>
	*/
	class ConnectionImpl : public proton::messaging_handler {

	public:
		ConnectionImpl(const FactoryContext& context);

		ConnectionImpl(const std::string& id, const FactoryContext& context);

		//!Constructor
		/*!Create instance of CMS connection
			\param id connection id <not used>
			\param url CMS broker address
			\param user
			\param password
			\param container reference to proton::container
		*/
		//ConnectionImpl(const std::string& id, const std::string& url, const std::string user, const std::string password, proton::container& container);

		////!Constructor
		///*!Create instance of CMS connection
		//	\param id connection id <not used>
		//	\param url CMS broker address
		//	\param user
		//	\param password
		//	\param container shared pointer to proton::container
		//*/
		//ConnectionImpl(const std::string& id, const std::string& url, const std::string user, const std::string password, std::shared_ptr<proton::container> container);


		ConnectionImpl(const ConnectionImpl&) = delete;
		ConnectionImpl(ConnectionImpl&&) = delete;

		ConnectionImpl& operator = (const ConnectionImpl&) = delete;
		ConnectionImpl& operator = (ConnectionImpl&&) = delete;

		~ConnectionImpl() override;

		void close();
		void start();
		void stop();

		//	const ConnectionMetaData* getMetaData() const;


			//!createSession()
			/*! instantienate new session for connection
			* Session creation request is passed to connection work_queue, session
			* method returns immiediately but created object is blocked until session creation is confirmed by broker
			*/
			//Session* createSession();
			//Session* createSession(Session::AcknowledgeMode ackMode);

		std::string getClientID() const;
		void setClientID(const std::string& clientID);

		::cms::ExceptionListener* getExceptionListener() const;
		void setExceptionListener(::cms::ExceptionListener* listener);

		void setMessageTransformer(::cms::MessageTransformer* transformer);
		::cms::MessageTransformer* getMessageTransformer() const;


		void on_transport_open(proton::transport& transport) override;
		void on_transport_close(proton::transport& transport) override;
		void on_transport_error(proton::transport& transport) override;
		void on_connection_open(proton::connection& connection) override;
		void on_connection_close(proton::connection& connection) override;
		void on_connection_error(proton::connection& connection) override;

		std::shared_ptr<proton::connection> connection() const { return mConnection; }

	private:
		bool syncClose();
		bool syncStart();
		bool syncStop();
	private:
		ClientState mState;
		cms::internal::AsyncCallSynchronizer mEXHandler;
		const std::string mConnectionId;
		std::string mBrokerUrl;
		std::shared_ptr<proton::connection> mConnection;
		::cms::ExceptionListener* mExceptionListener{ nullptr };
		FactoryContext mContext;

	};

};
