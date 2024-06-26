
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>
#include <API/CMSMessageProducer.h>

//#include <StdOutLogger/StdOutLogger.h>
//#include <Log4CxxLogger/Log4CxxLogger.h>

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


	StonexLogSource app;
	MyExceptionListener* exl = new MyExceptionListener;
	createConnection("CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3");
	createConnection2("CMSTEST", "CMSTEST", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1", &app, exl);

	createAddress("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3","STONEX_CMS_TEST_1",&app, exl);
	createAddress("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3","STONEX_CMS_TEST_2",&app, exl);
	exl->setExpect(true);
	createAddress("master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_3",&app, exl);
	createAddress("master_sender", "master_sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_4",&app, exl);
	createAddress("sender", "sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_5", &app, exl);
	exl->setExpect(true);
	createAddress("consumer", "consumer",  "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_6", &app, exl);

	createConsumer("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1::STONEX_CMS_TEST_1/TEST", &app, exl);
	createConsumer("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_2::STONEX_CMS_TEST_2/TEST", &app, exl);
	exl->setExpect(true);
	createConsumer("master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_3::STONEX_CMS_TEST_3/TEST", &app, exl);
	createConsumer("master_sender", "master_sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_4::STONEX_CMS_TEST_4/TEST", &app, exl);
	createConsumer("sender", "sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_5::STONEX_CMS_TEST_5/TEST", &app, exl);
	exl->setExpect(true);
	createConsumer("consumer", "consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_6::STONEX_CMS_TEST_6/TEST", &app, exl);

	createConsumerWithSelector("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_1::STONEX_CMS_TEST_1/TEST", &app, exl);
	createConsumerWithSelector("master_publisher", "master_publisher", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_2::STONEX_CMS_TEST_2/TEST", &app, exl);
	exl->setExpect(true);
	createConsumerWithSelector("master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_3::STONEX_CMS_TEST_3/TEST", &app, exl);
	createConsumerWithSelector("master_sender", "master_sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_4::STONEX_CMS_TEST_4/TEST", &app, exl);
	createConsumerWithSelector("sender", "sender", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_5::STONEX_CMS_TEST_5/TEST", &app, exl);
	exl->setExpect(true);
	createConsumerWithSelector("consumer", "consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_6::STONEX_CMS_TEST_6/TEST", &app, exl);

	sendAndReceive("master_publisher", "master_publisher","master_consumer","master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3","STONEX_CMS_TEST_7", "STONEX_CMS_TEST_7::STONEX_CMS_TEST_7/TEST", cms::Destination::TOPIC, &app, exl);
	sendAndReceive("master_sender", "master_sender", "master_consumer", "master_consumer", "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3", "STONEX_CMS_TEST_8", "STONEX_CMS_TEST_8::STONEX_CMS_TEST_8/TEST", cms::Destination::QUEUE, &app, exl);

	return 0;
}
