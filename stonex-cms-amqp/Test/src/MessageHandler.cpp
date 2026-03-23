#include "MessageHandler.h"
#include <cms/BytesMessage.h>
#include "BytesMessage.h"
#include "TextMessage.h"

#include <google/protobuf/util/message_differencer.h>
#include "Protocol/utils.h"
#include <iostream>


UserData MessageFactory::createMessage()
{
	UserData message;
	message.set_name("John");
	message.set_surname("Doe");
	message.set_age(30);
	Address* address = message.mutable_address();
	address->set_country("USA");
	address->set_city("New York");
	address->set_street("5th Avenue");
	address->set_postalcode("10001");
	return message;
}


cms::Message* MessageFactory::createBytesMessage(const UserData& message)
{

	unsigned char* array = new unsigned char[message.ByteSize()];
	message.SerializeToArray(array, message.ByteSize());
	stonex::amqp::BytesMessage* bytesMessage = new stonex::amqp::BytesMessage(array, message.ByteSize());
	delete[] array;

	bytesMessage->setBooleanProperty("bool", true);
	bytesMessage->setByteProperty("byte", 0x42);
	bytesMessage->setDoubleProperty("double", 3.14);
	bytesMessage->setFloatProperty("float", 2.71f);
	bytesMessage->setIntProperty("int", 42);
	bytesMessage->setLongProperty("long", 123456789L);
	bytesMessage->setShortProperty("short", 123);
	bytesMessage->setStringProperty("string", "Hello, World!");

	bytesMessage->setCMSCorrelationID("correlation-id-123");
	bytesMessage->setCMSPriority(5);

	return bytesMessage;
}

cms::Message* MessageFactory::createTextMessage(const UserData& message)
{
	unsigned char* array = new unsigned char[message.ByteSize()];
	message.SerializeToArray(array, message.ByteSize());
	stonex::amqp::TextMessage* textMessage = new stonex::amqp::TextMessage(message.DebugString());
	delete[] array;

	textMessage->setBooleanProperty("bool", true);
	textMessage->setByteProperty("byte", 0x42);
	textMessage->setDoubleProperty("double", 3.14);
	textMessage->setFloatProperty("float", 2.71f);
	textMessage->setIntProperty("int", 42);
	textMessage->setLongProperty("long", 123456789L);
	textMessage->setShortProperty("short", 123);
	textMessage->setStringProperty("string", "Hello, World!");

	textMessage->setCMSCorrelationID("correlation-id-123");
	textMessage->setCMSPriority(5);

	return textMessage;
}

void MessageFactory::setGroupId(const std::string& groupId, cms::Message* message)
{
	message->setStringProperty(internal::properties::JMSX_GROUP_ID, groupId);
}

MyMessageHandler::MyMessageHandler(const std::string& id, std::chrono::seconds timeout)
	:timeout{ timeout }, thread([this]() {run(); })
{
	
}

MyMessageHandler::~MyMessageHandler()
{
	if (thread.joinable())
		thread.join();

	for(const auto& groupId : receivedGroups)
	{
		LOG4CXX_INFO(mLogger, std::format("Received messages from groups: {}", groupId));
	}
	
}

void MyMessageHandler::onMessage(const cms::Message* message)
{
	trigger();
	if(message->propertyExists(internal::properties::JMSX_GROUP_ID))
	{
		std::string groupId = message->getStringProperty(internal::properties::JMSX_GROUP_ID);
		if (receivedGroups.find(groupId) == receivedGroups.end())
		{
			LOG4CXX_INFO(mLogger, std::format("Received new group: {}", groupId));
			receivedGroups.insert(groupId);
		}
	}

	messagesReceived++;
	delete message;
}

int MyMessageHandler::receivedCount() const
{
	return messagesReceived;
}

void MyMessageHandler::trigger()
{
	std::lock_guard<std::mutex> lock(mutex);
	triggered = true;
}

void MyMessageHandler::run()
{
	while (triggered)
	{
		{
			std::lock_guard<std::mutex> lock(mutex);
			triggered = false; // reset the trigger
		}

		std::this_thread::sleep_for(timeout);
	}
}