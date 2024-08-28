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
#include <proton/work_queue.hpp>
#include <proton/connection.hpp>
#include "ClientState.h"



namespace cms
{
namespace amqp
{
namespace config
{

class FactoryContext;

class ConnectionContext : public StateMachine
{
public:
	ConnectionContext(FactoryContext& context, const std::string& username = "", const std::string& password = "", const std::string& clientId = "");

	proton::connection_options config();
	std::string mainBroker();
	std::string failoverUrl();
	std::string user();
	std::string clientId();


	proton::work_queue* mWorkQueue{ nullptr };
	proton::connection mConnection;

private:
	const std::string mPrimaryUrl;
	const std::vector<std::string> mFailoverUrls;
	const std::string mUser;
	const std::string mPassword;
	const std::string mClientId;
	const int mInitialReconnectDelay;
	const int mMaxReconnectDelay;
	const int mMaxReconnectAttempts;
};

} //namespace config
} //namespace amqp
} //namespace cms







//AMQP_DEFINES_CLOSE

