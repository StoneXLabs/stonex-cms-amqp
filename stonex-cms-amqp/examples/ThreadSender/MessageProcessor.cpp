#include "MessageProcessor.h"
#include <cms/ConnectionFactory.h>
#include <functional>
#include <cms/Session.h>

void MessageProcessor::Stop()
{
	if (workingThread != 0)
	{
		if (workingThread->joinable())
		{
	//		workingThread->interrupt();
			workingThread->join();
		}
		delete workingThread;
		workingThread = 0;
	}
}

void MessageProcessor::process()
{
	if (connection == nullptr)
	{
		cms::ConnectionFactory* factory = cms::amqp::CMSConnectionFactory::createCMSConnectionFactory("localhost:5672");
		connection = factory->createConnection("CMSTEST","CMSTEST");
		session = connection->createSession();
		cms::Destination* destination = session->createTopic("Transaction");
		producer = session->createProducer(destination);
		delete factory;
		delete destination;
	}
}

void MessageProcessor::SendMessage(const std::string& message)
{
	if (session && producer)
	{
		auto ptr = reinterpret_cast<unsigned char*>(std::malloc(1024 * sizeof(unsigned char)));
		cms::BytesMessage* mess = session->createBytesMessage(ptr,1024);
		std::free(ptr);
	//	cms::TextMessage* mess = session->createTextMessage(message);
		producer->send(mess);
		delete mess;
	}
}

void MessageProcessor::Cleanup()
{
	Stop();
	producer->close();
	session->close();
	connection->close();
}

void MessageProcessor::Start()
{
	try
	{
		workingThread = new std::thread(std::bind(&MessageProcessor::process, this));
	}
	catch (std::exception const& ex)
	{
		std::cout<<ex.what();
	}
}
