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

#include <proton/work_queue.hpp>
#include <proton/session.hpp>
#include <proton/connection.hpp>
#include "ClientState.h"

namespace cms
{
namespace amqp
{
namespace config
{
class ConnectionContext;

class SessionContext : public StateMachine
{
public:
	SessionContext(ConnectionContext& context, bool auto_ack);

	bool isAutoAck();

	proton::work_queue* mWorkQueue{ nullptr };
	proton::session mSession;
	proton::connection mConnection;
	bool mAuto_ack;
};

} //namespace config
} //namespace amqp
} //namespace cms