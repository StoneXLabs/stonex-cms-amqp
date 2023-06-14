
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>
#include <API/CMSMessageProducer.h>

//#include <StdOutLogger/StdOutLogger.h>
#include <Log4CxxLogger/Log4CxxLogger.h>

//#include <fmt/format.h>

class MyExceptionListener : public cms::ExceptionListener
{

	void onException(const cms::CMSException& ex) override { std::cout << ex.getCause() << std::endl; };
};

class MyMessageListener : public cms::MessageListener, public StonexLogSource
{
public:
	

	void onMessage(const cms::Message* message) override {
		if (auto msg = dynamic_cast<const cms::TextMessage*>(message))
			info("MyMessageListener", msg->getText());
	}
};

int main() 
{


	auto logger = std::make_shared<Log4CxxLogger>();
	logger->configure("logger.xml");
	

	auto factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory("localhost:5672?maxReconnectAttempts=1");
	logger->attach("factory",(cms::amqp::CMSConnectionFactory*)factory);

	MyExceptionListener exl;
	try
	{
	//	std::cout << fmt::format("{}", (void *)&exl) << std::endl;;
		factory->setExceptionListener(&exl);

	}
	catch (const std::exception& ex)
	{
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}

	try
	{
		auto connection = factory->createConnection("admin", "admin");
		logger->attach("connection", (cms::amqp::CMSConnection*)connection);

		auto session = connection->createSession();
		logger->attach("session", (cms::amqp::CMSSession*)session);

		auto testQueue = session->createQueue("test_queue");
		auto queueProducer = session->createProducer(testQueue);
		logger->attach("queueProducer", (cms::amqp::CMSMessageProducer*)queueProducer);

		auto testTopic = session->createTopic("test_topic");
		auto topicProducer = session->createProducer(testTopic);
		logger->attach("topicProducer", (cms::amqp::CMSMessageProducer*)topicProducer);


		MyMessageListener testMessageListener;
		logger->attach("MessageListener", &testMessageListener);

		auto consumer1 = session->createConsumer(testQueue);
		consumer1->setMessageListener(&testMessageListener);

		logger->attach("consumer1", (cms::amqp::CMSMessageConsumer*)consumer1);

		auto consumer2 = session->createConsumer(testTopic);
		logger->attach("consumer2", (cms::amqp::CMSMessageConsumer*)consumer2);
		consumer2->setMessageListener(&testMessageListener);

		auto consumer3 = session->createConsumer(testTopic);
		logger->attach("consumer3", (cms::amqp::CMSMessageConsumer*)consumer3);
		consumer3->setMessageListener(&testMessageListener);

		auto message = session->createTextMessage("text message");


		topicProducer->send(message);
		queueProducer->send(message);

		getchar();
	}
	catch (const std::exception&)
	{

	}
	
	return 0;
}