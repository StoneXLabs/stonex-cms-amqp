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
#include "MessageDefaultProperties_UT.h"
#include "cms/MessageFormatException.h"
#include "CMSMessage.h"
#include "CMSBytesMessage.h"
#include "CMSTextMessage.h"


#include <chrono>
#include <thread>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/receiver.hpp>
#include <proton/source.hpp>
#include <proton/annotation_key.hpp>

#include <gtest/gtest-param-test.h>

using namespace cms::amqp;

INSTANTIATE_TEST_SUITE_P(Message_testing, MessageDefaultProperties_UT, ::testing::Values(new CMSMessage(), new CMSTextMessage("message body"), new CMSBytesMessage()));

MessageDefaultProperties_UT::MessageDefaultProperties_UT()
{

}

void MessageDefaultProperties_UT::SetUp()
{
	mUUT = GetParam();
}

void MessageDefaultProperties_UT::TearDownUp()
{
	delete mUUT;
}

TEST_P(MessageDefaultProperties_UT, Test_default_reply_to_null)
{

	EXPECT_EQ(GetParam()->getCMSReplyTo(), nullptr);
}

TEST_P(MessageDefaultProperties_UT, Test_default_jms_type)
{

	EXPECT_EQ(GetParam()->getCMSType(), "");
}

TEST_P(MessageDefaultProperties_UT, Test_default_delivery_mode)
{

	EXPECT_EQ(GetParam()->getCMSDeliveryMode(), cms::DeliveryMode::PERSISTENT);
}

TEST_P(MessageDefaultProperties_UT, Test_default_jms_priority)
{

	EXPECT_EQ(GetParam()->getCMSPriority(), 4);
}

TEST_P(MessageDefaultProperties_UT, Test_default_message_id)
{
	//MessageId is set on producer send(cms::message)
	//constructed messageId is empty by default
	EXPECT_EQ(GetParam()->getCMSMessageID().empty(), true);
}

TEST_P(MessageDefaultProperties_UT, Test_default_correlation_id_empty)
{

	EXPECT_TRUE(GetParam()->getCMSCorrelationID().empty());
}

TEST_P(MessageDefaultProperties_UT, Test_default_jms_expiration)
{

	EXPECT_EQ(GetParam()->getCMSExpiration(), 0);
}

TEST_P(MessageDefaultProperties_UT, Test_default_jms_redelivered)
{

	EXPECT_EQ(GetParam()->getCMSRedelivered(), false);
}