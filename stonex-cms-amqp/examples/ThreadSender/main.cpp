
#include "MessageProcessor.h"

#include <Log4CxxLogger/Log4CxxLogger.h>
#include <fmt/format.h>
#include <algorithm>
#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

#include <stdlib.h>
#include <csignal>
class ParameterParser
{
public:
	static std::map<std::string, std::string> parse(int argc, char* argv[])
	{
		std::vector<std::string> arguments(argv, argv + argc);

		std::map < std::string, std::string > params;

		std::transform(std::next(std::begin(arguments)), std::end(arguments), std::inserter(params, params.end()), [](std::string item) {
			auto split = std::find(item.begin(), item.end(), '=');
			if (split != std::end(item))
				return std::make_pair(std::string(item.begin(), split), std::string(std::next(split), item.end()));
			else
				return std::pair<std::string, std::string>();

		});

		params.erase("");

		return params;
	}
};

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


std::string_view param_broker_url = "url";
std::string_view param_user = "user";
std::string_view param_password = "password";
std::string_view param_destination = "destination";
std::string_view param_destination_type = "destination_type";
std::string_view param_data_file = "data_file";
std::string_view param_message_count = "message_count";

std::string_view config_logger = "logger_config";


int main(int argc, char* argv[])
{

	auto params = ParameterParser::parse(argc, argv);

	stonex::logger::initialize(LoggerFactory::LoggerType::LOG4CXX);
	log4cxx::xml::DOMConfigurator::configure(params.at(config_logger.data()));

	MessageProcessor processor;

	processor.Start();
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));

	std::thread senderThread([&]() {

		for (int i = 1;i < 100;i++)
		{
			processor.SendMessage("Hello");

	//		if (i % 10 == 0)
	//			throw std::exception("test crash");

		}
	});

	std::thread senderThread2([&]() {
		std::this_thread::sleep_for(std::chrono::seconds(4));
		raise(SIGSEGV);
	});

//	senderThread2.join();

//	throw std::exception("test crash");
//	cms::Session* ts;
//	ts->close();
	//if(senderThread2.joinable())
	//	senderThread2.join();
	//if (senderThread.joinable())
	//	senderThread.join();
//	while (1) {};
//	processor.SendMessage("Hello");
	return 0;
}
