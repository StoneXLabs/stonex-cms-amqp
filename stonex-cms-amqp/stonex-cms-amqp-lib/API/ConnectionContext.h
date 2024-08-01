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
#include <vector>
#include <memory>

#include "URIParser.h"

#include "stonex-cms-amqp-lib-defines.h"
#include <proton/connection_options.hpp>
#include <proton/reconnect_options.hpp>
#include <proton/source_options.hpp>
#include <proton/receiver_options.hpp>
#include <proton/sender_options.hpp>
#include <proton/target_options.hpp>
#include <proton/container.hpp>
#include <regex>
#include <cms/Destination.h>

#include "CMSQueue.h"
#include "CMSTopic.h"
#include "CMSTemporaryQueue.h"
#include "CMSTemporaryTopic.h"

namespace proton
{
	class container;
	class messaging_handler;
	class connection;
	class session;
};



AMQP_DEFINES


	class CMS_API ConnectionOptions
	{
	public:
	private:
		struct FailoverTransportOptions 
		{
			void setParametersFromString(const std::string& parameter);

			int mInitialReconnectDelay;
			int mMaxReconnectDelay;
			bool mUseExpotentialBackOff;
			int mMaxReconnectAttempts;
			bool mRandomize;
			int mTimeout;
		};
	};

	class FactoryContext
	{
	public:
		FactoryContext(const std::string& url);

		~FactoryContext() = default;
		FactoryContext(FactoryContext&& other) = delete;

		cms::amqp::FactoryContext& updateUser(const std::string& user);
		cms::amqp::FactoryContext& updatePassword(const std::string& password);
		cms::amqp::FactoryContext& updateCotainerId(const std::string& connectionId);

		std::shared_ptr<proton::container> container();

		std::string broker() const;
		std::vector<std::string> failoverAddresses() const;
		std::string user() const;
		int reconnectAttempts() const;

//	private:
		std::string mBroker;
		std::vector<std::string> mFailoverAddresses;
		std::string mUser;
		std::string mPassword;
		std::shared_ptr<proton::container> mContainer;
		int mReconnectAttempts{ 0 }; //0 - no limit
		std::string mConnectionId{};
		URIParser::Parameters mParameters;


	};

	class ConnectionContext
	{
	public:
		ConnectionContext(FactoryContext& context, const std::string& username = "", const std::string& password = "", const std::string& clientId = "")
			:mPrimaryUrl(context.broker()),
			mFailoverUrls(context.failoverAddresses()),
			mUser(username),
			mPassword(password),
			mClientId(clientId),
			mInitialReconnectDelay(context.mParameters.failoverOptrions.initialReconnectDelay),
			mMaxReconnectDelay(context.mParameters.failoverOptrions.maxReconnectDelay),
			mMaxReconnectAttempts(context.mParameters.failoverOptrions.maxReconnectAttempts),
			mContainer(context.container())
		{
		}

		proton::connection_options config()
		{
			proton::connection_options co;

			if (!mClientId.empty() && !mPassword.empty())
			{
				co.user(mUser);
				co.password(mPassword);
			}

			

			if (!mClientId.empty())
				co.container_id(mClientId);

			co.sasl_allow_insecure_mechs(true);
			co.sasl_allowed_mechs("PLAIN");

			if (!mClientId.empty())
				co.container_id(mClientId);

			// no idea how to map
			//	co.idle_timeout(proton::duration(1 * 2));

				//from URL


			proton::reconnect_options rco;

			rco.failover_urls(mFailoverUrls);

			//fromURL
			rco.delay(proton::duration(mInitialReconnectDelay));
			rco.max_delay(proton::duration(mMaxReconnectDelay));
			rco.max_attempts(mMaxReconnectAttempts);


			co.reconnect(rco);
			co.desired_capabilities({ "ANONYMOUS-RELAY" });
		}

		void requestBrokerConnection(proton::messaging_handler& handler)
		{
			proton::connection_options co = config();
			co.handler(handler);
			mContainer->connect(mPrimaryUrl, handler);
		}

	//private:
		const std::string mPrimaryUrl;
		const std::vector<std::string> mFailoverUrls;
		const std::string mUser;
		const std::string mPassword;
		const std::string mClientId;
		const int mInitialReconnectDelay;
		const int mMaxReconnectDelay;
		const int mMaxReconnectAttempts;
		std::shared_ptr<proton::container> mContainer;
		std::shared_ptr<proton::connection> mConnection;
	};



	class SessionContext
	{
	public:
		SessionContext(ConnectionContext& context, bool auto_ack)
			:mConnection{ context.mConnection },
			mAuto_ack{auto_ack}
		{
		}

		bool isAutoAck() { return mAuto_ack; }

		std::shared_ptr<proton::connection> connection() const { return mConnection; }
	private:
		std::shared_ptr<proton::connection> mConnection;
		bool mAuto_ack;
	};

	class ConsumerContext
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
		ConsumerContext(SessionContext& context, bool durable, bool shared, const cms::Destination* destination)
			:mDurable_subscriber{ durable },
			mShared_subscriber{ shared },
			mDestination(destination)
		{
		}

		bool isDurable() { return mDurable_subscriber; }
		bool isShared() { return mShared_subscriber; }

		proton::receiver_options config()
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

	private:
		bool mDurable_subscriber;
		bool mShared_subscriber;
		const cms::Destination* mDestination;
		const std::string mSelector;
		const std::string mSubscriptionName;
		DestinationParser destAddressParser;
	};

	class ProducerContext
	{

	public:
		ProducerContext(SessionContext& context, const cms::Destination* destination)
			:mDestination(destination)
		{
		}

		proton::sender_options config()
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
			return opts;
		}

	private:
		const cms::Destination* mDestination;
	};

AMQP_DEFINES_CLOSE

