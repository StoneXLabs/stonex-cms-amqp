/*
 * Copyright 2022 StoneX Financial Ltd.
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
#include "URIParser_UT.h"

#include "URIParser.h"

///////////////////



TEST_F(URIParser_UT, test1) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?inputBufferSize=1234");
	EXPECT_EQ(parser.parameters.socketOptions.inputBufferSize, 1234);
}

TEST_F(URIParser_UT, test2) {

URIParser parser;
parser.parseURI("tcp:\\123.456.789?outputBufferSize=1234");
EXPECT_EQ(parser.parameters.socketOptions.outputBufferSize, 1234);

}

TEST_F(URIParser_UT, test3) {

URIParser parser;
parser.parseURI("tcp:\\123.456.789?soLinger=1234");
EXPECT_EQ(parser.parameters.socketOptions.soLinger, 1234);
}

TEST_F(URIParser_UT, test4) {

URIParser parser;
parser.parseURI("tcp:\\123.456.789?soKeepAlive=true");
EXPECT_EQ(parser.parameters.socketOptions.soKeepAlive, true);
}

TEST_F(URIParser_UT, test5) {

URIParser parser;
parser.parseURI("tcp:\\123.456.789?soReceiveBufferSize=1234");
EXPECT_EQ(parser.parameters.socketOptions.soReceiveBufferSize, 1234);
}

TEST_F(URIParser_UT, test6) {

URIParser parser;
parser.parseURI("tcp:\\123.456.789?soSendBufferSize=1234");
EXPECT_EQ(parser.parameters.socketOptions.soSendBufferSize, 1234);
}

TEST_F(URIParser_UT, test7) {

URIParser parser;
parser.parseURI("tcp:\\123.456.789?soConnectionTimeout=1234");
EXPECT_EQ(parser.parameters.socketOptions.soConnectionTimeout, 1234);
}

TEST_F(URIParser_UT, test8) {

URIParser parser;
parser.parseURI("tcp:\\123.456.789?tcpNoDelay=false");
EXPECT_EQ(parser.parameters.socketOptions.tcpNoDelay, false);

}


TEST_F(URIParser_UT, test_transport_1) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?transport.commandTracingEnabled=true");
	EXPECT_EQ(parser.parameters.transportOptions.commandTracingEnabled, true);
}

TEST_F(URIParser_UT, test_transport_2) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?transport.responseCorrelator_MaxResponseWaitTime=1234");
	EXPECT_EQ(parser.parameters.transportOptions.responseCorrelator_MaxResponseWaitTime, 1234);
}

TEST_F(URIParser_UT, test_transport_3) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?transport.tcpTracingEnabled=true");
	EXPECT_EQ(parser.parameters.transportOptions.tcpTracingEnabled, true);
}

TEST_F(URIParser_UT, test_transport_4) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?transport.useAsyncSend=true");
	EXPECT_EQ(parser.parameters.transportOptions.useAsyncSend, true);
}


TEST_F(URIParser_UT, test_failover_1) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?backup=true");
	EXPECT_EQ(parser.parameters.failoverOptrions.backup, true);
}


TEST_F(URIParser_UT, test_failover_2) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?backupPoolSize=1234");
	EXPECT_EQ(parser.parameters.failoverOptrions.backupPoolSize, 1234);
}


TEST_F(URIParser_UT, test_failover_3) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?initialReconnectDelay=1234");
	EXPECT_EQ(parser.parameters.failoverOptrions.initialReconnectDelay, 1234);
}


TEST_F(URIParser_UT, test_failover_4) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?maxReconnectDelay=1234");
	EXPECT_EQ(parser.parameters.failoverOptrions.maxReconnectDelay, 1234);
}


TEST_F(URIParser_UT, test_failover_5) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?randomize=false");
	EXPECT_EQ(parser.parameters.failoverOptrions.randomize, false);
}


TEST_F(URIParser_UT, test_failover_6) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?startupMaxReconnectAttempts=1234");
	EXPECT_EQ(parser.parameters.failoverOptrions.startupMaxReconnectAttempts, 1234);
}


TEST_F(URIParser_UT, test_failover_7) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?timeout=1234");
	EXPECT_EQ(parser.parameters.failoverOptrions.timeout, 1234);
}


TEST_F(URIParser_UT, test_failover_8) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?useExpotentialBackOff=false");
	EXPECT_EQ(parser.parameters.failoverOptrions.useExpotentialBackOff, false);
}


TEST_F(URIParser_UT, test_failover_9) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?maxReconnectAttempts=1234");
	EXPECT_EQ(parser.parameters.failoverOptrions.maxReconnectAttempts, 1234);
}


TEST_F(URIParser_UT, test_transaction_1) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?transaction.maxRedeliveryCount=1234");
	EXPECT_EQ(parser.parameters.transactionOptions.maxRedeliveryCount, 1234);
}


TEST_F(URIParser_UT, test_connection_1) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?connection.sendTimeout=1234");
	EXPECT_EQ(parser.parameters.connectionOptions.sendTimeout,1234);
}


TEST_F(URIParser_UT, test_connection_2) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?connection.producerWindowSize=1234");
	EXPECT_EQ(parser.parameters.connectionOptions.producerWindowSize,1234 );
}


TEST_F(URIParser_UT, test_connection_3) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?connection.closeTimeout=1234");
	EXPECT_EQ(parser.parameters.connectionOptions.closeTimeout,1234 );
}


TEST_F(URIParser_UT, test_connection_4) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?connection.useAsyncSend=true");
	EXPECT_EQ(parser.parameters.connectionOptions.useAsyncSend,true );
}


TEST_F(URIParser_UT, test_connection_5) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?connection.alwaysSyncSend=true");
	EXPECT_EQ(parser.parameters.connectionOptions.alwaysSyncSend, true);
}


TEST_F(URIParser_UT, test_connection_6) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?connection.useCompression=true");
	EXPECT_EQ(parser.parameters.connectionOptions.useCompression,true );
}


TEST_F(URIParser_UT, test_connection_7) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?connection.dispatchAsync=false");
	EXPECT_EQ(parser.parameters.connectionOptions.dispatchAsync,false );
}


TEST_F(URIParser_UT, test_connection_policy__prefetch_1) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Prefetch.durableTopicPrefetch=1234");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.prefetchOptons.durableTopicPrefetch,1234 );
}


TEST_F(URIParser_UT, test_connection_policy_prefetch_2) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Prefetch.queueBrowserPrefetch=1234");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.prefetchOptons.queueBrowserPrefetch,1234);
}


TEST_F(URIParser_UT, test_connection_policy_prefetch_3) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Prefetch.queuePrefetch=1234");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.prefetchOptons.queuePrefetch,1234);
}


TEST_F(URIParser_UT, test_connection_policy_prefetch_4) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Prefetch.topicPrefetch=1234");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.prefetchOptons.topicPrefetch,1234);
}


TEST_F(URIParser_UT, test_connection_policy_redelivery_1) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Redelivery.backOffMultiplier=1234");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.redeliveryOptions.backOffMultiplier, 1234);
}


TEST_F(URIParser_UT, test_connection_policy_redelivery_2) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Redelivery.collisionAvoidancePercent=89");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.redeliveryOptions.collisionAvoidancePercent, 89);
}


TEST_F(URIParser_UT, test_connection_policy_redelivery_3) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Redelivery.initialRedeliveryDelay=1234");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.redeliveryOptions.initialRedeliveryDelay, 1234);
}


TEST_F(URIParser_UT, test_connection_policy_redelivery_4) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Redelivery.maximumRedeliveries=1234");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.redeliveryOptions.maximumRedeliveries, 1234);
}


TEST_F(URIParser_UT, test_connection_policy_redelivery_5) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Redelivery.useCollisionAvoidance=true");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.redeliveryOptions.useCollisionAvoidance, true);
}


TEST_F(URIParser_UT, test_connection_policy_redelivery_6) {

	URIParser parser;
	parser.parseURI("tcp:\\123.456.789?cms.Redelivery.useExponentialBackOff=true");
	EXPECT_EQ(parser.parameters.connectionPolicyOptions.redeliveryOptions.useExponentialBackOff, true);
}

TEST_F(URIParser_UT, test_all_parameters_1) {

	URIParser parser;
	auto params = parser.parseURI("tcp:\\123.456.789?inputBufferSize=1234&outputBufferSize=1234&soLinger=1234&soKeepAlive=true&soReceiveBufferSize=1234&soSendBufferSize=1234&soConnectionTimeout=1234&tcpNoDelay=false&transport.commandTracingEnabled=true&transport.responseCorrelator_MaxResponseWaitTime=1234&transport.tcpTracingEnabled=true&transport.useAsyncSend=true&backup=true&backupPoolSize=1234&initialReconnectDelay=1234&maxReconnectDelay=1234&randomize=false&startupMaxReconnectAttempts=1234&timeout=1234&useExpotentialBackOff=false&transaction.maxRedeliveryCount=1234&connection.sendTimeout=1234&connection.producerWindowSize=1234&connection.closeTimeout=1234&connection.useAsyncSend=true&connection.alwaysSyncSend=true&connection.useCompression=true&connection.dispatchAsync=false&cms.Prefetch.durableTopicPrefetch=1234&cms.Prefetch.queueBrowserPrefetch=1234&cms.Prefetch.queuePrefetch=1234&cms.Prefetch.topicPrefetch=1234&cms.Redelivery.backOffMultiplier=1234&cms.Redelivery.collisionAvoidancePercent=89&cms.Redelivery.initialRedeliveryDelay=1234&cms.Redelivery.maximumRedeliveries=1234&cms.Redelivery.useCollisionAvoidance=true&cms.Redelivery.useExponentialBackOff=true&maxReconnectAttempts=1234");
	EXPECT_EQ(params.socketOptions.inputBufferSize, 1234);
	EXPECT_EQ(params.socketOptions.outputBufferSize, 1234);
	EXPECT_EQ(params.socketOptions.soLinger, 1234);
	EXPECT_EQ(params.socketOptions.soKeepAlive, true);
	EXPECT_EQ(params.socketOptions.soReceiveBufferSize, 1234);
	EXPECT_EQ(params.socketOptions.soSendBufferSize, 1234);
	EXPECT_EQ(params.socketOptions.soConnectionTimeout, 1234);
	EXPECT_EQ(params.socketOptions.tcpNoDelay, false);
	EXPECT_EQ(params.transportOptions.commandTracingEnabled, true);
	EXPECT_EQ(params.transportOptions.responseCorrelator_MaxResponseWaitTime, 1234);
	EXPECT_EQ(params.transportOptions.tcpTracingEnabled, true);
	EXPECT_EQ(params.transportOptions.useAsyncSend, true);
	EXPECT_EQ(params.failoverOptrions.backup, true);
	EXPECT_EQ(params.failoverOptrions.backupPoolSize, 1234);
	EXPECT_EQ(params.failoverOptrions.initialReconnectDelay, 1234);
	EXPECT_EQ(params.failoverOptrions.maxReconnectDelay, 1234);
	EXPECT_EQ(params.failoverOptrions.randomize, false);
	EXPECT_EQ(params.failoverOptrions.startupMaxReconnectAttempts, 1234);
	EXPECT_EQ(params.failoverOptrions.timeout, 1234);
	EXPECT_EQ(params.failoverOptrions.useExpotentialBackOff, false);
	EXPECT_EQ(params.transactionOptions.maxRedeliveryCount, 1234);
	EXPECT_EQ(params.connectionOptions.sendTimeout, 1234);
	EXPECT_EQ(params.connectionOptions.producerWindowSize, 1234);
	EXPECT_EQ(params.connectionOptions.closeTimeout, 1234);
	EXPECT_EQ(params.connectionOptions.useAsyncSend, true);
	EXPECT_EQ(params.connectionOptions.alwaysSyncSend, true);
	EXPECT_EQ(params.connectionOptions.useCompression, true);
	EXPECT_EQ(params.connectionOptions.dispatchAsync, false);
	EXPECT_EQ(params.connectionPolicyOptions.prefetchOptons.durableTopicPrefetch, 1234);
	EXPECT_EQ(params.connectionPolicyOptions.prefetchOptons.queueBrowserPrefetch, 1234);
	EXPECT_EQ(params.connectionPolicyOptions.prefetchOptons.queuePrefetch, 1234);
	EXPECT_EQ(params.connectionPolicyOptions.prefetchOptons.topicPrefetch, 1234);
	EXPECT_EQ(params.connectionPolicyOptions.redeliveryOptions.backOffMultiplier, 1234);
	EXPECT_EQ(params.connectionPolicyOptions.redeliveryOptions.collisionAvoidancePercent, 89);
	EXPECT_EQ(params.connectionPolicyOptions.redeliveryOptions.initialRedeliveryDelay, 1234);
	EXPECT_EQ(params.connectionPolicyOptions.redeliveryOptions.maximumRedeliveries, 1234);
	EXPECT_EQ(params.connectionPolicyOptions.redeliveryOptions.useCollisionAvoidance, true);
	EXPECT_EQ(params.connectionPolicyOptions.redeliveryOptions.useExponentialBackOff, true);

	EXPECT_EQ(parser.parameters.failoverOptrions.maxReconnectAttempts, 1234);

}
