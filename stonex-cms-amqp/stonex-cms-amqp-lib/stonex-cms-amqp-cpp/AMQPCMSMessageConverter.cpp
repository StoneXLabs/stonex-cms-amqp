/*
 * Copyright 2022 StoneX Financial Ltd.
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "AMQPCMSMessageConverter.h"

#include <regex>
#include <array>
#include <map>
#include <iomanip>
#include <sstream>

#include <proton/target.hpp>
#include <proton/source.hpp>
#include <proton/symbol.hpp>
#include <proton/annotation_key.hpp>
#include <proton/message_id.hpp>
#include <proton/annotation_key.hpp>
#include <proton/message.hpp>
#include <proton/types.hpp>

#include <cms/InvalidDestinationException.h>

#include "../API/CMSQueue.h"
#include "../API/CMSTopic.h"
#include "../API/CMSTemporaryQueue.h"
#include "../API/CMSTemporaryTopic.h"

cms::Destination* cms::amqp::AMQPCMSMessageConverter::createCMSDestination(const proton::sender* sender)
{
	::cms::Destination* dest{ nullptr };
	std::string t;
	switch (capabilityToDestinationType(sender->target().capabilities()))
	{
	case ::cms::Destination::DestinationType::QUEUE:
		dest = new CMSQueue(sender->target().address());
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		dest = new CMSTopic(sender->target().address());
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		dest = new CMSTemporaryQueue();
		dynamic_cast<CMSTemporaryQueue*>(dest)->mQueueName = sender->target().address();
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		dest = new CMSTemporaryTopic();
		dynamic_cast<CMSTemporaryTopic*>(dest)->mTopicName = sender->target().address();
		break;
	default:
		throw ::cms::InvalidDestinationException("invalid destination type");
		break;
	}
	return dest;
}

cms::Destination* cms::amqp::AMQPCMSMessageConverter::createCMSDestination(const proton::receiver* receiver)
{
	::cms::Destination* dest{ nullptr };

	switch (capabilityToDestinationType(receiver->source().capabilities()))
	{
	case ::cms::Destination::DestinationType::QUEUE:
		dest = new CMSQueue(receiver->source().address());
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		dest = new CMSTopic(receiver->source().address());
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		dest = new CMSTemporaryQueue();
		dynamic_cast<CMSTemporaryQueue*>(dest)->mQueueName = receiver->source().address();
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		dest = new CMSTemporaryTopic();
		dynamic_cast<CMSTemporaryTopic*>(dest)->mTopicName = receiver->source().address();
		break;
	default:
		throw ::cms::InvalidDestinationException("invalid destination type");
		break;
	}
	return dest;
}

::cms::Destination* cms::amqp::AMQPCMSMessageConverter::createCMSDestination(const std::shared_ptr<proton::message> message)
{
	::cms::Destination* dest{ nullptr };

	if(message->address().empty())
		throw ::cms::InvalidDestinationException("missing message destination address");

	switch (annotationsToDestinationType(message->message_annotations(),X_OPT_JMS_DEST.data()))
	{
	case ::cms::Destination::DestinationType::QUEUE:
		dest = new CMSQueue(message->address());
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		dest = new CMSTopic(message->address());
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		dest = new CMSTemporaryQueue();
		dynamic_cast<CMSTemporaryQueue*>(dest)->mQueueName = message->address();
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		dest = new CMSTemporaryTopic();
		dynamic_cast<CMSTemporaryTopic*>(dest)->mTopicName = message->address();
		break;
	default:
		throw ::cms::InvalidDestinationException("invalid destination type");
		break;
	}
	return dest;

}

::cms::Destination* cms::amqp::AMQPCMSMessageConverter::createCMSDestinationUsingMessageDestination(const proton::sender* sender, const std::shared_ptr<proton::message> message)
{
	::cms::Destination* dest{ nullptr };

	switch (capabilityToDestinationType(sender->target().capabilities()))
	{
	case ::cms::Destination::DestinationType::QUEUE:
		dest = new CMSQueue(message->address());
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		dest = new CMSTopic(message->address());
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		dest = new CMSTemporaryQueue();
		dynamic_cast<CMSTemporaryQueue*>(dest)->mQueueName = message->address();
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		dynamic_cast<CMSTemporaryTopic*>(dest)->mTopicName = message->address();
		break;
	default:
		throw ::cms::InvalidDestinationException("invalid destination type");
		break;
	}
	return dest;
}

::cms::Destination* cms::amqp::AMQPCMSMessageConverter::createCMSDestinationUsingMessageDestination(const proton::receiver* receiver, const std::shared_ptr<proton::message> message)
{
	::cms::Destination* dest{ nullptr };

	switch (capabilityToDestinationType(receiver->source().capabilities()))
	{
	case ::cms::Destination::DestinationType::QUEUE:
		dest = new CMSQueue(message->address());
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		dest = new CMSTopic(message->address());
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		dest = new CMSTemporaryQueue();
		dynamic_cast<CMSTemporaryQueue*>(dest)->mQueueName = message->address();
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		dynamic_cast<CMSTemporaryTopic*>(dest)->mTopicName = message->address();
		break;
	default:
		throw ::cms::InvalidDestinationException("invalid destination type");
		break;
	}
	return dest;
}

::cms::Destination* cms::amqp::AMQPCMSMessageConverter::createCMSReplyToUsingMessageDestination(const std::shared_ptr<proton::message> message)
{
	::cms::Destination* dest{ nullptr };

	if (message->reply_to().empty())
		return dest;

	switch (annotationsToDestinationType(message->message_annotations(), X_OPT_JMS_REPLY_TO.data()))
	{
	case ::cms::Destination::DestinationType::QUEUE:
		dest = new CMSQueue(message->reply_to());
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		dest = new CMSTopic(message->reply_to());
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		dest = new CMSTemporaryQueue();
		dynamic_cast<CMSTemporaryQueue*>(dest)->mQueueName = message->reply_to();
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		dynamic_cast<CMSTemporaryTopic*>(dest)->mTopicName = message->reply_to();
		break;
	default:
		throw ::cms::InvalidDestinationException("invalid destination type");
		break;
	}
	return dest;
}

cms::Destination::DestinationType cms::amqp::AMQPCMSMessageConverter::capabilityToDestinationType(const std::vector<proton::symbol>& capabilities)
{
	auto capability = std::find_if(std::cbegin(capabilities), std::cend(capabilities), [](const proton::symbol item) {
		return item == QUEUE_CAPABILITY || item == TOPIC_CAPABILITY || item == TEMPORARY_QUEUE_CAPABILITY || item == TEMPORARY_TOPIC_CAPABILITY; });

	if (capability == std::cend(capabilities))
		throw::cms::InvalidDestinationException("missing capability type.");  //to do more explicit message


	if (auto setting = *capability; setting == QUEUE_CAPABILITY)
		return ::cms::Destination::DestinationType::QUEUE;
	else if (setting == TOPIC_CAPABILITY)
		return ::cms::Destination::DestinationType::TOPIC;
	else if (setting == TEMPORARY_QUEUE_CAPABILITY)
		return ::cms::Destination::DestinationType::TEMPORARY_QUEUE;
	else if (setting == TEMPORARY_TOPIC_CAPABILITY)
		return ::cms::Destination::DestinationType::TEMPORARY_TOPIC;
	else
		throw ::cms::InvalidDestinationException("illegal capability type.");  //to do more explicit message
}

::cms::Destination::DestinationType cms::amqp::AMQPCMSMessageConverter::annotationsToDestinationType(const proton::message::annotation_map& annotations, const proton::symbol& annotation_key)
{
	if (annotations.exists(annotation_key) & (annotation_key == X_OPT_TO_TYPE | annotation_key == X_OPT_REPLY_TO_TYPE))
		return capabilityToDestinationType({ proton::to_string(annotations.get(annotation_key)) });
	else if (annotations.exists(annotation_key) & (annotation_key == X_OPT_JMS_DEST | annotation_key == X_OPT_JMS_REPLY_TO))
	{
		auto type_id = annotations.get(annotation_key).type();
		if (type_id == proton::type_id::UBYTE)
			return JMSTypeToDestinationType(annotations.get(annotation_key).get<uint8_t>());
		else if (type_id == proton::type_id::BYTE)
			return JMSTypeToDestinationType(annotations.get(annotation_key).get<int8_t>());
		else
			throw ::cms::InvalidDestinationException(annotation_key + " type error expected UBYTEor BUTE");
	}
	else
		throw ::cms::InvalidDestinationException("missing capability type.");
}

::cms::Destination::DestinationType cms::amqp::AMQPCMSMessageConverter::JMSTypeToDestinationType(const uint8_t type)
{
	if (type == 0)
		return ::cms::Destination::DestinationType::QUEUE;
	else if (type == 1)
		return ::cms::Destination::DestinationType::TOPIC;
	else if (type == 2)
		return ::cms::Destination::DestinationType::TEMPORARY_QUEUE;
	else if (type == 3)
		return ::cms::Destination::DestinationType::TEMPORARY_TOPIC;
	else
		throw ::cms::InvalidDestinationException("illegal capability type.");  //to do more explicit message
}

::cms::Destination::DestinationType cms::amqp::AMQPCMSMessageConverter::JMSTypeToDestinationType(const int8_t type)
{
	if (type == 0)
		return ::cms::Destination::DestinationType::QUEUE;
	else if (type == 1)
		return ::cms::Destination::DestinationType::TOPIC;
	else if (type == 2)
		return ::cms::Destination::DestinationType::TEMPORARY_QUEUE;
	else if (type == 3)
		return ::cms::Destination::DestinationType::TEMPORARY_TOPIC;
	else
		throw ::cms::InvalidDestinationException("illegal capability type.");  //to do more explicit message
}

std::string cms::amqp::AMQPCMSMessageConverter::destinationToAddress(const::cms::Destination* destintion)
{
	std::string output{};

	switch (destintion->getDestinationType())
	{
	case ::cms::Destination::DestinationType::QUEUE:
		output = dynamic_cast<const CMSQueue*>(destintion)->getQueueName();
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		output = dynamic_cast<const CMSTopic*>(destintion)->getTopicName();
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		output = dynamic_cast<const CMSTemporaryQueue*>(destintion)->getQueueName();
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		//			output = dynamic_cast<const CMSTemporaryTopic*>(&message)->getTopicName();
		break;
	}

	return output;
}

std::string cms::amqp::AMQPCMSMessageConverter::destinationAddress(const ::cms::Message* message)
{
	return destinationToAddress(message->getCMSDestination());
}

std::string cms::amqp::AMQPCMSMessageConverter::replyToAddress(const::cms::Message* message)
{
	return destinationToAddress(message->getCMSReplyTo());
}

::cms::Message::ValueType cms::amqp::AMQPCMSMessageConverter::type_id_to_ValueType(proton::type_id t_id) 
{
	::cms::Message::ValueType value_type(::cms::Message::ValueType::UNKNOWN_TYPE);
	switch (t_id)
	{
	case proton::type_id::NULL_TYPE:
		value_type = ::cms::Message::ValueType::NULL_TYPE;
		break;
	case proton::type_id::BOOLEAN:
		value_type = ::cms::Message::ValueType::BOOLEAN_TYPE;
		break;
	case proton::type_id::UBYTE:
		//cms treats unsigned char as Byte
		//value_type = ::cms::Message::ValueType::CHAR_TYPE;
		value_type = ::cms::Message::ValueType::BYTE_TYPE;
		break;
	case proton::type_id::BYTE:
		value_type = ::cms::Message::ValueType::BYTE_TYPE;
		break;
	case proton::type_id::USHORT:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;		
		break;
	case proton::type_id::SHORT:
		value_type = ::cms::Message::ValueType::SHORT_TYPE;
		break;
	case proton::type_id::UINT:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	case proton::type_id::INT:
		value_type = ::cms::Message::ValueType::INTEGER_TYPE;
		break;
	case proton::type_id::CHAR:
		value_type = ::cms::Message::ValueType::CHAR_TYPE;
		break;
	case proton::type_id::ULONG:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	case proton::type_id::LONG:
		value_type = ::cms::Message::ValueType::LONG_TYPE;
		break;
	case proton::type_id::TIMESTAMP:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	case proton::type_id::FLOAT:
		value_type = ::cms::Message::ValueType::FLOAT_TYPE;
		break;
	case proton::type_id::DOUBLE:
		value_type = ::cms::Message::ValueType::DOUBLE_TYPE;
		break;
	case proton::type_id::DECIMAL32:
		value_type = ::cms::Message::ValueType::FLOAT_TYPE;
		break;
	case proton::type_id::DECIMAL64:
		value_type = ::cms::Message::ValueType::FLOAT_TYPE;
		break;
	case proton::type_id::DECIMAL128:
		value_type = ::cms::Message::ValueType::DOUBLE_TYPE;
		break;
	case proton::type_id::UUID:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	case proton::type_id::BINARY:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	case proton::type_id::STRING:
		value_type = ::cms::Message::ValueType::STRING_TYPE;
		break;
	case proton::type_id::SYMBOL:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	case proton::type_id::DESCRIBED:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	case proton::type_id::ARRAY:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	case proton::type_id::MAP:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	default:
		value_type = ::cms::Message::ValueType::UNKNOWN_TYPE;
		break;
	}
	return value_type;
}

std::tuple<proton::type_id, int, bool> cms::amqp::AMQPCMSMessageConverter::prefixToType(const std::string& s) 
{
	std::regex prefixRegex("^ID:(AMQP_BINARY:|AMQP_UUID:|AMQP_ULONG:|AMQP_NO_PREFIX|AMQP_STRING:)");

	proton::type_id t_id{ proton::type_id::NULL_TYPE };

	int type_string_end{ 0 };

	if (std::regex_search(s, prefixRegex))
	{
		/*
			get type from prefix
			type string first char == 3
			type string last char == second occurence of ':'
		*/

		type_string_end = s.find(':', 3);

		if (std::string type_string = s.substr(3, type_string_end - 3); type_string == "AMQP_BINARY")
		{
			t_id = proton::type_id::BINARY;
		}
		else if (type_string == "AMQP_UUID")
		{
			t_id = proton::type_id::UUID;
		}
		else if (type_string == "AMQP_ULONG")
		{
			t_id = proton::type_id::ULONG;
		}
		else if (type_string == "AMQP_STRING" || type_string == "AMQP_NO_PREFIX")
		{
			t_id = proton::type_id::STRING;
		}
	}
	else
	{
		/*
			correlation id without AMQP type prefix is hhandled as string
			ID:someCorrelationId01923
		*/
		t_id = proton::type_id::STRING;
	}

	/*
	*  if : delimeter was found, returned type_string_end should be incremented to fit possition of first char after ID:AMQP_TYPE:
	*  else : no AMQP_TYPE prefix was found, type_string_end should not be incremented
	*/
	if (type_string_end > 0)
		type_string_end += 1;

	return std::make_tuple(t_id, type_string_end, t_id != proton::type_id::NULL_TYPE);
}

std::array<char, 16> cms::amqp::AMQPCMSMessageConverter::hexStringToArray(const std::string& s) 

{
	std::regex uuidPattern("[a-f A-F 0-9]{8}-[a-f A-F 0-9]{4}-[a-f A-F 0-9]{4}-[a-f A-F 0-9]{4}-[a-f A-F 0-9]{12}");

	if (!std::regex_match(s, uuidPattern))
		throw ::cms::CMSException("not uuid pattern");

	std::array<char, 16> binaryArray;
	binaryArray.fill(0x00);
	auto out_iter = binaryArray.begin();
	auto ptr = s.begin();
	size_t pos{ 0 };

	try
	{
		while (ptr != s.end())
		{
			if (*ptr != '-')
			{
				auto tmp = std::stoll(&*ptr, &pos, 16);
				ptr = std::next(ptr, pos);
				std::for_each(out_iter, std::next(out_iter, pos / 2), [size = pos / 2, &tmp](char& item) mutable {item = (char)(tmp >> ((8 * size) - 8)); size--; });
				out_iter += pos / 2;
			}
			else
				ptr++;
		}
	}
	catch (const std::invalid_argument& ex)
	{
		std::cout << "ex what " << ex.what() << std::endl;
		throw;
	}
	catch (const std::out_of_range& ex)
	{
		std::cout << "ex what " << ex.what() << std::endl;
		throw;
	}

	return binaryArray;
}
//TO REFACTOR!!!
std::string cms::amqp::AMQPCMSMessageConverter::hexStringToBinaryArray(const std::string& s) 
{
	if (s.size() % 2)
		throw ::cms::CMSException("string to binary conversion error, char count ");

	std::string binaryArray(s.size() / 2, 0x00);
	auto out_iter = binaryArray.begin();
	auto ptr = s.begin();
	size_t pos{ 0 };

	try
	{
		while (ptr != s.end())
		{
			auto tmp = std::stoll(&*ptr, &pos, 16);
			ptr = std::next(ptr, pos);
			std::for_each(out_iter, std::next(out_iter, pos / 2), [size = pos / 2, &tmp](char& item) mutable {item = (char)(tmp >> ((8 * size) - 8)); size--; });
			out_iter += pos / 2;
		}
	}
	catch (const std::invalid_argument& ex)
	{
		std::cout << "ex what " << ex.what() << std::endl;
		throw;
	}
	catch (const std::out_of_range& ex)
	{
		std::cout << "ex what " << ex.what() << std::endl;
		throw;
	}

	return binaryArray;
}

std::string cms::amqp::AMQPCMSMessageConverter::binaryArrayToHexString(const std::vector<uint8_t>& b) 
{
	std::string output_string;
	std::stringstream ss(output_string);

	std::for_each(b.cbegin(), b.cend(), [&ss](uint8_t item) {ss << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << static_cast<int>(item); });

	return ss.str();
}


std::string cms::amqp::AMQPCMSMessageConverter::destinationTypeToString(::cms::Destination::DestinationType dest_type) 
{
	std::string destination_type_string;

	switch (dest_type)
	{
	case ::cms::Destination::DestinationType::QUEUE:
		destination_type_string = "queue";
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		destination_type_string = "topic";
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		destination_type_string = "temporary-queue";
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		destination_type_string = "temporary-topic";
		break;
	default:
		break;
	}

	return destination_type_string;
}

int8_t cms::amqp::AMQPCMSMessageConverter::destinationTypeToJMSType(::cms::Destination::DestinationType dest_type)
{
	int8_t jms_to_type;

	switch (dest_type)
	{
	case ::cms::Destination::DestinationType::QUEUE:
		jms_to_type = 0;
		break;
	case ::cms::Destination::DestinationType::TOPIC:
		jms_to_type = 1;
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_QUEUE:
		jms_to_type = 2;
		break;
	case ::cms::Destination::DestinationType::TEMPORARY_TOPIC:
		jms_to_type = 3;
		break;
	default:
		break;
	}

	return jms_to_type;
}

std::vector<std::string> cms::amqp::AMQPCMSMessageConverter::getPropertyNames(const std::shared_ptr<proton::message> message)
{
	std::map<std::string, proton::scalar> properties;
	proton::get(message->properties(), properties);

	std::vector<std::string> keys;

	for (auto const& [key, val] : properties)
	{
		keys.push_back(key);
	}

	return keys;
}

bool cms::amqp::AMQPCMSMessageConverter::propertyExists(const std::string& name, const std::shared_ptr<proton::message> message)
{
	return message->properties().exists(name);
}

/////////////////
bool cms::amqp::AMQPCMSMessageConverter::getCMSBooleanProperty(const std::string& name,const std::shared_ptr<proton::message> message)
{
	if (!message->properties().exists(name))
		throw ::cms::CMSException("property name " + name + "does not exist");

	try
	{
		return proton::get<bool>(message->properties().get(name));
	}
	catch (const proton::conversion_error& er)
	{
		throw ::cms::MessageFormatException(er.what());
	}


}

unsigned char cms::amqp::AMQPCMSMessageConverter::getCMSByteProperty(const std::string& name, const std::shared_ptr<proton::message> message)
{
	if (!message->properties().exists(name))
		throw ::cms::CMSException("property name " + name + "does not exist");

	try
	{
		return proton::get<unsigned char>(message->properties().get(name));
	}
	catch (const proton::conversion_error& er)
	{
		throw ::cms::MessageFormatException(er.what());
	}

}

double cms::amqp::AMQPCMSMessageConverter::getCMSDoubleProperty(const std::string& name, const std::shared_ptr<proton::message> message)
{
	if (!message->properties().exists(name))
		throw ::cms::CMSException("property name " + name + "does not exist");

	try
	{
		return proton::get<double>(message->properties().get(name));
	}
	catch (const proton::conversion_error& er)
	{
		throw ::cms::MessageFormatException(er.what());
	}
}

float cms::amqp::AMQPCMSMessageConverter::getCMSFloatProperty(const std::string& name, const std::shared_ptr<proton::message> message)
{
	if (!message->properties().exists(name))
		throw ::cms::CMSException("property name " + name + "does not exist");

	try
	{
		return proton::get<float>(message->properties().get(name));
	}
	catch (const proton::conversion_error& er)
	{
		throw ::cms::MessageFormatException(er.what());
	}
}

int cms::amqp::AMQPCMSMessageConverter::getCMSIntProperty(const std::string& name, const std::shared_ptr<proton::message> message)
{
	if (!message->properties().exists(name))
		throw ::cms::CMSException("property name " + name + "does not exist");

	try
	{
		return proton::get<int>(message->properties().get(name));
	}
	catch (const proton::conversion_error& er)
	{
		throw ::cms::MessageFormatException(er.what());
	}
}

long long cms::amqp::AMQPCMSMessageConverter::getCMSLongProperty(const std::string& name, const std::shared_ptr<proton::message> message)
{
	if (!message->properties().exists(name))
		throw ::cms::CMSException("property name " + name + "does not exist");

	try
	{
		return proton::get<long long>(message->properties().get(name));
	}
	catch (const proton::conversion_error& er)
	{
		throw ::cms::MessageFormatException(er.what());
	}
}

short cms::amqp::AMQPCMSMessageConverter::getCMSShortProperty(const std::string& name, const std::shared_ptr<proton::message> message)
{
	if (!message->properties().exists(name))
		throw ::cms::CMSException("property name " + name + "does not exist");

	try
	{
		return proton::get<short>(message->properties().get(name));
	}
	catch (const proton::conversion_error& er)
	{
		throw ::cms::MessageFormatException(er.what());
	}
}

std::string cms::amqp::AMQPCMSMessageConverter::getCMSStringProperty(const std::string& name, const std::shared_ptr<proton::message> message)
{
	if (!message->properties().exists(name))
		throw ::cms::CMSException("property name " + name + "does not exist");

	try
	{
		return proton::get<std::string>(message->properties().get(name));
	}
	catch (const proton::conversion_error& er)
	{
		throw ::cms::MessageFormatException(er.what());
	}
}




//setters
void cms::amqp::AMQPCMSMessageConverter::setAMQPBooleanProperty(const std::string& name, bool value, std::shared_ptr<proton::message> message)
{
	message->properties().put(name, value);
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPByteProperty(const std::string& name, unsigned char value, std::shared_ptr<proton::message> message)
{
	message->properties().put(name, value);
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPDoubleProperty(const std::string& name, double value, std::shared_ptr<proton::message> message)
{
	message->properties().put(name, value);
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPFloatProperty(const std::string& name, float value, std::shared_ptr<proton::message> message)
{
	message->properties().put(name, value);
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPIntProperty(const std::string& name, int value, std::shared_ptr<proton::message> message)
{
	if (name == CMSXGroupSeq)
		message->group_sequence(value);
	else
		message->properties().put(name, value);
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPLongProperty(const std::string& name, long long value, std::shared_ptr<proton::message> message)
{
	message->properties().put(name, value);
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPShortProperty(const std::string& name, short value, std::shared_ptr<proton::message> message)
{
	message->properties().put(name, value);
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPStringProperty(const std::string& name, const std::string& value, std::shared_ptr<proton::message> message)
{
	if (name == CMSXGroupID)
		message->group_id(value);
	else
		message->properties().put(name, value);

}

///////////////////
std::string cms::amqp::AMQPCMSMessageConverter::getCMSCorrelationID(const std::shared_ptr<proton::message> message)
{
	std::string CMSCorrelationId;

	std::regex prefixRegex("^ID:(AMQP_BINARY:|AMQP_UUID:|AMQP_ULONG:|AMQP_NO_PREFIX:|AMQP_STRING:)");
	switch (proton::type_id type = message->correlation_id().type())
	{
	case proton::type_id::STRING:
		CMSCorrelationId.append(proton::get<std::string>(message->correlation_id()));
		if (std::regex_search(CMSCorrelationId, prefixRegex))
			CMSCorrelationId = "ID:AMQP_AMQP_STRING:" + CMSCorrelationId;
		//else if (CMSCorrelationId.substr(0, 3) != "ID:")
		//	CMSCorrelationId = "ID:AMQP_NO_PREFIX:" + CMSCorrelationId; //remove AMQP_NO_PREFIX?
		break;
	case proton::type_id::UUID:
		CMSCorrelationId = "ID:AMQP_UUID:";
		CMSCorrelationId.append(proton::get<proton::uuid>(message->correlation_id()).str());
		break;
	case proton::type_id::BINARY:
		CMSCorrelationId = "ID:AMQP_BINARY:";
		CMSCorrelationId.append(AMQPCMSMessageConverter::binaryArrayToHexString(proton::get<proton::binary>(message->correlation_id())));
		break;
	case proton::type_id::ULONG:
		CMSCorrelationId = "ID:AMQP_ULONG:";
		CMSCorrelationId.append(proton::to_string(proton::get<uint64_t>(message->correlation_id())));
		break;
	default:
		//to do handle default case
		break;
	}

	return CMSCorrelationId;
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPCorrelationID(const std::string& correlationId, std::shared_ptr<proton::message> message)
{
	auto prefixData = AMQPCMSMessageConverter::prefixToType(correlationId);
	std::string CorrelationId = correlationId.substr(std::get<1>(prefixData));

	switch (std::get<0>(prefixData))
	{
	case proton::type_id::STRING:
		message->correlation_id(correlationId);
		break;
	case proton::type_id::UUID:
		message->correlation_id(proton::uuid::copy(&*AMQPCMSMessageConverter::hexStringToArray(CorrelationId).begin()));
		break;
	case proton::type_id::BINARY:
		message->correlation_id(proton::binary(AMQPCMSMessageConverter::hexStringToBinaryArray(CorrelationId)));
		break;
	case proton::type_id::ULONG:
		message->correlation_id(std::stoul(CorrelationId));
		break;
	default:
		//to do handle default case
		break;
	}

	//	message->correlation_id(correlationId);
}

int cms::amqp::AMQPCMSMessageConverter::getCMSDeliveryMode(const std::shared_ptr<proton::message> message)
{
	//durable : true ::cms::DeliveryMode::PERSISTENT (0)
	// 	return !message->durable() -> 0 -> ::cms::DeliveryMode::PERSISTENT;
	return !message->durable();
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPDeliveryMode(int mode, std::shared_ptr<proton::message> message)
{
	message->durable(mode == ::cms::DeliveryMode::PERSISTENT);
}



void cms::amqp::AMQPCMSMessageConverter::setAMQPDestination(const::cms::Destination* destination, std::shared_ptr<proton::message> message)
{
	if (destination)
	{
		message->address(AMQPCMSMessageConverter::destinationToAddress(destination));
		message->to(message->address());
		message->message_annotations().put(X_OPT_JMS_DEST.data(), AMQPCMSMessageConverter::destinationTypeToJMSType(destination->getDestinationType()));
	
	}
}

long long cms::amqp::AMQPCMSMessageConverter::getCMSExpiration(const std::shared_ptr<proton::message> message)
{
	if (message->ttl() > proton::duration(0))
		return message->expiry_time().milliseconds();
	else
		return 0;
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPExpiration(long long expireTime, std::shared_ptr<proton::message> message)
{
	auto absolute_expire_time = proton::timestamp::now() + proton::duration(expireTime);

	if (expireTime > 0)
	{
		message->ttl(proton::duration(expireTime));
		message->expiry_time(absolute_expire_time);
		//		mMessage.message_annotations().put(proton::symbol("absolute-expiry-time"), proton::timestamp(absolute_expire_time));  //REDUNDANT
	}
}

//refactor, almost duplicate for CorrelationId
std::string cms::amqp::AMQPCMSMessageConverter::getCMSMessageID(const std::shared_ptr<proton::message> message)
{
	std::string CMSMessageId;

	std::regex prefixRegex("^ID:(AMQP_BINARY:|AMQP_UUID:|AMQP_ULONG:|AMQP_NO_PREFIX:|AMQP_STRING:)");
	switch (proton::type_id type = message->id().type())
	{
	case proton::type_id::STRING:
		CMSMessageId.append(proton::get<std::string>(message->id()));
		if (std::regex_search(CMSMessageId, prefixRegex))
			CMSMessageId = "ID:AMQP_STRING:" + CMSMessageId;
		else if (CMSMessageId.substr(0, 3) != "ID:")
			CMSMessageId = "ID:AMQP_NO_PREFIX:" + CMSMessageId;
		break;
	case proton::type_id::UUID:
		CMSMessageId = "ID:AMQP_UUID:";
		CMSMessageId.append(proton::get<proton::uuid>(message->id()).str());
		break;
	case proton::type_id::BINARY:
		CMSMessageId = "ID:AMQP_BINARY:";
		CMSMessageId.append(AMQPCMSMessageConverter::binaryArrayToHexString(proton::get<proton::binary>(message->id())));
		break;
	case proton::type_id::ULONG:
		CMSMessageId = "ID:AMQP_ULONG:";
		CMSMessageId.append(proton::to_string(proton::get<uint64_t>(message->id())));
		break;
	default:
		//to do handle default case
		break;
	}

	return CMSMessageId;
}


void cms::amqp::AMQPCMSMessageConverter::setAMQPMessageID(const std::string& id, std::shared_ptr<proton::message> message)
{
	auto prefixData = AMQPCMSMessageConverter::prefixToType(id);
	std::string messageId = id.substr(std::get<1>(prefixData));
	std::regex stringPrefixRegex("^ID:(AMQP_NO_PREFIX:|AMQP_STRING:)");

	switch (std::get<0>(prefixData))
	{
	case proton::type_id::STRING:
		message->id(std::regex_replace(id, stringPrefixRegex, ""));
		break;
	case proton::type_id::UUID:
		message->id(proton::uuid::copy(&*AMQPCMSMessageConverter::hexStringToArray(messageId).begin()));
		break;
	case proton::type_id::BINARY:
		message->id(proton::binary(AMQPCMSMessageConverter::hexStringToBinaryArray(messageId)));
		break;
	case proton::type_id::ULONG:
		message->id(std::stoul(messageId));
		break;
	default:
		//to do handle default case
		break;
	}

}

int cms::amqp::AMQPCMSMessageConverter::getCMSPriority(const std::shared_ptr<proton::message> message)
{
	//AMQP allows all range of uint8_t but CMS allows ints in range <0,9>

	return std::min(message->priority(), static_cast<uint8_t>(9));
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPPriority(int priority, std::shared_ptr<proton::message> message)
{	//AMQP allows all range of uint8_t but CMS allows ints in range <0,9>
	message->priority(std::min(std::abs(priority), 9));
}

bool cms::amqp::AMQPCMSMessageConverter::getCMSRedelivered(const std::shared_ptr<proton::message> message)
{
	return message->delivery_count() > 0;
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPRedelivered(bool redelivered, std::shared_ptr<proton::message> message)
{
	if(redelivered)
		message->delivery_count(message->delivery_count()+1);
	else
		message->delivery_count(0);
}


void cms::amqp::AMQPCMSMessageConverter::setAMQPReplyTo(const::cms::Destination* destination, std::shared_ptr<proton::message> message)
{
	message->reply_to(AMQPCMSMessageConverter::destinationToAddress(destination));
	message->message_annotations().put(X_OPT_JMS_REPLY_TO.data(), AMQPCMSMessageConverter::destinationTypeToJMSType(destination->getDestinationType()));
}

long long cms::amqp::AMQPCMSMessageConverter::getCMSTimestamp(const std::shared_ptr<proton::message> message)
{
	return message->creation_time().milliseconds();
}


void cms::amqp::AMQPCMSMessageConverter::setAMQPTimestamp(long long timeStamp, std::shared_ptr<proton::message> message)
{
	message->creation_time(proton::timestamp(timeStamp));
}

std::string cms::amqp::AMQPCMSMessageConverter::getCMSType(const std::shared_ptr<proton::message> message)
{
	return message->subject();
}

void cms::amqp::AMQPCMSMessageConverter::setAMQPType(const std::string& type, std::shared_ptr<proton::message> message)
{
	message->subject(type);
}