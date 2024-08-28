
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageProducer.h>

#include <Log4CxxLogger/Log4CxxLogger.h>
#include <fmt/format.h>
#include <algorithm>
#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

#include "../common/ParameterParser .h"

std::string_view param_broker_url = "url";
std::string_view param_user = "user";
std::string_view param_password = "password";
std::string_view param_destination = "destination";
std::string_view param_destination_type = "destination_type";
std::string_view param_data_file = "data_file";
std::string_view param_message_count = "message_count";
std::string_view param_message_delay_ms = "message_delay";

std::string_view config_logger = "logger_config";


int main(int argc, char* argv[])
{


	stonex::logger::initialize(LoggerFactory::LoggerType::LOG4CXX);
	auto params = ParameterParser::parse(argc, argv);

	log4cxx::xml::DOMConfigurator::configure(params.at(config_logger.data()));
	auto logger = LoggerFactory::getInstance().create("com.stonex.app.sender");


	logger->log(SEVERITY::LOG_INFO, fmt::format("{}", ParameterParser::print(params)));

	MyExceptionListener* exl = new MyExceptionListener;

	logger->log(SEVERITY::LOG_INFO, "example sender started");

	logger->log(SEVERITY::LOG_INFO, "creating connection");

	std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(params.at(param_broker_url.data())));

	logger->log(SEVERITY::LOG_INFO, fmt::format("created connection factory {}", params.at(param_broker_url.data())));



	factory->setExceptionListener(exl);

	///producer
	auto producer_logger = LoggerFactory::getInstance().create("com.stonex.app.producer");

	try
	{

		cms::Connection* producer_connection(factory->createConnection(params.at(param_user.data()), params.at(param_password.data())));

		producer_logger->log(SEVERITY::LOG_INFO, fmt::format("created producer connection {} {}", params.at(param_user.data()), params.at(param_password.data())));

		producer_connection->setExceptionListener(exl);
	
		cms::Session* producer_session(producer_connection->createSession());
		producer_logger->log(SEVERITY::LOG_INFO, "producer session established");

		cms::Destination* testAddress = producer_session->createTopic(params.at(param_destination.data()));


		cms::MessageProducer* producer(producer_session->createProducer(testAddress));

		producer_logger->log(SEVERITY::LOG_INFO, "producer created");


		std::thread t1([producer, producer_session, &params, &producer_logger]() {

			bool delay = params.find(param_message_delay_ms.data()) != params.end();
			auto delay_ms = std::chrono::milliseconds(std::stoi(params.at(param_message_delay_ms.data())));

			for (int i = 0; i < std::stoi(params.at(param_message_count.data())); i++)
			{
				if (delay)
				{
					std::this_thread::sleep_for(delay_ms);
				}
				

				auto mes = producer_session->createTextMessage(fmt::format("hello UE {}", i));
				mes->setStringProperty("REGION", "UE");
				mes->setStringProperty("ENV", "TEST");

				producer_logger->log(SEVERITY::LOG_INFO, fmt::format("sending message {}", mes->getText()));
				producer->send(mes);
				delete mes;
			}
		});

		t1.join();
	}
	catch (const std::exception& ex)
	{
		logger->log(SEVERITY::LOG_ERROR, ex.what());
	}
	catch (const cms::CMSException& ex)
	{
		logger->log(SEVERITY::LOG_ERROR, ex.what());
	}



	return 0;
}
