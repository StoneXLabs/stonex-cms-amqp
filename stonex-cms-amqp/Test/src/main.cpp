#include <ConnectionFactory.h>
#include <Connection.h>
#include <Session.h>
#include <MessageProducer.h>
#include <MessageConsumer.h>
#include "ProtonCppLibrary.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "UserData.pb.h"

#include <numeric>

class MyMessageHandler : public cms::MessageListener
{
public:
	void onMessage(const cms::Message* message) override
	{

		mLogger->info(
			"cmsMessage headers:\n"
			"CorrelationId: {}\nDeliveryMode: {}\nDestination: {}\nExpiration: {}\nMessageId: {}\nPriority: {}\n"
			"Redelivered: {}\nReplyTo: {}\nTimestamp: {}\nType: {}\n",
			message->getCMSCorrelationID(),
			message->getCMSDeliveryMode(),
			message->getCMSDestination() ? internal::DestinationConverter::address(message->getCMSDestination()) : "",
			message->getCMSExpiration(),
			"message->getCMSMessageID()",
			message->getCMSPriority(),
			message->getCMSRedelivered(),
			message->getCMSReplyTo() ? internal::DestinationConverter::address(message->getCMSReplyTo()) : "",
			message->getCMSTimestamp(),
			message->getCMSType()
		);

		if(dynamic_cast<const cms::TextMessage*>(message))
		{
			auto textMessage = dynamic_cast<const cms::TextMessage*>(message);
			mLogger->info("TextMessage content: {}", textMessage->getText());
		}
		else if(dynamic_cast<const cms::BytesMessage*>(message))
		{
			auto bytesMessage = dynamic_cast<const cms::BytesMessage*>(message);

			{
				UserData userData;
				userData.ParseFromArray(bytesMessage->getBodyBytes(), bytesMessage->getBodyLength());
				std::cout << userData.DebugString() << std::endl;
			}
		}
	}

private:
	std::shared_ptr<spdlog::logger> mLogger{ spdlog::stdout_color_mt("console2") };
};


void publishSubscribe()
{
	
	MyMessageHandler msgHandler;

	int msgCount = 1;

    auto factory = stonex::amqp::ConnectionFactory::createCMSConnectionFactory("localhost:5672");
    auto connection = factory->createConnection("CMSTEST", "CMSTEST");
    connection->start();

	auto session = connection->createSession();
	session->start();

	auto dest = session->createTopic("TESTADDRESS");
	auto producer = session->createProducer(dest);
	auto message = session->createTextMessage("CMS-HELLO");
	
	auto consumerDest = session->createTopic("TESTADDRESS::STONEX");
	auto consumer = session->createConsumer(consumerDest);
	consumer->setMessageListener(&msgHandler);

	{
		UserData userData;
		userData.set_name("John");
		userData.set_surname("Doe");
		userData.set_age(30);
		
		Address* address = userData.mutable_address();
		address->set_country("USA");
		address->set_city("New York");
		address->set_street("5th Avenue");
		address->set_postalcode("10001");

		int size = userData.ByteSize();
		cms::BytesMessage* bMessage = nullptr;

		{
			unsigned char* array = new unsigned char[size];
			userData.SerializeToArray(array, size);
			bMessage = session->createBytesMessage(array, size);
			delete[] array;
			producer->send(bMessage);
		}

		{
			UserData userData;
			userData.ParseFromArray(bMessage->getBodyBytes(), bMessage->getBodyLength());
			std::cout << userData.DebugString() << std::endl;
		}


	}

	
	std::this_thread::sleep_for(std::chrono::seconds(30));

}

int main(int argc, char* argv[])
{
	publishSubscribe();
	//MyMessageHandler msgHandler;

	//int msgCount = 1;

	//if(argc > 1)
	//	msgCount = std::stoi(argv[1]);

 //   auto factory = stonex::amqp::ConnectionFactory::createCMSConnectionFactory("localhost:5672");
 //   auto connection = factory->createConnection("CMSTEST", "CMSTEST");
 //   connection->start();

	//auto session = connection->createSession();
	//session->start();

	//auto dest = session->createTopic("TESTADDRESS");
	//auto producer = session->createProducer(dest);
	//auto message = session->createTextMessage("CMS-HELLO");


	//{
	//	auto bMessage = session->createBytesMessage();
	//	bMessage->writeBoolean(true);
	//	bMessage->writeByte(0xF3);
	//	bMessage->writeChar('G');
	//	bMessage->writeFloat(-1234.56);
	//	bMessage->writeDouble(-1234.56);
	//	bMessage->writeShort(-1234);
	//	bMessage->writeUnsignedShort(1234);
	//	bMessage->writeInt(1234);
	//	bMessage->writeLong(12345678);
	//	bMessage->writeString("Test");



	//	std::cout << std::boolalpha << bMessage->readBoolean() << std::endl;
	//	std::cout << std::hex << static_cast<int>(bMessage->readByte()) << std::endl;
	//	std::cout << std::dec;
	//	std::cout << bMessage->readChar() << std::endl;
	//	std::cout << bMessage->readFloat() << std::endl;
	//	std::cout << bMessage->readDouble() << std::endl;
	//	std::cout << bMessage->readShort() << std::endl;
	//	std::cout << bMessage->readUnsignedShort() << std::endl;
	//	std::cout << bMessage->readInt() << std::endl;
	//	std::cout << bMessage->readLong() << std::endl;
	//	std::cout << bMessage->readString() << std::endl;
	//
	//	bMessage->reset();
	//	std::cout << std::hex << static_cast<int>(bMessage->readBoolean()) << std::endl;
	//	std::cout << std::hex << static_cast<int>(bMessage->readByte()) << std::endl;
	//	std::cout << std::hex << static_cast<int>(bMessage->readChar()) << std::endl;
	//	std::cout << std::hex << static_cast<int>(bMessage->readFloat()) << std::endl;
	//	std::cout << std::hex << static_cast<int>(bMessage->readDouble()) << std::endl;
	//	std::cout << std::hex << bMessage->readShort() << std::endl;
	//	std::cout << std::hex << bMessage->readUnsignedShort() << std::endl;
	//	std::cout << std::hex << bMessage->readInt() << std::endl;
	//	std::cout << std::hex << bMessage->readLong() << std::endl;
	//	std::cout << std::hex << bMessage->readString() << std::endl;

	//	bMessage->reset();
	//	std::vector<unsigned char> byteContent(bMessage->getBodyLength());
	//	auto bytesWritten = bMessage->readBytes(byteContent);

	//}

	//{
	//	UserData userData;
	//	userData.set_name("John");
	//	userData.set_surname("Doe");
	//	userData.set_age(30);
	//	
	//	Address* address = userData.mutable_address();
	//	address->set_country("USA");
	//	address->set_city("New York");
	//	address->set_street("5th Avenue");
	//	address->set_postalcode("10001");

	//	int size = userData.ByteSize();
	//	cms::BytesMessage* bMessage = nullptr;

	//	{
	//		unsigned char* array = new unsigned char[size];
	//		userData.SerializeToArray(array, size);
	//		bMessage = session->createBytesMessage(array, size);
	//		delete[] array;
	//	}

	//	{
	//		UserData userData;
	//		userData.ParseFromArray(bMessage->getBodyBytes(), bMessage->getBodyLength());
	//		std::cout << userData.DebugString() << std::endl;
	//	}


	//}

	//auto consumerDest = session->createTopic("TESTADDRESS::STONEX");
	//auto consumer = session->createConsumer(consumerDest);
	//consumer->setMessageListener(&msgHandler);

	//for (int i = 0; i < msgCount; i++)
	//{

 //   producer->send(message);

	////std::cout << "  CMSMessageID: " << message->getCMSMessageID() << std::endl;
	////std::cout << "  CMSCorrelationID: " << message->getCMSCorrelationID() << std::endl;
	////std::cout << "  CMSDestination: " << (message->getCMSDestination() ? "some destination" : "null") << std::endl;
	////std::cout << "  CMSReplyTo: " << (message->getCMSReplyTo() ? "some destination" : "null") << std::endl;
	////std::cout << "  CMSTimestamp: " << message->getCMSTimestamp() << std::endl;
	////std::cout << "  CMSExpiration: " << message->getCMSExpiration() << std::endl;
	////std::cout << "  CMSPriority: " << static_cast<int>(message->getCMSPriority()) << std::endl;
	////std::cout << "  Redelivered: " << (message->getCMSRedelivered() ? "true" : "false") << std::endl;
	////std::cout << "  Delivery Mode: " << message->getCMSDeliveryMode() << std::endl;
	////std::cout << "  Type: " << message->getCMSType() << std::endl;
	////for (const auto& property : message->getPropertyNames()) {
	////	std::cout << "  Property: " << property << " = " << message->getStringProperty(property) << std::endl;
	////}

	//std::this_thread::sleep_for(std::chrono::seconds(1));

	//}
}