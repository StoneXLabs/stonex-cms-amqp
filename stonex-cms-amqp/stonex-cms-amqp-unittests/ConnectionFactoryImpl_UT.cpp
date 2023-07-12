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
#include "ConnectionFactoryImpl_UT.h"

#include "../API/CMSConnectionFactory.h"

#include "MOCK/ExceptionListenerMock.h"
#include "MOCK/MessageTransformerMock.h"

using namespace cms::amqp;

void ConnectionFactoryImpl_UT::SetUp()
{
	mUUT = CMSConnectionFactory::createCMSConnectionFactory("localhost:5675?maxReconnectAttempts=3");
}

void ConnectionFactoryImpl_UT::TearDown()
{
	delete mUUT;
	mUUT = nullptr;
}


TEST_F(ConnectionFactoryImpl_UT, test_default_exception_listener) {

	EXPECT_EQ(mUUT->getExceptionListener(), nullptr);
}

TEST_F(ConnectionFactoryImpl_UT, test_default_message_transformer) {

	EXPECT_EQ(mUUT->getMessageTransformer(), nullptr);
}

TEST_F(ConnectionFactoryImpl_UT, test_set_exception_listener) {

	ExceptionListenerMock mock;
	mUUT->setExceptionListener(&mock);
	EXPECT_NE(mUUT->getExceptionListener(), nullptr);
}

//TEST_F(ConnectionFactoryImpl_UT, test_set_message_transformer) {
//	MessageTransformerMock mock;
//	mUUT->setMessageTransformer(&mock);
//	EXPECT_NE(mUUT->getMessageTransformer(), nullptr);
//}
//
//TEST_F(ConnectionFactoryImpl_UT, test_set_message_transformer) {
//
//	ExceptionListenerMock mock;
//	mUUT->setExceptionListener(&mock);
//	mUUT->createConnection("user", "password");
//	EXPECT_NE(mUUT->getMessageTransformer(), nullptr);
//}


TEST_F(ConnectionFactoryImpl_UT, test_exception_thrown_on_connection_to_non_existent_broker) {

	TearDown();
	mUUT = CMSConnectionFactory::createCMSConnectionFactory("dummy:5675?maxReconnectAttempts=3");
	ExceptionListenerMock mock;
	mUUT->setExceptionListener(&mock);


	EXPECT_CALL(mock, onException(testing::_));

	try
	{
		auto conn1 = mUUT->createConnection();
		delete conn1;
	}
	catch (const cms::CMSException &ex)
	{

	}


	
}