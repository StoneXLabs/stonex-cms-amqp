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
#include <regex>
#include <proton/session.hpp>
#include <proton/receiver.hpp>
#include <proton/receiver_options.hpp>
#include <cms/Destination.h>
#include "ClientState.h"

namespace cms
{
namespace amqp
{
namespace config
{
class SessionContext;

class ConsumerContext : public StateMachine
{
	class DestinationParser
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="FQQN"></param>
		/// https://activemq.apache.org/components/artemis/migration-documentation/VirtualTopics.html
		/// "VirtualTopic.Orders::Consumer.A.VirtualTopic.Orders"
		/// <returns></returns>
		bool isShared(const std::string& FQQN)
		{
			return std::regex_match(FQQN, FQQN_regex);
		}

	private:

		const std::regex FQQN_regex{ "^VirtualTopic\\.[a-zA-Z0-9_-]+::Consumer(\\.[a-zA-Z0-9_-]+)+" };
	};

public:
	ConsumerContext(SessionContext& context, bool durable, bool shared, const cms::Destination* destination);

	bool isDurable();
	bool isShared();
	std::pair<const std::string, proton::receiver_options> config();

	proton::work_queue* mWorkQueue{ nullptr };
	proton::session mSession;
	proton::receiver mReceiver;
	const cms::Destination* mDestination{ nullptr };

private:
	bool mDurable_subscriber;
	bool mShared_subscriber;
	const std::string mSelector;
	const std::string mSubscriptionName;
	DestinationParser destAddressParser;
};

} //namespace config
} //namespace amqp
} //namespace cms