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

#include <cms/ConnectionFactory.h>
#include "ConnectionImpl.h"

#include "FactoryContext.h"
#include <logger/StoneXLogger.h>

namespace cms::amqp
{
	class ConnectionFactoryImpl :  public std::enable_shared_from_this<ConnectionFactoryImpl>
	{
	public:
		explicit ConnectionFactoryImpl(const std::string& brokerURI);
		~ConnectionFactoryImpl() = default;

		config::ConnectionContext createConnectionContext();
		config::ConnectionContext createConnectionContext(const std::string& username, const std::string& password);
		config::ConnectionContext createConnectionContext(const std::string& username, const std::string& password, const std::string& clientId);
	
	private:
		StonexLoggerPtr mLogger;
		config::FactoryContext mContext;
	};
};




