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
#include <string>
#include <LoggerFactory/LoggerFactory.h>

#include "CMSConnection.h"

#include "cms/ConnectionFactory.h"

#include "stonex-cms-amqp-lib-defines.h"

AMQP_DEFINES


	class ConnectionFactoryImpl;

	class CMS_API CMSConnectionFactory : public cms::ConnectionFactory
	{
	public:

		explicit CMSConnectionFactory(const std::string& brokerURI);

		~CMSConnectionFactory() override = default;

		cms::Connection* createConnection() override;

		cms::Connection* createConnection(const std::string& username, const std::string& password) override;

		cms::Connection* createConnection(const std::string& username, const std::string& password, const std::string& clientId) override;


		void setExceptionListener(cms::ExceptionListener* listener) override;

		cms::ExceptionListener* getExceptionListener() const override;

		void setMessageTransformer(cms::MessageTransformer* transformer) override;

		cms::MessageTransformer* getMessageTransformer() const  override;

		static cms::ConnectionFactory* createCMSConnectionFactory(const std::string& brokerURI);

	private:
		StonexLoggerPtr mLogger;
		std::shared_ptr<ConnectionFactoryImpl> mPimpl;
		cms::ExceptionListener* mExceptionListener{ nullptr };
		cms::MessageTransformer* mMessageTransformer{ nullptr };
	};


AMQP_DEFINES_CLOSE