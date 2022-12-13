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
#include "ConnectionMetadataImpl_UT.h"
#include <regex>
using namespace cms::amqp;

void ConnectionMetadataImpl_UT::SetUp()
{
	mUUT = new ConnectionMetadataImpl();
}

void ConnectionMetadataImpl_UT::TearDown()
{
	delete mUUT;
	mUUT = nullptr;
}


TEST_F(ConnectionMetadataImpl_UT, test_CMSVersion) {

	std::regex expr_format("[0-9]+.[0-9]+");
	EXPECT_TRUE(std::regex_match(mUUT->getCMSVersion(), expr_format));
}

TEST_F(ConnectionMetadataImpl_UT, test_CMSVersionMajor) {

	EXPECT_EQ(mUUT->getCMSMajorVersion(),3);
}

TEST_F(ConnectionMetadataImpl_UT, test_CMSVersionMinor) {

	EXPECT_EQ(mUUT->getCMSMinorVersion(), 2);
}

TEST_F(ConnectionMetadataImpl_UT, test_CMSProviderName) {

	EXPECT_FALSE(mUUT->getCMSProviderName().empty());
}

TEST_F(ConnectionMetadataImpl_UT, test_CMSProviderVersion) {

	std::regex expr_format("[0-9]+.[0-9]+.[0-9]+");
	EXPECT_TRUE(std::regex_match(mUUT->getProviderVersion(), expr_format));

}

TEST_F(ConnectionMetadataImpl_UT, DISABLED_test_ProviderMajorVersion) {

	EXPECT_NE(mUUT->getProviderMajorVersion(),0);
}

TEST_F(ConnectionMetadataImpl_UT, DISABLED_test_ProviderMinorVersion) {

	EXPECT_NE(mUUT->getProviderMinorVersion(), 0);
}

TEST_F(ConnectionMetadataImpl_UT, DISABLED_test_ProviderPatchVersion) {

	EXPECT_NE(mUUT->getProviderPatchVersion(), 0);
}

TEST_F(ConnectionMetadataImpl_UT, DISABLED_test_CMSXPropertyNames) {

	EXPECT_FALSE(mUUT->getCMSXPropertyNames().empty());
}