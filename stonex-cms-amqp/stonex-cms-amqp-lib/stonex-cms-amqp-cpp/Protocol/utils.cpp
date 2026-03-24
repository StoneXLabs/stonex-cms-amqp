#include "utils.h"
#include "Queue.h"
#include "TemporaryQueue.h"
#include "Topic.h"
#include "TemporaryTopic.h"

#include "TextMessage.h"
#include "BytesMessage.h"

#include <proton/annotation_key.hpp>
#include <proton/types.hpp>
#include <proton/message_id.hpp>

#include <cms/DeliveryMode.h>
AMQP_DEFINES

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

	
	annotation::DESTINATION_TYPE DestinationConverter::jmsDestinationType(const cms::Destination::DestinationType& destinationType)
	{
		
		annotation::DESTINATION_TYPE destType;

		switch (destinationType)
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
	
	cms::Destination* DestinationConverter::createCMSDestination(const Destination& destination)
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

		auto annotation = message.message_annotations().get(annotation::JMS_DESTINATION_TYPE);
		if (annotation.type() != proton::type_id::BYTE && annotation.type() == proton::type_id::NULL_TYPE)
		{
			//TO DO throw And handle
			return nullptr;
		}

		try
		{
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

		auto annotation = message.message_annotations().get(annotation::JMS_REPLY_TO_TYPE);
		if (annotation.type() != proton::type_id::BYTE && annotation.type() == proton::type_id::NULL_TYPE)
		{
			//TO DO throw And handle
			return nullptr;
		}

		try
		{

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
	
	Destination* DestinationConverter::createProtonDestination(const cms::Destination* destination)
	{
		Destination* dest{ nullptr };

		switch (destination->getDestinationType())
		{
		case cms::Destination::DestinationType::QUEUE:
			dest = new Destination{ cms::Destination::DestinationType::QUEUE, dynamic_cast<const cms::Queue*>(destination)->getQueueName() };
			break;
		case cms::Destination::DestinationType::TOPIC:
			dest = new Destination{ cms::Destination::DestinationType::TOPIC, dynamic_cast<const cms::Topic*>(destination)->getTopicName() };
			break;
		case cms::Destination::DestinationType::TEMPORARY_QUEUE:
			dest = new Destination{ cms::Destination::DestinationType::TEMPORARY_QUEUE, dynamic_cast<const cms::TemporaryQueue*>(destination)->getQueueName() };
			break;
		case cms::Destination::DestinationType::TEMPORARY_TOPIC:
			dest = new Destination{ cms::Destination::DestinationType::TEMPORARY_TOPIC, dynamic_cast<const cms::TemporaryTopic*>(destination)->getTopicName() };
			break;
		default:
			break;
		}

		return dest;
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
			auto annotation = message.message_annotations().get(annotation::JMS_MESSAGE_TYPE);

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

		if(cmsMessage)
			transformProperties(message, cmsMessage);

		return cmsMessage;
	}
	
	void MessageConverter::transformProperties(const proton::message& message, cms::Message* cmsMessage)
	{
		
		std::map<std::string, proton::scalar> properties;
		proton::get(message.properties(), properties);
		
		for (const auto& [property, value] : properties)
		{

			switch (value.type())
			{
			case proton::type_id::NULL_TYPE:
				break;
			case proton::type_id::BOOLEAN:
				cmsMessage->setBooleanProperty(property, proton::get<bool>(value));
				break;
			case proton::type_id::UBYTE:
			case proton::type_id::BYTE:
				cmsMessage->setByteProperty(property, proton::get<unsigned char>(value));
				break;
			case proton::type_id::USHORT:
			case proton::type_id::SHORT:
				cmsMessage->setShortProperty(property, proton::get<short>(value));
				break;
			case proton::type_id::UINT:
			case proton::type_id::INT:
				cmsMessage->setIntProperty(property, proton::get<int>(value));
				break;
			case proton::type_id::CHAR:
				cmsMessage->setShortProperty(property, proton::get<unsigned char>(value));
				break;
			case proton::type_id::ULONG:
			case proton::type_id::LONG:
				cmsMessage->setLongProperty(property, proton::get<long long>(value));
				break;
			case proton::type_id::TIMESTAMP:
				break;
			case proton::type_id::FLOAT:
				cmsMessage->setFloatProperty(property, proton::get<float>(value));
				break;
			case proton::type_id::DOUBLE:
				cmsMessage->setDoubleProperty(property, proton::get<double>(value));
				break;
			case proton::type_id::DECIMAL32:
			case proton::type_id::DECIMAL64:
			case proton::type_id::DECIMAL128:
				cmsMessage->setFloatProperty(property, proton::get<float>(value));
			case proton::type_id::UUID:
				break;
			case proton::type_id::BINARY:
				break;
			case proton::type_id::STRING:
				cmsMessage->setStringProperty(property, proton::get<std::string>(value));
				break;
			case proton::type_id::SYMBOL:
				break;
			case proton::type_id::DESCRIBED:
				break;
			case proton::type_id::ARRAY:
				break;
			case proton::type_id::MAP:
				break;
			default:
				break;
			}
		}

		if (const auto protonCorrelationId = message.correlation_id(); protonCorrelationId.type() == proton::type_id::STRING)
		{
			cmsMessage->setCMSCorrelationID(proton::get<std::string>(message.correlation_id()));
		}


		//delivery mode
		cmsMessage->setCMSDeliveryMode(message.durable() ? cms::DeliveryMode::DELIVERY_MODE::PERSISTENT : cms::DeliveryMode::DELIVERY_MODE::NON_PERSISTENT);

		if (auto destination = DestinationConverter::createCMSDestination(message))
		{
			cmsMessage->setCMSDestination(destination);
			delete destination;
		}

		//expiration
		cmsMessage->setCMSExpiration(message.expiry_time().milliseconds());

		cmsMessage->setCMSMessageID(proton::get<std::string>(message.id()));
		
		//priority
		cmsMessage->setCMSPriority(message.priority());

		// redelivered ??
		cmsMessage->setCMSRedelivered(message.delivery_count() > 1);


		if (auto replyTo = DestinationConverter::createCMSReplyTo(message))
		{
			cmsMessage->setCMSReplyTo(replyTo);
			delete replyTo;
		}

		//timestamp
		cmsMessage->setCMSTimestamp(message.creation_time().milliseconds());
		
		//type
		cmsMessage->setCMSType(message.subject());
	}
	
	MessageProperties::MessageProperties(const MessageProperties& other)
	:	correlationId(other.correlationId),
		deliveryMode(other.deliveryMode),
		expiration(other.expiration),
		messageId(other.messageId),
		priority(other.priority),
		redelivered(other.redelivered),
		timeStamp(other.timeStamp),
		type(other.type),
		mProperties(other.mProperties)
	{
		if(other.destination)
			destination.reset(other.destination->clone());

		if (other.replyTo)
			replyTo.reset(other.replyTo->clone());
	}
	
	void MessageProperties::clear()
	{
		mProperties.clear();
	}

	std::vector<std::string> MessageProperties::getNames() const
	{
		std::vector<std::string> names;
		for (const auto& [key, value] : mProperties)
			names.push_back(key);

		return names;
	}

	cms::Message::ValueType MessageProperties::getType(const std::string& name) const
	{
		auto it = mProperties.find(name);
		if (it == mProperties.end())
			return cms::Message::UNKNOWN_TYPE;
		return std::visit([](auto&& arg) -> cms::Message::ValueType {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, bool>)
				return cms::Message::BOOLEAN_TYPE;
			else if constexpr (std::is_same_v<T, unsigned char>)
				return cms::Message::BYTE_TYPE;
			else if constexpr (std::is_same_v<T, double>)
				return cms::Message::DOUBLE_TYPE;
			else if constexpr (std::is_same_v<T, float>)
				return cms::Message::FLOAT_TYPE;
			else if constexpr (std::is_same_v<T, int>)
				return cms::Message::INTEGER_TYPE;
			else if constexpr (std::is_same_v<T, long long>)
				return cms::Message::LONG_TYPE;
			else if constexpr (std::is_same_v<T, short>)
				return cms::Message::SHORT_TYPE;
			else if constexpr (std::is_same_v<T, std::string>)
				return cms::Message::STRING_TYPE;
			else
				return cms::Message::UNKNOWN_TYPE;
		}, it->second);
	}

	bool MessageProperties::exists(const std::string& name) const
	{
		return mProperties.find(name) != mProperties.end();
	}

	void MessageProperties::set(const std::string& name, PropertyValue value)
	{
		mProperties[name] = std::move(value);
	}
}

AMQP_DEFINES_CLOSE