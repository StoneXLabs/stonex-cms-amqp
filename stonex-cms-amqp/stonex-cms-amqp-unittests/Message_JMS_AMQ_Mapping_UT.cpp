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
#include "Message_JMS_AMQ_Mapping_UT.h"

#include <chrono>
#include <thread>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/receiver.hpp>
#include <proton/source.hpp>
#include <proton/annotation_key.hpp>

#include "CMSMessage.h"
#include "..\API\CMSQueue.h"
#include "..\API\CMSTopic.h"
#include "..\API\CMSTemporaryQueue.h"
//#include "..\API\CMSTemporaryTopic.h"
#include <AMQPCMSMessageConverter.h>

using namespace cms::amqp;



/// 
/// 3.2.1 p.10 table 2
/// <AMQPMessageID -> JMSMessageId >
/// 
/// 3 cases of 3
/// 
/// brief:
/// set MessageIdID using proton::message->id(message_id), verify if correlationID is set properly using CMSMessage metghod getCMSCorrelationId()
/// 
	/// <summary>
	/// setting AMQMessageID on proton::message object that is passed to create CMSMessage object.
	/// AMQMessageID of proton::message is set to tested value [testData]
	/// CorrelationID value is verified using getCMSCorrelationId(). 
	/// 
	/// getCMSMessageID validation rules:
	/// 
	/// 1. AMQP message-id string type with ID: prefix AND without AMQP_<type>: prefix should be converted to string by getCMSMessageID
	/// 
	///		ex. proton::message->id("ID:my-string-id") -> getCMSMessageID() returns "ID:my-string-id"
	/// 
	/// 2. AMQP message-id string type without AMQP_<type>: and ID: prefix should be converted to string by getCMSMessageID without changes
	/// 
	///		ex. proton::message->id("non-prefixed-string-id") -> getCMSMessageID() returns "ID:AMQP_NO_PREFIX:non-prefixed-string-id"
	/// 
	/// 3. AMQP message-id non string types
	/// 
	/// 
	/// 3.1 AMPQ message-id UUID type should be converted to string by getCMSMessageID() adding prefix "ID:AMQP_UUID:" to string representation of UUID
	///		UUID pattern:
	///			binary: 32 Bytes
	///			string: 11111111-2222-3333-4444-555555555555
	/// 
	/// 
	///		ex.  proton::message->id(uuid(11111111-2222-3333-4444-555555555555)) -> getCMSMessageID() returns "ID:AMQP_UUID:11111111-2222-3333-4444-555555555555"
	/// 3.2 AMPQ message-id ulong type should be converted to string by getCMSMessageID() adding prefix "ID:AMQP_ULONG:" to string representation of ulong
	/// 
	///		ex.  proton::message->id(1234567) -> getCMSMessageID() returns "ID:AMQP_ULONG:1234567"
	/// 
	/// 3.3 AMPQ message-id binary type should be converted to string by getCMSMessageID() adding prefix "ID:AMQP_BINARY:" to string hex string of binary
	/// 
	///		ex.  proton::message->id(0x1a2b3c4d) -> getCMSMessageID() returns "ID:AMQP_BINARY:1A2B3C4D"
	/// 
	/// 4. AMQP message-id string type with ID: prefix AND with AMQP_<type> should be converted to string by getCMSMessageID() adding prefix "ID:AMQP_STRING:" to correlation-id string
	/// 
	///		ex.  proton::message->id(ID:AMQP_ULONG:string-id) -> getCMSMessageID() returns "ID:AMQP_STRING:ID:AMQP_ULONG:string-id"
	/// 
	/// 
	/// 
	/// 
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>
	/// 


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_messageID_String)
{
	const std::string testData{ "ID:message-id" };

	auto message = std::make_shared<proton::message>();
	message->id(testData);

	CMSMessage* testMsg = new CMSMessage(message.get());

	EXPECT_EQ(testMsg->getCMSMessageID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_non_prefixed_messageID_String)
{
	const std::string testData{ "non-prefixed-string-id" };

	auto message = std::make_shared<proton::message>();
	message->id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSMessageID(), "ID:AMQP_NO_PREFIX:" + testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_messageID_UUID)
{
	const proton::uuid testData = proton::uuid::random();

	auto message = std::make_shared<proton::message>();
	message->id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSMessageID(), "ID:AMQP_UUID:" + testData.str());
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_messageID_Ulong)
{
	const unsigned long testData{ 123456 };

	auto message = std::make_shared<proton::message>();
	message->id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSMessageID(), "ID:AMQP_ULONG:" + std::to_string(testData));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_messageID_Byte)
{

	std::string binary_corrid = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f };
	const proton::binary testData(binary_corrid);
	auto size = testData.size();

	auto message = std::make_shared<proton::message>();
	message->id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSMessageID(), "ID:AMQP_BINARY:0102030405060708090A0B0C0D0E0F1011121314151718191A1B1C1D1E1F");
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_amqp_type_string_messageID_string)
{
	const std::string testData{ "ID:AMQP_ULONG:string-id" };

	auto message = std::make_shared<proton::message>();
	message->id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSMessageID(), "ID:AMQP_STRING:" + testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_amqp_type_string_MessageID_string_type_prefix_only)
{

//	GTEST_FATAL_FAILURE_("functionality not implemented, verify documentation and handle case");


	////ex.  proton::message->correlation_id(AMQP_ULONG:foo) -> getCMSCorrelationID() returns "ID:AMQP_ULONG:foo"

	const std::string testData{ "AMQP_ULONG:foo" };

	auto message = std::make_shared<proton::message>();
	message->correlation_id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSCorrelationID(), testData);
}

/// </AMQPMessageID -> JMSMessageId >


/// 
/// 3.2.1 p.10 table 2
/// <JMSMessageId -> AMQPMessageID -> JMSMessageId >
/// 

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_ID_only_prefixed_JSM_messageID_string)
{

	const std::string testData{ "ID:my-string-id" };

	CMSMessage* testMsg = new CMSMessage;
	testMsg->setCMSMessageID(testData);

	EXPECT_EQ(testMsg->getCMSMessageID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_non_prefixed_JSM_messageID_string)
{

	const std::string testData{ "non-prefixed-string-id" };

	CMSMessage* testMsg = new CMSMessage;
	testMsg->setCMSMessageID(testData);

	EXPECT_EQ(testMsg->getCMSMessageID(), "ID:AMQP_NO_PREFIX:" + testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_ID_binary_prefixed_JSM_messageID_string)
{

	const std::string testData{ "ID:AMQP_BINARY:01ABCDEF0F3E" };

	CMSMessage* testMsg = new CMSMessage;
	testMsg->setCMSMessageID(testData);

	EXPECT_EQ(testMsg->getCMSMessageID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_ID_uuid_prefixed_JSM_messageID_string)
{

	std::string testData{ "ID:AMQP_UUID:" };
	testData.append(proton::uuid::random().str());

	CMSMessage* testMsg = new CMSMessage;
	testMsg->setCMSMessageID(testData);

	EXPECT_EQ(testMsg->getCMSMessageID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_ID_ulong_prefixed_JSM_messageID_string)
{

	std::string testData{ "ID:AMQP_ULONG:12345678" };

	CMSMessage* testMsg = new CMSMessage;
	testMsg->setCMSMessageID(testData);

	EXPECT_EQ(testMsg->getCMSMessageID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_ID_no_prefix_JSM_messageID_string)
{

	std::string testData{ "ID:AMQP_NO_PREFIX:no-prefix-corrid" };

	CMSMessage* testMsg = new CMSMessage;
	testMsg->setCMSMessageID(testData);

	EXPECT_EQ(testMsg->getCMSMessageID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_ID_string_prefix_JSM_messageID_string)
{

	std::string testData{ "ID:AMQP_STRING:ID:AMQP_ULONG:string-prefix-corrid" };

	CMSMessage* testMsg = new CMSMessage;
	testMsg->setCMSMessageID(testData);

	EXPECT_EQ(testMsg->getCMSMessageID(), testData);
}
/// </JMSMessageId -> AMQPMessageID -> JMSMessageId >

//conversion error tests

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_messageID_UUID_set_get_conversion_exception)
{

	std::string testData("ID:AMQP_UUID:");
	testData += "123456"; //non uuid



	CMSMessage testMsg;

	EXPECT_ANY_THROW(testMsg.setCMSMessageID(testData));

}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_messageID_Ulong_set_get_conversion_exception)
{
	std::string testData("ID:AMQP_ULONG:corrid");

	CMSMessage testMsg;

	EXPECT_ANY_THROW(testMsg.setCMSMessageID(testData));

}


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_messageID_Byte_set_get_conversion_exception)
{
	std::string testData("ID:AMQP_BINARY:nonhexstring");

	CMSMessage testMsg;

	EXPECT_ANY_THROW(testMsg.setCMSMessageID(testData));
}


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_messageID_ulong_with_leading_zeros_set_get_conversion_exception)
{
	std::string testData("ID:AMQP_BINARY:nonhexstring");

	CMSMessage testMsg;

	EXPECT_ANY_THROW(testMsg.setCMSMessageID(testData));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_messageID__Byte_with_odd_number_of_chars_in_representing_string_set_get_conversion_exception)
{
	std::string testData("ID:AMQP_BINARY:ab13f");

	CMSMessage testMsg;

	EXPECT_ANY_THROW(testMsg.setCMSMessageID(testData));
}


/// 
/// 3.2.1 p.11 table 1
/// <JMSCorrelationId -> AMQPCorrelationId>
/// 
/// 3 cases of 3
/// 
/// brief:
/// set CorrelationID using CMSMessage::setCMSCorrelationID(string), verify if correlationID is set properly using CMSMessage metghod getCMSCorrelationId()
/// 

	/// <summary>
	/// setting JMSCorrelationId with value non containing "ID:" prefix.
	/// AMQPCorrelationID of proton::message is set to the same value. 
	/// CorrelationID value is verified using getCMSCorrelationId(). 
	/// 
	/// getCMSCorrelationID validation rules:
	/// 1. CMSCorrelationID is set to value  without AMQP_<type>: or ID: prefix, AMQP correlation-id should be converted by getCMSCorrelationID to string type without changes
	///		ex. setCMSCorrelationID("application-specific") -> getCMSCorrelationID() returns "application-specific"
	/// 2. CMSCorrelationID is set to value without ID: prefix AND with AMQP_<type>: prefix, AMQP correlation-id  should be converted to string type without changes and 
	/// returned by getCMSCorrelationID without changes even if value can't be converted to AMQP_TYPE. just like "application_specific" string above
	///		ex. setCMSCorrelationID("AMQP_ULONG:42") -> getCMSCorrelationID() returns "AMQP_ULONG:42"
	///		ex. setCMSCorrelationID("AMQP_ULONG:foo") -> getCMSCorrelationID() returns "AMQP_ULONG:foo"
	/// 
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_non_ID_prefixed_JSM_correlationID_application_specific_data)
{

	const std::string testData{ "application-specific" };

	CMSMessage* testMsg = new CMSMessage;
	testMsg->setCMSCorrelationID(testData);

	EXPECT_EQ(testMsg->getCMSCorrelationID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_non_ID_prefixed_JMS_correlationID_AMQP_prefixed_numeric_data)
{
	const std::string testData{ "AMQP_ULONG:42" };

	CMSMessage testMsg;
	testMsg.setCMSCorrelationID(testData);

	EXPECT_EQ(testMsg.getCMSCorrelationID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_non_ID_prefixed_JMS_correlationID_AMQP_prefixed_non_numeric_data)
{
	const std::string testData{ "AMQP_ULONG:foo" };

	CMSMessage testMsg;
	testMsg.setCMSCorrelationID(testData);

	EXPECT_EQ(testMsg.getCMSCorrelationID(), testData);
}

///  </JMSCorrelationId -> AMQPCorrelationId>

/// 
/// 3.2.1 p.11 table 2
/// <AMQPCorrelationId -> JMSCorrelationId>
/// 
/// 7 cases of 7
/// 
/// :brief
/// set correltaion id on proton::message object that is part of CMSMessage, verify conversion between AMQP correlation-id and CMSCorrelationID
/// 

	/// <summary>
	/// setting AMQPCorrelationId on proton::message object that is passed to create CMSMessage object.
	/// AMQPCorrelationID of proton::message is set to tested value [testData]
	/// CorrelationID value is verified using getCMSCorrelationId(). 
	/// 
	/// getCMSCorrelationID validation rules:
	/// 
	/// 1. AMQP correlation-id string type with ID: prefix AND without AMQP_<type>: prefix should be converted to string by getCMSCorrelationID
	/// 
	///		ex. proton::message->correlation_id("ID:my-string-id") -> getCMSCorrelationID() returns "ID:my-string-id"
	/// 
	/// 2. AMQP correlation-id string type without AMQP_<type>: and ID: prefix should be converted to string by getCMSCorrelationID without changes
	/// 
	///		ex. proton::message->correlation_id("application-specific") -> getCMSCorrelationID() returns "application-specific"
	/// 
	/// 3. AMQP correlation-id non string types
	/// 
	/// 3.1 AMPQ correlation-id UUID type should be converted to string by getCMSCorrelationID() adding prefix "ID::AMQP_UUID:" to string representation of UUID
	///		UUID pattern:
	///			binary: 32 Bytes
	///			string: 11111111-2222-3333-4444-555555555555
	/// 
	///		ex.  proton::message->correlation_id(uuid(11111111-2222-3333-4444-555555555555)) -> getCMSCorrelationID() returns "ID:AMQP_UUID:11111111-2222-3333-4444-555555555555"
	/// 
	/// 3.2 AMPQ correlation-id ulong type should be converted to string by getCMSCorrelationID() adding prefix "ID:AMQP_ULONG:" to string representation of ulong
	/// 
	///		ex.  proton::message->correlation_id(1234567) -> getCMSCorrelationID() returns "ID:AMQP_ULONG:1234567"
	/// 
	/// 3.3 AMPQ correlation-id binary type should be converted to string by getCMSCorrelationID() adding prefix "ID:AMQP_BINARY:" to string hex string of binary
	/// 
	///		ex.  proton::message->correlation_id(0x1a2b3c4d) -> getCMSCorrelationID() returns "ID:AMQP_BINARY:1A2B3C4D"
	/// 
	/// 4. AMQP correlation-id string type with ID: prefix AND with AMQP_<type> should be converted to string by getCMSCorrelationID() adding prefix "ID:AMQP_STRING:" to correlation-id string
	/// 
	///		ex.  proton::message->correlation_id(ID:AMQP_ULONG:string-id) -> getCMSCorrelationID() returns "ID:AMQP_STRING:ID:AMQP_ULONG:string-id"
	/// 
	/// 5. AMQP correlation-id string type without ID: prefix AND with AMQP_<type> should be converted to string by getCMSCorrelationID() adding prefix "ID:AMQP_STRING:" to correlation-id string
	/// 
	///		ex.  proton::message->correlation_id(AMQP_ULONG:foo) -> getCMSCorrelationID() returns "AMQP_ULONG:foo"
	/// 
	/// 
	/// 
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_correlationID_String)
{
	//ex. proton::message->correlation_id("ID:my-string-id") -> getCMSCorrelationID() returns "ID:my-string-id"

	const std::string testData{ "ID:string-corrID" };

	auto message = std::make_shared<proton::message>();
	message->correlation_id(testData);

	CMSMessage* testMsg = new CMSMessage(message.get());

	EXPECT_EQ(testMsg->getCMSCorrelationID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_non_prefixed_correlationID_String)
{
	//ex. proton::message->correlation_id("application-specific") -> getCMSCorrelationID() returns "application-specific"

	const std::string testData{ "application-specific" };

	auto message = std::make_shared<proton::message>();
	message->correlation_id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSCorrelationID(),testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_correlationID_UUID)
{
	//proton::message->correlation_id(uuid(11111111-2222-3333-4444-555555555555))->getCMSCorrelationID() returns "ID:AMQP_UUID:11111111-2222-3333-4444-555555555555"

	const proton::uuid testData = proton::uuid::random();

	auto message = std::make_shared<proton::message>();
	message->correlation_id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSCorrelationID(), "ID:AMQP_UUID:" + testData.str());
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_correlationID_Ulong)
{
	//ex.  proton::message->correlation_id(1234567) -> getCMSCorrelationID() returns "ID:AMQP_ULONG:1234567"

	const unsigned long testData{ 123456 };

	auto message = std::make_shared<proton::message>();
	message->correlation_id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSCorrelationID(), "ID:AMQP_ULONG:" + std::to_string(testData));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_correlationID_Byte)
{
	//ex.  proton::message->correlation_id( 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f)
	//-> getCMSCorrelationID() returns "ID:AMQP_BINARY:0102030405060708090A0B0C0D0E0F1011121314151718191A1B1C1D1E1F"

	std::string binary_corrid = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f };
	const proton::binary testData(binary_corrid);
	auto size = testData.size();

	auto message = std::make_shared<proton::message>();
	message->correlation_id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSCorrelationID(), "ID:AMQP_BINARY:0102030405060708090A0B0C0D0E0F1011121314151718191A1B1C1D1E1F");
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_amqp_type_string_correlationID_string)
{
	//ex.  proton::message->correlation_id(ID:AMQP_ULONG:123456) -> getCMSCorrelationID() returns "ID:AMQP_STRING:ID:AMQP_ULONG:123456"

	const std::string testData{ "ID:AMQP_ULONG:123456" };

	auto message = std::make_shared<proton::message>();
	message->correlation_id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSCorrelationID(), "ID:AMQP_AMQP_STRING:" + testData);
}


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_amqp_type_string_correlationID_string_wrong_type)
{
	//ex.  proton::message->correlation_id(AMQP_ULONG:foo) -> getCMSCorrelationID() returns "ID:AMQP_ULONG:foo"

	const std::string testData{ "AMQP_ULONG:foo" };

	auto message = std::make_shared<proton::message>();
	message->correlation_id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSCorrelationID(), testData);
}

/// </AMQPCorrelationId -> JMSCorrelationId>

///  <JMSCorrelationId -> AMQPCorrelationId -> JMSCorrelationId>


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_correlationID_AMQP_string_id_data)
{
	const std::string testData{ "ID:my-string-id" };

	CMSMessage testMsg;
	testMsg.setCMSCorrelationID(testData);

	EXPECT_EQ(testMsg.getCMSCorrelationID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_correlationID_AMQP_non_prefixed_string_id_data)
{
	const std::string testData{ "non-prefixed-string-id" };

	CMSMessage testMsg;
	testMsg.setCMSCorrelationID(testData);

	EXPECT_EQ(testMsg.getCMSCorrelationID(), testData);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_correlationID_UUID_set_get)
{
	auto uuid = proton::uuid::random();

	std::string testData("ID:AMQP_UUID:");
	testData += uuid.str();



	CMSMessage testMsg;
	testMsg.setCMSCorrelationID(testData);

	EXPECT_EQ(testMsg.getCMSCorrelationID(), testData);

}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_correlationID_Ulong_set_get)
{
	std::string testData("ID:AMQP_ULONG:");

	testData.append(std::to_string((unsigned long)123456));

	CMSMessage testMsg;
	testMsg.setCMSCorrelationID(testData);

	EXPECT_EQ(testMsg.getCMSCorrelationID(), testData);

}


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_correlationID_Byte_set_get)
{

	std::string binary_corrid = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f };
	const proton::binary testData(binary_corrid);
	auto size = testData.size();

	auto message = std::make_shared<proton::message>();
	message->correlation_id(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSCorrelationID(), "ID:AMQP_BINARY:0102030405060708090A0B0C0D0E0F1011121314151718191A1B1C1D1E1F");

}

// whats that ???
TEST_F(Message_JMS_AMQ_Mapping_UT, Test_correlationID_JMSString_STRING_to_AMQP)
{
	const std::string JMSCorrelationId{ "ID:AMQP_STRING:string-correlation-id" };
	CMSMessage testMsg;
	testMsg.setCMSCorrelationID(JMSCorrelationId);
}

//conversion error tests

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_correlationID_UUID_set_get_conversion_exception)
{

	std::string testData("ID:AMQP_UUID:");
	testData += "123456"; //non uuid



	CMSMessage testMsg;

	EXPECT_ANY_THROW(testMsg.setCMSCorrelationID(testData));

}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_correlationID_Ulong_set_get_conversion_exception)
{
	std::string testData("ID:AMQP_ULONG:corrid");

	CMSMessage testMsg;

	EXPECT_ANY_THROW(testMsg.setCMSCorrelationID(testData));

}


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_correlationID_Byte_set_get_conversion_exception)
{
	std::string testData("ID:AMQP_BINARY:nonhexstring");

	CMSMessage testMsg;

	EXPECT_ANY_THROW(testMsg.setCMSCorrelationID(testData));
}

// Delivery Mode


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_PERSISTENT_DeliveryMode_from_AMQP_message_with_durable_set_true)
{
	std::string testData("ID:AMQP_BINARY:nonhexstring");

	auto message = std::make_shared<proton::message>();
	message->durable(true);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSDeliveryMode(),cms::DeliveryMode::PERSISTENT);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_PERSISTENT_DeliveryMode_from_AMQP_message_with_durable_set_false)
{
	auto message = std::make_shared<proton::message>();
	message->durable(false);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSDeliveryMode(), cms::DeliveryMode::NON_PERSISTENT);
}


TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_set_DeliveryMode_PERSISTENT)
{


	CMSMessage testMsg;
	testMsg.setCMSDeliveryMode(cms::DeliveryMode::PERSISTENT);

	EXPECT_EQ(testMsg.getCMSDeliveryMode(), cms::DeliveryMode::PERSISTENT);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_set_DeliveryMode_NON_PERSISTENT)
{


	CMSMessage testMsg;
	testMsg.setCMSDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);

	EXPECT_EQ(testMsg.getCMSDeliveryMode(), cms::DeliveryMode::NON_PERSISTENT);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_default_DeliveryMode_PERSISTENT)
{


	CMSMessage testMsg;
	testMsg.setCMSDeliveryMode(cms::DeliveryMode::PERSISTENT);

	EXPECT_EQ(testMsg.getCMSDeliveryMode(), cms::DeliveryMode::PERSISTENT);
}
// Timestamp

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_timestamp_set_get)
{


	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	//delay to check if creation time is set by set method or in creation om msg;
	std::this_thread::sleep_for(std::chrono::seconds(2));

	CMSMessage testMsg;
	testMsg.setCMSTimestamp(time);

	EXPECT_EQ(time, testMsg.getCMSTimestamp());
}

///destination
TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_from_AMQP_message_without_address)
{

	const uint8_t testCapabilityData{ 0 };
	//const std::string testCapabilityData = "queue";

	auto message = std::make_shared<proton::message>();
	message->message_annotations().put("x-opt-jms-dest", testCapabilityData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSDestination(),nullptr);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_from_AMQP_message_with_address_without_capabilities)
{
	const std::string testData = "test_address";

	auto message = std::make_shared<proton::message>();
	message->to(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSDestination(), nullptr);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_from_AMQP_message_without_address_and_capabilities)
{
	auto message = std::make_shared<proton::message>();

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSDestination(), nullptr);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_destination_Queue_from_AMQP_message)
{
	const std::string testData = "test_address";
	const uint8_t testCapabilityData{0};
	//const std::string testCapabilityData = "queue";
	const CMSQueue compareDestination(testData);

	auto message = std::make_shared<proton::message>();
	message->to(testData);
	message->message_annotations().put("x-opt-jms-dest", testCapabilityData);

	CMSMessage testMsg(message.get());

	GTEST_ASSERT_NE(testMsg.getCMSDestination(), nullptr);
	EXPECT_TRUE(testMsg.getCMSDestination()->equals(compareDestination));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_destination_from_AMQP_message_with_incorrect_capability)
{
	const std::string testData = "test_address";
	const uint8_t testCapabilityData { 5 };
	//const std::string testCapabilityData = "oncorrect_capability";
	const CMSQueue compareDestination(testData);

	auto message = std::make_shared<proton::message>();
	message->to(testData);
	message->message_annotations().put("x-opt-jms-dest", testCapabilityData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSDestination(), nullptr);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_destination_Topic_from_AMQP_message)
{
	const std::string testData = "test_address";
	const uint8_t testCapabilityData{ 1 };
	//const std::string testCapabilityData = "topic";
	const CMSTopic compareDestination(testData);

	auto message = std::make_shared<proton::message>();
	message->to(testData);
	message->message_annotations().put("x-opt-jms-dest", testCapabilityData);

	CMSMessage testMsg(message.get());

	GTEST_ASSERT_NE(testMsg.getCMSDestination(), nullptr);
	EXPECT_TRUE(testMsg.getCMSDestination()->equals(compareDestination));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_CMS_Queue_equality_identical_queues)
{
	const std::string testData{ "test_address" };

	CMSQueue item1(testData);
	CMSQueue item2(testData);

	EXPECT_TRUE(item1.equals(item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_CMS_Topic_equality_identical_topics)
{
	const std::string testData{ "test_address" };

	CMSTopic item1(testData);
	CMSTopic item2(testData);

	EXPECT_TRUE(item1.equals(item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_CMS_Queue_equality_non_identical_queues)
{
	const std::string testData{ "test_address" };

	CMSQueue item1(testData);
	CMSQueue item2("other_address");

	EXPECT_FALSE(item1.equals(item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_CMS_Topic_equality_non_identical_topics)
{
	const std::string testData{ "test_address" };

	CMSTopic item1(testData);
	CMSTopic item2("other_address");

	EXPECT_FALSE(item1.equals(item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_CMS_compare_queue_to_topic_with_identical_address)
{
	const std::string testData{ "test_address" };

	CMSQueue item1(testData);
	CMSTopic item2(testData);

	EXPECT_FALSE(item1.equals(item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_CMS_compare_topic_to_queue_with_identical_address)
{
	const std::string testData{ "test_address" };

	CMSTopic item1(testData);
	CMSQueue item2(testData);

	EXPECT_FALSE(item1.equals(item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_CMS_compare_queue_to_topic_with_non_identical_address)
{
	const std::string testData{ "test_address" };

	CMSQueue item1(testData);
	CMSTopic item2("other_address");;

	EXPECT_FALSE(item1.equals(item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_CMS_compare_topic_to_queue_with_non_identical_address)
{
	const std::string testData{ "test_address" };

	CMSTopic item1(testData);
	CMSQueue item2("other_address");;

	EXPECT_FALSE(item1.equals(item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_destination_to_Queue)
{
	const std::string testData{ "test_address" };


	auto item1 = new CMSQueue(testData);

	CMSMessage testMsg;
	testMsg.setCMSDestination(item1);

	GTEST_ASSERT_NE(testMsg.getCMSDestination(), nullptr);
	EXPECT_TRUE(testMsg.getCMSDestination()->equals(*item1));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_destination_to_Topic)
{
	const std::string testData{ "test_address" };


	auto item1 = new CMSTopic(testData);

	CMSMessage testMsg;
	testMsg.setCMSDestination(item1);

	GTEST_ASSERT_NE(testMsg.getCMSDestination(), nullptr);
	EXPECT_TRUE(testMsg.getCMSDestination()->equals(*item1));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_destination_to_Queue_and_compare_with_diferent_address)
{
	const std::string testData{ "test_address" };
	const std::string compareData{ "compared_address" };


	auto item1 = new CMSQueue(testData);
	auto item2 = new CMSQueue(compareData);

	CMSMessage testMsg;
	testMsg.setCMSDestination(item1);

	GTEST_ASSERT_NE(testMsg.getCMSDestination(), nullptr);
	EXPECT_FALSE(testMsg.getCMSDestination()->equals(*item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_destination_to_Queue_and_compare_with_Topic_type_and_same_address)
{
	const std::string testData{ "test_address" };


	auto item1 = new CMSQueue(testData);
	auto item2 = new CMSTopic(testData);

	CMSMessage testMsg;
	testMsg.setCMSDestination(item1);

	GTEST_ASSERT_NE(testMsg.getCMSDestination(), nullptr);
	EXPECT_FALSE(testMsg.getCMSDestination()->equals(*item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_destination_to_Topic_and_compare_with_Queue_type_and_same_address)
{
	const std::string testData{ "test_address" };


	auto item1 = new CMSTopic(testData);
	auto item2 = new CMSQueue(testData);

	CMSMessage testMsg;
	testMsg.setCMSDestination(item1);

	GTEST_ASSERT_NE(testMsg.getCMSDestination(), nullptr);
	EXPECT_FALSE(testMsg.getCMSDestination()->equals(*item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_destination_to_Queue_and_compare_with_Topic_type_and_different_address)
{
	const std::string testData{ "test_address" };
	const std::string compareData{ "compared_address" };


	auto item1 = new CMSQueue(testData);
	auto item2 = new CMSTopic(compareData);

	CMSMessage testMsg;
	testMsg.setCMSDestination(item1);

	GTEST_ASSERT_NE(testMsg.getCMSDestination(), nullptr);
	EXPECT_FALSE(testMsg.getCMSDestination()->equals(*item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_destination_to_Topic_and_compare_with_Queue_type_and_different_address)
{
	const std::string testData{ "test_address" };
	const std::string compareData{ "compared_address" };


	auto item1 = new CMSTopic(testData);
	auto item2 = new CMSQueue(compareData);

	CMSMessage testMsg;
	testMsg.setCMSDestination(item1);

	GTEST_ASSERT_NE(testMsg.getCMSDestination(), nullptr);
	EXPECT_FALSE(testMsg.getCMSDestination()->equals(*item2));
}
///reply to
TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_reply_to_Queue_from_AMQP_message)
{
	const std::string testData = "test_address";
	const uint8_t testCapabilityData{ 0 };
	//const std::string testCapabilityData = "queue";
	const CMSQueue compareDestination(testData);

	auto message = std::make_shared<proton::message>();
	message->reply_to(testData);
	message->message_annotations().put("x-opt-jms-reply-to", testCapabilityData);

	CMSMessage testMsg(message.get());

	GTEST_ASSERT_NE(testMsg.getCMSReplyTo(), nullptr);
	EXPECT_TRUE(testMsg.getCMSReplyTo()->equals(compareDestination));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_reply_to_Topic_from_AMQP_message)
{
	const std::string testData = "test_address";
	const uint8_t testCapabilityData{ 1 };
	//const std::string testCapabilityData = "topic";
	const CMSTopic compareDestination(testData);

	auto message = std::make_shared<proton::message>();
	message->reply_to(testData);
	message->message_annotations().put("x-opt-jms-reply-to", testCapabilityData);

	CMSMessage testMsg(message.get());

	GTEST_ASSERT_NE(testMsg.getCMSReplyTo(), nullptr);
	EXPECT_TRUE(testMsg.getCMSReplyTo()->equals(compareDestination));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_reply_to_from_AMQP_message_without_replyto_type)
{
	const std::string testData = "test_address";

	auto message = std::make_shared<proton::message>();
	message->reply_to(testData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSReplyTo(),nullptr);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_message_reply_to_from_AMQP_message_with_incorrect_replyto_type)
{
	const std::string testData = "test_address";
	const uint8_t testCapabilityData{ 5 };
	//const std::string testCapabilityData = "wrong_key";

	auto message = std::make_shared<proton::message>();
	message->reply_to(testData);
	message->message_annotations().put("x-opt-jms-reply-to", testCapabilityData);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSReplyTo(), nullptr);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_reply_to_to_Queue)
{
	const std::string testData{ "test_address" };


	auto item1 = new CMSQueue(testData);

	CMSMessage testMsg;
	testMsg.setCMSReplyTo(item1);

	GTEST_ASSERT_NE(testMsg.getCMSReplyTo(), nullptr);
	EXPECT_TRUE(testMsg.getCMSReplyTo()->equals(*item1));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_reply_to_to_Topic)
{
	const std::string testData{ "test_address" };


	auto item1 = new CMSTopic(testData);

	CMSMessage testMsg;
	testMsg.setCMSReplyTo(item1);

	GTEST_ASSERT_NE(testMsg.getCMSReplyTo(), nullptr);
	EXPECT_TRUE(testMsg.getCMSReplyTo()->equals(*item1));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_reply_to_to_Queue_and_compare_with_diferent_address)
{
	const std::string testData{ "test_address" };
	const std::string compareData{ "compared_address" };


	auto item1 = new CMSQueue(testData);
	auto item2 = new CMSQueue(compareData);

	CMSMessage testMsg;
	testMsg.setCMSReplyTo(item1);

	GTEST_ASSERT_NE(testMsg.getCMSReplyTo(), nullptr);
	EXPECT_FALSE(testMsg.getCMSReplyTo()->equals(*item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_reply_to_to_Queue_and_compare_with_Topic_type_and_same_address)
{
	const std::string testData{ "test_address" };


	auto item1 = new CMSQueue(testData);
	auto item2 = new CMSTopic(testData);

	CMSMessage testMsg;
	testMsg.setCMSReplyTo(item1);

	GTEST_ASSERT_NE(testMsg.getCMSReplyTo(), nullptr);
	EXPECT_FALSE(testMsg.getCMSReplyTo()->equals(*item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_reply_to_to_Topic_and_compare_with_Queue_type_and_same_address)
{
	const std::string testData{ "test_address" };


	auto item1 = new CMSTopic(testData);
	auto item2 = new CMSQueue(testData);

	CMSMessage testMsg;
	testMsg.setCMSReplyTo(item1);

	GTEST_ASSERT_NE(testMsg.getCMSReplyTo(), nullptr);
	EXPECT_FALSE(testMsg.getCMSReplyTo()->equals(*item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_reply_to_to_Queue_and_compare_with_Topic_type_and_different_address)
{
	const std::string testData{ "test_address" };
	const std::string compareData{ "compared_address" };


	auto item1 = new CMSQueue(testData);
	auto item2 = new CMSTopic(compareData);

	CMSMessage testMsg;
	testMsg.setCMSReplyTo(item1);

	GTEST_ASSERT_NE(testMsg.getCMSReplyTo(), nullptr);
	EXPECT_FALSE(testMsg.getCMSReplyTo()->equals(*item2));
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_set_message_reply_to_to_Topic_and_compare_with_Queue_type_and_different_address)
{
	const std::string testData{ "test_address" };
	const std::string compareData{ "compared_address" };


	auto item1 = new CMSTopic(testData);
	auto item2 = new CMSQueue(compareData);

	CMSMessage testMsg;
	testMsg.setCMSReplyTo(item1);

	GTEST_ASSERT_NE(testMsg.getCMSReplyTo(), nullptr);
	EXPECT_FALSE(testMsg.getCMSReplyTo()->equals(*item2));
}

///CMSExpiration
TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_expiration_to_CMSExpiration_ttl_and_expiration_set)
{
//	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	auto message = std::make_shared<proton::message>();
	auto expected_expiration = proton::timestamp::now() + proton::duration(100);
	message->expiry_time(expected_expiration);
	message->ttl(proton::duration(100));
	
	CMSMessage testMsg(message.get());

	EXPECT_EQ(expected_expiration.milliseconds() - testMsg.getCMSExpiration(),  0);

}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_expiration_to_CMSExpiration_expiration_without_expiration_set)
{
	auto message = std::make_shared<proton::message>();

	message->ttl(proton::duration(100));

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSExpiration(), 0);

}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_expiration_to_CMSExpiration_expiration_without_ttl_set)
{
	auto message = std::make_shared<proton::message>();

	auto expected_expiration = proton::timestamp::now() + proton::duration(100);
	message->expiry_time(expected_expiration);

	CMSMessage testMsg(message.get());


	EXPECT_EQ(message->ttl().milliseconds(),0);
	EXPECT_EQ(testMsg.getCMSExpiration(), 0);

}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_AMQP_expiration_to_CMSExpiration)
{
	auto message = std::make_shared<proton::message>();

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSExpiration(), 0);

}

// priority
// JMS priority range <0-9>

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_lowest_Priority_from_AMQP_message_with_lowest_possible_priority_set)
{
	auto message = std::make_shared<proton::message>();
	message->priority(std::numeric_limits<uint8_t>::min());

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSPriority(), 0);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_highest_Priority_from_AMQP_message_with_highest_possible_priority_set)
{
	auto message = std::make_shared<proton::message>();
	message->priority(std::numeric_limits<uint8_t>::max());

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSPriority(), 9);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_lowest_Priority_set)
{
	CMSMessage testMsg;
	testMsg.setCMSPriority(std::numeric_limits<int>::min());


	EXPECT_EQ(testMsg.getCMSPriority(), 0);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_highest_Priority_set)
{
	CMSMessage testMsg;
	testMsg.setCMSPriority(std::numeric_limits<int>::max());

	EXPECT_EQ(testMsg.getCMSPriority(), 9);
}

// Redelived
TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_Redelivered_from_AMQP_redelivered_message)
{
	int deliveryCount{ 3 };

	auto message = std::make_shared<proton::message>();
	message->delivery_count(deliveryCount);

	CMSMessage testMsg(message.get());

	EXPECT_TRUE(testMsg.getCMSRedelivered());
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_Redelivered_from_AMQP_non_redelivered_message)
{
	int deliveryCount{ 0 };

	auto message = std::make_shared<proton::message>();
	message->delivery_count(deliveryCount);

	CMSMessage testMsg(message.get());

	EXPECT_FALSE(testMsg.getCMSRedelivered());
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_Redelivered_from_AMQP_default_message)
{
	int deliveryCount{ 0 };

	auto message = std::make_shared<proton::message>();

	CMSMessage testMsg(message.get());

	EXPECT_FALSE(testMsg.getCMSRedelivered());
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_Redelivered_message)
{
	bool redelivered{ true };

	CMSMessage testMsg;
	testMsg.setCMSRedelivered(redelivered);

	EXPECT_TRUE(testMsg.getCMSRedelivered());
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_non_redelivered_message)
{
	bool redelivered{ false };

	CMSMessage testMsg;
	testMsg.setCMSRedelivered(redelivered);

	EXPECT_FALSE(testMsg.getCMSRedelivered());
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_default_message)
{
	bool redelivered{ false };

	CMSMessage testMsg;
	testMsg.setCMSRedelivered(redelivered);

	EXPECT_FALSE(testMsg.getCMSRedelivered());
}


//type
TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_Type_from_AMQP_message_subject)
{
	const std::string subject{ "user_definde_subject" };

	auto message = std::make_shared<proton::message>();
	message->subject(subject);

	CMSMessage testMsg(message.get());

	EXPECT_EQ(testMsg.getCMSType(), subject);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_Type_from_AMQP_message_default_subject)
{
	auto message = std::make_shared<proton::message>();

	CMSMessage testMsg(message.get());

	EXPECT_TRUE(testMsg.getCMSType().empty());
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_Type)
{
	const std::string subject{ "user_definde_subject" };

	CMSMessage testMsg;
	testMsg.setCMSType(subject);

	EXPECT_EQ(testMsg.getCMSType(), subject);
}

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMS_default_Type)
{
	const std::string subject{};

	CMSMessage testMsg;

	EXPECT_TRUE(testMsg.getCMSType().empty());
}


//JMS-defined ’JMSX’ Properties  3.2.2

TEST_F(Message_JMS_AMQ_Mapping_UT, Test_JMSXUserID)
{
	GTEST_SKIP() << "test not implemented";
}

TEST_F(Message_JMS_AMQ_Mapping_UT, JMSXDeliveryCount)
{
	GTEST_SKIP() << "test not implemented";
}

TEST_F(Message_JMS_AMQ_Mapping_UT, JMSXGroupID)
{
	CMSMessage testMsg;
	testMsg.setStringProperty("JMSXGroupID","Group-0");

	EXPECT_EQ(testMsg.getStringProperty("JMSXGroupID"), "Group-0");
}

TEST_F(Message_JMS_AMQ_Mapping_UT, JMSXGroupSeq)
{
	GTEST_SKIP() << "test not implemented";
}

TEST_F(Message_JMS_AMQ_Mapping_UT, JMSXProducerTXID)
{
	GTEST_SKIP() << "test not implemented";
}

TEST_F(Message_JMS_AMQ_Mapping_UT, JMSXConsumerTXID)
{
	GTEST_SKIP() << "test not implemented";
}

TEST_F(Message_JMS_AMQ_Mapping_UT, JMSXRcvTimestamp)
{
	GTEST_SKIP() << "test not implemented";
}

TEST_F(Message_JMS_AMQ_Mapping_UT, JMSXState)
{
	GTEST_SKIP() << "test not implemented";
}