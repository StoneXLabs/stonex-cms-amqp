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

#include "Message.h"

#include <proton/message.hpp>
#include <proton/sender.hpp>
#include <proton/receiver.hpp>
#include <proton/value.hpp>
#include <proton/annotation_key.hpp>
#include <proton/types.hpp>
#include "Protocol/utils.h"

#include <cms/CMSException.h>


#include "MessageProducer.h"

cms::Message* stonex::amqp::Message::clone() const
{
	return new Message(*this);
}

void stonex::amqp::Message::clearBody()
{
	mMessage.clear();
}

void stonex::amqp::Message::clearProperties()
{
	mProperties.clear();
}

std::vector<std::string> stonex::amqp::Message::getPropertyNames() const
{
	return mProperties.getNames();
}

bool stonex::amqp::Message::propertyExists(const std::string& name) const
{
	return mProperties.exists(name);
}

cms::Message::ValueType stonex::amqp::Message::getPropertyValueType(const std::string& name) const
{
	if (!mProperties.exists(name))
		throw cms::CMSException("property name " + name + "does not exist");

	return mProperties.getType(name);
}

bool stonex::amqp::Message::getBooleanProperty(const std::string& name) const
{
	try
	{
		return mProperties.get<bool>(name);
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

unsigned char stonex::amqp::Message::getByteProperty(const std::string& name) const
{

	try
	{
		return mProperties.get<unsigned char>(name);
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

double stonex::amqp::Message::getDoubleProperty(const std::string& name) const
{

	try
	{
		return mProperties.get<double>(name);
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

float stonex::amqp::Message::getFloatProperty(const std::string& name) const
{

	try
	{
		return mProperties.get<float>(name);
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

int stonex::amqp::Message::getIntProperty(const std::string& name) const
{
	try
	{
		return mProperties.get<int>(name);
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

long long stonex::amqp::Message::getLongProperty(const std::string& name) const
{
	try
	{
		return mProperties.get<long long>(name);
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

short stonex::amqp::Message::getShortProperty(const std::string& name) const
{
	try
	{
		return mProperties.get<short>(name);
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

std::string stonex::amqp::Message::getStringProperty(const std::string& name) const
{
	try
	{
		return mProperties.get<std::string>(name);
	}
	catch (const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

void stonex::amqp::Message::setBooleanProperty(const std::string& name, bool value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");
	mProperties.set(name, value);
}

void stonex::amqp::Message::setByteProperty(const std::string& name, unsigned char value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::Message::setDoubleProperty(const std::string& name, double value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::Message::setFloatProperty(const std::string& name, float value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::Message::setIntProperty(const std::string& name, int value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::Message::setLongProperty(const std::string& name, long long value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::Message::setShortProperty(const std::string& name, short value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::Message::setStringProperty(const std::string& name, const std::string& value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

std::string stonex::amqp::Message::getCMSCorrelationID() const
{
	return mProperties.correlationId;
}

void stonex::amqp::Message::setCMSCorrelationID(const std::string& correlationId)
{
	mProperties.correlationId = correlationId;
}

int stonex::amqp::Message::getCMSDeliveryMode() const
{
	return mProperties.deliveryMode;
}

void stonex::amqp::Message::setCMSDeliveryMode(int mode)
{
	mProperties.deliveryMode = mode;
}

const cms::Destination* stonex::amqp::Message::getCMSDestination() const
{
	if(mProperties.destination)
		return internal::DestinationConverter::createCMSDestination(*mProperties.destination);
}

void stonex::amqp::Message::setCMSDestination(const cms::Destination* destination)
{
	mProperties.destination.reset(internal::DestinationConverter::createProtonDestination(destination));
}

long long stonex::amqp::Message::getCMSExpiration() const
{
	return mProperties.expiration;
}

void stonex::amqp::Message::setCMSExpiration(long long expireTime)
{
	mProperties.expiration = expireTime;
}

std::string stonex::amqp::Message::getCMSMessageID() const
{
	return mProperties.messageId;
}


void stonex::amqp::Message::setCMSMessageID(const std::string& id)
{
	mProperties.messageId = id;
}

int stonex::amqp::Message::getCMSPriority() const
{
	return mProperties.priority;
}

void stonex::amqp::Message::setCMSPriority(int priority)
{
	if (priority > std::numeric_limits<uint8_t>::max())
		throw cms::CMSException("Priority value cannot be greater than 255");

	mProperties.priority = priority;
}

bool stonex::amqp::Message::getCMSRedelivered() const
{
	return mProperties.redelivered;
}

void stonex::amqp::Message::setCMSRedelivered(bool redelivered)
{
	mProperties.redelivered = redelivered;
}

const cms::Destination* stonex::amqp::Message::getCMSReplyTo() const
{
	if (mProperties.replyTo)
		return internal::DestinationConverter::createCMSDestination(*mProperties.replyTo);
}

void stonex::amqp::Message::setCMSReplyTo(const cms::Destination* destination)
{
	mProperties.replyTo.reset(internal::DestinationConverter::createProtonDestination(destination));
}

long long stonex::amqp::Message::getCMSTimestamp() const
{
	return mProperties.timeStamp;
}

void stonex::amqp::Message::setCMSTimestamp(long long timeStamp)
{
	mProperties.timeStamp = timeStamp;
}

std::string stonex::amqp::Message::getCMSType() const
{
	return mProperties.type;
}


void stonex::amqp::Message::setCMSType(const std::string& type)
{
	mProperties.type = type;
}

