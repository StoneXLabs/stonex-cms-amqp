#include <map>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <LoggerFactory/LoggerFactory.h>

class ParameterParser
{
public:
	static std::map<std::string, std::string> parse(int argc, char* argv[]);
	static std::string print(std::map<std::string, std::string>& map);
};

class MyExceptionListener : public cms::ExceptionListener
{
public:
	explicit MyExceptionListener(bool expected = false);
	~MyExceptionListener();
	void onException(const cms::CMSException& ex) override;
	void setExpect(bool expect);


private:
	bool mExpect;
	std::shared_ptr<StonexLogger> mLogger;
};

class MyMessageListener : public cms::MessageListener
{
public:
	explicit MyMessageListener(const std::string& logger);
	void onMessage(const cms::Message* message) override;
	void setExpectedCount(unsigned int count, bool reset_counter = true);
	void resetMessageCount();
	std::pair<bool, int> check();
	void process();

private:
	std::string mLoggerName;
	std::shared_ptr<StonexLogger> mLogger;
	unsigned int mExpectedMessagesCount{ 0 };
	unsigned int mMessagesCount{ 0 };
};