#include "Test.h"
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>
#include <API/CMSMessageProducer.h>
#include <fmt/core.h>


#include <Log4CxxLogger/Log4CxxLogger.h>
#include <log4cxx/xml/domconfigurator.h>

#include <cassert>

void devTest()
{
	auto logger = LoggerFactory::getInstance().create("com.stonex.app");
	auto producer_logger = LoggerFactory::getInstance().create("com.stonex.app.producer1");
	auto consumer_logger = LoggerFactory::getInstance().create("com.stonex.app.consumer1");


	MyExceptionListener factoryExceptionListener;
	MyExceptionListener connectionExceptionListener;
	MyExceptionListener producerExceptionListener;
	MyExceptionListener consumerExceptionListener;
	MyMessageListener ml1("consumer1");
	const std::string broker = "failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3";
	const std::string user = "CMSTEST";
	const std::string password = "CMSTEST";
	const std::string producer_address = "CMSTEST";
	const std::string consumer_address = "CMSTEST::Test";

	try
	{


		logger->log(SEVERITY::LOG_INFO, "app start");
		logger->log(SEVERITY::LOG_INFO, "test started");
		logger->log(SEVERITY::LOG_INFO, "creating connection");
		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);


		logger->log(SEVERITY::LOG_INFO, fmt::format("created connection factory {}", broker));

		factory->setExceptionListener(&factoryExceptionListener);

		///producer

		cms::Connection* producer_connection = factory->createConnection(user, password);

		producer_logger->log(SEVERITY::LOG_INFO, fmt::format("created producer connection {} {}", user, password));

		producer_connection->setExceptionListener(&connectionExceptionListener);

		cms::Session*  session = producer_connection->createSession();

		producer_logger->log(SEVERITY::LOG_INFO, "producer session established");


		cms::Destination*  sender_destination = session->createTopic(producer_address);
		cms::MessageProducer* producer = session->createProducer(sender_destination);

		producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));



		///consumer 1


		cms::Destination*  consumer_destination = session->createTopic(consumer_address);
		cms::MessageConsumer* consumer = session->createConsumer(consumer_destination);
	//	consumer->start();
		consumer_logger->log(SEVERITY::LOG_INFO, fmt::format("created consumer {}", dynamic_cast<cms::Topic*>(consumer_destination)->getTopicName()));

		consumer->setMessageListener(&ml1);


		logger->log(SEVERITY::LOG_INFO, "consumer created");

		///1
		/*
		  send message to 
		*/

		logger->log(SEVERITY::LOG_INFO,fmt::format("-------------------step {}-------------------", 1));
		logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection, expecting no message is consumed");
		ml1.setExpectedCount(0);

		std::thread t1([session, sender_destination, producer_logger, logger]() {
			cms::MessageProducer* producer = nullptr;
			cms::Message* message = nullptr;
			try
			{
				cms::MessageProducer* producer = session->createProducer(sender_destination);

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));

				logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection");

				for (int i = 0; i < 15; i++)
				{
				//	std::this_thread::sleep_for(std::chrono::seconds(2));
					message = session->createTextMessage(fmt::format("hello {}",  i));
					producer->send(message);
					delete message;
					message = nullptr;
				}
				if (producer)
				{
					producer->close();
					delete producer;
					producer = nullptr;
				}
			}
			catch (const std::exception& ex)
			{
				if (producer)
				{
					delete producer;
					producer = nullptr;
				}
				if (message)
				{
					delete message;
					message = nullptr;
				}
				producer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			}

		});

		t1.join();

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 2));			   		
		logger->log(SEVERITY::LOG_INFO, "starting connection, expecting consumer consumes all 15 messages");
		ml1.setExpectedCount(15);
		producer_connection->start();
		std::this_thread::sleep_for(std::chrono::seconds(5));
		ml1.check();


		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 3));
		logger->log(SEVERITY::LOG_INFO, "stoping connection, expecting consumer consumes no messages");
		ml1.setExpectedCount(0);
		producer_connection->stop();


		 t1 = std::thread([session, sender_destination, producer_logger, logger]() {
			cms::MessageProducer* producer = nullptr;
			cms::Message* message = nullptr;
			try
			{
				cms::MessageProducer* producer = session->createProducer(sender_destination);

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));

				logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection");

				for (int i = 0; i < 15; i++)
				{
					//	std::this_thread::sleep_for(std::chrono::seconds(2));
					message = session->createTextMessage(fmt::format("hello {}", i));
					producer->send(message);
					delete message;
					message = nullptr;
				}
				if (producer)
				{
					producer->close();
					delete producer;
					producer = nullptr;
				}
			}
			catch (const std::exception& ex)
			{
				if (producer)
				{
					delete producer;
					producer = nullptr;
				}
				if (message)
				{
					delete message;
					message = nullptr;
				}
				producer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			}

		});

		t1.join();
		ml1.check();


		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 4));
		logger->log(SEVERITY::LOG_INFO, "starting session, expecting consumer consumes all 15 messages");

		ml1.setExpectedCount(15);
		session->start();
		std::this_thread::sleep_for(std::chrono::seconds(5));
		ml1.check();

		//////////////////////////////////////////////////////////////////////////////////////////////
		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 5));
		logger->log(SEVERITY::LOG_INFO, "stoping connection, expecting no messages consumed");

		ml1.setExpectedCount(0);
		producer_connection->stop();

		t1 = std::thread([session, sender_destination, producer_logger, logger]() {
			cms::MessageProducer* producer = nullptr;
			cms::Message* message = nullptr;
			try
			{
				cms::MessageProducer* producer = session->createProducer(sender_destination);

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));

				logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection and opened session");

				for (int i = 0; i < 15; i++)
				{
					//	std::this_thread::sleep_for(std::chrono::seconds(2));
					message = session->createTextMessage(fmt::format("hello {}", i));
					producer->send(message);
					delete message;
					message = nullptr;
				}
				if (producer)
				{
					producer->close();
					delete producer;
					producer = nullptr;
				}
			}
			catch (const std::exception& ex)
			{
				if (producer)
				{
					delete producer;
					producer = nullptr;
				}
				if (message)
				{
					delete message;
					message = nullptr;
				}
				producer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			}

		});

		t1.join();

		std::this_thread::sleep_for(std::chrono::seconds(1));
		logger->log(SEVERITY::LOG_INFO, "check if no messages were received");
		ml1.check();



		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 6));

		logger->log(SEVERITY::LOG_INFO, fmt::format("starting consumer, expecting 15 messages consumed"));

		ml1.setExpectedCount(15);
		consumer->start();

		std::this_thread::sleep_for(std::chrono::seconds(5));
		logger->log(SEVERITY::LOG_INFO, "check if no messages were received");
		ml1.check();

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 7));
		logger->log(SEVERITY::LOG_INFO, "stoping  consumer, expecting no messages consumed");
		ml1.setExpectedCount(0);
		consumer->stop();

		t1 = std::thread([session, sender_destination, producer_logger, logger]() {
			cms::MessageProducer* producer = nullptr;
			cms::Message* message = nullptr;
			try
			{
				cms::MessageProducer* producer = session->createProducer(sender_destination);

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));

				logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection");

				for (int i = 0; i < 15; i++)
				{
					//	std::this_thread::sleep_for(std::chrono::seconds(2));
					message = session->createTextMessage(fmt::format("hello {}", i));
					producer->send(message);
					delete message;
					message = nullptr;
				}
				if (producer)
				{
					producer->close();
					delete producer;
					producer = nullptr;
				}
			}
			catch (const std::exception& ex)
			{
				if (producer)
				{
					delete producer;
					producer = nullptr;
				}
				if (message)
				{
					delete message;
					message = nullptr;
				}
				producer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			}

		});

		t1.join();

		std::this_thread::sleep_for(std::chrono::seconds(5));
		ml1.check();

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 8));
		logger->log(SEVERITY::LOG_INFO, "starting connection , expecting 15 messages received");

		ml1.setExpectedCount(15);

		producer_connection->start();
		std::this_thread::sleep_for(std::chrono::seconds(2));
		ml1.check();

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 9));
		logger->log(SEVERITY::LOG_INFO, "stoping session expect ne messages received");

		ml1.setExpectedCount(0);
		session->stop();
		t1 = std::thread([session, sender_destination, producer_logger, logger]() {
			cms::MessageProducer* producer = nullptr;
			cms::Message* message = nullptr;
			try
			{
				cms::MessageProducer* producer = session->createProducer(sender_destination);

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));

				logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection");

				for (int i = 0; i < 15; i++)
				{
					//	std::this_thread::sleep_for(std::chrono::seconds(2));
					message = session->createTextMessage(fmt::format("hello {}", i));
					producer->send(message);
					delete message;
					message = nullptr;
				}
				if (producer)
				{
					producer->close();
					delete producer;
					producer = nullptr;
				}
			}
			catch (const std::exception& ex)
			{
				if (producer)
				{
					delete producer;
					producer = nullptr;
				}
				if (message)
				{
					delete message;
					message = nullptr;
				}
				producer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			}

		});

		t1.join();
		std::this_thread::sleep_for(std::chrono::seconds(2));
		ml1.check();
		

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 10));

		logger->log(SEVERITY::LOG_INFO, fmt::format("starting connection, expecting 15 messages consumed"));

		ml1.setExpectedCount(15);
		producer_connection->start();

		std::this_thread::sleep_for(std::chrono::seconds(5));
		logger->log(SEVERITY::LOG_INFO, "check if no messages were received");
		ml1.check();

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 11));
		logger->log(SEVERITY::LOG_INFO, "stoping consumer, expect no messages received");

		ml1.setExpectedCount(0);

		consumer->stop(); 
		t1 = std::thread([session, sender_destination, producer_logger, logger]() {
			cms::MessageProducer* producer = nullptr;
			cms::Message* message = nullptr;
			try
			{
				cms::MessageProducer* producer = session->createProducer(sender_destination);

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));

				logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection");

				for (int i = 0; i < 15; i++)
				{
					//	std::this_thread::sleep_for(std::chrono::seconds(2));
					message = session->createTextMessage(fmt::format("hello {}", i));
					producer->send(message);
					delete message;
					message = nullptr;
				}
				if (producer)
				{
					producer->close();
					delete producer;
					producer = nullptr;
				}
			}
			catch (const std::exception& ex)
			{
				if (producer)
				{
					delete producer;
					producer = nullptr;
				}
				if (message)
				{
					delete message;
					message = nullptr;
				}
				producer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			}

		});

		t1.join();
		ml1.check();
		

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 12));
		logger->log(SEVERITY::LOG_INFO, "starting session, expecting 15 messages received to clear queue");

		ml1.setExpectedCount(15);
		session->start();
		std::this_thread::sleep_for(std::chrono::seconds(2));
		ml1.check();
		


		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 13));
		logger->log(SEVERITY::LOG_INFO, "stoping session, expect no messages received");

		ml1.setExpectedCount(0);

		session->stop();
		t1 = std::thread([session, sender_destination, producer_logger, logger]() {
			cms::MessageProducer* producer = nullptr;
			cms::Message* message = nullptr;
			try
			{
				cms::MessageProducer* producer = session->createProducer(sender_destination);

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));

				logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection");

				for (int i = 0; i < 15; i++)
				{
					//	std::this_thread::sleep_for(std::chrono::seconds(2));
					message = session->createTextMessage(fmt::format("hello {}", i));
					producer->send(message);
					delete message;
					message = nullptr;
				}
				if (producer)
				{
					producer->close();
					delete producer;
					producer = nullptr;
				}
			}
			catch (const std::exception& ex)
			{
				if (producer)
				{
					delete producer;
					producer = nullptr;
				}
				if (message)
				{
					delete message;
					message = nullptr;
				}
				producer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			}

		});

		t1.join();
		ml1.check();



		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 14));
		logger->log(SEVERITY::LOG_INFO, "starting consumer, expecting 15 messages received to clear queue");

		ml1.setExpectedCount(15);
		consumer->start();
		std::this_thread::sleep_for(std::chrono::seconds(2));
		ml1.check();

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 15));
		logger->log(SEVERITY::LOG_INFO, "stoping session, expect no messages received");

		ml1.setExpectedCount(0);

		session->stop();
		t1 = std::thread([session, sender_destination, producer_logger, logger]() {
			cms::MessageProducer* producer = nullptr;
			cms::Message* message = nullptr;
			try
			{
				cms::MessageProducer* producer = session->createProducer(sender_destination);

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));

				logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection");

				for (int i = 0; i < 15; i++)
				{
					//	std::this_thread::sleep_for(std::chrono::seconds(2));
					message = session->createTextMessage(fmt::format("hello {}", i));
					producer->send(message);
					delete message;
					message = nullptr;
				}
				if (producer)
				{
					producer->close();
					delete producer;
					producer = nullptr;
				}
			}
			catch (const std::exception& ex)
			{
				if (producer)
				{
					delete producer;
					producer = nullptr;
				}
				if (message)
				{
					delete message;
					message = nullptr;
				}
				producer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			}

		});

		t1.join();
		ml1.check();


		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 16));
		logger->log(SEVERITY::LOG_INFO, "starting consumer, expecting 15 messages received to clear queue");

		ml1.setExpectedCount(15);
		consumer->start();
		std::this_thread::sleep_for(std::chrono::seconds(2));
		ml1.check();


		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 17));
		logger->log(SEVERITY::LOG_INFO, "stoping session, expect no messages received");

		ml1.setExpectedCount(0);

		consumer->stop();
		t1 = std::thread([session, sender_destination, producer_logger, logger]() {
			cms::MessageProducer* producer = nullptr;
			cms::Message* message = nullptr;
			try
			{
				cms::MessageProducer* producer = session->createProducer(sender_destination);

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(sender_destination)->getTopicName()));

				logger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection");

				for (int i = 0; i < 15; i++)
				{
					//	std::this_thread::sleep_for(std::chrono::seconds(2));
					message = session->createTextMessage(fmt::format("hello {}", i));
					producer->send(message);
					delete message;
					message = nullptr;
				}
				if (producer)
				{
					producer->close();
					delete producer;
					producer = nullptr;
				}
			}
			catch (const std::exception& ex)
			{
				if (producer)
				{
					delete producer;
					producer = nullptr;
				}
				if (message)
				{
					delete message;
					message = nullptr;
				}
				producer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			}

		});

		t1.join();
		ml1.check();


		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 18));
		logger->log(SEVERITY::LOG_INFO, "creating session on stoped connection");
		cms::Session* session2 = nullptr;

		try
		{
			producer_connection->stop();
			session2 = producer_connection->createSession();
			logger->log(SEVERITY::LOG_INFO, "creating session on stoped connection SUCCESS");
			delete session2;
			session2 = nullptr;
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception creating session on colsed connection {}", ex.what()));
		}

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 19));
		logger->log(SEVERITY::LOG_INFO, "creating session on started connection");

		try
		{
			producer_connection->start();
			session2 = producer_connection->createSession();
			logger->log(SEVERITY::LOG_INFO, "creating session on started connection SUCCESS");
			delete session2;
			session2 = nullptr;
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception creating session on colsed connection {}", ex.what()));
		}

		///

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 20));
		logger->log(SEVERITY::LOG_INFO, "closing consumer");

		try
		{
			consumer->close();
			logger->log(SEVERITY::LOG_INFO, "closing consumer SUCCESS");
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception closing consumer {}", ex.what()));
		}

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 21));
		logger->log(SEVERITY::LOG_INFO, "closing closed consumer");

		try
		{
			consumer->close();
			logger->log(SEVERITY::LOG_INFO, "closing closed consumer SUCCESS");
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception closing closed consumer {}", ex.what()));
		}

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 22));
		logger->log(SEVERITY::LOG_INFO, "closing session");


		try
		{
			consumer->close();
			logger->log(SEVERITY::LOG_INFO, "closing session SUCCESS");
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception closing session {}", ex.what()));
		}

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 23));
		logger->log(SEVERITY::LOG_INFO, "closing closed session");


		try
		{
			consumer->close();
			logger->log(SEVERITY::LOG_INFO, "closing closed session SUCCESS");
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception closing closed session {}", ex.what()));
		}
		///

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 24));
		logger->log(SEVERITY::LOG_INFO, "creating session on closed connection");
	
		try
		{
			producer_connection->close();
			session2 = producer_connection->createSession();
			logger->log(SEVERITY::LOG_ERROR, "creating session on stoped connection FAILED - should throw exception");
			delete session2;
			session2 = nullptr;
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_INFO, fmt::format("creating session on  closed Connection got expected exception : Exception  {}", ex.what()));
		}

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 25));
		logger->log(SEVERITY::LOG_INFO, "closing closed connection");

		try
		{
			producer_connection->close();
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_INFO, fmt::format("creating session on stoped closed Connection got expected exception : Exception  {}", ex.what()));
		}

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step connection recreating session and consumer {}-------------------", 26));

		delete producer_connection;
		delete consumer;
		delete session;

		producer_connection = factory->createConnection(user, password);
		producer_logger->log(SEVERITY::LOG_INFO, fmt::format("created producer connection {} {}", user, password));
		producer_connection->setExceptionListener(&connectionExceptionListener);
		session = producer_connection->createSession();
		producer_logger->log(SEVERITY::LOG_INFO, "producer session established");
		consumer = session->createConsumer(consumer_destination);

		//////////////////

		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 27)); 
		logger->log(SEVERITY::LOG_INFO, "creating consumer on started session");
		cms::MessageConsumer* consumer2 = nullptr;

		try
		{
			session->start();
			consumer2 = session->createConsumer(consumer_destination);
			logger->log(SEVERITY::LOG_INFO, "creating consumer on stoped session SUCCESS");
			delete consumer2;
			consumer2 = nullptr;
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception creating consumer on started session {}", ex.what()));
		}



		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 28));
		logger->log(SEVERITY::LOG_INFO, "creating consumer on stoped session");
		consumer2 = nullptr;

		try
		{
			session->stop();
			consumer2 = session->createConsumer(consumer_destination);
			logger->log(SEVERITY::LOG_INFO, "creating consumer on stoped session SUCCESS");
			delete consumer2;
			consumer2 = nullptr;
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception consumer session on started session {}", ex.what()));
		}


		logger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", 29));
		logger->log(SEVERITY::LOG_INFO, "creating consumer on closed session");
		consumer2 = nullptr;

		try
		{
			session->close();
			consumer2 = session->createConsumer(consumer_destination);
			logger->log(SEVERITY::LOG_ERROR, "creating consumer on closed session FAILED expected exception");
			delete consumer2;
			consumer2 = nullptr;
		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_INFO, fmt::format("expected Exception creating consumer on closed session {}", ex.what()));
		}
		//////////////////////////////////////////////////////////////////////////////////////////////

		producer_connection->close();
		std::this_thread::sleep_for(std::chrono::seconds(2));
		try
		{
			cms::Session*  session2 = producer_connection->createSession();

		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception creating session on colsed connection {}", ex.what()));

		}


		try
		{
			producer_connection->start();

		}
		catch (const std::exception& ex)
		{
			logger->log(SEVERITY::LOG_ERROR, fmt::format("Exception creating session on colsed connection {}", ex.what()));

		}

		consumer->start();
	//	producer->start();
		session->start();
		producer_connection->start();
		std::this_thread::sleep_for(std::chrono::seconds(2));
		/////////////////////////////////
		//mes1->setStringProperty("REGION", "UE");
		//mes1->setStringProperty("ENV", "TEST");



	}
	catch (const std::exception& ex)
	{
		logger->log(SEVERITY::LOG_ERROR, ex.what());

		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}

	producer_logger->log(SEVERITY::LOG_INFO, "test done");
}


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

		cms::Connection* connection = factory->createConnection(user, password);
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

		cms::Session* session = connection->createSession();
		logger->log(SEVERITY::LOG_INFO, "session established");
		


		cms::Destination*  testTopic = session->createTopic(address);
		cms::MessageProducer* topicProducer = session->createProducer(testTopic);

		logger->log(SEVERITY::LOG_INFO, "producer created");
		
		connection->close();
		delete connection;
		connection = NULL;
		delete topicProducer;
		delete session;

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

		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);
		
		if(exListener)
			factory->setExceptionListener(exListener);

		cms::Connection* connection = factory->createConnection(user, password);
		logger->log(SEVERITY::LOG_INFO, "connection established");

		if (exListener)
			connection->setExceptionListener(exListener);

		cms::Session* session = connection->createSession();

		logger->log(SEVERITY::LOG_INFO, "session established");
	


		cms::Destination*  testTopic = session->createTopic(address);
		cms::MessageProducer* topicProducer = session->createProducer(testTopic);

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
		

		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);

		if (exListener)
			factory->setExceptionListener(exListener);

		cms::Connection* connection = factory->createConnection(user, password);
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


		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);



		if (exListener)
			factory->setExceptionListener(exListener);

		cms::Connection* connection = factory->createConnection(user, password);

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
		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);


		logger->log(SEVERITY::LOG_INFO, fmt::format("created connection factory {}", broker));
		if (exListener)
			factory->setExceptionListener(exListener);
		
		///producer

		cms::Connection* producer_connection = factory->createConnection(producer_user, producer_password);

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
				std::this_thread::sleep_for(std::chrono::seconds(2));
				producer->send(mes1);
				producer->send(mes2);
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
				std::this_thread::sleep_for(std::chrono::seconds(2));
				producer->send(mes1);
				producer->send(mes2);
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
		consumer2->close();
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
