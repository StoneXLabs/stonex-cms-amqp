#pragma once

#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <cms/TextMessage.h>

#include <logger/StonexLogSource.h>

#include <cms/Destination.h>

class MyExceptionListener : public cms::ExceptionListener, public StonexLogSource
{
public:
	MyExceptionListener(bool expected = false)
		:mExpect(expected)
	{
	}

	~MyExceptionListener() {
		info("com.stonex.app", __FUNCTION__);
		
	}

	void onException(const cms::CMSException& ex) override {
		if (mExpect)
			info("com.stonex.app", "got expected exception test done\n\n");
		else
			error("com.stonex.app", "got unexpected exception test done\n\n");
	};

	void setExpect(bool expect) {
		mExpect = expect;
	}

private:
	bool mExpect;
};

class MyMessageListener : public cms::MessageListener, public StonexLogSource
{
public:

	void onMessage(const cms::Message* message) override {
		if (auto msg = dynamic_cast<const cms::TextMessage*>(message))
			info("com.stonex.app.MyMessageListener", msg->getText());
	}
};

void createConnection(const std::string& user, const std::string& password, const std::string& broker, StonexLogSource* log_src = nullptr, cms::ExceptionListener* exListener = nullptr);
void createConnection2(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, StonexLogSource* log_src = nullptr, cms::ExceptionListener* exListener = nullptr);


void createAddress(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, StonexLogSource* log_src = nullptr, cms::ExceptionListener* exListener = nullptr);

void createConsumer(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, StonexLogSource* log_src = nullptr, cms::ExceptionListener* exListener = nullptr);

void createConsumerWithSelector(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, StonexLogSource* log_src = nullptr, cms::ExceptionListener* exListener = nullptr);

void sendAndReceive(const std::string& producer_user, const std::string& producer_password, const std::string& consumer_user, const std::string& consumer_password, const std::string& broker, const std::string& prodcer_address, const std::string& consumer_address, cms::Destination::DestinationType destType = cms::Destination::QUEUE, StonexLogSource* log_src = nullptr, cms::ExceptionListener* exListener = nullptr);