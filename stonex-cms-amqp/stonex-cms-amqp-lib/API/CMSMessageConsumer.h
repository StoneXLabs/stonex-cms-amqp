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

#include <cms/MessageConsumer.h>
#include <cms/MessageAvailableListener.h>

#include <LoggerFactory/LoggerFactory.h>

#include "stonex-cms-amqp-lib-defines.h"

AMQP_DEFINES


	class MessageConsumerImpl;
	class CMSSession;

	class CMS_API CMSMessageConsumer : public cms::MessageConsumer
	{
	public:
		CMSMessageConsumer(std::shared_ptr<MessageConsumerImpl> impl);
		~CMSMessageConsumer();

		cms::Message* receive() override;
		cms::Message* receive(int milis) override;
		cms::Message* receiveNoWait() override;
		
		void setMessageListener(cms::MessageListener* listener) override;
		cms::MessageListener* getMessageListener() const override;
		
		std::string getMessageSelector() const override;
		
		void setMessageTransformer(cms::MessageTransformer* transformer) override;
		cms::MessageTransformer* getMessageTransformer() const override;
		
		void setMessageAvailableListener(cms::MessageAvailableListener* listener) override;
		cms::MessageAvailableListener* getMessageAvailableListener() const override;

		void start() override;
		void stop() override;
		void close() override;
	private:
		StonexLoggerPtr mLogger;
		std::shared_ptr<MessageConsumerImpl> mPimpl;

	};


AMQP_DEFINES_CLOSE

