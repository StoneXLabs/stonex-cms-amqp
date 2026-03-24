#include "UserData.pb.h"
#include <cms/MessageListener.h>

#include <queue>
#include <mutex>
#include <chrono>

#include <log4cxx/logger.h>

class MessageFactory
{
	public:
		static UserData createMessage();
		static cms::Message* createBytesMessage(const UserData& message);
		static cms::Message* createTextMessage(const UserData& message);
		static void setGroupId(const std::string& groupId, cms::Message* message);
};

class MyMessageHandler : public cms::MessageListener
{
public:
	MyMessageHandler(const std::string& id, std::chrono::seconds timeout);
	~MyMessageHandler() override;
	void onMessage(const cms::Message* message) override;
	int receivedCount() const;

	void trigger();
	void run();

protected:
	std::set<std::string> receivedGroups;
	int messagesReceived{ 0 };
	std::chrono::seconds timeout{ 1 };
	bool triggered = true;
	std::mutex mutex;
	std::thread thread;
	log4cxx::LoggerPtr mLogger{ log4cxx::Logger::getLogger("App.messageHandler") };
};