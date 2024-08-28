#include "DevelopmentTest.h"


DevelopmentTest::DevelopmentTest()
		:mLogger(LoggerFactory::getInstance().create("com.stonex.app")),
		mProducer_logger(LoggerFactory::getInstance().create("com.stonex.app.producer1")),
		mConsumer_logger(LoggerFactory::getInstance().create("com.stonex.app.consumer1"))
	{}

DevelopmentTest::~DevelopmentTest()
{
	if (mFactory)
		delete mFactory;
	if (mConnection)
		delete mConnection;
	if (mSession)
		delete mSession;
	if (mConsumer)
		delete mConsumer;
	if (mProducer)
		delete mProducer;
}

void DevelopmentTest::initialize(const std::string& broker, const std::string& user, const std::string& password, const std::string& producer_address, const std::string& consumer_address)
{
	mBroker = broker;
	mUser = user;
	mPassword = password;
	mProducer_address = producer_address;
	mConsumer_address = consumer_address;


	mLogger->log(SEVERITY::LOG_INFO, fmt::format("initialize tests:\n{}\n{}\n{}\n{}\n{}", mBroker, mUser, mPassword, mProducer_address, mConsumer_address));


	mLogger->log(SEVERITY::LOG_INFO, "creating connection");
	mFactory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(broker);


	mLogger->log(SEVERITY::LOG_INFO, fmt::format("created connection factory {}", broker));

	mFactory->setExceptionListener(&factoryExceptionListener);

	///producer

	mConnection = mFactory->createConnection(user, password);

	mLogger->log(SEVERITY::LOG_INFO, fmt::format("created producer connection {} {}", user, password));

	mConnection->setExceptionListener(&connectionExceptionListener);

	mSession = mConnection->createSession();

	mLogger->log(SEVERITY::LOG_INFO, "producer session established");


	mProducerDestination = mSession->createTopic(mProducer_address);
	mProducer = mSession->createProducer(mProducerDestination);

	mProducer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(mProducerDestination)->getTopicName()));



	///consumer 1


	mConsumerDestination = mSession->createTopic(mConsumer_address);
	mConsumer = mSession->createConsumer(mConsumerDestination);
	//	consumer->start();
	mConsumer_logger->log(SEVERITY::LOG_INFO, fmt::format("created consumer {}", dynamic_cast<cms::Topic*>(mConsumerDestination)->getTopicName()));

	mConsumer->setMessageListener(&mMessageListener);
}


void DevelopmentTest::reinitialize()
{
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	if (mConnection)
	{
		delete mConnection;
		mConnection = nullptr;
	}
	if (mSession)
	{
		delete mSession;
		mSession = nullptr;
	}
	if (mConsumer)
	{
		delete mConsumer;
		mConsumer = nullptr;
	}
	if (mProducer)
	{
		delete mProducer;
		mProducer = nullptr;
	}
	mMessageListener.setExpectedCount(0);
	initialize(mBroker, mUser, mPassword, mProducer_address, mConsumer_address);
}

Test DevelopmentTest::test1()
{

	Test result({ __func__,"sending 15 messages on created connection, expecting no message is consumed",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mMessageListener.setExpectedCount(0);
	auto sender = produceMessages();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	sender->join();
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test2()
{

	Test result({ __func__,"starting connection, expecting consumer consumes all 15 messages from previous test",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mMessageListener.setExpectedCount(15);
	mConnection->start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test3()
{

	Test result({ __func__,"stoping connection, sedning 15 messages, expecting consumer consumes no messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mConnection->stop();
	mMessageListener.setExpectedCount(0);
	auto sender = produceMessages();
	sender->join();
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test4()
{

	Test result({ __func__,"starting session, expecting 15 messages consumed",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mMessageListener.setExpectedCount(15);
	mSession->start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test5()
{

	Test result({ __func__,"stoping connection, sedning 15 messages, expecting consumer consumes no messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mConnection->stop();
	mMessageListener.setExpectedCount(0);
	auto sender = produceMessages();
	sender->join();
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test6()
{

	Test result({ __func__,"starting consumer, expecting consumer consumes all 15 messages from previous test",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mMessageListener.setExpectedCount(15);
	mConsumer->start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test7()
{

	Test result({ __func__,"stoping consumer, sedning 15 messages, expecting consumer consumes no messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mConnection->stop();
	mMessageListener.setExpectedCount(0);
	auto sender = produceMessages();
	sender->join();
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test8()
{

	Test result({ __func__, "starting connection, expecting consumer consumes all 15 messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	mMessageListener.setExpectedCount(15);
	mConnection->start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test9()
{

	Test result({ __func__,"stoping session, sedning 15 messages, expecting consumer consumes no messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mSession->stop();
	mMessageListener.setExpectedCount(0);
	auto sender = produceMessages();
	sender->join();
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test10()
{

	Test result({ __func__,"starting connection, expecting consumer consumes all 15 messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	mMessageListener.setExpectedCount(15);
	mConnection->start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test11()
{

	Test result({ __func__, "stoping consumer, sedning 15 messages, expecting consumer consumes no messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mConsumer->stop();
	mMessageListener.setExpectedCount(0);
	auto sender = produceMessages();
	sender->join();
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test12()
{

	Test result({ __func__,"starting session, expecting consumer consumes all 15 messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	mMessageListener.setExpectedCount(15);
	mSession->start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test13()
{

	Test result({ __func__,"stoping session, sedning 15 messages, expecting consumer consumes no messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mSession->stop();
	mMessageListener.setExpectedCount(0);
	auto sender = produceMessages();
	sender->join();
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test14()
{

	Test result({ __func__,"starting consumer, expecting consumer consumes all 15 messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	mMessageListener.setExpectedCount(15);
	mConsumer->start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test15()
{

	Test result({ __func__,"stoping session, sedning 15 messages, expecting consumer consumes no messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mSession->stop();
	mMessageListener.setExpectedCount(0);
	auto sender = produceMessages();
	sender->join();
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test16()
{

	Test result({ __func__,"starting consumer, expecting consumer consumes all 15 messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	mMessageListener.setExpectedCount(15);
	mConsumer->start();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	result.setResult(mMessageListener.check());
	return result;
}

Test DevelopmentTest::test17()
{

	Test result({ __func__, "stoping session, sedning 15 messages, expecting consumer consumes no messages",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	mConsumer->stop();
	mMessageListener.setExpectedCount(0);
	auto sender = produceMessages();
	sender->join();
	result.setResult(mMessageListener.check());
	return result;
}
////////////////////////////
Test DevelopmentTest::test18()
{

	Test result({ __func__, "creating session on stoped connection",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	cms::Session* session2 = nullptr;

	try
	{
		mConnection->stop();
		session2 = mConnection->createSession();
		mLogger->log(SEVERITY::LOG_INFO, "creating session on stoped connection SUCCESS");
		delete session2;
		session2 = nullptr;
		result.success_ = true;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("Exception creating session on colsed connection {}", ex.what()));
		result.error_ = ex.what();
		result.success_ = false;
	}
	return result;
}

Test DevelopmentTest::test19()
{

	Test result({ __func__, "creating session on started connection",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	cms::Session* session2 = nullptr;

	try
	{
		mConnection->start();
		session2 = mConnection->createSession();
		mLogger->log(SEVERITY::LOG_INFO, "creating session on started connection SUCCESS");
		delete session2;
		session2 = nullptr;
		result.success_ = true;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("Exception creating session on colsed connection {}", ex.what()));
		result.error_ = ex.what();
		result.success_ = false;
	}
	return result;
}

Test DevelopmentTest::test20()
{

	Test result({ __func__, "closing consumer",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		mConsumer->close();
		mLogger->log(SEVERITY::LOG_INFO, "closing consumer SUCCESS");
		result.success_ = true;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("Exception closing consumer {}", ex.what()));
		result.error_ = ex.what();
		result.success_ = false;
	}
	return result;
}

Test DevelopmentTest::test21()
{

	Test result({ __func__, "closing closed consumer",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		mConsumer->close();
		mLogger->log(SEVERITY::LOG_INFO, "closing closed consumer SUCCESS");
		result.success_ = true;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("Exception closing closed consumer {}", ex.what()));
		result.error_ = ex.what();
		result.success_ = false;
	}
	return result;
}

Test DevelopmentTest::test22()
{

	Test result({ __func__, "starting closed consumer",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		mConsumer->start();
		mLogger->log(SEVERITY::LOG_ERROR, "starting closed consumer ERROR, expected exception thrown");
		result.error_ = "starting closed consumer ERROR, expected exception thrown";
		result.success_ = false;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception starting closed consumer {}", ex.what()));
		result.success_ = true;
	}
	return result;
}

Test DevelopmentTest::test23()
{

	Test result({ __func__, "stoping closed consumer",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		mConsumer->stop();
		mLogger->log(SEVERITY::LOG_ERROR, "starting closed consumer ERROR, expected exception thrown");
		result.error_ = "starting closed consumer ERROR, expected exception thrown";
		result.success_ = false;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception starting closed consumer {}", ex.what()));
		result.success_ = true;
	}
	return result;
}


Test DevelopmentTest::test24()
{

	Test result({ __func__, "closing session",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		mSession->close();
		mLogger->log(SEVERITY::LOG_INFO, "closing session SUCCESS");
		result.success_ = true;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("Exception closing session {}", ex.what()));
		result.success_ = false;
	}
	return result;
}

Test DevelopmentTest::test25()
{

	Test result({ __func__, "closing closed session",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		mSession->close();
		mLogger->log(SEVERITY::LOG_INFO, "closing closed session SUCCESS");
		result.success_ = true;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("Exception closing closed session {}", ex.what()));
		result.success_ = false;
	}
	return result;
}

Test DevelopmentTest::test26()
{

	Test result({ __func__, "starting closed session",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		mSession->start();
		mLogger->log(SEVERITY::LOG_ERROR, "starting closed session ERROR, expected exception thrown");
		result.error_ = "starting closed session ERROR, expected exception thrown";
		result.success_ = false;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception starting closed session {}", ex.what()));
		result.success_ = true;
	}
	return result;
}
///////////////////
Test DevelopmentTest::test27()
{

	Test result({ __func__, "creating consumer on closed session",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);
	cms::MessageConsumer* consumer2 = nullptr;
	try
	{
		consumer2 = mSession->createConsumer(mConsumerDestination);
		mLogger->log(SEVERITY::LOG_ERROR, "creating consumer on closed session ERROR, expected exception thrown");
		result.error_ = "creating consumer on closed session ERROR, expected exception thrown";
		result.success_ = false;
		delete consumer2;
		consumer2 = nullptr;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception creating consumer on closed session {}", ex.what()));
		result.success_ = true;
	}
	return result;
}

Test DevelopmentTest::test28()
{

	cms::MessageProducer* producer2 = nullptr;
	Test result({ __func__, "creating producer on closed session",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		producer2 = mSession->createProducer(mProducerDestination);
		mLogger->log(SEVERITY::LOG_ERROR, "creating producer on closed session ERROR, expected exception thrown");
		result.error_ = "creating producer on closed session ERROR, expected exception thrown";
		result.success_ = false;
		delete producer2;
		producer2 = nullptr;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception creating producer on closed session {}", ex.what()));
		result.success_ = true;
	}
	return result;
}

Test DevelopmentTest::test29()
{
	cms::TextMessage* message2;
	Test result({ __func__, "creating message on closed session",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		message2 = mSession->createTextMessage("");
		mLogger->log(SEVERITY::LOG_ERROR, "creating message on closed session ERROR, expected exception thrown");
		result.error_ = "creating consumer on closed session ERROR, expected exception thrown";
		result.success_ = false;
		delete message2;
		message2 = nullptr;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception creating message on closed session {}", ex.what()));
		result.success_ = true;
	}
	return result;
}

Test DevelopmentTest::test30()
{
	cms::Destination* destination2;
	Test result({ __func__, "creating destination on closed session",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		destination2 = mSession->createQueue(mProducer_address);
		mLogger->log(SEVERITY::LOG_ERROR, "creating destination on closed session ERROR, expected exception thrown");
		result.error_ = "creating destination on closed session ERROR, expected exception thrown";
		result.success_ = false;
		delete destination2;
		destination2 = nullptr;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception creating destination on closed session {}", ex.what()));
		result.success_ = true;
	}
	return result;
}

Test DevelopmentTest::test31()
{
	Test result({ __func__, "sending messages on closed connection",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
//		mConnection->start();

		auto message = mSession->createTextMessage("");
		mConnection->close();
		mProducer->send(message);
		mLogger->log(SEVERITY::LOG_ERROR, "sending message on closed connection ERROR, expected exception thrown");
		result.error_ = "csending message on closed connection ERROR, expected exception thrown";
		result.success_ = false;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception sending message on closed connection {}", ex.what()));
		result.success_ = true;
	}
	return result;
}

Test DevelopmentTest::test32()
{
	Test result({ __func__, "sending messages on closed session",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		auto message = mSession->createTextMessage("");
		mSession->close();
		mProducer->send(message);
		mLogger->log(SEVERITY::LOG_ERROR, "sending message on closed session ERROR, expected exception thrown");
		result.error_ = "csending message on closed session ERROR, expected exception thrown";
		result.success_ = false;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception sending message on closed session {}", ex.what()));
		result.success_ = true;
	}
	return result;
}

Test DevelopmentTest::test33()
{
	Test result({ __func__, "sending messages on closed  producer",false,"" });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("-------------------step {}-------------------", __func__));
	mLogger->log(SEVERITY::LOG_INFO, result.desc_);

	try
	{
		auto message = mSession->createTextMessage("");
		mProducer->close();
		mProducer->send(message);
		mLogger->log(SEVERITY::LOG_ERROR, "sending message on closed producer ERROR, expected exception thrown");
		result.error_ = "csending message on closed producer ERROR, expected exception thrown";
		result.success_ = false;
	}
	catch (const std::exception& ex)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("Exception sending message on closed connection {}", ex.what()));
		result.success_ = true;
	}
	return result;
}




void DevelopmentTest::addResult(const Test result)
{
	mResults.emplace_back(result);
}

void DevelopmentTest::verify()
{
	int success{ 0 };
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("----------------------TESTS DONE {} ----------------------", mResults.size()));
	for (const auto& result : mResults)
	{
		if (result.success_)
		{
			success++;
			mLogger->log(SEVERITY::LOG_INFO, fmt::format("{}", result));
		}

	}

	mLogger->log(SEVERITY::LOG_INFO, fmt::format("----------------------FAILED TESTS {}----------------------", mResults.size() - success));

	for (const auto& result : mResults)
	{
		if (!result.success_)
			mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{}", result));
	}
}


std::thread* DevelopmentTest::produceMessages(unsigned int messagesToSend)
{
	std::thread* senderThread = new std::thread([this, messagesToSend]() {
		cms::MessageProducer* producer = nullptr;
		cms::Message* message = nullptr;
		try
		{
			cms::MessageProducer* producer = mSession->createProducer(mProducerDestination);

			mProducer_logger->log(SEVERITY::LOG_INFO, fmt::format("producer created {}", dynamic_cast<cms::Topic*>(mProducerDestination)->getTopicName()));

			mLogger->log(SEVERITY::LOG_INFO, "sending 15 messages on stoped connection");

			for (int i = 0; i < messagesToSend; i++)
			{
				//	std::this_thread::sleep_for(std::chrono::seconds(2));
				message = mSession->createTextMessage(fmt::format("hello {}", i));
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
			mProducer_logger->log(SEVERITY::LOG_ERROR, fmt::format("producer thread exception {}", ex.what()));
			throw;
		}

	});

	return senderThread;
}

void DevelopmentTest::destructionCombination1()
{
	delete mConnection;
	delete mSession;
	delete mConsumer;
	delete mProducer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination2()
{
	delete mConnection;
	delete mSession;
	delete mProducer;
	delete mConsumer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination3()
{
	delete mConnection;
	delete mConsumer;
	delete mSession;
	delete mProducer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination4()
{
	delete mConnection;
	delete mConsumer;
	delete mProducer;
	delete mSession;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination5()
{
	delete mProducer;
	delete mConsumer;
	delete mSession;
	delete mConnection;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination6()
{
	delete mConnection;
	delete mProducer;
	delete mSession;
	delete mConsumer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination7()
{
	delete mConnection;
	delete mProducer;
	delete mConsumer;
	delete mSession;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination8()
{
	delete mSession;
	delete mConnection;
	delete mConsumer;
	delete mProducer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination9()
{
	delete mSession;
	delete mConnection;
	delete mProducer;
	delete mConsumer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination10()
{
	delete mSession;
	delete mConsumer;
	delete mConnection;
	delete mProducer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination11()
{
	delete mSession;
	delete mConsumer;
	delete mProducer;
	delete mConnection;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination12()
{
	delete mSession;
	delete mProducer;
	delete mConnection;
	delete mConsumer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination13()
{
	delete mSession;
	delete mProducer;
	delete mConsumer;
	delete mConnection;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination14()
{
	delete mConsumer;
	delete mConnection;
	delete mSession;
	delete mProducer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination15()
{
	delete mConsumer;
	delete mConnection;
	delete mProducer;
	delete mSession;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination16()
{
	delete mConsumer;
	delete mSession;
	delete mConnection;
	delete mProducer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination17()
{
	delete mConsumer;
	delete mSession;
	delete mProducer;
	delete mConnection;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination18()
{
	delete mConsumer;
	delete mProducer;
	delete mConnection;
	delete mSession;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination19()
{
	delete mConsumer;
	delete mProducer;
	delete mSession;
	delete mConnection;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination20()
{
	delete mProducer;
	delete mConnection;
	delete mSession;
	delete mConsumer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination21()
{
	delete mProducer;
	delete mConnection;
	delete mConsumer;
	delete mSession;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination22()
{
	delete mProducer;
	delete mSession;
	delete mConnection;
	delete mConsumer;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination23()
{
	delete mProducer;
	delete mSession;
	delete mConsumer;
	delete mConnection;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}

void DevelopmentTest::destructionCombination24()
{
	delete mProducer;
	delete mConsumer;
	delete mConnection;
	delete mSession;
	if (mFactory)
	{
		delete mFactory;
		mFactory = nullptr;
	}
	mConnection = nullptr;
	mSession = nullptr;
	mConsumer = nullptr;
	mProducer = nullptr;
}