#include "samples.h"
#include "MessageHandler.h"
#include <ProtonCppLibrary.h>
#include <ConnectionFactory.h>
#include <Connection.h>
#include <Session.h>
#include <MessageProducer.h>
#include <MessageConsumer.h>

#include "UserData.pb.h"

#include <queue>

void publishSubscribe(int messageCount, Destination producer, std::vector<Destination> consumers, int groupCount)
{
	activemq::library::ActiveMQCPP::initialize_library();
	//prepare messages
	std::queue<cms::Message*> messageQueue;

		for(int i = 0 ; i < messageCount; i++)
		{
			messageQueue.push(MessageFactory::createBytesMessage(MessageFactory::createMessage()));
			if (groupCount > 0)
			{
				messageQueue.back()->setStringProperty(internal::properties::JMSX_GROUP_ID, "group" + std::to_string(i%groupCount));
			}
		}
	
	{
		//start consumers
		auto factory = stonex::amqp::ConnectionFactory::createCMSConnectionFactory("localhost:5672");
		auto connection = factory->createConnection("CMSTEST", "CMSTEST");
		connection->start();
	
		auto session = connection->createSession();
		session->start();
	
		std::vector<cms::MessageConsumer*> consumerObjects;
		std::vector<cms::MessageListener*> messageHandlers;
	
		for(const auto& consumer : consumers)
		{
			switch (consumer.type)
			{
				case cms::Destination::TOPIC:
					consumerObjects.push_back(session->createConsumer(session->createTopic(consumer.name), consumer.selector));
					break;
				case cms::Destination::QUEUE:
					consumerObjects.push_back(session->createConsumer(session->createQueue(consumer.name), consumer.selector));
					break;
				default:
					throw std::runtime_error("Unknown destination type");
					break;
			}
	
			messageHandlers.push_back(new MyMessageHandler("consumer" + std::to_string(consumerObjects.size()), std::chrono::seconds(6)));
			consumerObjects.back()->setMessageListener(messageHandlers.back());
		}
	
		//create producer
		cms::MessageProducer* producerObject;
		switch (producer.type)
		{
		case cms::Destination::TOPIC:
			producerObject = session->createProducer(session->createTopic(producer.name));
			break;
		case cms::Destination::QUEUE:
			producerObject = session->createProducer(session->createQueue(producer.name));
			break;
		default:
			throw std::runtime_error("Unknown destination type");
			break;
		}
	
		while(!messageQueue.empty())
		{
			auto msg = messageQueue.front();
			producerObject->send(msg);
			delete msg;
	
			messageQueue.pop();
		}

		std::this_thread::sleep_for(std::chrono::seconds(5));

		delete producerObject;
		for (auto& consumer : consumerObjects)
		{
			delete consumer;
		}
		for (auto& listener : messageHandlers)
		{
			delete listener;
		}

		delete session;
		delete connection;

		
	}

}

void publishSubscribeText(int messageCount, Destination producer, std::vector<Destination> consumers, int groupCount)
{
	//prepare messages
	activemq::library::ActiveMQCPP::initialize_library();
	std::queue<cms::Message*> messageQueue;

	for (int i = 0; i < messageCount; i++)
	{
		messageQueue.push(MessageFactory::createTextMessage(MessageFactory::createMessage()));
		if (groupCount > 0)
		{
			messageQueue.back()->setStringProperty(internal::properties::JMSX_GROUP_ID, "group" + std::to_string(i % groupCount));
		}
	}

	{
		//start consumers
		auto factory = stonex::amqp::ConnectionFactory::createCMSConnectionFactory("localhost:5672");
		auto connection = factory->createConnection("CMSTEST", "CMSTEST");
		connection->start();

		auto session = connection->createSession();
		session->start();

		std::vector<cms::MessageConsumer*> consumerObjects;
		std::vector<cms::MessageListener*> messageHandlers;

		for (const auto& consumer : consumers)
		{
			switch (consumer.type)
			{
			case cms::Destination::TOPIC:
				consumerObjects.push_back(session->createConsumer(session->createTopic(consumer.name), consumer.selector));
				break;
			case cms::Destination::QUEUE:
				consumerObjects.push_back(session->createConsumer(session->createQueue(consumer.name), consumer.selector));
				break;
			default:
				throw std::runtime_error("Unknown destination type");
				break;
			}

			messageHandlers.push_back(new MyMessageHandler("consumer" + std::to_string(consumerObjects.size()), std::chrono::seconds(6)));
			consumerObjects.back()->setMessageListener(messageHandlers.back());
		}

		//create producer
		cms::MessageProducer* producerObject;
		switch (producer.type)
		{
		case cms::Destination::TOPIC:
			producerObject = session->createProducer(session->createTopic(producer.name));
			break;
		case cms::Destination::QUEUE:
			producerObject = session->createProducer(session->createQueue(producer.name));
			break;
		default:
			throw std::runtime_error("Unknown destination type");
			break;
		}

		while (!messageQueue.empty())
		{
			auto msg = messageQueue.front();
			producerObject->send(msg);
			delete msg;

			messageQueue.pop();
		}

		std::this_thread::sleep_for(std::chrono::seconds(5));

		delete producerObject;
		for (auto& consumer : consumerObjects)
		{
			delete consumer;
		}
		for (auto& listener : messageHandlers)
		{
			delete listener;
		}

		delete session;
		delete connection;


	}

}

//void publishSubscribeGrouping(int messageCount)
//{
//
//	std::queue<UserData> messageQueue;
//
//
//	auto factory = stonex::amqp::ConnectionFactory::createCMSConnectionFactory("localhost:5672");
//	auto connection = factory->createConnection("CMSTEST", "CMSTEST");
//	connection->start();
//
//	auto session = connection->createSession();
//	session->start();
//
//	auto dest = session->createTopic("TESTADDRESS");
//	auto producer = session->createProducer(dest);
//
//	auto consumerDest = session->createTopic("TESTADDRESS::STONEX5");
//	auto consumer1 = session->createConsumer(consumerDest);
//	delete consumerDest;
//
//	MyMessageHandler msgHandler1("consumer1", std::chrono::seconds(6));
//	consumer1->setMessageListener(&msgHandler1);
//
//	consumerDest = session->createTopic("TESTADDRESS::STONEX5");
//	auto consumer2 = session->createConsumer(consumerDest);
//	MyMessageHandler msgHandler2("consumer2", std::chrono::seconds(6));
//	consumer2->setMessageListener(&msgHandler2);
//
//	for (int i = 0; i < messageCount; i++)
//	{
//		auto msg = MessageFactory::createMessage();
//		messageQueue.push(msg);
//	}
//
//	int messagesSent{ 0 };
//
//	while (!messageQueue.empty())
//	{
//		auto msg = messageQueue.front();
//
//		int size = msg.ByteSize();
//
//		unsigned char* array = new unsigned char[size];
//		msg.SerializeToArray(array, size);
//		auto byteMessage = session->createBytesMessage(array, size);
//		delete[] array;
//		if (messagesSent % 2 == 0 && messagesSent < 5)
//			byteMessage->setStringProperty("JMSXGroupID", "group1");
//		else
//			byteMessage->setStringProperty("JMSXGroupID", "group2");
//		producer->send(byteMessage);
//		messagesSent++;
//
//		messageQueue.pop();
//	}
//
//	std::this_thread::sleep_for(std::chrono::seconds(5));
//}
//
//
//void publishSubscribeTemporaryTopic(int messageCount)
//{
//	
//	std::queue<UserData> messageQueue;
//
//	for(int i = 0 ; i < messageCount; i++)
//	{
//		messageQueue.push(MessageFactory::createMessage());
//	}
//
//
//    auto factory = stonex::amqp::ConnectionFactory::createCMSConnectionFactory("localhost:5672");
//    auto connection = factory->createConnection("CMSTEST", "CMSTEST");
//    connection->start();
//
//	auto session = connection->createSession();
//	session->start();
//
//	auto dest = session->createTemporaryTopic();
//	auto producer = session->createProducer(dest);
//	
//	// auto consumerDest = session->createTopic("TESTADDRESS::STONEX");
//	// auto consumer1 = session->createConsumer(consumerDest);
//	// delete consumerDest;
//
//	// MyMessageHandler msgHandler1("consumer1",messageCount);
//	// consumer1->setMessageListener(&msgHandler1);
//
//	// consumerDest = session->createTopic("TESTADDRESS::STONEX2");
//	// auto consumer2 = session->createConsumer(consumerDest);
//	// MyMessageHandler msgHandler2("consumer2",messageCount);
//	// consumer2->setMessageListener(&msgHandler2);
//
//	// while(!messageQueue.empty())
//	// {
//	// 	auto msg = messageQueue.front();
//
//	// 	msgHandler1.addMessage(msg);
//	// 	msgHandler2.addMessage(msg);
//	// 	int size = msg.ByteSize();
//		
//	// 	unsigned char* array = new unsigned char[size];
//	// 	msg.SerializeToArray(array, size);
//	// 	auto byteMessage = session->createBytesMessage(array, size);
//	// 	delete[] array;
//	// 	producer->send(byteMessage);
//
//	// 	messageQueue.pop();
//	// }
//	
//	std::this_thread::sleep_for(std::chrono::seconds(30));
//}