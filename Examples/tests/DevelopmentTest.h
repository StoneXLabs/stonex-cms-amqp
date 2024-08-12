#pragma once

#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <cms/TextMessage.h>

#include <cms/Destination.h>
#include <LoggerFactory/LoggerFactory.h>
#include "../common/ParameterParser .h"

#include <API/CMSConnectionFactory.h>
#include <API/CMSSession.h>
#include <API/CMSMessageConsumer.h>
#include <API/CMSMessageProducer.h>
#include <functional>
#include <thread>

#include <fmt/format.h>


struct Test
{
	const std::string id_;
	const std::string desc_;
	bool success_;
	std::string error_;
	void setResult(const std::pair<bool, std::string>& result)
	{
		success_ = result.first;
		error_ = result.second;
	}
};

template <>
class fmt::formatter<Test> {
public:
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename Context>
	constexpr auto format(Test const& Test, Context& ctx) const {
		if (Test.success_)
		{
			return format_to(ctx.out(), "{} SUCCESS {}", Test.id_, Test.desc_);
		}
		else
		{
			return format_to(ctx.out(), "{} FAILED {} \nfailed message {}", Test.id_, Test.desc_, Test.error_);
		}
	}
};


class DevelopmentTest
{
public:
	enum class NodeState
	{
		START,
		STOP,
		CLOSE,
		DEFAULT
	};

public:
	DevelopmentTest();

	~DevelopmentTest();

	void initialize(const std::string& broker, const std::string& user, const std::string& password, const std::string& producer_address, const std::string& consumer_address);

	Test test1();
	Test test2();
	Test test3();
	Test test4();
	Test test5();
	Test test6();
	Test test7();
	Test test8();
	Test test9();
	Test test10();
	Test test11();
	Test test12();
	Test test13();
	Test test14();
	Test test15();
	Test test16();
	Test test17();
	Test test18();
	Test test19();
	Test test20();
	Test test21();
	Test test22();
	Test test23();
	Test test24();
	Test test25();
	Test test26();
	Test test27();
	Test test28();
	Test test29();
	Test test30();
	void addResult(const Test result);

	void verify();

	void destructionCombination1();
	void destructionCombination2();
	void destructionCombination3();
	void destructionCombination4();
	void destructionCombination5();
	void destructionCombination6();
	void destructionCombination7();
	void destructionCombination8();
	void destructionCombination9();
	void destructionCombination10();
	void destructionCombination11();
	void destructionCombination12();
	void destructionCombination13();
	void destructionCombination14();
	void destructionCombination15();
	void destructionCombination16();
	void destructionCombination17();
	void destructionCombination18();
	void destructionCombination19();
	void destructionCombination20();
	void destructionCombination21();
	void destructionCombination22();
	void destructionCombination23();
	void destructionCombination24();

private:

	std::thread* produceMessages();

private:
	std::string mBroker;
	std::string mUser;
	std::string mPassword;
	std::string mProducer_address;
	std::string mConsumer_address;

	MyExceptionListener factoryExceptionListener;
	MyExceptionListener connectionExceptionListener;
	MyExceptionListener producerExceptionListener;
	MyExceptionListener consumerExceptionListener;
	MyMessageListener mMessageListener = MyMessageListener("consumer1");
	MyMessageListener mDrainingListener = MyMessageListener("consumer1");

	logger_ptr mLogger;
	logger_ptr mProducer_logger;
	logger_ptr mConsumer_logger;

	cms::ConnectionFactory* mFactory{ nullptr };
	cms::Connection* mConnection{ nullptr };
	cms::Session* mSession{ nullptr };
	cms::MessageConsumer* mConsumer{ nullptr };
	cms::MessageProducer* mProducer{ nullptr };
	cms::Destination* mProducerDestination{ nullptr };
	cms::Destination* mConsumerDestination{ nullptr };
	std::vector<Test> mResults;
};

