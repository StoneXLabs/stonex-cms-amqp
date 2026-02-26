/*
 * Copyright 2022 - 2023 StoneX Financial Ltd.
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

#include "BytesMessage.h"


#include <regex>
#include <string_view>

#include <proton/delivery.hpp>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/annotation_key.hpp>
#include <proton/byte_array.hpp>
#include <proton/types.hpp>

#include <cms/InvalidDestinationException.h>
#include <cms/BytesMessage.h>
#include "Protocol/utils.h"
#include <proton/type_id.hpp>


stonex::amqp::BytesMessage::BytesMessage(const unsigned char* array, size_t size)
{
	if(array == nullptr || size == 0)
	{
		mMessage.body(proton::binary());
		return;
	}
	else
	{
		std::vector<unsigned char> buf(array, array + size);
		mMessage.body(proton::binary(buf));
	}

	mMessage.message_annotations().put(internal::annotation::JMS_MESSAGE_TYPE, static_cast<int8_t>(internal::annotation::MESSAGE_TYPE::BYTES_MESSAGE));
	mMessage.priority(static_cast<uint8_t>(cms::Message::DEFAULT_MSG_PRIORITY));
}

stonex::amqp::BytesMessage::BytesMessage(const proton::message& message)
:mMessage(message)
{

}

stonex::amqp::BytesMessage::BytesMessage(const BytesMessage& other)
	:mMessage{other.mMessage}
{
}

void stonex::amqp::BytesMessage::acknowledge() const
{
//	const_cast<proton::delivery*>(mMessageDelivery)->accept();
}

void stonex::amqp::BytesMessage::clearBody()
{
	mMessage.clear();
}


void stonex::amqp::BytesMessage::clearProperties()
{
	mMessage.properties().clear();
}

std::vector<std::string> stonex::amqp::BytesMessage::getPropertyNames() const
{
	std::map<std::string, proton::scalar> properties;
	proton::get(mMessage.properties(), properties);

	std::vector<std::string> propertyVector;
	propertyVector.reserve(properties.size());
	
	for(const auto& [key, value] : properties)
	{
		propertyVector.emplace_back(key);
	}

	return propertyVector;
}

bool stonex::amqp::BytesMessage::propertyExists(const std::string& name) const
{
	return mMessage.properties().exists(name);
}

cms::Message::ValueType stonex::amqp::BytesMessage::getPropertyValueType(const std::string& name) const
{
	if (!propertyExists(name))
		throw cms::CMSException("property name " + name + "does not exist");

	return internal::ValueTypeConverter::amqpToCms(mMessage.properties().get(name).type());
}

//getters
bool stonex::amqp::BytesMessage::getBooleanProperty(const std::string& name) const
{
	try
	{
		return proton::get<bool>(mMessage.properties().get(name));
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

unsigned char stonex::amqp::BytesMessage::getByteProperty(const std::string& name) const
{

	try
	{
		return proton::get<unsigned char>(mMessage.properties().get(name));
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

double stonex::amqp::BytesMessage::getDoubleProperty(const std::string& name) const
{

	try
	{
		return proton::get<double>(mMessage.properties().get(name));
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

float stonex::amqp::BytesMessage::getFloatProperty(const std::string& name) const
{

	try
	{
		return proton::get<float>(mMessage.properties().get(name));
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

int stonex::amqp::BytesMessage::getIntProperty(const std::string& name) const
{

	try
	{
		return proton::get<int>(mMessage.properties().get(name));
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

long long stonex::amqp::BytesMessage::getLongProperty(const std::string& name) const
{

	try
	{
		return proton::get<long long>(mMessage.properties().get(name));
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

short stonex::amqp::BytesMessage::getShortProperty(const std::string& name) const
{

	try
	{
		return proton::get<short>(mMessage.properties().get(name));
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

std::string stonex::amqp::BytesMessage::getStringProperty(const std::string& name) const
{

	try
	{
		return proton::get<std::string>(mMessage.properties().get(name));
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}


//setters
void stonex::amqp::BytesMessage::setBooleanProperty(const std::string& name, bool value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::BytesMessage::setByteProperty(const std::string& name, unsigned char value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::BytesMessage::setDoubleProperty(const std::string& name, double value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::BytesMessage::setFloatProperty(const std::string& name, float value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::BytesMessage::setIntProperty(const std::string& name, int value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::BytesMessage::setLongProperty(const std::string& name, long long value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::BytesMessage::setShortProperty(const std::string& name, short value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::BytesMessage::setStringProperty(const std::string& name, const std::string& value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

///////

std::string stonex::amqp::BytesMessage::getCMSCorrelationID() const
{
	try
	{
		return proton::get<std::string>(mMessage.correlation_id());
	}
	catch (const std::exception&)
	{
		return "";
	}
}

void stonex::amqp::BytesMessage::setCMSCorrelationID(const std::string& correlationId)
{
	mMessage.correlation_id(correlationId);
}

int stonex::amqp::BytesMessage::getCMSDeliveryMode() const
{
	return mMessage.durable() ? cms::DeliveryMode::PERSISTENT : cms::DeliveryMode::NON_PERSISTENT;
}

void stonex::amqp::BytesMessage::setCMSDeliveryMode(int mode)
{
	if (mode == cms::DeliveryMode::DELIVERY_MODE::NON_PERSISTENT)
	{
		mMessage.durable(false);
	}
	else if (mode == cms::DeliveryMode::DELIVERY_MODE::PERSISTENT)
	{
		mMessage.durable(true);
	}
}

const cms::Destination* stonex::amqp::BytesMessage::getCMSDestination() const
{
	return internal::DestinationConverter::createCMSDestination(mMessage);
}

void stonex::amqp::BytesMessage::setCMSDestination(const cms::Destination* destination)
{
	mMessage.to(internal::DestinationConverter::address(destination));
	mMessage.message_annotations().put(internal::annotation::JMS_DESTINATION_TYPE, static_cast<int8_t>(internal::DestinationConverter::jmsDestinationType(destination)));
}

long long stonex::amqp::BytesMessage::getCMSExpiration() const
{
	return mMessage.expiry_time().milliseconds();
}

void stonex::amqp::BytesMessage::setCMSExpiration(long long expireTime)
{
	mMessage.expiry_time(proton::timestamp(expireTime));
}

std::string stonex::amqp::BytesMessage::getCMSMessageID() const
{
	return proton::get<std::string>(mMessage.id());
}


void stonex::amqp::BytesMessage::setCMSMessageID(const std::string& id)
{
	mMessage.id(id);
	mMessage.properties().put("message-id-string", "ID:"+id);
	mMessage.properties().put("JMSMessageID", "ID:AMQP_STRING:" + id);
}

int stonex::amqp::BytesMessage::getCMSPriority() const
{
	return static_cast<int>(mMessage.priority());
}

void stonex::amqp::BytesMessage::setCMSPriority(int priority)
{
	if (priority > std::numeric_limits<uint8_t>::max())
		throw cms::CMSException("Priority value cannot be greater than 255");

	mMessage.priority(static_cast<uint8_t>(priority));
}

bool stonex::amqp::BytesMessage::getCMSRedelivered() const
{
	//should use delivery annotations?
	return mMessage.delivery_count() > 0;
}

void stonex::amqp::BytesMessage::setCMSRedelivered(bool redelivered)
{
	if (redelivered)
		mMessage.delivery_count(mMessage.delivery_count() + 1);
	else
		mMessage.delivery_count(0);
}

const cms::Destination* stonex::amqp::BytesMessage::getCMSReplyTo() const
{
	return internal::DestinationConverter::createCMSReplyTo(mMessage);
}

void stonex::amqp::BytesMessage::setCMSReplyTo(const cms::Destination* destination)
{
	mMessage.reply_to(internal::DestinationConverter::address(destination));
	mMessage.message_annotations().put(internal::annotation::JMS_REPLY_TO_TYPE, static_cast<int8_t>(internal::DestinationConverter::jmsDestinationType(destination)));
}

long long stonex::amqp::BytesMessage::getCMSTimestamp() const
{
	return mMessage.creation_time().milliseconds();
}

void stonex::amqp::BytesMessage::setCMSTimestamp(long long timeStamp)
{
	mMessage.creation_time(proton::timestamp(timeStamp));
}

std::string stonex::amqp::BytesMessage::getCMSType() const
{
	return mMessage.subject();
}


void stonex::amqp::BytesMessage::setCMSType(const std::string& type)
{
	mMessage.subject(type);
}

///Bytes message impl
void stonex::amqp::BytesMessage::setBodyBytes(const unsigned char* buffer, int numBytes)
{
	std::vector<char> bufferVector(buffer, buffer + numBytes);
	mMessage.clear();
	mMessage.decode(bufferVector);
	read_position = 0;
}

unsigned char* stonex::amqp::BytesMessage::getBodyBytes() const
{
	try
	{
		const auto body = proton::get<proton::binary>(mMessage.body());
		unsigned char* data = new uint8_t[body.size()];
		std::memcpy(data, body.data(), body.size());
		return data;
	}
	catch (const std::exception& e)
	{
		throw cms::CMSException(e.what());
	}
}

int stonex::amqp::BytesMessage::getBodyLength() const
{
	try
	{
		return static_cast<int>(proton::get<proton::binary>(mMessage.body()).size());
	}
	catch (const std::exception& e)
	{
		throw cms::CMSException(e.what());
	}
}

void stonex::amqp::BytesMessage::reset()
{
	read_position = 0;
}

bool stonex::amqp::BytesMessage::readBoolean() const
{
	return get<bool>();

}

void stonex::amqp::BytesMessage::writeBoolean(bool value)
{
	set<bool>(value);
}

unsigned char stonex::amqp::BytesMessage::readByte() const
{
	return get<unsigned char>();
}

void stonex::amqp::BytesMessage::writeByte(unsigned char value)
{
	
	set<unsigned char>(value);
}

int stonex::amqp::BytesMessage::readBytes(std::vector<unsigned char>& value) const
{
	try
	{
		const auto body = proton::get<proton::binary>(mMessage.body());

		if (body.empty())
			return -1;

		const auto remainingBytes = body.size() - read_position >= value.size() ? value.size() : body.size() - read_position;


		value = std::vector<unsigned char>(body.begin() + read_position, body.begin() + remainingBytes + read_position);
		read_position = read_position += remainingBytes;
		return remainingBytes;
	}
	catch (const std::exception& ex)
	{
		throw cms::CMSException(ex.what());
	}
}

void stonex::amqp::BytesMessage::writeBytes(const std::vector<unsigned char>& value)
{
	auto body = proton::get<proton::binary>(mMessage.body());

	for(const auto& byte : value)
	{
		body.push_back(static_cast<uint8_t>(byte));
	}

	mMessage.body(std::move(body));
}

int stonex::amqp::BytesMessage::readBytes(unsigned char* buffer, int length) const
{
	try
	{
		float value{ 0 };
		const auto body = proton::get<proton::binary>(mMessage.body());

		if (body.size() - read_position < length)
			return -1;


		const auto remainingBytes = sizeof(float);
		memcpy(&value, &body.at(read_position), remainingBytes);
		read_position = read_position += remainingBytes;
		return value;
	}
	catch (const std::exception&)
	{
	}
	return -1;
}

void stonex::amqp::BytesMessage::writeBytes(const unsigned char* value, int offset, int length)
{
	//TO DO offset
	throw std::exception("not implemented");
}

char stonex::amqp::BytesMessage::readChar() const
{
	return get<char>();
}

void stonex::amqp::BytesMessage::writeChar(char value)
{
	set<char>(value);
}

float stonex::amqp::BytesMessage::readFloat() const
{
	return get<float>();
}

void stonex::amqp::BytesMessage::writeFloat(float value)
{
	set<float>(value);
}

double stonex::amqp::BytesMessage::readDouble() const
{
	return get<double>();
}

void stonex::amqp::BytesMessage::writeDouble(double value)
{
	set<double>(value);
}

short stonex::amqp::BytesMessage::readShort() const
{
	return get<short>();
}

void stonex::amqp::BytesMessage::writeShort(short value)
{
	set<short>(value);
}

unsigned short stonex::amqp::BytesMessage::readUnsignedShort() const
{
	return get<unsigned short>();
}

void stonex::amqp::BytesMessage::writeUnsignedShort(unsigned short value)
{
	set<unsigned short>(value);
}

int stonex::amqp::BytesMessage::readInt() const
{
	return get<int>();
}

void stonex::amqp::BytesMessage::writeInt(int value)
{
	set<int>(value);
}

long long stonex::amqp::BytesMessage::readLong() const
{
	return get<long long>();
}

void stonex::amqp::BytesMessage::writeLong(long long value)
{
	set<long long>(value);
}

std::string stonex::amqp::BytesMessage::readString() const
{
	return get<std::string>();
}

void stonex::amqp::BytesMessage::writeString(const std::string& value)
{
	set<std::string>(value);
}

std::string stonex::amqp::BytesMessage::readUTF() const
{
	return {};
}

void stonex::amqp::BytesMessage::writeUTF(const std::string& value)
{}

cms::BytesMessage* stonex::amqp::BytesMessage::clone() const
{
	return new BytesMessage(*this);
}