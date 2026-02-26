#include "utils.h"
#include "Queue.h"
#include "TemporaryQueue.h"
#include "Topic.h"
#include "TemporaryTopic.h"

#include "TextMessage.h"
#include "BytesMessage.h"

#include <proton/annotation_key.hpp>

namespace internal
{
	std::vector<proton::symbol>  DestinationConverter::capabilities(const cms::Destination *destination)
	{
		std::vector<proton::symbol> capabilities;
		switch (destination->getDestinationType())
		{
		case cms::Destination::QUEUE:
			capabilities.push_back(capability::QUEUE);
			break;
		case cms::Destination::TEMPORARY_QUEUE:
			capabilities.push_back(capability::TEMPORARY_QUEUE);
			capabilities.push_back(capability::DELETE_ON_CLOSE);
			break;
		case cms::Destination::TOPIC:
			capabilities.push_back(capability::TOPIC);
			break;
		case cms::Destination::TEMPORARY_TOPIC:
			capabilities.push_back(capability::TEMPORARY_TOPIC);
			capabilities.push_back(capability::DELETE_ON_CLOSE);
			break;
		default:
			throw cms::CMSException("Unknown destination type");
			break;
		}

		return capabilities;
	}


	annotation::DESTINATION_TYPE DestinationConverter::jmsDestinationType(const cms::Destination* destination)
	{
		annotation::DESTINATION_TYPE destType;

		switch (destination->getDestinationType())
		{
		case cms::Destination::QUEUE:
			destType = annotation::DESTINATION_TYPE::QUEUE;
			break;
		case cms::Destination::TEMPORARY_QUEUE:
			destType = annotation::DESTINATION_TYPE::TEMPORARY_QUEUE;
			break;
		case cms::Destination::TOPIC:
			destType = annotation::DESTINATION_TYPE::TOPIC;
			break;
		case cms::Destination::TEMPORARY_TOPIC:
			destType = annotation::DESTINATION_TYPE::TEMPORARY_TOPIC;
			break;
		default:
			throw cms::CMSException("Unknown destination type");
			break;
		}

		return destType;

	}

	
	std::string DestinationConverter::address(const cms::Destination *destination)
	{
		switch (destination->getDestinationType())
		{
		case cms::Destination::QUEUE:
			return dynamic_cast<const cms::Queue*>(destination)->getQueueName();
		case cms::Destination::TEMPORARY_QUEUE:
			return dynamic_cast<const cms::TemporaryQueue*>(destination)->getQueueName();
		case cms::Destination::TOPIC:
			return dynamic_cast<const cms::Topic*>(destination)->getTopicName();
		case cms::Destination::TEMPORARY_TOPIC:
			return dynamic_cast<const cms::TemporaryTopic*>(destination)->getTopicName();
		default:
			throw cms::CMSException("Unknown destination type");
		}
	}
	
	cms::Destination* DestinationConverter::createCMSDestination(const internal::Destination& destination)
	{
		if(destination.valid)
		{
			switch(destination.type)
			{
			case cms::Destination::DestinationType::QUEUE:
				return new stonex::amqp::Queue(destination.address);
				break;
			case cms::Destination::DestinationType::TOPIC:
				return new stonex::amqp::Topic(destination.address);
				break;
			case cms::Destination::DestinationType::TEMPORARY_QUEUE:
				return new stonex::amqp::TemporaryQueue(destination.address);
				break;
			case cms::Destination::DestinationType::TEMPORARY_TOPIC:
				return new stonex::amqp::TemporaryTopic(destination.address);
			default:
				return nullptr;
			}
		}
	
		return nullptr;
	}
	
	cms::Destination* DestinationConverter::createCMSDestination(const std::string& address ,const proton::target& target)
	{
		for (const auto& capability : target.capabilities())
		{
			if (capability == capability::QUEUE)
			{
				return new stonex::amqp::Queue(address);
			}
			else if (capability == capability::TOPIC)
			{
				return new stonex::amqp::Topic(address);
			}
			else if (capability == capability::TEMPORARY_QUEUE)
			{
				return new stonex::amqp::TemporaryQueue(address);
			}
			else if (capability == capability::TEMPORARY_TOPIC)
			{
				return new stonex::amqp::TemporaryTopic(address);
			}
		}
	}

	cms::Destination* DestinationConverter::createCMSDestination(const std::string& address ,const proton::source& target)
	{
		for (const auto& capability : target.capabilities())
		{
			if (capability == capability::QUEUE)
			{
				return new stonex::amqp::Queue(address);
			}
			else if (capability == capability::TOPIC)
			{
				return new stonex::amqp::Topic(address);
			}
			else if (capability == capability::TEMPORARY_QUEUE)
			{
				return new stonex::amqp::TemporaryQueue(address);
			}
			else if (capability == capability::TEMPORARY_TOPIC)
			{
				return new stonex::amqp::TemporaryTopic(address);
			}
		}
	}

	cms::Destination* DestinationConverter::createCMSDestination(const proton::message& message)
	{
		cms::Destination* destination{ nullptr };

		try
		{
			auto annotation = message.message_annotations().get(internal::annotation::JMS_DESTINATION_TYPE);

			switch (annotation::DESTINATION_TYPE(proton::get<int8_t>(annotation)))
			{
			case annotation::DESTINATION_TYPE::QUEUE:
				destination = new stonex::amqp::Queue(message.to());
				break;
			case annotation::DESTINATION_TYPE::TOPIC:
				destination = new stonex::amqp::Topic(message.to());
				break;
			case annotation::DESTINATION_TYPE::TEMPORARY_QUEUE:
				destination = new stonex::amqp::TemporaryQueue(message.to());
				break;
			case annotation::DESTINATION_TYPE::TEMPORARY_TOPIC:
				destination = new stonex::amqp::TemporaryTopic(message.to());
				break;
			default:
				break;
			}
		}
		catch (const std::exception& ex)
		{

		}

		return destination;
	}


	cms::Destination* DestinationConverter::createCMSReplyTo(const proton::message& message)
	{
		cms::Destination* destination{ nullptr };

		try
		{
			auto annotation = message.message_annotations().get(internal::annotation::JMS_REPLY_TO_TYPE);

			switch (annotation::DESTINATION_TYPE(proton::get<int8_t>(annotation)))
			{
			case annotation::DESTINATION_TYPE::QUEUE:
				destination = new stonex::amqp::Queue(message.reply_to());
				break;
			case annotation::DESTINATION_TYPE::TOPIC:
				destination = new stonex::amqp::Topic(message.reply_to());
				break;
			case annotation::DESTINATION_TYPE::TEMPORARY_QUEUE:
				destination = new stonex::amqp::TemporaryQueue(message.reply_to());
				break;
			case annotation::DESTINATION_TYPE::TEMPORARY_TOPIC:
				destination = new stonex::amqp::TemporaryTopic(message.reply_to());
				break;
			default:
				break;
			}
		}
		catch (const std::exception& ex)
		{

		}

		return destination;
	}

	cms::Message::ValueType ValueTypeConverter::amqpToCms(proton::type_id t_id)
	{
		switch (t_id)
		{
		case proton::type_id::NULL_TYPE:
			return cms::Message::ValueType::NULL_TYPE;
			break;
		case proton::type_id::BOOLEAN:
			return cms::Message::ValueType::BOOLEAN_TYPE;
			break;
		case proton::type_id::UBYTE:
			// cms treats unsigned char as Byte
			// return cms::Message::ValueType::CHAR_TYPE;
			return cms::Message::ValueType::BYTE_TYPE;
			break;
		case proton::type_id::BYTE:
			return cms::Message::ValueType::BYTE_TYPE;
			break;
		case proton::type_id::USHORT:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		case proton::type_id::SHORT:
			return cms::Message::ValueType::SHORT_TYPE;
			break;
		case proton::type_id::UINT:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		case proton::type_id::INT:
			return cms::Message::ValueType::INTEGER_TYPE;
			break;
		case proton::type_id::CHAR:
			return cms::Message::ValueType::CHAR_TYPE;
			break;
		case proton::type_id::ULONG:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		case proton::type_id::LONG:
			return cms::Message::ValueType::LONG_TYPE;
			break;
		case proton::type_id::TIMESTAMP:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		case proton::type_id::FLOAT:
			return cms::Message::ValueType::FLOAT_TYPE;
			break;
		case proton::type_id::DOUBLE:
			return cms::Message::ValueType::DOUBLE_TYPE;
			break;
		case proton::type_id::DECIMAL32:
			return cms::Message::ValueType::FLOAT_TYPE;
			break;
		case proton::type_id::DECIMAL64:
			return cms::Message::ValueType::FLOAT_TYPE;
			break;
		case proton::type_id::DECIMAL128:
			return cms::Message::ValueType::DOUBLE_TYPE;
			break;
		case proton::type_id::UUID:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		case proton::type_id::BINARY:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		case proton::type_id::STRING:
			return cms::Message::ValueType::STRING_TYPE;
			break;
		case proton::type_id::SYMBOL:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		case proton::type_id::DESCRIBED:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		case proton::type_id::ARRAY:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		case proton::type_id::MAP:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		default:
			return cms::Message::ValueType::UNKNOWN_TYPE;
			break;
		}
	}


	cms::Message* MessageConverter::fromProtonMessage(const proton::message& message)
	{

		cms::Message* cmsMessage{ nullptr };
		try
		{
			auto annotation = message.message_annotations().get(internal::annotation::JMS_MESSAGE_TYPE);

			switch (annotation::MESSAGE_TYPE(proton::get<int8_t>(annotation)))
			{
			case annotation::MESSAGE_TYPE::MESSAGE:
				break;
			case annotation::MESSAGE_TYPE::OBJECT_MESSAGE:
				break;
			case annotation::MESSAGE_TYPE::MAP_MESSAGE:
				break;
			case annotation::MESSAGE_TYPE::BYTES_MESSAGE:
				cmsMessage = new stonex::amqp::BytesMessage(message);
				break;
			case annotation::MESSAGE_TYPE::STREAM_MESSAGE:
				break;
			case annotation::MESSAGE_TYPE::TEXT_MESSAGE:
				cmsMessage = new stonex::amqp::TextMessage(message);
				break;
			default:
				break;
			}
		}
		catch (const std::exception& ex)
		{
			
		}

		return cmsMessage;
	}
	//proton::message MessageConverter::fromCmsMessage(const cms::Message *message)
	//{
 //    
	//}

	//proton::message MessageConverter::fromCmsMessage(const cms::TextMessage *destination)
	//{

	//}

	//proton::message MessageConverter::fromCmsMessage(const cms::BytesMessage *destination)
	//{
	//	return {};
	//}
}