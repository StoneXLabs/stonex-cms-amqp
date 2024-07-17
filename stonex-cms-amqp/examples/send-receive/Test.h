#pragma once

#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <cms/TextMessage.h>

#include <cms/Destination.h>
#include <LoggerFactory/LoggerFactory.h>

class MyExceptionListener : public cms::ExceptionListener
{
public:
	explicit MyExceptionListener(bool expected = false)
		:mExpect(expected),
		mLogger(LoggerFactory::getInstance().create("com.stonex.app"))
	{
	}

	~MyExceptionListener() {
		mLogger->log(SEVERITY::LOG_INFO, __FUNCTION__);
		
	}

	void onException(const cms::CMSException& ex) override {
		if (mExpect)
			mLogger->log(SEVERITY::LOG_INFO, "got expected exception test done\n\n");
		else
			mLogger->log(SEVERITY::LOG_ERROR, "got unexpected exception test done\n\n");
	};

	void setExpect(bool expect) {
		mExpect = expect;
	}


private:
	bool mExpect;
	std::shared_ptr<StonexLogger> mLogger;
};

class MyMessageListener : public cms::MessageListener
{
public:
	explicit MyMessageListener(const std::string& logger)
		:mLogger(LoggerFactory::getInstance().create("com.stonex.app" + logger))
	
	{
	}

	void onMessage(const cms::Message* message) override {
		if (auto msg = dynamic_cast<const cms::TextMessage*>(message))
			mLogger->log(SEVERITY::LOG_INFO, msg->getText());
	}

private:
	std::string mLoggerName;
	std::shared_ptr<StonexLogger> mLogger;
};

void createConnection(const std::string& user, const std::string& password, const std::string& broker, cms::ExceptionListener* exListener = nullptr);
void createConnection2(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener = nullptr);


void createAddress(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener = nullptr);

void createConsumer(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener = nullptr);

void createConsumerWithSelector(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener = nullptr);

void sendAndReceive(const std::string& producer_user, const std::string& producer_password, const std::string& consumer_user, const std::string& consumer_password, const std::string& broker, const std::string& prodcer_address, const std::string& consumer_address, cms::Destination::DestinationType destType = cms::Destination::QUEUE, cms::ExceptionListener* exListener = nullptr);