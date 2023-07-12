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

#include "CMSBytesMessage.h"

#include <AMQPCMSMessageConverter.h>

#include <stdexcept>
#include <regex>
#include <string_view>

#include <proton/delivery.hpp>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/annotation_key.hpp>
#include <proton/byte_array.hpp>

#include <cms/InvalidDestinationException.h>
#include <cms/BytesMessage.h>

cms::amqp::CMSBytesMessage::CMSBytesMessage()
{
	mMessage = std::make_shared<proton::message>();
	mMessage->durable(cms::DeliveryMode::PERSISTENT == 0); //default delivery mode PERISTENT
	mMessage->priority(::cms::Message::DEFAULT_MSG_PRIORITY);
	mMessage->message_annotations().put(X_OPT_JMS_MESSAGE_TYPE.data(), static_cast<int8_t>(MESSAGE_TYPE::BYTES_MESSAGE));
	mMessage->content_type("application/octet-stream");
}

cms::amqp::CMSBytesMessage::CMSBytesMessage(const CMSBytesMessage& other)
	:mMessage{ std::make_shared<proton::message>(*other.mMessage) },
	mMessageBody{other.mMessageBody},
	mMessageDelivery{other.mMessageDelivery},
	mDestination{other.mDestination}
{
}

cms::amqp::CMSBytesMessage::CMSBytesMessage(const unsigned char* bytes, int bytesSize)
	:CMSBytesMessage()
{
	setBodyBytes(bytes, bytesSize);
}

cms::amqp::CMSBytesMessage::CMSBytesMessage(proton::message* message, const proton::delivery* delivery, const proton::receiver* receiver)
	: mMessage(std::make_shared<proton::message>(*message)),
	mMessageDelivery{ delivery },
	mMessageBody(proton::get<proton::binary>(mMessage->body()))
{
	try
	{
		mDestination.reset(AMQPCMSMessageConverter::createCMSDestination(mMessage));
	}
	catch (const ::cms::InvalidDestinationException& ex)
	{

		try
		{
			mDestination.reset(AMQPCMSMessageConverter::createCMSDestination(receiver));
		}
		catch (const ::cms::InvalidDestinationException& ex)
		{

		}
	}

	try
	{
		if (!mMessage->reply_to().empty() && !mReplyTo)
			mReplyTo.reset(AMQPCMSMessageConverter::createCMSReplyToUsingMessageDestination(mMessage));
	}
	catch (const ::cms::InvalidDestinationException& ex)
	{

	}

}



void cms::amqp::CMSBytesMessage::acknowledge() const
{
//	mMessageDelivery-> accept();
	const_cast<proton::delivery*>(mMessageDelivery)->accept();
}

void cms::amqp::CMSBytesMessage::clearBody()
{
	mMessage->clear();
	read_position = 0;
	mMessageBody.clear();
}


void cms::amqp::CMSBytesMessage::clearProperties()
{
	mMessage->properties().clear();
}

std::vector<std::string> cms::amqp::CMSBytesMessage::getPropertyNames() const
{
	return AMQPCMSMessageConverter::getPropertyNames(mMessage);
}

bool cms::amqp::CMSBytesMessage::propertyExists(const std::string& name) const
{
	return AMQPCMSMessageConverter::propertyExists(name, mMessage);
}

::cms::Message::ValueType cms::amqp::CMSBytesMessage::getPropertyValueType(const std::string& name) const
{
	return AMQPCMSMessageConverter::type_id_to_ValueType(mMessage->properties().get(name).type());
}


//getters
bool cms::amqp::CMSBytesMessage::getBooleanProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSBooleanProperty(name, mMessage);
}

unsigned char cms::amqp::CMSBytesMessage::getByteProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSByteProperty(name, mMessage);
}

double cms::amqp::CMSBytesMessage::getDoubleProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSDoubleProperty(name, mMessage);
}

float cms::amqp::CMSBytesMessage::getFloatProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSFloatProperty(name, mMessage);
}

int cms::amqp::CMSBytesMessage::getIntProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSIntProperty(name, mMessage);
}
long long cms::amqp::CMSBytesMessage::getLongProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSLongProperty(name, mMessage);
}

short cms::amqp::CMSBytesMessage::getShortProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSShortProperty(name, mMessage);
}

std::string cms::amqp::CMSBytesMessage::getStringProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSStringProperty(name, mMessage);
}




//setters
void cms::amqp::CMSBytesMessage::setBooleanProperty(const std::string& name, bool value)
{
	AMQPCMSMessageConverter::setAMQPBooleanProperty(name, value, mMessage);
}

void cms::amqp::CMSBytesMessage::setByteProperty(const std::string& name, unsigned char value)
{
	AMQPCMSMessageConverter::setAMQPByteProperty(name, value, mMessage);
}

void cms::amqp::CMSBytesMessage::setDoubleProperty(const std::string& name, double value)
{
	AMQPCMSMessageConverter::setAMQPDoubleProperty(name, value, mMessage);
}

void cms::amqp::CMSBytesMessage::setFloatProperty(const std::string& name, float value)
{
	AMQPCMSMessageConverter::setAMQPFloatProperty(name, value, mMessage);
}

void cms::amqp::CMSBytesMessage::setIntProperty(const std::string& name, int value)
{
	AMQPCMSMessageConverter::setAMQPIntProperty(name, value, mMessage);
}

void cms::amqp::CMSBytesMessage::setLongProperty(const std::string& name, long long value)
{
	AMQPCMSMessageConverter::setAMQPLongProperty(name, value, mMessage);
}

void cms::amqp::CMSBytesMessage::setShortProperty(const std::string& name, short value)
{
	AMQPCMSMessageConverter::setAMQPShortProperty(name, value, mMessage);
}

void cms::amqp::CMSBytesMessage::setStringProperty(const std::string& name, const std::string& value)
{
	AMQPCMSMessageConverter::setAMQPStringProperty(name, value, mMessage);
}
//////
std::string cms::amqp::CMSBytesMessage::getCMSCorrelationID() const
{
	return AMQPCMSMessageConverter::getCMSCorrelationID(mMessage);
}

void cms::amqp::CMSBytesMessage::setCMSCorrelationID(const std::string& correlationId)
{
	return AMQPCMSMessageConverter::setAMQPCorrelationID(correlationId, mMessage);
}

int cms::amqp::CMSBytesMessage::getCMSDeliveryMode() const
{
	return AMQPCMSMessageConverter::getCMSDeliveryMode(mMessage);
}

void cms::amqp::CMSBytesMessage::setCMSDeliveryMode(int mode)
{
	return AMQPCMSMessageConverter::setAMQPDeliveryMode(mode, mMessage);
}

const::cms::Destination* cms::amqp::CMSBytesMessage::getCMSDestination() const
{
	return mDestination.get();
}

void cms::amqp::CMSBytesMessage::setCMSDestination(const::cms::Destination* destination)
{
	AMQPCMSMessageConverter::setAMQPDestination(destination, mMessage);
	if (destination)
		mDestination.reset(destination->clone());
	else
		mDestination.reset();
}

long long cms::amqp::CMSBytesMessage::getCMSExpiration() const
{
	return AMQPCMSMessageConverter::getCMSExpiration(mMessage);
}

void cms::amqp::CMSBytesMessage::setCMSExpiration(long long expireTime)
{
	return AMQPCMSMessageConverter::setAMQPExpiration(expireTime, mMessage);
}

std::string cms::amqp::CMSBytesMessage::getCMSMessageID() const
{
	return AMQPCMSMessageConverter::getCMSMessageID(mMessage);
}


void cms::amqp::CMSBytesMessage::setCMSMessageID(const std::string& id)
{
	return AMQPCMSMessageConverter::setAMQPMessageID(id, mMessage);
}

int cms::amqp::CMSBytesMessage::getCMSPriority() const
{
	return AMQPCMSMessageConverter::getCMSPriority(mMessage);
}

void cms::amqp::CMSBytesMessage::setCMSPriority(int priority)
{
	return AMQPCMSMessageConverter::setAMQPPriority(priority, mMessage);
}

bool cms::amqp::CMSBytesMessage::getCMSRedelivered() const
{
	return AMQPCMSMessageConverter::getCMSRedelivered(mMessage);
}

void cms::amqp::CMSBytesMessage::setCMSRedelivered(bool redelivered)
{
	return AMQPCMSMessageConverter::setAMQPRedelivered(redelivered, mMessage);
}

const::cms::Destination* cms::amqp::CMSBytesMessage::getCMSReplyTo() const
{
	return mReplyTo.get();
}

void cms::amqp::CMSBytesMessage::setCMSReplyTo(const::cms::Destination* destination)
{
	AMQPCMSMessageConverter::setAMQPReplyTo(destination, mMessage);
	if (destination)
		mReplyTo.reset(destination->clone());
	else
		mReplyTo.reset();
}

long long cms::amqp::CMSBytesMessage::getCMSTimestamp() const
{
	return AMQPCMSMessageConverter::getCMSTimestamp(mMessage);
}

void cms::amqp::CMSBytesMessage::setCMSTimestamp(long long timeStamp)
{
	return AMQPCMSMessageConverter::setAMQPTimestamp(timeStamp, mMessage);
}

std::string cms::amqp::CMSBytesMessage::getCMSType() const
{
	return AMQPCMSMessageConverter::getCMSType(mMessage);
}


void cms::amqp::CMSBytesMessage::setCMSType(const std::string& type)
{
	return AMQPCMSMessageConverter::setAMQPType(type, mMessage);
}

///Bytes message impl
void cms::amqp::CMSBytesMessage::setBodyBytes(const unsigned char* buffer, int numBytes)
{
	mMessageBody.clear();
	std::copy_n(buffer, numBytes, std::back_inserter(mMessageBody));
	read_position = 0;
}

unsigned char* cms::amqp::CMSBytesMessage::getBodyBytes() const
{
	if (mMessageBody.empty())
		throw ::cms::CMSException("no more data");

	auto message = proton::get<proton::binary>(mMessage->body());

	uint8_t* data = new uint8_t[mMessageBody.size()+1];
	uint8_t* data_last = data + mMessageBody.size();
	std::copy(std::begin(mMessageBody), std::end(mMessageBody), data);
	*data_last = 0x00;
	return data;
}

int  cms::amqp::CMSBytesMessage::getBodyLength() const
{
	return mMessageBody.size();
}

void cms::amqp::CMSBytesMessage::reset()
{
	read_position = 0;
}

bool cms::amqp::CMSBytesMessage::readBoolean() const
{
	try
	{
		auto value = static_cast<bool>(mMessageBody.at(read_position));
		read_position++;
		return value;
	}
	catch (const std::out_of_range&)
	{
		throw ::cms::MessageEOFException();
	}

}

void cms::amqp::CMSBytesMessage::writeBoolean(bool value)
{
	mMessageBody.push_back(static_cast<uint8_t>(value));
}

unsigned char cms::amqp::CMSBytesMessage::readByte() const
{
	try
	{
		auto value =  mMessageBody.at(read_position);
		read_position++;
		return value;
	}
	catch (const std::out_of_range&)
	{
		throw ::cms::MessageEOFException();
	}
}

void cms::amqp::CMSBytesMessage::writeByte(unsigned char value)
{
	mMessageBody.push_back(static_cast<uint8_t>(value));
}

int cms::amqp::CMSBytesMessage::readBytes(std::vector<unsigned char>& value) const
{
	int copy_count{ 0 };
	std::copy_if(std::next(mMessageBody.begin(), read_position), std::cend(mMessageBody), std::begin(value), [&copy_count](uint8_t item) {copy_count++; return true; });

	read_position += copy_count;

	return copy_count;
}

void cms::amqp::CMSBytesMessage::writeBytes(const std::vector<unsigned char>& value)
{
	std::copy(std::cbegin(value), std::cend(value), std::back_inserter(mMessageBody));
}

int cms::amqp::CMSBytesMessage::readBytes(unsigned char* buffer, int length) const
{
	//TO DO ranges
	if (int size = mMessageBody.size();size == 0 || size < read_position)
		throw ::cms::MessageEOFException();

	std::copy_n(std::next(mMessageBody.begin(), read_position), length, buffer);
	read_position += length;
	return length;
}

void cms::amqp::CMSBytesMessage::writeBytes(const unsigned char* value, int offset, int length)
{
	//TO DO offset
	throw std::runtime_error("not implemented");
	std::copy_n(value, length, std::back_inserter(mMessageBody));
}

char cms::amqp::CMSBytesMessage::readChar() const
{
	return get<char>();
}

void cms::amqp::CMSBytesMessage::writeChar(char value)
{
	set<char>(value);
}

float cms::amqp::CMSBytesMessage::readFloat() const
{
	return get<float>();
}

void cms::amqp::CMSBytesMessage::writeFloat(float value)
{
	set<float>(value);
}

double cms::amqp::CMSBytesMessage::readDouble() const
{
	return get<double>();
}
void cms::amqp::CMSBytesMessage::writeDouble(double value)
{
	set<double>(value);
}

short cms::amqp::CMSBytesMessage::readShort() const
{
	return get<short>();
}

void cms::amqp::CMSBytesMessage::writeShort(short value)
{
	set<short>(value);
}

unsigned short cms::amqp::CMSBytesMessage::readUnsignedShort() const
{
	return get<unsigned short>();
}

void cms::amqp::CMSBytesMessage::writeUnsignedShort(unsigned short value)
{
	set<unsigned short>(value);
}

int cms::amqp::CMSBytesMessage::readInt() const
{
	return get<int>();
}

void cms::amqp::CMSBytesMessage::writeInt(int value)
{
	set<int>(value);
}

long long cms::amqp::CMSBytesMessage::readLong() const
{
	return get<long long>();
}

void cms::amqp::CMSBytesMessage::writeLong(long long value)
{
	set<long long>(value);
}

std::string cms::amqp::CMSBytesMessage::readString() const
{
	return get<std::string>();
}

void cms::amqp::CMSBytesMessage::writeString(const std::string& value)
{
	set<std::string>(value);
}

std::string cms::amqp::CMSBytesMessage::readUTF() const
{
	return {};
}

void cms::amqp::CMSBytesMessage::writeUTF(const std::string& value)
{}

::cms::BytesMessage* cms::amqp::CMSBytesMessage::clone() const
{
	return new CMSBytesMessage(*this);
}

std::shared_ptr<proton::message> cms::amqp::CMSBytesMessage::message()
{
	mMessage->body(proton::binary(std::cbegin(mMessageBody), std::cend(mMessageBody)));
	return mMessage;
}
