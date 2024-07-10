
#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>

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

class MyMessageListener : public cms::MessageListener, public StonexLogSource
{
public:
	explicit MyMessageListener(const std::string& logger)
	{
		mLoggerName = "com.stonex.app." + logger;
	}

	void onMessage(const cms::Message* message) override {
		if (auto msg = dynamic_cast<const cms::TextMessage*>(message))
			info(mLoggerName.c_str(), msg->getText());
	}

private:
	std::string mLoggerName;
};

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

	MyMessageListener ml("consumer");

	log4cxx::xml::DOMConfigurator::configure(params.at(config_logger.data()));

	StonexLogSource log_src;
	MyExceptionListener* exl = new MyExceptionListener;

	auto logger = std::make_shared<Log4CxxLogger>();
	logger->attach("com.stonex.app", &log_src);

	log_src.info("com.stonex.app", "example receiver started");
    log_src.info("com.stonex.app", "creating connection");

	std::shared_ptr<cms::ConnectionFactory> factory(cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(params.at(param_broker_url.data())));

	log_src.info("com.stonex.app", fmt::format("created connection factory {}", params.at(param_broker_url.data())));

	logger->attach("com.stonex.app", (cms::amqp::CMSConnectionFactory*)factory.get());


	factory->setExceptionListener(exl);

	///consumer

	std::shared_ptr<cms::Connection> consumer_connection(factory->createConnection(params.at(param_user.data()), params.at(param_password.data())));
	log_src.info("com.stonex.app.consumer", fmt::format("created consumer connection {} {}", params.at(param_user.data()), params.at(param_password.data())));

	logger->attach("com.stonex.app", (cms::amqp::CMSConnection*)consumer_connection.get());
	consumer_connection->setExceptionListener(exl);

	std::shared_ptr<cms::Session> consumer_session(consumer_connection->createSession());
	logger->attach("com.stonex.app", (cms::amqp::CMSSession*)consumer_session.get());

	log_src.info("com.stonex.app.consumer", "consumer session established");

	cms::Destination* testAddress = consumer_session->createTopic(params.at(param_destination.data()));


	std::shared_ptr<cms::MessageConsumer> consumer(consumer_session->createConsumer(testAddress));
	consumer->setMessageListener(&ml);


	logger->attach("com.stonex.app", (cms::amqp::CMSMessageConsumer*)consumer.get());

	log_src.info("com.stonex.app", "consumer created");

	while (1)
	{
		for (int ch; (ch = getchar()) && ch != '\n';)
		{
			if (ch == 's')
			{
				log_src.info("com.stonex.app.consumer", "consumer connection stop");
				consumer_connection->stop();
			}
			else if (ch == 'r')
			{
				log_src.info("com.stonex.app.consumer", "consumer connection start");
				consumer_connection->start();
			}
			else if (ch == 'x')
			{
				break;
			}
		}
	}

	return 0;
}
