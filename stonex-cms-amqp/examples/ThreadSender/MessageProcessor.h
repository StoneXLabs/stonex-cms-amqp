#pragma once
#include <API/CMSConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>

#include <thread>

class MessageProcessor
{
private:
	void Stop();
	void process();
public:
	void SendMessage(const std::string& message);
	void Cleanup();
	void Start();
	void CloseProducer()
	{
		producer->close();
	}
	~MessageProcessor(){
		delete connection;
//		delete session;
//		delete producer;
		workingThread->join();
	}
private:
	cms::Connection* connection = nullptr;
	cms::Session* session = nullptr;
	std::thread* workingThread = nullptr;
	cms::MessageProducer* producer = nullptr;
};