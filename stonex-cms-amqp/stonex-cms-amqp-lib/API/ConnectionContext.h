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
#include <string>
#include <vector>
#include <memory>

#include "URIParser.h"

#include "stonex-cms-amqp-lib-defines.h"

namespace proton
{
	class container;
	class messaging_handler;
	class connection;
	class session;
};



AMQP_DEFINES


	class CMS_API ConnectionOptions
	{
	public:
	private:
		struct FailoverTransportOptions 
		{
			FailoverTransportOptions(const std::string& options);
			void setParametersFromString(const std::string& parameter);

			int mInitialReconnectDelay;
			int mMaxReconnectDelay;
			bool mUseExpotentialBackOff;
			int mMaxReconnectAttempts;
			bool mRandomize;
			int mTimeout;
		};
	};

	class FactoryContext
	{
	public:
		FactoryContext(const std::string& url, const std::string& user, const std::string& password, const std::vector<std::string>& failover_urls, int reconnect_attempts, std::shared_ptr<proton::container> container);

		FactoryContext(const std::string& url, const std::string& user, const std::string& password, int reconnect_attempts, std::shared_ptr<proton::container> container);

		FactoryContext(const FactoryContext& other) = default;

		~FactoryContext() = default;
		FactoryContext(FactoryContext&& other) = delete;

		cms::amqp::FactoryContext& updateUser(const std::string& user);
		cms::amqp::FactoryContext& updatePassword(const std::string& password);
		cms::amqp::FactoryContext& updateCotainerId(const std::string& connectionId);
		void requestBrokerConnection(proton::messaging_handler* handler);

		std::shared_ptr<proton::container> container();

		std::string broker() const;
		std::string failoverAddresses() const;
		std::string user() const;
		int reconnectAttempts() const;

	private:
		std::string mBroker;
		std::vector<std::string> mFailoverAddresses;
		std::string mUser;
		std::string mPassword;
		std::shared_ptr<proton::container> mContainer;
		int mReconnectAttempts{ 0 }; //0 - no limit
		std::string mConnectionId{};
		URIParser::Parameters mParameters;


	};

	class ConnectionContext
	{
	public:
		ConnectionContext(std::shared_ptr<proton::connection> conn)
			:mConnection{conn}
		{
		}

		std::shared_ptr<proton::connection> connection() const { return mConnection; }
	private:
		std::shared_ptr<proton::connection> mConnection;
	};

	class SessionContext
	{
	public:
		SessionContext(std::shared_ptr<proton::session> sess, bool durable, bool shared, bool auto_ack)
			:mSession{ sess },
			mDurable_subscriber{durable},
			mShared_subscriber{shared},
			mAuto_ack{auto_ack}
		{
		}

		bool isDurable() { return mDurable_subscriber; }
		bool isShared() { return mShared_subscriber; }
		bool isAutoAck() { return mAuto_ack; }

		std::shared_ptr<proton::session> connection() const { return mSession; }
	private:
		std::shared_ptr<proton::session> mSession;
		bool mDurable_subscriber;
		bool mShared_subscriber;
		bool mAuto_ack;
	};

AMQP_DEFINES_CLOSE

