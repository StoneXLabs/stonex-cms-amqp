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
#include "stonex-cms-amqp-lib-defines.h"

class CMS_API URIParser
{
public:
	class CMS_API Parameters
	{
		class Socket
		{
		public:
	
			int inputBufferSize{ 10000 };
			int outputBufferSize{ 10000 };
			int soLinger{ 0 };
			bool soKeepAlive{ false };
			int soReceiveBufferSize{ -1 };
			int soSendBufferSize{ -1 };
			int soConnectionTimeout{ -1 };
			bool tcpNoDelay{ true };
	
		};
		class TransportOptions {
		public:
	//		std::string optionPrefix{ "transport" };
			bool commandTracingEnabled{ false };
			bool tcpTracingEnabled{ false };
			bool useAsyncSend{ false };
			int responseCorrelator_MaxResponseWaitTime{ 3000 };
	
		};
		class Failover
	{
	public:
		int initialReconnectDelay{ 10 }; //proton::reconnect_options::delay
		int maxReconnectDelay{ 30000 };//proton::reconnect_options::max_delay
		bool useExpotentialBackOff{ true };  //NOT SUPPORTED
		int maxReconnectAttempts{ 0 }; //proton::reconnect_options::max_attempts
		int startupMaxReconnectAttempts{ 0 }; //NOT SUPPORTED
		bool randomize{ true };
		bool backup{ false };
		int backupPoolSize{ 1 };
		int timeout{ -1 };
	};
		class TransactionOptions
		{
		public:
	//		const std::string optionPrefix{ "transaction" };
			int maxRedeliveryCount{ 5 };
		};
		class Connection
		{
		public:
	//		const std::string optionPrefix{ "connection" };
			int sendTimeout{ 0 }; //NOT SUPPORTED
			int producerWindowSize{ 0 };
			int closeTimeout{ 15000 }; //NOT SUPPORTED
			bool useAsyncSend{ false };
			bool alwaysSyncSend{ false };
			bool useCompression{ false };
			bool dispatchAsync{ true };
		};
		class ConnectionPolicy
		{
			class Prefetch
			{
			public:
		//		const std::string optionPrefix{ "Prefetch" };
				int durableTopicPrefetch{ 100 };
				int queueBrowserPrefetch{ 500 };
				int queuePrefetch{ 1000 };
				int topicPrefetch{ 65535 };
	
			};
			class Redelivery
			{
			public:
//				const std::string optionPrefix{ "Redelivery" };
				float backOffMultiplier{ 5.0 };
				int collisionAvoidancePercent{ 15 };
				int initialRedeliveryDelay{ 1000 };
				int maximumRedeliveries{ 6 };
				bool useCollisionAvoidance{ false };
				bool useExponentialBackOff{ false };
	
			};
	
		public:
//			const std::string optionPrefix{ "cms" };
			Prefetch prefetchOptons;
			Redelivery redeliveryOptions;
			
			//wireformat not implemented
		};
	public:
		Parameters() = default;
		std::vector<std::string> url_vec;
		Socket socketOptions;
		TransportOptions transportOptions;
		Failover failoverOptrions;
		TransactionOptions transactionOptions;
		Connection connectionOptions;
		ConnectionPolicy connectionPolicyOptions;
	};
public:

	Parameters parseURI(const std::string& url);
	
	Parameters parameters;

private:
	std::string getOptionsString(const std::string& uri, bool compound);
	std::string getOptionsString(const std::string::const_iterator& uri_begin, const std::string::const_iterator& uri_end, bool compound);
	void updateOption(const std::string& option);
	void updateOption(const std::string& option_group,const std::string& option);



};

