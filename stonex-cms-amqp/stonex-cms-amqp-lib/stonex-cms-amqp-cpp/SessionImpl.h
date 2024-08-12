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

#include <cms/Session.h>
#include "SessionContext.h"
#include <proton/connection.hpp>
#include <proton/messaging_handler.hpp>

#include <condition_variable>
#include <mutex>

#include "API/ClientState.h"

#include <logger/StoneXLogger.h>

namespace cms::amqp
{
	class MessageConsumerImpl;
	class MessageProducerImpl;

	class SessionImpl : public proton::messaging_handler
	{
	public:
		explicit SessionImpl(const config::SessionContext& context);
		~SessionImpl();

		void start();
		void stop();
		void close();
		void commit();
		void rollback();
		void recover();

		::cms::Session::AcknowledgeMode ackMode();

		void on_session_open(proton::session& session) override;
		void on_session_close(proton::session& session) override;
		void on_session_error(proton::session& session) override;

		void addConsumer(std::shared_ptr<MessageConsumerImpl> consumer);
		void addProducer(std::shared_ptr<MessageProducerImpl> producer);
		void check();
	public:
		StonexLoggerPtr mLogger;
		config::SessionContext mContext;
	private:
		std::mutex mMutex;
		std::condition_variable mCv;
		std::vector<std::weak_ptr<MessageConsumerImpl>> mConsumers;
		std::vector<std::weak_ptr<MessageProducerImpl>> mProducers;
	};

};

