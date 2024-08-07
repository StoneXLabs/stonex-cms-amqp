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

#include <string>
#include <vector>
#include <memory>
#include <proton/container.hpp>
#include "../API/URIParser.h"

namespace cms
{
namespace amqp
{
namespace config
{
class FactoryContext
{
public:
	FactoryContext(const std::string& url);

	~FactoryContext() = default;
	FactoryContext(FactoryContext&& other) = delete;

	std::shared_ptr<proton::container> container();

	std::string broker() const;
	std::vector<std::string> failoverAddresses() const;
	std::string user() const;
	int reconnectAttempts() const;

	//	private:
	std::string mBroker;
	std::vector<std::string> mFailoverAddresses;
	std::string mUser;
	std::string mPassword;
	std::shared_ptr<proton::container> mContainer;
	int mReconnectAttempts{ 0 }; //0 - no limit
	std::string mConnectionId{};
	URIParser::Parameters mParameters;


};
} //namespace config
} //namespace amqp
} //namespace cms

