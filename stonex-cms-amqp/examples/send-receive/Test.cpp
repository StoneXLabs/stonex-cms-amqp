#include "Test.h"
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>
#include <API/CMSMessageProducer.h>


#include <Log4CxxLogger/Log4CxxLogger.h>

#include <cassert>


void createConnection(const std::string& user, const std::string& password, const std::string& broker, StonexLogSource* log_src, cms::ExceptionListener* exListener)
{
//	try
//	{

		auto logger = std::make_shared<Log4CxxLogger>();
		logger->configure("logger.xml");

		if (log_src) {
			logger->attach("com.stonex.app", log_src);
			log_src->info("com.stonex.app", "test started");
		}


		if (log_src) {
			log_src->info("com.stonex.app", "creating connection");
		}

		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);

		logger->attach("com.stonex.app.factory", (cms::amqp::CMSConnectionFactory*)factory);


		if (exListener)
			factory->setExceptionListener(exListener);

		cms::Connection* connection(factory->createConnection(user, password));
		if (log_src) {
			log_src->info("com.stonex.app", "connection established");
		}

	//	connection->close();
		std::getchar();
		log_src->info("com.stonex.app", "waiting on close");
		connection->close();
		std::getchar();
		log_src->info("com.stonex.app", "waiting on delete");
		delete connection;
		connection = NULL;
/* }
	catch (const std::exception& ex)
	{

		if (log_src) {
			log_src->info("com.stonex.app", ex.what());
		}
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}*/

}

void createConnection2(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, StonexLogSource* log_src, cms::ExceptionListener* exListener)
{
	try
	{

		auto logger = std::make_shared<Log4CxxLogger>();
		logger->configure("logger.xml");

		if (log_src) {
			logger->attach("com.stonex.app", log_src);
			log_src->info("com.stonex.app", "test started");
		}


		if (log_src) {
			log_src->info("com.stonex.app", "creating connection");
		}

		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);

		logger->attach("com.stonex.app.factory", (cms::amqp::CMSConnectionFactory*)factory);


		if (exListener)
			factory->setExceptionListener(exListener);

		cms::Connection* connection = factory->createConnection(user, password);
		if (log_src) {
			log_src->info("com.stonex.app", "connection established");
		}

		logger->attach("com.stonex.app.connection", (cms::amqp::CMSConnection*)connection);
		if (exListener)
			connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> session(connection->createSession());
		logger->attach("com.stonex.app.session", (cms::amqp::CMSSession*)session.get());

		if (log_src) {
			log_src->info("com.stonex.app", "session established");
		}


		cms::Destination*  testTopic = session->createTopic(address);
		cms::MessageProducer* topicProducer = session->createProducer(testTopic);


		if (log_src) {
			log_src->info("com.stonex.app", "producer created");
		}

		logger->attach("com.stonex.app.topicProducer", (cms::amqp::CMSMessageProducer*)topicProducer);

//		topicProducer->close();
//		session->close();
		std::getchar();
		log_src->info("com.stonex.app", "waiting on close");
		connection->close();
		std::getchar();
		log_src->info("com.stonex.app", "waiting on delete");
		delete connection;
		connection = NULL;

		if (log_src) {
			log_src->info("com.stonex.app", "test done\n\n");
		}
	}
	catch (const std::exception& ex)
	{

		if (log_src) {
			log_src->info("com.stonex.app", ex.what());
		}
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
}

void createAddress(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, StonexLogSource* log_src, cms::ExceptionListener* exListener)
{	
	try
	{

		auto logger = std::make_shared<Log4CxxLogger>();
		logger->configure("logger.xml");

		if (log_src) {
			logger->attach("com.stonex.app", log_src);
			log_src->info("com.stonex.app", "test started");
		}


		if (log_src) {
			log_src->info("com.stonex.app", "creating connection");
		}

		std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker));
		
		logger->attach("com.stonex.app.factory", (cms::amqp::CMSConnectionFactory*)factory.get());


		if(exListener)
			factory->setExceptionListener(exListener);

		std::shared_ptr<cms::Connection> connection(factory->createConnection(user, password));
		if (log_src) {
			log_src->info("com.stonex.app", "connection established");
		}

		logger->attach("com.stonex.app.connection", (cms::amqp::CMSConnection*)connection.get());
		if (exListener)
			connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> session(connection->createSession());
		logger->attach("com.stonex.app.session", (cms::amqp::CMSSession*)session.get());

		if (log_src) {
			log_src->info("com.stonex.app", "session established");
		}


		std::shared_ptr<cms::Destination>  testTopic(session->createTopic(address));
		std::shared_ptr<cms::MessageProducer> topicProducer( session->createProducer(testTopic.get()));


		if (log_src) {
			log_src->info("com.stonex.app", "producer created");
		}

		logger->attach("com.stonex.app.topicProducer", (cms::amqp::CMSMessageProducer*)topicProducer.get());

		topicProducer->close();
		session->close();
		connection->close();

		if (log_src) {
			log_src->info("com.stonex.app", "test done\n\n");
		}
	}
	catch (const std::exception& ex)
	{

		if (log_src) {
			log_src->info("com.stonex.app", ex.what());
		}
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
}

void createConsumer(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, StonexLogSource* log_src, cms::ExceptionListener* exListener)
{
	try
	{

		auto logger = std::make_shared<Log4CxxLogger>();
		logger->configure("logger.xml");

		if (log_src) {
			logger->attach("com.stonex.app", log_src);
			log_src->info("com.stonex.app", "test started");
		}


		if (log_src) {
			log_src->info("com.stonex.app", "creating connection");
		}

		std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker));

		logger->attach("com.stonex.app.factory", (cms::amqp::CMSConnectionFactory*)factory.get());


		if (exListener)
			factory->setExceptionListener(exListener);

		std::shared_ptr<cms::Connection> connection(factory->createConnection(user, password));
		if (log_src) {
			log_src->info("com.stonex.app", "connection established");
		}

		logger->attach("com.stonex.app.connection", (cms::amqp::CMSConnection*)connection.get());
		if (exListener)
			connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> session(connection->createSession());
		logger->attach("com.stonex.app.session", (cms::amqp::CMSSession*)session.get());

		if (log_src) {
			log_src->info("com.stonex.app", "session established");
		}


		std::shared_ptr<cms::Destination>  testTopic(session->createTopic(address));
		std::shared_ptr<cms::MessageConsumer> consumer(session->createConsumer(testTopic.get()));


		if (log_src) {
			log_src->info("com.stonex.app", "producer created");
		}

		logger->attach("com.stonex.app.consumer", (cms::amqp::CMSMessageProducer*)consumer.get());

		consumer->close();
		session->close();
		connection->close();

		if (log_src) {
			log_src->info("com.stonex.app", "test done\n\n");
		}
	}
	catch (const std::exception& ex)
	{

		if (log_src) {
			log_src->info("com.stonex.app", ex.what());
		}
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
}

void createConsumerWithSelector(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, StonexLogSource* log_src, cms::ExceptionListener* exListener)
{
	try
	{

		auto logger = std::make_shared<Log4CxxLogger>();
		logger->configure("logger.xml");

		if (log_src) {
			logger->attach("com.stonex.app", log_src);
			log_src->info("com.stonex.app", "test started");
		}


		if (log_src) {
			log_src->info("com.stonex.app", "creating connection");
		}

		std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker));

		logger->attach("com.stonex.app.factory", (cms::amqp::CMSConnectionFactory*)factory.get());


		if (exListener)
			factory->setExceptionListener(exListener);

		std::shared_ptr<cms::Connection> connection(factory->createConnection(user, password));
		if (log_src) {
			log_src->info("com.stonex.app", "connection established");
		}

		logger->attach("com.stonex.app.connection", (cms::amqp::CMSConnection*)connection.get());
		if (exListener)
			connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> session(connection->createSession());
		logger->attach("com.stonex.app.session", (cms::amqp::CMSSession*)session.get());

		if (log_src) {
			log_src->info("com.stonex.app", "session established");
		}


		std::shared_ptr<cms::Destination>  testTopic(session->createTopic(address));
		std::shared_ptr<cms::MessageConsumer> consumer(session->createConsumer(testTopic.get(), "REGION=\"UE\" AND ENV=\"TEST\""));


		if (log_src) {
			log_src->info("com.stonex.app", "producer created");
		}

		logger->attach("com.stonex.app.consumer", (cms::amqp::CMSMessageProducer*)consumer.get());

		consumer->close();
		session->close();
		connection->close();

		if (log_src) {
			log_src->info("com.stonex.app", "test done\n\n");
		}
	}
	catch (const std::exception& ex)
	{

		if (log_src) {
			log_src->info("com.stonex.app", ex.what());
		}
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
}

void sendAndReceive(const std::string & producer_user, const std::string & producer_password, const std::string & consumer_user, const std::string & consumer_password, const std::string & broker, const std::string& prodcer_address, const std::string& consumer_address, cms::Destination::DestinationType destType, StonexLogSource * log_src, cms::ExceptionListener * exListener)
{
	assert(destType != cms::Destination::TEMPORARY_QUEUE);
	assert(destType != cms::Destination::TEMPORARY_TOPIC);

	try
	{

		auto logger = std::make_shared<Log4CxxLogger>();
		logger->configure("logger.xml");

		if (log_src) {
			logger->attach("com.stonex.app", log_src);
			log_src->info("com.stonex.app", "test started");
		}


		if (log_src) {
			log_src->info("com.stonex.app", "creating connection");
		}

		std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker));

		logger->attach("com.stonex.app.factory", (cms::amqp::CMSConnectionFactory*)factory.get());


		if (exListener)
			factory->setExceptionListener(exListener);
		
		///producer

		std::shared_ptr<cms::Connection> producer_connection(factory->createConnection(producer_user, producer_password));
		if (log_src) {
			log_src->info("com.stonex.app", "connection established");
		}

		logger->attach("com.stonex.app.connection", (cms::amqp::CMSConnection*)producer_connection.get());
		if (exListener)
			producer_connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> producer_session(producer_connection->createSession());
		logger->attach("com.stonex.app.session", (cms::amqp::CMSSession*)producer_session.get());

		if (log_src) {
			log_src->info("com.stonex.app", "session established");
		}


		cms::Destination*  testAddress = nullptr;
		if (destType == cms::Destination::QUEUE)
			testAddress = producer_session->createQueue(prodcer_address);
		else if (destType == cms::Destination::TOPIC)
			testAddress = producer_session->createTopic(prodcer_address);


		std::shared_ptr<cms::MessageProducer> producer(producer_session->createProducer(testAddress));


		logger->attach("com.stonex.app.producer", (cms::amqp::CMSMessageProducer*)producer.get());

		if (log_src) {
			log_src->info("com.stonex.app", "producer created");
		}

		///consumer

		std::shared_ptr<cms::Connection> consumer_connection(factory->createConnection(consumer_user, consumer_password));
		if (log_src) {
			log_src->info("com.stonex.app", "connection established");
		}

		logger->attach("com.stonex.app.connection", (cms::amqp::CMSConnection*)consumer_connection.get());
		if (exListener)
			consumer_connection->setExceptionListener(exListener);

		std::shared_ptr<cms::Session> consumer_session(consumer_connection->createSession());
		logger->attach("com.stonex.app.session", (cms::amqp::CMSSession*)consumer_session.get());

		if (log_src) {
			log_src->info("com.stonex.app", "session established");
		}


		cms::Destination*  consumerAddress1 = nullptr;
		cms::Destination*  consumerAddress2 = nullptr;
		if (destType == cms::Destination::QUEUE) {
			consumerAddress1 = producer_session->createQueue(consumer_address + "US");
			consumerAddress2 = producer_session->createQueue(consumer_address + "UE");
		}
		else if (destType == cms::Destination::TOPIC) {
			consumerAddress1 = producer_session->createTopic(consumer_address + "US");
			consumerAddress2 = producer_session->createTopic(consumer_address + "UE");
		}

		//std::shared_ptr<cms::MessageConsumer> consumer1(consumer_session->createConsumer(consumerAddress1, "REGION=\"UE\" AND ENV=\"TEST\""));
		//std::shared_ptr<cms::MessageConsumer> consumer2(consumer_session->createConsumer(consumerAddress2, "REGION=\"US\" AND ENV=\"TEST\""));

		std::shared_ptr<cms::MessageConsumer> consumer1(consumer_session->createConsumer(consumerAddress1));
		std::shared_ptr<cms::MessageConsumer> consumer2(consumer_session->createConsumer(consumerAddress2));
	

		MyMessageListener ml1;
		logger->attach("com.stonex.app.listener", &ml1);
		consumer1->setMessageListener(&ml1);
		consumer2->setMessageListener(&ml1);

		if (log_src) {
			log_src->info("com.stonex.app", "consumer created");
		}

		logger->attach("com.stonex.app.consumer", (cms::amqp::CMSMessageConsumer*)consumer1.get());
		logger->attach("com.stonex.app.consumer", (cms::amqp::CMSMessageConsumer*)consumer2.get());

		auto mes1 = producer_session->createTextMessage("hello UE");
		mes1->setStringProperty("REGION", "UE");
		mes1->setStringProperty("ENV", "TEST");

		auto mes2 = producer_session->createTextMessage("hello US");
		mes2->setStringProperty("REGION", "US");
		mes2->setStringProperty("ENV", "TEST");

		std::thread t1([producer, mes1, mes2]() {
			for (int i = 0; i < 10; i++)
			{
				std::this_thread::sleep_for(std::chrono::seconds(2));
				producer->send(mes1);
				producer->send(mes2);
			}
		});

		t1.join();


		consumer1->close();
		consumer2->close();
		producer->close();
		consumer_session->close();
		producer_session->close();
		consumer_connection->close();
		producer_connection->close();

		if (log_src) {
			log_src->info("com.stonex.app", "test done\n\n");
		}
	}
	catch (const std::exception& ex)
	{

		if (log_src) {
			log_src->info("com.stonex.app", ex.what());
		}
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
}
