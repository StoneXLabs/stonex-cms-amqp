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
#include "ProtonCppLibrary_UT.h"
#include "ProtonCppLibrary.h"

#include "MOCK/ExceptionListenerMock.h"

#include "proton/connection.hpp"
#include "proton/connection_options.hpp"


TEST_F(ProtonCppLibrary_UT, DISABLED_test_default_exception_listener) {

	 //activemq::library::ActiveMQCPP::initialize_library();
	 //auto a = cms::ProtonCppLibrary::getInstance();

	 //ExceptionListenerMock mock1;

	 //a->registerExceptionListener(&mock1);
	 //
	 //EXPECT_CALL(mock1, onException(testing::_));

	 //proton::connection_options opts;
	 //opts.handler(*a);

	 //a->getContainer()->connect("localhost:1234",opts);
	 //
	 //std::this_thread::sleep_for(std::chrono::seconds(5));
	
	 EXPECT_EQ(true, false);
}
