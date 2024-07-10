
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

	log4cxx::xml::DOMConfigurator::configure(params.at(config_logger.data()));

	StonexLogSource log_src;
	MyExceptionListener* exl = new MyExceptionListener;

	auto logger = std::make_shared<Log4CxxLogger>();
	logger->attach("com.stonex.app", &log_src);

	log_src.info("com.stonex.app.sender", "example sender started");

	log_src.info("com.stonex.app.sender", "creating connection");

	std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(params.at(param_broker_url.data())));

	log_src.info("com.stonex.app.sender", fmt::format("created connection factory {}", params.at(param_broker_url.data())));

	logger->attach("com.stonex.app", (cms::amqp::CMSConnectionFactory*)factory.get());


	factory->setExceptionListener(exl);

	///producer

	std::shared_ptr<cms::Connection> producer_connection(factory->createConnection(params.at(param_user.data()), params.at(param_password.data())));
		log_src.info("com.stonex.app.sender.producer", fmt::format("created producer connection {} {}", params.at(param_user.data()), params.at(param_password.data())));

	logger->attach("com.stonex.app", (cms::amqp::CMSConnection*)producer_connection.get());
	producer_connection->setExceptionListener(exl);

	std::shared_ptr<cms::Session> producer_session(producer_connection->createSession());
	logger->attach("com.stonex.app", (cms::amqp::CMSSession*)producer_session.get());

	log_src.info("com.stonex.app.sender.producer", "producer session established");

	cms::Destination* testAddress = producer_session->createTopic(params.at(param_destination.data()));


	std::shared_ptr<cms::MessageProducer> producer(producer_session->createProducer(testAddress));


	logger->attach("com.stonex.app", (cms::amqp::CMSMessageProducer*)producer.get());
    log_src.info("com.stonex.app.producer", "producer created");


	std::thread t1([producer, producer_session, &params, &log_src]() {
		for (int i = 0; i < std::stoi(params.at(param_message_count.data())); i++)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));

			auto mes = producer_session->createTextMessage(fmt::format("hello UE {}",i));
			mes->setStringProperty("REGION", "UE");
			mes->setStringProperty("ENV", "TEST");

			log_src.info("com.stonex.app.producer", fmt::format("sending message {}",mes->getText()));
			producer->send(mes);
			delete mes;
		}
	});

	t1.join();

	return 0;
}
