
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>
#include <API/CMSMessageProducer.h>

#include <Log4CxxLogger/Log4CxxLogger.h>
#include <log4cxx/xml/domconfigurator.h>

#include "Test.h"
//#include <fmt/format.h>



int main() 
{


	//auto logger = std::make_shared<Log4CxxLogger>();
	//logger->configure("logger.xml");
	//

	//auto factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory("failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=30");
	//logger->attach("com.stonex.app.factory",(cms::amqp::CMSConnectionFactory*)factory);

	//MyExceptionListener exl;
	//try
	//{
	////	std::cout << fmt::format("{}", (void *)exl) << std::endl;;
	//	factory->setExceptionListener(exl);

	//}
	//catch (const std::exception& ex)
	//{
	//	std::cout << "EXCEPTION " << ex.what() << std::endl;
	//}
	stonex::logger::initialize(LoggerFactory::LoggerType::LOG4CXX);
	log4cxx::xml::DOMConfigurator::configure("bin/logger.xml");

	MyExceptionListener* exl = new MyExceptionListener;
	sendAndReceive("CMSTEST", "CMSTEST", "CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_8", "STONEX_CMS_TEST_8::STONEX_CMS_TEST_8/TEST", cms::Destination::QUEUE, exl);
	createConnection("CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", exl);
	createConnection2("CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1", exl);
	createConsumer("CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1::STONEX_CMS_TEST_1/TEST", exl);


	createAddress("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3","STONEX_CMS_TEST_1", exl);
	createAddress("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3","STONEX_CMS_TEST_2", exl);
	exl->setExpect(true);
	createAddress("master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_3", exl);
	createAddress("master_sender", "master_sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_4", exl);
	createAddress("sender", "sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_5", exl);
	exl->setExpect(true);
	createAddress("consumer", "consumer",  "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_6", exl);

	createConsumer("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1::STONEX_CMS_TEST_1/TEST", exl);
	createConsumer("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_2::STONEX_CMS_TEST_2/TEST", exl);
	exl->setExpect(true);
	createConsumer("master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_3::STONEX_CMS_TEST_3/TEST", exl);
	createConsumer("master_sender", "master_sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_4::STONEX_CMS_TEST_4/TEST", exl);
	createConsumer("sender", "sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_5::STONEX_CMS_TEST_5/TEST", exl);
	exl->setExpect(true);
	createConsumer("consumer", "consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_6::STONEX_CMS_TEST_6/TEST", exl);

	createConsumerWithSelector("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1::STONEX_CMS_TEST_1/TEST", exl);
	createConsumerWithSelector("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_2::STONEX_CMS_TEST_2/TEST", exl);
	exl->setExpect(true);
	createConsumerWithSelector("master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_3::STONEX_CMS_TEST_3/TEST", exl);
	createConsumerWithSelector("master_sender", "master_sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_4::STONEX_CMS_TEST_4/TEST", exl);
	createConsumerWithSelector("sender", "sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_5::STONEX_CMS_TEST_5/TEST", exl);
	exl->setExpect(true);
	createConsumerWithSelector("consumer", "consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_6::STONEX_CMS_TEST_6/TEST", exl);

	sendAndReceive("master_publisher", "master_publisher","master_consumer","master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3","STONEX_CMS_TEST_7", "STONEX_CMS_TEST_7::STONEX_CMS_TEST_7/TEST", cms::Destination::TOPIC, exl);
	sendAndReceive("master_sender", "master_sender", "master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_8", "STONEX_CMS_TEST_8::STONEX_CMS_TEST_8/TEST", cms::Destination::QUEUE, exl);

	return 0;
}
