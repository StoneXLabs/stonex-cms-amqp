
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>

#include <Log4CxxLogger/Log4CxxLogger.h>
#include <fmt/format.h>
#include <algorithm>
#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>
#include "../common/ParameterParser .h"

std::string_view param_broker_url = "url";
std::string_view param_user = "user";
std::string_view param_password = "password";
std::string_view param_destination_type = "destination_type";
std::string_view param_destination = "destination";
std::string_view param_iddle_timeout = "iddle_timeout";
std::string_view config_logger = "logger_config";
std::string_view param_filter = "filter";


int main(int argc, char* argv[])
{
	auto params = ParameterParser::parse(argc, argv);

	stonex::logger::initialize(LoggerFactory::LoggerType::LOG4CXX);
	log4cxx::xml::DOMConfigurator::configure(params.at(config_logger.data()));


	auto logger = LoggerFactory::getInstance().create("com.stonex.app");

	logger->log(SEVERITY::LOG_INFO, fmt::format("{}", ParameterParser::print(params)));
	MyMessageListener ml("consumer");


	MyExceptionListener* exl = new MyExceptionListener;


	logger->log(SEVERITY::LOG_INFO, "example receiver started");
    logger->log(SEVERITY::LOG_INFO, "creating connection");

	std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(params.at(param_broker_url.data())));

	logger->log(SEVERITY::LOG_INFO, fmt::format("created connection factory {}", params.at(param_broker_url.data())));



	factory->setExceptionListener(exl);

	///consumer

	auto consumer_logger = LoggerFactory::getInstance().create("com.stonex.app.consumer");

	std::shared_ptr<cms::Connection> consumer_connection(factory->createConnection(params.at(param_user.data()), params.at(param_password.data())));
	consumer_logger->log(SEVERITY::LOG_INFO, fmt::format("created consumer connection {} {}", params.at(param_user.data()), params.at(param_password.data())));

	consumer_connection->setExceptionListener(exl);

	std::shared_ptr<cms::Session> consumer_session(consumer_connection->createSession());

	consumer_logger->log(SEVERITY::LOG_INFO, "consumer session established");

	cms::Destination* testAddress = consumer_session->createTopic(params.at(param_destination.data()));


	std::shared_ptr<cms::MessageConsumer> consumer(consumer_session->createConsumer(testAddress));
	consumer->setMessageListener(&ml);


	logger->log(SEVERITY::LOG_INFO, "consumer created");

	bool run = true;
	while (run)
	{
		for (int ch; (ch = getchar()) && ch != '\n';)
		{
			if (ch == 'c')
			{
				consumer_logger->log(SEVERITY::LOG_INFO, "consumer connection stop");
				consumer_connection->stop();
			}
			else if (ch == 'v')
			{
				consumer_logger->log(SEVERITY::LOG_INFO, "consumer connection start");
				consumer_connection->start();
			}
			else if (ch == 'b')
			{
				consumer_logger->log(SEVERITY::LOG_INFO, "consumer connection close");
				consumer_connection->close();

			}
			else if (ch == 'd')
			{
				consumer_logger->log(SEVERITY::LOG_INFO, "consumer session stop");
				consumer_session->stop();
			}
			else if (ch == 'f')
			{
				consumer_logger->log(SEVERITY::LOG_INFO, "consumer session start");
				consumer_session->start();
			}
			else if (ch == 'g')
			{
				consumer_logger->log(SEVERITY::LOG_INFO, "consumer session close");
				consumer_session->close();

			}
			else if (ch == 'e')
			{
				consumer_logger->log(SEVERITY::LOG_INFO, "consumer stop");
				consumer->stop();
			}
			else if (ch == 'r')
			{
				consumer_logger->log(SEVERITY::LOG_INFO, "consumer start");
				consumer->start();
			}
			else if (ch == 't')
			{
				consumer_logger->log(SEVERITY::LOG_INFO, "consumer close");
				consumer->close();

			}
			else if (ch == 'x')
			{
				run = false;
			}
		}
	}

	return 0;
}
