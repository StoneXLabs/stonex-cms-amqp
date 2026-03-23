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

#pragma once

#include <cms/ConnectionFactory.h>
#include <log4cxx/logger.h>
#include <vector>

namespace stonex::amqp
{
	class ConnectionFactory :  public cms::ConnectionFactory
	{
	public:
		explicit ConnectionFactory(const std::string& brokerURI);
		~ConnectionFactory() = default;

		cms::Connection* createConnection() override;
		cms::Connection* createConnection(const std::string& username, const std::string& password) override;
		cms::Connection* createConnection(const std::string& username, const std::string& password, const std::string& clientId) override;
		void setExceptionListener(cms::ExceptionListener* listener) override;
        cms::ExceptionListener* getExceptionListener() const override;
        void setMessageTransformer(cms::MessageTransformer* transformer) override;
        cms::MessageTransformer* getMessageTransformer() const override;
        static cms::ConnectionFactory* createCMSConnectionFactory(const std::string& brokerURI);

	private:
		const std::string mPrimaryUrl;
		const std::vector<std::string> mFailoverUrl;
		log4cxx::LoggerPtr mLogger{ log4cxx::Logger::getLogger("CMS") };
	};
};




