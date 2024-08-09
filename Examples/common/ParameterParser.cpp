#include "ParameterParser .h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <cms/TextMessage.h>
#include <fmt/format.h>

std::map<std::string, std::string> ParameterParser::parse(int argc, char* argv[])
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


std::string ParameterParser::print(std::map<std::string, std::string>& map)
{
	std::stringstream ss;
	ss << "Parameters\n";
	for (const auto&[key, value] : map)
	{
		ss << key << " : " << value << "\n";
	}

	return ss.str();
}



MyExceptionListener::MyExceptionListener(bool expected)
	:mExpect(expected),
	mLogger(LoggerFactory::getInstance().create("com.stonex.app"))
{
}

MyExceptionListener::~MyExceptionListener()
{
	mLogger->log(SEVERITY::LOG_INFO, __FUNCTION__);

}

void MyExceptionListener::onException(const cms::CMSException& ex)
{
	if (mExpect)
		mLogger->log(SEVERITY::LOG_INFO, "got expected exception test done\n\n");
	else
		mLogger->log(SEVERITY::LOG_ERROR, "got unexpected exception test done\n\n");
};

void MyExceptionListener::setExpect(bool expect)
{
	mExpect = expect;
}


MyMessageListener::MyMessageListener(const std::string& logger)
	:mLogger(LoggerFactory::getInstance().create("com.stonex.app" + logger))

{
}

void MyMessageListener::onMessage(const cms::Message* message){
	if (auto msg = dynamic_cast<const cms::TextMessage*>(message))
		mLogger->log(SEVERITY::LOG_INFO, msg->getText());
	mMessagesCount++;
}

void MyMessageListener::setExpectedCount(unsigned int count, bool reset_counter)
{
	mExpectedMessagesCount = count;
	if (reset_counter)
	{
		resetMessageCount();
	}
}

void MyMessageListener::resetMessageCount()
{
	mMessagesCount = 0;
}

std::pair<bool, int> MyMessageListener::check()
{
	int diff = mExpectedMessagesCount - mMessagesCount;
	bool ok = diff == 0;


	if (ok)
	{
		mLogger->log(SEVERITY::LOG_INFO, "received expected number of messages");
	}
	else
	{
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("expected messages - received messages = {}", diff));
	}

	return { ok, diff };
}