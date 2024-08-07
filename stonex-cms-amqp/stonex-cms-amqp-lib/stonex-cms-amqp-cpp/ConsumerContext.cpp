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
#include "ConsumerContext.h"
#include <proton/source_options.hpp>
#include "API/CMSQueue.h"
#include "API/CMSTopic.h"
#include "API/CMSTemporaryQueue.h"
#include "API/CMSTemporaryTopic.h"

cms::amqp::config::ConsumerContext::ConsumerContext(SessionContext& context, bool durable, bool shared, const cms::Destination* destination)
	:mDurable_subscriber{ durable },
	mShared_subscriber{ shared },
	mDestination(destination)
{
}

bool cms::amqp::config::ConsumerContext::isDurable() { return mDurable_subscriber; }
bool cms::amqp::config::ConsumerContext::isShared() { return mShared_subscriber; }

proton::receiver_options cms::amqp::config::ConsumerContext::config()
{
	proton::receiver_options ropts;
	proton::source_options sopts{};
	std::string address;

	switch (auto destType = mDestination->getDestinationType())
	{
	case ::cms::Destination::DestinationType::QUEUE:
		address = dynamic_cast<const CMSQueue*>(mDestination)->getQueueName();
		if (destAddressParser.isShared(address)/*shared*/)
		{
			sopts.capabilities(std::vector<proton::symbol> { "queue", "shared" });
		}
		else
			sopts.capabilities(std::vector<proton::symbol> { "queue" });
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		address = dynamic_cast<const CMSTopic*>(mDestination)->getTopicName();
		sopts.capabilities(std::vector<proton::symbol> { "topic" });
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		address = dynamic_cast<const CMSTemporaryQueue*>(mDestination)->getQueueName();
		sopts.capabilities(std::vector<proton::symbol> { "temporary-queue", "delete-on-close"});
		sopts.dynamic(true); //taret or source options
		sopts.expiry_policy(proton::terminus::expiry_policy::LINK_CLOSE); //according to documentation should be link detatch!!!!
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		address = dynamic_cast<const CMSTemporaryTopic*>(mDestination)->getTopicName();
		sopts.capabilities(std::vector<proton::symbol> { "temporary-topic", "delete-on-close"});
		sopts.dynamic(true); //taret or source options
		sopts.expiry_policy(proton::terminus::expiry_policy::LINK_CLOSE);
		break;
	}

	if (!mSubscriptionName.empty())
		ropts.name(mSubscriptionName);

	if (mDurable_subscriber)
	{
		//configure durable subscription
		sopts.durability_mode(proton::source::UNSETTLED_STATE);
		sopts.expiry_policy(proton::source::NEVER);
	}

	if (!mSelector.empty())
	{
		proton::source::filter_map _filters;
		proton::symbol filter_key("selector");
		proton::value filter_value;
		// The value is a specific CMS "described type": binary string with symbolic descriptor
		proton::codec::encoder enc(filter_value);
		enc << proton::codec::start::described()
			<< proton::symbol("apache.org:selector-filter:string")
			<< mSelector
			<< proton::codec::finish();
		// In our case, the map has this one element
		_filters.put(filter_key, filter_value);
		sopts.filters(_filters);
	}

	ropts.auto_accept(true); //to do var
	ropts.source(sopts);
	ropts.credit_window(0);

	return ropts;
}