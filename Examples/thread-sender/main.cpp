
#include "MessageProcessor.h"

#include <Log4CxxLogger/Log4CxxLogger.h>
#include <fmt/format.h>
#include <algorithm>
#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

#include <stdlib.h>
#include <csignal>

#include "../common/ParameterParser .h"


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

		for (int i = 1;i < 1000;i++)
		{
			processor.SendMessage("Hello");

	//		if (i % 10 == 0)
	//			throw std::exception("test crash");

		}
	});

	std::thread senderThread2([&]() {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	//	processor.Cleanup();
		raise(SIGSEGV);
	});

	senderThread2.join();
	senderThread.join();

	return 0;
}
