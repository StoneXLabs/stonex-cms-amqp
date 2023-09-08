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

#include <TestSuite/TestRunner.h>
#include <TestSuite/TestFunctionRegister.h>
#include <TestSuite/TestCasePerformer.h>
#include <ConfigurationParser/TestSuiteJsonConfigParser.h>

#include <Cases/proton-cpp-client-connection-tests.h>
#include <Cases/proton-cpp-client-session-tests.h>
#include <Cases/proton-cpp-client-tests.h>

#include <MessageSender/TestMessageSenderFactory.h>

#include <MessageReceiver/TestMessageReceiverFactory.h>

#include <Log4CxxLogger/Log4CxxLogger.h>
#include <Notifier/StdOutTestObserver.h>

//temporary

#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>

int main(int argc, char** argv)
{	
	if (argc != 2)
		return 1;

	if (FILE* file = fopen(argv[1], "r")) {
		fclose(file);
	}
	else {
		return 2;
	}


	TestFunctionRegister testFunctionRegister;
	testFunctionRegister.registerTestFunction("test_create_producer_and_consumer_for_destination", test_create_producer_and_consumer_for_destination);
	testFunctionRegister.registerTestFunction("test_close_session", test_close_session);
	testFunctionRegister.registerTestFunction("test_create_session", test_create_session);
	testFunctionRegister.registerTestFunction("test_stop_and_restart_connection", test_stop_and_restart_connection);
	testFunctionRegister.registerTestFunction("test_create_connection", test_create_connection);
	
	TestSuiteJsonConfigParser parser(argv[1]);
	
	
	StdOutTestObserver testResultObserver;

	stonex::messaging::test::TestSenderFactory _senderFactory;
	stonex::messaging::test::TestMessageReceiverFactory _receiverFactory;

	
	std::shared_ptr<StonexLogger> logger = std::make_shared<Log4CxxLogger>();
	logger->configure("logger.xml");

	TestRunner tr(parser, testFunctionRegister, _receiverFactory, _senderFactory, &testResultObserver, logger);

	tr.run();

	return testResultObserver.errorCount();
}