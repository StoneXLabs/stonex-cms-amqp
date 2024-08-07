
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageProducer.h>

#include <Log4CxxLogger/Log4CxxLogger.h>
#include <fmt/format.h>
#include <algorithm>
#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>


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

	MyExceptionListener* exl = new MyExceptionListener;

	auto logger = LoggerFactory::getInstance().create("com.stonex.app.sender");

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
			for (int i = 0; i < std::stoi(params.at(param_message_count.data())); i++)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));

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
