
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>
#include <API/CMSMessageProducer.h>

#include <Log4CxxLogger/Log4CxxLogger.h>
#include <log4cxx/xml/domconfigurator.h>

#include "Test.h"
#include "DevelopmentTest.h"

int main() 
{
	stonex::logger::initialize(LoggerFactory::LoggerType::LOG4CXX);
	log4cxx::xml::DOMConfigurator::configure("test_logger.xml");
	const std::string broker = "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3";
	const std::string user = "CMSTEST";
	const std::string password = "CMSTEST";
	const std::string producer_address = "CMSTEST";
	const std::string consumer_address = "CMSTEST::Test";


	try
	{


		DevelopmentTest Test;

		Test.initialize(broker, user, password, producer_address, consumer_address);
			   
		//Test.addResult(Test.test1());
		//Test.addResult(Test.test2());
		//Test.addResult(Test.test3());
		//Test.addResult(Test.test4());
		//Test.addResult(Test.test5());
		//Test.addResult(Test.test6());
		//Test.addResult(Test.test7());
		//Test.addResult(Test.test8());
		//Test.addResult(Test.test9());
		//Test.addResult(Test.test10());
		//Test.addResult(Test.test11());
		//Test.addResult(Test.test12());
		//Test.addResult(Test.test13());
		//Test.addResult(Test.test14());
		//Test.addResult(Test.test15());
		//Test.addResult(Test.test16());
		//Test.addResult(Test.test17());
		//Test.addResult(Test.test18());
		//Test.addResult(Test.test19());
		//Test.addResult(Test.test20());
		//Test.addResult(Test.test21());
		//Test.addResult(Test.test22());
		//Test.addResult(Test.test23());
		//Test.addResult(Test.test24());
		//Test.addResult(Test.test25());
		//Test.addResult(Test.test26());
		//Test.addResult(Test.test27());
		//Test.addResult(Test.test28());
		//Test.addResult(Test.test29());
		//Test.addResult(Test.test30());

		//reinit cms components
		Test.reinitialize();
		Test.addResult(Test.test31());
		Test.reinitialize();
		Test.addResult(Test.test32());
		Test.reinitialize();
		Test.addResult(Test.test33());
	//	Test.reinitialize();
	//	Test.addResult(Test.test33());

		Test.verify();


		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination1();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination2();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination3();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination4();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination5();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination6();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination7();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination8();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination9();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination10();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination11();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination12();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination13();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination14();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination15();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination16();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination17();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination18();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination19();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination20();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination21();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination22();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination23();
		}

		{
			DevelopmentTest Test;
			Test.initialize(broker, user, password, producer_address, consumer_address);
			Test.destructionCombination24();
		}

	}
	catch (std::exception& ex)
	{
	}



//
//	MyExceptionListener* exl = new MyExceptionListener;
////	sendAndReceive("CMSTEST", "CMSTEST", "CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_8", "STONEX_CMS_TEST_8::STONEX_CMS_TEST_8/TEST", cms::Destination::QUEUE, exl);
//	createConnection("CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", exl);
//	createConnection2("CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1", exl);
//	createConsumer("CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1::STONEX_CMS_TEST_1/TEST", exl);
//
//
//	createAddress("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3","STONEX_CMS_TEST_1", exl);
//	createAddress("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3","STONEX_CMS_TEST_2", exl);
//	exl->setExpect(true);
//	createAddress("master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_3", exl);
//	createAddress("master_sender", "master_sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_4", exl);
//	createAddress("sender", "sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_5", exl);
//	exl->setExpect(true);
//	createAddress("consumer", "consumer",  "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_6", exl);
//
//	createConsumer("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1::STONEX_CMS_TEST_1/TEST", exl);
//	createConsumer("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_2::STONEX_CMS_TEST_2/TEST", exl);
//	exl->setExpect(true);
//	createConsumer("master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_3::STONEX_CMS_TEST_3/TEST", exl);
//	createConsumer("master_sender", "master_sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_4::STONEX_CMS_TEST_4/TEST", exl);
//	createConsumer("sender", "sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_5::STONEX_CMS_TEST_5/TEST", exl);
//	exl->setExpect(true);
//	createConsumer("consumer", "consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_6::STONEX_CMS_TEST_6/TEST", exl);
//
//	createConsumerWithSelector("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1::STONEX_CMS_TEST_1/TEST", exl);
//	createConsumerWithSelector("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_2::STONEX_CMS_TEST_2/TEST", exl);
//	exl->setExpect(true);
//	createConsumerWithSelector("master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_3::STONEX_CMS_TEST_3/TEST", exl);
//	createConsumerWithSelector("master_sender", "master_sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_4::STONEX_CMS_TEST_4/TEST", exl);
//	createConsumerWithSelector("sender", "sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_5::STONEX_CMS_TEST_5/TEST", exl);
//	exl->setExpect(true);
//	createConsumerWithSelector("consumer", "consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_6::STONEX_CMS_TEST_6/TEST", exl);
//
//	sendAndReceive("master_publisher", "master_publisher","master_consumer","master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3","STONEX_CMS_TEST_7", "STONEX_CMS_TEST_7::STONEX_CMS_TEST_7/TEST", cms::Destination::TOPIC, exl);
//	sendAndReceive("master_sender", "master_sender", "master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_8", "STONEX_CMS_TEST_8::STONEX_CMS_TEST_8/TEST", cms::Destination::QUEUE, exl);

	return 0;
}
