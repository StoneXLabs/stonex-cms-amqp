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

#include "ProducerContext.h"
#include "API/CMSQueue.h"
#include "API/CMSTopic.h"
#include "API/CMSTemporaryQueue.h"
#include "API/CMSTemporaryTopic.h"
#include "SessionContext.h"

#include <proton/target_options.hpp>
#include <proton/symbol.hpp>

cms::amqp::config::ProducerContext::ProducerContext(SessionContext& context, const cms::Destination* destination)
	:mDestination(destination),
	mWorkQueue(context.mWorkQueue),
	mSession(context.mSession)
{
}

std::pair<std::string,proton::sender_options> cms::amqp::config::ProducerContext::config()
{
	proton::sender_options opts;
	proton::target_options topts{};
	std::string address;

	if (mDestination)
	{
		switch (auto destType = mDestination->getDestinationType())
		{
		case ::cms::Destination::DestinationType::QUEUE:
			address = dynamic_cast<const CMSQueue*>(mDestination)->getQueueName();
			topts.capabilities(std::vector<proton::symbol> { "queue" });
			break;
		case ::cms::Destination::DestinationType::TOPIC:
			address = dynamic_cast<const CMSTopic*>(mDestination)->getTopicName();
			topts.capabilities(std::vector<proton::symbol> { "topic" });
			break;
		case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
			address = dynamic_cast<const CMSTemporaryQueue*>(mDestination)->getQueueName();
			topts.capabilities(std::vector<proton::symbol> { "temporary-queue", "delete-on-close" });
			if (address.empty())
				topts.dynamic(true); //taret or source options
			topts.expiry_policy(proton::terminus::expiry_policy::LINK_CLOSE); //according to documentation should be link detatch!!!!
			break;
		case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
			address = dynamic_cast<const CMSTemporaryTopic*>(mDestination)->getTopicName();
			topts.capabilities(std::vector<proton::symbol> { "temporary-topic", "delete-on-close" });
			topts.dynamic(true); //taret or source options
			topts.expiry_policy(proton::terminus::expiry_policy::LINK_CLOSE); //according to documentation should be link detatch!!!!
			break;

		}
	}
	else
	{
		topts.capabilities(std::vector<proton::symbol> { "queue" });
	}

	opts.target(topts);
	return { address, opts };
}