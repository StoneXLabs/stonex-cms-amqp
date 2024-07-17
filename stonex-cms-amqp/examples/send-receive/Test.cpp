#include "Test.h"
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>
#include <API/CMSMessageProducer.h>
#include <fmt/core.h>


#include <Log4CxxLogger/Log4CxxLogger.h>
#include <log4cxx/xml/domconfigurator.h>

#include <cassert>


void createConnection(const std::string& user, const std::string& password, const std::string& broker, cms::ExceptionListener* exListener)
{


	auto logger = LoggerFactory::getInstance().create("com.stonex.app");

	try
	{

		logger->log(SEVERITY::LOG_INFO, "test started");

		logger->log(SEVERITY::LOG_INFO, "creating connection");

		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);
		if (exListener)
			factory->setExceptionListener(exListener);

		cms::Connection* connection(factory->createConnection(user, password));
		logger->log(SEVERITY::LOG_INFO, "connection established");
		

		connection->close();
		delete connection;
		connection = NULL;
    }
	catch (const std::exception& ex)
	{
		logger->log(SEVERITY::LOG_ERROR, ex.what());

		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}

}

void createConnection2(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener)
{



	auto logger = LoggerFactory::getInstance().create("com.stonex.app");

	try
	{
		logger->log(SEVERITY::LOG_INFO, "test started");
		logger->log(SEVERITY::LOG_INFO, "creating connection");


		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);

		if (exListener)
			factory->setExceptionListener(exListener);

		cms::Connection* connection = factory->createConnection(user, password);
		logger->log(SEVERITY::LOG_INFO, "connection established");

		if (exListener)
			connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> session(connection->createSession());
		logger->log(SEVERITY::LOG_INFO, "session established");
		


		cms::Destination*  testTopic = session->createTopic(address);
		cms::MessageProducer* topicProducer = session->createProducer(testTopic);

		logger->log(SEVERITY::LOG_INFO, "producer created");
		

//		topicProducer->close();
//		session->close();
		connection->close();
		delete connection;
		connection = NULL;

		logger->log(SEVERITY::LOG_INFO, "test done\n\n");
		
	}
	catch (const std::exception& ex)
	{

		logger->log(SEVERITY::LOG_INFO, ex.what());
		
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
}

void createAddress(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener)
{


	auto logger = LoggerFactory::getInstance().create("com.stonex.app");

	try
	{
	
		logger->log(SEVERITY::LOG_INFO, "test started");
		logger->log(SEVERITY::LOG_INFO, "creating connection");

		std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker));
		
		if(exListener)
			factory->setExceptionListener(exListener);

		std::shared_ptr<cms::Connection> connection(factory->createConnection(user, password));
		logger->log(SEVERITY::LOG_INFO, "connection established");

		if (exListener)
			connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> session(connection->createSession());

		logger->log(SEVERITY::LOG_INFO, "session established");
	


		std::shared_ptr<cms::Destination>  testTopic(session->createTopic(address));
		std::shared_ptr<cms::MessageProducer> topicProducer( session->createProducer(testTopic.get()));

		logger->log(SEVERITY::LOG_INFO, "producer created");

		topicProducer->close();
		session->close();
		connection->close();
		logger->log(SEVERITY::LOG_INFO, "test done\n\n");

	}
	catch (const std::exception& ex)
	{
		logger->log(SEVERITY::LOG_INFO, ex.what());
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
}

void createConsumer(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener)
{


	auto logger = LoggerFactory::getInstance().create("com.stonex.app");

	try
	{
		logger->log(SEVERITY::LOG_INFO, "test started");	
		logger->log(SEVERITY::LOG_INFO, "creating connection");
		

		std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker));

		if (exListener)
			factory->setExceptionListener(exListener);

		std::shared_ptr<cms::Connection> connection(factory->createConnection(user, password));
		logger->log(SEVERITY::LOG_INFO, "connection established");

		if (exListener)
			connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> session(connection->createSession());

		logger->log(SEVERITY::LOG_INFO, "session established");

		std::shared_ptr<cms::Destination>  testTopic(session->createTopic(address));
		std::shared_ptr<cms::MessageConsumer> consumer(session->createConsumer(testTopic.get()));


		logger->log(SEVERITY::LOG_INFO, "producer created");

		consumer->close();
		session->close();
		connection->close();

		logger->log(SEVERITY::LOG_INFO, "test done\n\n");
	}
	catch (const std::exception& ex)
	{
		logger->log(SEVERITY::LOG_INFO, ex.what());
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
}

void createConsumerWithSelector(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener)
{


	auto logger = LoggerFactory::getInstance().create("com.stonex.app");

	try
	{

		logger->log(SEVERITY::LOG_INFO, "test started");

		logger->log(SEVERITY::LOG_INFO, "creating connection");


		std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker));



		if (exListener)
			factory->setExceptionListener(exListener);

		std::shared_ptr<cms::Connection> connection(factory->createConnection(user, password));

		logger->log(SEVERITY::LOG_INFO, "connection established");

		if (exListener)
			connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> session(connection->createSession());

		logger->log(SEVERITY::LOG_INFO, "session established");



		std::shared_ptr<cms::Destination>  testTopic(session->createTopic(address));
		std::shared_ptr<cms::MessageConsumer> consumer(session->createConsumer(testTopic.get(), "REGION=\"UE\" AND ENV=\"TEST\""));

		logger->log(SEVERITY::LOG_INFO, "producer created");


		consumer->close();
		session->close();
		connection->close();

		logger->log(SEVERITY::LOG_INFO, "test done\n\n");
	}
	catch (const std::exception& ex)
	{

		logger->log(SEVERITY::LOG_INFO, ex.what());
		
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
}

void sendAndReceive(const std::string & producer_user, const std::string & producer_password, const std::string & consumer_user, const std::string & consumer_password, const std::string & broker, const std::string& prodcer_address, const std::string& consumer_address, cms::Destination::DestinationType destType, cms::ExceptionListener * exListener)
{


	auto logger = LoggerFactory::getInstance().create("com.stonex.app");
	auto producer_logger = LoggerFactory::getInstance().create("com.stonex.app.producer1");
	auto consumer_logger = LoggerFactory::getInstance().create("com.stonex.app.consumer1");

	assert(destType != cms::Destination::TEMPORARY_QUEUE);
	assert(destType != cms::Destination::TEMPORARY_TOPIC);

	try
	{


		logger->log(SEVERITY::LOG_INFO, "app start");
		logger->log(SEVERITY::LOG_INFO, "test started");
		logger->log(SEVERITY::LOG_INFO, "creating connection");
		std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker));


		logger->log(SEVERITY::LOG_INFO, fmt::format("created connection factory {}", broker));
		if (exListener)
			factory->setExceptionListener(exListener);
		
		///producer

		std::shared_ptr<cms::Connection> producer_connection(factory->createConnection(producer_user, producer_password));

		producer_logger->log(SEVERITY::LOG_INFO, fmt::format("created producer connection {} {}", producer_user, producer_password));

		if (exListener)
			producer_connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> producer_session(producer_connection->createSession());

		producer_logger->log(SEVERITY::LOG_INFO, "producer session established");


		cms::Destination*  testAddress = nullptr;
		if (destType == cms::Destination::QUEUE)
			testAddress = producer_session->createQueue(prodcer_address);
		else if (destType == cms::Destination::TOPIC)
			testAddress = producer_session->createTopic(prodcer_address);


		std::shared_ptr<cms::MessageProducer> producer(producer_session->createProducer(testAddress));

		producer_logger->log(SEVERITY::LOG_INFO, "producer created");


		///consumer 1

		std::shared_ptr<cms::Connection> consumer1_connection(factory->createConnection(consumer_user, consumer_password));

			logger->log(SEVERITY::LOG_INFO, fmt::format("created consumer 1 connection {} {}", consumer_user, consumer_user));

		if (exListener)
			consumer1_connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> consumer1_session(consumer1_connection->createSession());

		logger->log(SEVERITY::LOG_INFO, "consumer session established");

		cms::Destination*  consumerAddress1 = nullptr;
		if (destType == cms::Destination::QUEUE) {
			consumerAddress1 = producer_session->createQueue(consumer_address + "US");
		}
		else if (destType == cms::Destination::TOPIC) {
			consumerAddress1 = producer_session->createTopic(consumer_address + "US");
		}

		//std::shared_ptr<cms::MessageConsumer> consumer1(consumer_session->createConsumer(consumerAddress1, "REGION=\"UE\" AND ENV=\"TEST\""));
		//std::shared_ptr<cms::MessageConsumer> consumer2(consumer_session->createConsumer(consumerAddress2, "REGION=\"US\" AND ENV=\"TEST\""));

		std::shared_ptr<cms::MessageConsumer> consumer1(consumer1_session->createConsumer(consumerAddress1));


		consumer_logger->log(SEVERITY::LOG_INFO, fmt::format("created consumer {}", dynamic_cast<cms::Queue*>(consumerAddress1)->getQueueName()));

		MyMessageListener ml1("consumer1");
		consumer1->setMessageListener(&ml1);



			logger->log(SEVERITY::LOG_INFO, "consumer created");
	
		///////////////////////////////////////////

		///consumer 2

		std::shared_ptr<cms::Connection> consumer2_connection(factory->createConnection(consumer_user, consumer_password));

		logger->log(SEVERITY::LOG_INFO, fmt::format("created consumer 2 connection {} {}", consumer_user, consumer_user));

		if (exListener)
			consumer2_connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> consumer2_session(consumer2_connection->createSession());


		logger->log(SEVERITY::LOG_INFO, "consumer session established");

		cms::Destination* consumerAddress2 = nullptr;
		if (destType == cms::Destination::QUEUE) {
			consumerAddress2 = producer_session->createQueue(consumer_address + "UE");
		}
		else if (destType == cms::Destination::TOPIC) {
			consumerAddress2 = producer_session->createTopic(consumer_address + "UE");
		}

		std::shared_ptr<cms::MessageConsumer> consumer2(consumer2_session->createConsumer(consumerAddress2));


		logger->log(SEVERITY::LOG_INFO, fmt::format("created consumer {}", dynamic_cast<cms::Queue*>(consumerAddress2)->getQueueName()));


		MyMessageListener ml2("consumer2");
		consumer2->setMessageListener(&ml2);


		logger->log(SEVERITY::LOG_INFO, "consumer 2 created");

		///////////////////////////////////////////

		auto mes1 = producer_session->createTextMessage("hello UE");
		mes1->setStringProperty("REGION", "UE");
		mes1->setStringProperty("ENV", "TEST");

		auto mes2 = producer_session->createTextMessage("hello US");
		mes2->setStringProperty("REGION", "US");
		mes2->setStringProperty("ENV", "TEST");

		std::thread t1([producer, mes1, mes2]() {
			for (int i = 0; i < 15; i++)
			{
		//		std::this_thread::sleep_for(std::chrono::seconds(2));
				producer->send(mes1);
	//			producer->send(mes2);
			}
		});




		logger->log(SEVERITY::LOG_INFO, "press to start connection");

		getchar();
		consumer1_connection->start();
		consumer2_connection->stop();

		logger->log(SEVERITY::LOG_INFO, "consumer connection start");

		logger->log(SEVERITY::LOG_INFO, "press to stop connection");

		getchar();

		consumer1_connection->stop();

		logger->log(SEVERITY::LOG_INFO, "consumer connection stop");

		std::thread t2([producer, mes1, mes2]() {
			for (int i = 0; i < 15; i++)
			{
		//		std::this_thread::sleep_for(std::chrono::seconds(2));
				producer->send(mes1);
		//		producer->send(mes2);
			}
		});

		t1.join();
		t2.join();


		logger->log(SEVERITY::LOG_INFO, "press to start connection");
		getchar();

		consumer1_connection->start();

		logger->log(SEVERITY::LOG_INFO, "consumer connection start");

		logger->log(SEVERITY::LOG_INFO, "press to close app");

		getchar();

		consumer1->close();
	//	consumer2->close();
		producer->close();
		consumer1_session->close();
		producer_session->close();
		consumer1_connection->close();
		producer_connection->close();


		logger->log(SEVERITY::LOG_INFO, "test done\n\n");
	}
	catch (const std::exception& ex)
	{


			logger->log(SEVERITY::LOG_INFO, ex.what());
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}

}
