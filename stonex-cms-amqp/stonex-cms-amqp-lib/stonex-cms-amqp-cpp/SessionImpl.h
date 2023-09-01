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

#include "../activemq-cpp/src/main/cms/Session.h"
#include "../API/ConnectionContext.h"
#include <proton/connection.hpp>
#include <proton/messaging_handler.hpp>

#include <condition_variable>
#include <mutex>

#include "AsyncCallSynchronizer.h"
#include "ClientState.h"

#include <logger/StonexLogSource.h>

namespace cms::amqp
{

	class SessionImpl : public proton::messaging_handler, public StonexLogSource
	{
	public:
		explicit SessionImpl(std::shared_ptr<proton::connection>  connection, ::cms::Session::AcknowledgeMode ack_mode = ::cms::Session::AUTO_ACKNOWLEDGE, std::shared_ptr<StonexLogger> logger = nullptr);
		~SessionImpl();
	

		void close();
		void commit();
		void rollback();
		void recover();

		void start();
		void stop();

		::cms::Session::AcknowledgeMode ackMode();

		void on_session_open(proton::session& session) override;
		void on_session_close(proton::session& session) override;
		void on_session_error(proton::session& session) override;

		std::shared_ptr<proton::session> session();

	private:
		bool syncClose();
		bool syncStart(std::shared_ptr<proton::connection>  connection);
		bool syncStop();

	private:
		ClientState mState;
		std::shared_ptr<proton::session> mSession;
		cms::internal::AsyncCallSynchronizer mEXHandler;
		const ::cms::Session::AcknowledgeMode mACKMode;
	};

};

