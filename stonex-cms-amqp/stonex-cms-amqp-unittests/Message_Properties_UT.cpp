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
#include "Message_Properties_UT.h"

#include <limits>

#include "cms/MessageFormatException.h"
#include "CMSMessage.h"
#include "CMSBytesMessage.h"
#include "CMSTextMessage.h"

#include <gtest/gtest-param-test.h>

using namespace cms::amqp;

INSTANTIATE_TEST_SUITE_P(Message_testing, Message_Properties_UT,	::testing::Values(new CMSMessage(),	new CMSTextMessage("message body"), new CMSBytesMessage()));

Message_Properties_UT::Message_Properties_UT()
{

}

void Message_Properties_UT::SetUp()
{
	mUUT = GetParam();
}

void Message_Properties_UT::TearDownUp()
{
	delete mUUT;
}

TEST_P(Message_Properties_UT, Test_property_empty_on_non_existing_property) {
	EXPECT_FALSE(mUUT->propertyExists("non_existing_property"));
}

TEST_P(Message_Properties_UT, Test_property_empty_on_existing_property) {
	mUUT->setBooleanProperty("existing_property", false);
	EXPECT_TRUE(mUUT->propertyExists("existing_property"));
}

TEST_P(Message_Properties_UT, Test_set_get_edge_values_bool)
{
	mUUT->setBooleanProperty("boolean_max", std::numeric_limits<bool>::max());
	mUUT->setBooleanProperty("boolean_min", std::numeric_limits<bool>::min());

	EXPECT_EQ(mUUT->getBooleanProperty("boolean_max"), std::numeric_limits<bool>::max());
	EXPECT_EQ(mUUT->getBooleanProperty("boolean_min"), std::numeric_limits<bool>::min());
}

//set edge values of variables and verify value using getters

TEST_P(Message_Properties_UT, Test_set_get_edge_values_byte)
{
	mUUT->setByteProperty("byte_max", std::numeric_limits<unsigned char>::max());
	mUUT->setByteProperty("byte_min", std::numeric_limits<unsigned char>::min());

	EXPECT_EQ(mUUT->getByteProperty("byte_max"), std::numeric_limits<unsigned char>::max());
	EXPECT_EQ(mUUT->getByteProperty("byte_min"), std::numeric_limits<unsigned char>::min());
}

TEST_P(Message_Properties_UT, Test_set_get_edge_values_double)
{
	mUUT->setDoubleProperty("double_max", std::numeric_limits<double>::max());
	mUUT->setDoubleProperty("double_min", std::numeric_limits<double>::min());

	EXPECT_EQ(mUUT->getDoubleProperty("double_max"), std::numeric_limits<double>::max());
	EXPECT_EQ(mUUT->getDoubleProperty("double_min"), std::numeric_limits<double>::min());
}

TEST_P(Message_Properties_UT, Test_set_get_edge_values_float)
{
	mUUT->setFloatProperty("flost_max", std::numeric_limits<float>::max());
	mUUT->setFloatProperty("flost_min", std::numeric_limits<float>::min());

	EXPECT_EQ(mUUT->getFloatProperty("flost_max"), std::numeric_limits<float>::max());
	EXPECT_EQ(mUUT->getFloatProperty("flost_min"), std::numeric_limits<float>::min());
}

TEST_P(Message_Properties_UT, Test_set_get_edge_values_int)
{
	mUUT->setIntProperty("int_max", std::numeric_limits<int>::max());
	mUUT->setIntProperty("int_min", std::numeric_limits<int>::min());

	EXPECT_EQ(mUUT->getIntProperty("int_max"), std::numeric_limits<int>::max());
	EXPECT_EQ(mUUT->getIntProperty("int_min"), std::numeric_limits<int>::min());
}

TEST_P(Message_Properties_UT, Test_set_get_edge_values_long)
{
	mUUT->setLongProperty("long_max", std::numeric_limits<long long>::max());
	mUUT->setLongProperty("long_min", std::numeric_limits<long long>::min());

	EXPECT_EQ(mUUT->getLongProperty("long_max"), std::numeric_limits<long long>::max());
	EXPECT_EQ(mUUT->getLongProperty("long_min"), std::numeric_limits<long long>::min());
}

TEST_P(Message_Properties_UT, Test_set_get_non_existing_edge_values_short)
{
	mUUT->setShortProperty("short_max", std::numeric_limits<short>::max());
	mUUT->setShortProperty("short_min", std::numeric_limits<short>::min());

	EXPECT_EQ(mUUT->getShortProperty("short_max"), std::numeric_limits<short>::max());
	EXPECT_EQ(mUUT->getShortProperty("short_min"), std::numeric_limits<short>::min());
}

//string property
TEST_P(Message_Properties_UT, Test_set_get_non_existing_string_property)
{
	mUUT->setStringProperty("string_propery", "string_value");
	EXPECT_EQ(mUUT->getStringProperty("string_propery"), "string_value");
}
///max


TEST_P(Message_Properties_UT, Test_get_non_existing_max_values_bool)
{
	EXPECT_THROW(mUUT->getBooleanProperty("boolean_max"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_max_values_byte)
{
	EXPECT_THROW(mUUT->getByteProperty("byte_max"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_max_values_double)
{
	EXPECT_THROW(mUUT->getDoubleProperty("double_max"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_max_values_float)
{
	EXPECT_THROW(mUUT->getFloatProperty("flost_max"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_max_values_int)
{
	EXPECT_THROW(mUUT->getIntProperty("int_max"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_max_values_long)
{
	EXPECT_THROW(mUUT->getLongProperty("long_max"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_max_values_short)
{
	EXPECT_THROW(mUUT->getShortProperty("short_max"), cms::CMSException);
}

//min 
TEST_P(Message_Properties_UT, Test_get_non_existing_min_values_bool)
{
	EXPECT_THROW(mUUT->getBooleanProperty("boolean_min"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_min_values_byte)
{
	EXPECT_THROW(mUUT->getByteProperty("byte_min"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_min_values_double)
{
	EXPECT_THROW(mUUT->getDoubleProperty("double_min"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_min_values_float)
{
	EXPECT_THROW(mUUT->getFloatProperty("flost_min"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_min_values_int)
{

	EXPECT_THROW(mUUT->getIntProperty("int_min"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_min_values_long)
{
	EXPECT_THROW(mUUT->getLongProperty("long_min"), cms::CMSException);
}

TEST_P(Message_Properties_UT, Test_get_non_existing_min_values_short)
{
	EXPECT_THROW(mUUT->getShortProperty("short_min"), cms::CMSException);
}

//string property
TEST_P(Message_Properties_UT, Test_get_non_existing_string_property)
{
	EXPECT_THROW(mUUT->getStringProperty("string_property"), cms::CMSException);
}

//conversion throw

TEST_P(Message_Properties_UT, Test_conversion_from_bool_to_other_type)
{
	mUUT->setBooleanProperty("boolean_max", std::numeric_limits<bool>::max());


	EXPECT_THROW(mUUT->getByteProperty("boolean_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getDoubleProperty("boolean_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("boolean_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("boolean_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("boolean_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("boolean_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("boolean_max"), cms::MessageFormatException);


	mUUT->setBooleanProperty("boolean_min", std::numeric_limits<bool>::min());

	EXPECT_THROW(mUUT->getByteProperty("boolean_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getDoubleProperty("boolean_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("boolean_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("boolean_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("boolean_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("boolean_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("boolean_min"), cms::MessageFormatException);
}

TEST_P(Message_Properties_UT, Test_conversion_from_byte_to_other_type)
{
	mUUT->setByteProperty("byte_max", std::numeric_limits<unsigned char>::max());

	EXPECT_THROW(mUUT->getDoubleProperty("byte_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("byte_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("byte_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("byte_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("byte_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("byte_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("byte_max"), cms::MessageFormatException);


	mUUT->setByteProperty("byte_min", std::numeric_limits<unsigned char>::min());

	EXPECT_THROW(mUUT->getDoubleProperty("byte_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("byte_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("byte_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("byte_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("byte_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("byte_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("byte_min"), cms::MessageFormatException);


}

TEST_P(Message_Properties_UT, Test_conversion_from_double_to_other_type)
{
	mUUT->setDoubleProperty("double_max", std::numeric_limits<double>::max());

	EXPECT_THROW(mUUT->getShortProperty("double_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("double_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("double_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("double_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("double_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("double_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("double_max"), cms::MessageFormatException);


	mUUT->setDoubleProperty("double_min", std::numeric_limits<double>::min());

	EXPECT_THROW(mUUT->getShortProperty("double_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("double_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("double_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("double_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("double_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("double_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("double_min"), cms::MessageFormatException);


}

TEST_P(Message_Properties_UT, Test_conversion_from_float_to_other_type)
{
	mUUT->setFloatProperty("flost_max", std::numeric_limits<float>::max());

	EXPECT_THROW(mUUT->getDoubleProperty("flost_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("flost_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("flost_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("flost_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("flost_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("flost_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("flost_max"), cms::MessageFormatException);


	mUUT->setFloatProperty("flost_min", std::numeric_limits<float>::min());

	EXPECT_THROW(mUUT->getDoubleProperty("flost_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("flost_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("flost_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("flost_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("flost_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("flost_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("flost_min"), cms::MessageFormatException);


}

TEST_P(Message_Properties_UT, Test_conversion_from_int_to_other_type)
{
	mUUT->setIntProperty("int_max", std::numeric_limits<int>::max());

	EXPECT_THROW(mUUT->getDoubleProperty("int_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("int_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("int_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("int_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("int_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("int_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("int_max"), cms::MessageFormatException);


	mUUT->setIntProperty("int_min", std::numeric_limits<int>::min());

	EXPECT_THROW(mUUT->getDoubleProperty("int_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("int_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("int_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("int_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("int_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("int_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("int_min"), cms::MessageFormatException);


}

TEST_P(Message_Properties_UT, Test_conversion_from_long_to_other_type)
{
	mUUT->setLongProperty("long_max", std::numeric_limits<long long>::max());

	EXPECT_THROW(mUUT->getDoubleProperty("long_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("long_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("long_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("long_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("long_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("long_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("long_max"), cms::MessageFormatException);


	mUUT->setLongProperty("long_min", std::numeric_limits<long long>::min());

	EXPECT_THROW(mUUT->getDoubleProperty("long_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("long_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("long_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("long_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("long_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("long_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("long_min"), cms::MessageFormatException);


}

TEST_P(Message_Properties_UT, Test_conversion_from_short_to_other_type)
{
	mUUT->setShortProperty("short_max", std::numeric_limits<short>::max());

	EXPECT_THROW(mUUT->getDoubleProperty("short_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("short_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("short_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("short_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getByteProperty("short_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("short_max"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("short_max"), cms::MessageFormatException);


	mUUT->setShortProperty("short_min", std::numeric_limits<short>::min());

	EXPECT_THROW(mUUT->getDoubleProperty("short_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("short_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("short_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("short_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getByteProperty("short_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("short_min"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getStringProperty("short_min"), cms::MessageFormatException);


}

TEST_P(Message_Properties_UT, Test_conversion_from_string_to_other_type)
{
	mUUT->setStringProperty("string_property", "string_value");

	EXPECT_THROW(mUUT->getDoubleProperty("string_property"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getFloatProperty("string_property"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getIntProperty("string_property"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getLongProperty("string_property"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getByteProperty("string_property"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getBooleanProperty("string_property"), cms::MessageFormatException);
	EXPECT_THROW(mUUT->getShortProperty("string_property"), cms::MessageFormatException);


}

//value type

// BYTE_ARRAY_TYPE to do





TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_NULL_TYPE_to_NULL_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}

TEST_P(Message_Properties_UT, Test_qpid_type_id_to_ValueType_conversion_BOOLEAN_to_BOOLEAN_TYPE)
{
	mUUT->setBooleanProperty("boolean", std::numeric_limits<bool>::max());
	auto val = mUUT->getPropertyValueType("boolean");
	
	EXPECT_EQ(val, cms::Message::ValueType::BOOLEAN_TYPE);
}


TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_UBYTE_to_CHAR_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}
TEST_P(Message_Properties_UT, Test_qpid_type_id_to_ValueType_conversion_BYTE_to_BYTE_TYPE)
{
	mUUT->setByteProperty("byte", std::numeric_limits<unsigned char>::max());
	auto val = mUUT->getPropertyValueType("byte");

	EXPECT_EQ(val, cms::Message::ValueType::BYTE_TYPE);
}


TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_USHORT_to_SHORT_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}
TEST_P(Message_Properties_UT, Test_qpid_type_id_to_ValueType_conversion_SHORT_to_SHORT_TYPE)
{
	mUUT->setShortProperty("short", std::numeric_limits<short>::max());
	auto val = mUUT->getPropertyValueType("short");

	EXPECT_EQ(val, cms::Message::ValueType::SHORT_TYPE);
}


TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_UINT_to_INTEGER_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}
TEST_P(Message_Properties_UT, Test_qpid_type_id_to_ValueType_conversion_INT_to_INTEGER_TYPE)
{
	mUUT->setIntProperty("int", std::numeric_limits<int>::max());
	auto val = mUUT->getPropertyValueType("int");

	EXPECT_EQ(val, cms::Message::ValueType::INTEGER_TYPE);
}


TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_CHAR_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}


TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_ULONG_to_LONG_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}
TEST_P(Message_Properties_UT, Test_qpid_type_id_to_ValueType_conversion_LONG_to_LONG_TYPE)
{
	mUUT->setLongProperty("long", std::numeric_limits<long long>::max());
	auto val = mUUT->getPropertyValueType("long");

	EXPECT_EQ(val, cms::Message::ValueType::LONG_TYPE);
}

TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_TIMESTAMP_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}

TEST_P(Message_Properties_UT, Test_qpid_type_id_to_ValueType_conversion_FLOAT_to_FLOAT_TYPE)
{
	mUUT->setFloatProperty("float", std::numeric_limits<float>::max());
	auto val = mUUT->getPropertyValueType("float");

	EXPECT_EQ(val, cms::Message::ValueType::FLOAT_TYPE);
}

TEST_P(Message_Properties_UT, Test_qpid_type_id_to_ValueType_conversion_DOUBLE_to_DOUBLE_TYPE)
{
	mUUT->setDoubleProperty("double", std::numeric_limits<double>::max());
	auto val = mUUT->getPropertyValueType("double");

	EXPECT_EQ(val, cms::Message::ValueType::DOUBLE_TYPE);
}


TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_DECIMAL32_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}
TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_DECIMAL64_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}
TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_DECIMAL128_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}


TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_UUID_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}

TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_BINARY_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}

TEST_P(Message_Properties_UT, Test_qpid_type_id_to_ValueType_conversion_STRING_to_STRING_TYPE)
{
	mUUT->setStringProperty("string", "string_property");
	auto val = mUUT->getPropertyValueType("string");

	EXPECT_EQ(val, cms::Message::ValueType::STRING_TYPE);
}

TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_SYMBOL_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}

TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_DESCRIBED_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}

TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_ARRAY_to_)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}

TEST_P(Message_Properties_UT, DISABLED_Test_qpid_type_id_to_ValueType_conversion_MAP_to_UNKNOWN_TYPE)
{
	GTEST_FATAL_FAILURE_("functionality not implemented");
}

TEST_P(Message_Properties_UT, Test_get_property_names)
{
	mUUT->setBooleanProperty("boolean", false);
	mUUT->setByteProperty("byte", 0xA1);
	mUUT->setDoubleProperty("double", 123456);
	mUUT->setFloatProperty("float", 1.234);
	mUUT->setIntProperty("int", 22);
	mUUT->setLongProperty("long", 12345);
	mUUT->setShortProperty("short", 122);
	mUUT->setStringProperty("string", "string");
	
	std::vector<std::string> expectedList{ "boolean", "byte", "double", "float", "int", "long", "short", "string"};

	EXPECT_EQ(mUUT->getPropertyNames(), expectedList);

}

///header properties


//message body

TEST_P(Message_Properties_UT, Test_message_properties_clear)
{
	mUUT->setBooleanProperty("boolean", false);
	mUUT->setByteProperty("byte", 0xA1);
	mUUT->setDoubleProperty("double", 123456);
	mUUT->setFloatProperty("float", 1.234);
	mUUT->setIntProperty("int", 22);
	mUUT->setLongProperty("long", 12345);
	mUUT->setShortProperty("short", 122);
	mUUT->setStringProperty("string", "string");

	EXPECT_EQ(mUUT->getPropertyNames().size(), 8);

	mUUT->clearProperties();

	EXPECT_EQ(mUUT->getPropertyNames().size(), 0);

}


TEST_P(Message_Properties_UT, Test_message_body_clear)
{
	if (auto obj = dynamic_cast<CMSBytesMessage*>(mUUT))
	{

		const unsigned char BytesMessageBody[5]{ 0x01, 0x02, 0x03, 0x04, 0x05 };
		obj->setBodyBytes(BytesMessageBody,5);

		EXPECT_NE(obj->getBodyLength(), 0);
		obj->clearBody();
		EXPECT_EQ(obj->getBodyLength(), 0);
	}
	else if (auto obj = dynamic_cast<CMSTextMessage*>(mUUT))
	{
		auto body = obj->getText();
		EXPECT_FALSE(body.empty());
		obj = dynamic_cast<CMSTextMessage*>(mUUT);
		obj->clearBody();
		body = obj->getText();
		EXPECT_TRUE(body.empty());
	}

}