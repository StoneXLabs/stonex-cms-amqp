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

#include "TextMessage.h"

#include <regex>
#include <string_view>

#include <proton/delivery.hpp>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/annotation_key.hpp>
#include <proton/types.hpp>
#include "Protocol/utils.h"

#include <cms/InvalidDestinationException.h>


stonex::amqp::TextMessage::TextMessage(const std::string& text)
:mTextMessage(text)
{
}


stonex::amqp::TextMessage::TextMessage(const proton::message& message)
:mMessage(message)
{
	if (message.body().type() == proton::type_id::STRING)
	{
		mTextMessage = message.body().get<std::string>();
	}
}


cms::Message* stonex::amqp::TextMessage::clone() const
{
	return new TextMessage(*this);
}

void stonex::amqp::TextMessage::acknowledge() const
{ 

	//mMessageDelivery.accept();
}

void stonex::amqp::TextMessage::clearBody()
{
	mTextMessage.clear();
}


void stonex::amqp::TextMessage::clearProperties()
{
	mProperties.clear();
}

std::vector<std::string> stonex::amqp::TextMessage::getPropertyNames() const
{
	return mProperties.getNames();
}

bool stonex::amqp::TextMessage::propertyExists(const std::string& name) const
{
	return mProperties.exists(name);
}

cms::Message::ValueType stonex::amqp::TextMessage::getPropertyValueType(const std::string& name) const
{
	if (!propertyExists(name))
		throw cms::CMSException("property name " + name + "does not exist");

	return mProperties.getType(name);
}


//getters
bool stonex::amqp::TextMessage::getBooleanProperty(const std::string& name) const
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

unsigned char stonex::amqp::TextMessage::getByteProperty(const std::string& name) const
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

double stonex::amqp::TextMessage::getDoubleProperty(const std::string& name) const
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

float stonex::amqp::TextMessage::getFloatProperty(const std::string& name) const
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

int stonex::amqp::TextMessage::getIntProperty(const std::string& name) const
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

long long stonex::amqp::TextMessage::getLongProperty(const std::string& name) const
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

short stonex::amqp::TextMessage::getShortProperty(const std::string& name) const
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

std::string stonex::amqp::TextMessage::getStringProperty(const std::string& name) const
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

//setters
void stonex::amqp::TextMessage::setBooleanProperty(const std::string& name, bool value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");
	mProperties.set(name, value);
}

void stonex::amqp::TextMessage::setByteProperty(const std::string& name, unsigned char value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::TextMessage::setDoubleProperty(const std::string& name, double value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::TextMessage::setFloatProperty(const std::string& name, float value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::TextMessage::setIntProperty(const std::string& name, int value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::TextMessage::setLongProperty(const std::string& name, long long value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::TextMessage::setShortProperty(const std::string& name, short value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}

void stonex::amqp::TextMessage::setStringProperty(const std::string& name, const std::string& value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mProperties.set(name, value);
}
//////

std::string stonex::amqp::TextMessage::getCMSCorrelationID() const
{
	return mProperties.correlationId;
}

void stonex::amqp::TextMessage::setCMSCorrelationID(const std::string& correlationId)
{
	mProperties.correlationId = correlationId;
}

int stonex::amqp::TextMessage::getCMSDeliveryMode() const
{
	return mProperties.deliveryMode;
}

void stonex::amqp::TextMessage::setCMSDeliveryMode(int mode)
{
	mProperties.deliveryMode = mode;
}

const cms::Destination* stonex::amqp::TextMessage::getCMSDestination() const
{
	if (mProperties.destination)
		return internal::DestinationConverter::createCMSDestination(*mProperties.destination.get());
}

void stonex::amqp::TextMessage::setCMSDestination(const cms::Destination* destination)
{
	mProperties.destination.reset(internal::DestinationConverter::createProtonDestination(destination));
	/*
	mMessage.to(internal::DestinationConverter::address(destination));
	mMessage.message_annotations().put(internal::annotation::JMS_DESTINATION_TYPE, static_cast<int8_t>(internal::DestinationConverter::jmsDestinationType(destination)));*/
}

long long stonex::amqp::TextMessage::getCMSExpiration() const
{
	return mProperties.expiration;
}

void stonex::amqp::TextMessage::setCMSExpiration(long long expireTime)
{
	mProperties.expiration = expireTime;
}

std::string stonex::amqp::TextMessage::getCMSMessageID() const
{
	return mProperties.messageId;
}


void stonex::amqp::TextMessage::setCMSMessageID(const std::string& id)
{
	mProperties.messageId = id;
}

int stonex::amqp::TextMessage::getCMSPriority() const
{
	return mProperties.priority;
}

void stonex::amqp::TextMessage::setCMSPriority(int priority)
{
	if (priority > std::numeric_limits<uint8_t>::max())
		throw cms::CMSException("Priority value cannot be greater than 255");

	mProperties.priority = priority;
}

bool stonex::amqp::TextMessage::getCMSRedelivered() const
{
	//should use delivery annotations?
	return mProperties.redelivered;
}

void stonex::amqp::TextMessage::setCMSRedelivered(bool redelivered)
{
	mProperties.redelivered = redelivered;
}

const cms::Destination* stonex::amqp::TextMessage::getCMSReplyTo() const
{
	if (mProperties.replyTo)
		return internal::DestinationConverter::createCMSDestination(*mProperties.replyTo.get());
}

void stonex::amqp::TextMessage::setCMSReplyTo(const cms::Destination* destination)
{
	mProperties.replyTo.reset(internal::DestinationConverter::createProtonDestination(destination));
	/*
	mMessage.reply_to(internal::DestinationConverter::address(destination));
	mMessage.message_annotations().put(internal::annotation::JMS_REPLY_TO_TYPE, static_cast<int8_t>(internal::DestinationConverter::jmsDestinationType(destination)));*/
}

long long stonex::amqp::TextMessage::getCMSTimestamp() const
{
	return mProperties.timeStamp;
}

void stonex::amqp::TextMessage::setCMSTimestamp(long long timeStamp)
{
	mProperties.timeStamp = timeStamp;
}

std::string stonex::amqp::TextMessage::getCMSType() const
{
	return mProperties.type;
}


void stonex::amqp::TextMessage::setCMSType(const std::string& type)
{
	mProperties.type = type;
}

std::string stonex::amqp::TextMessage::getText() const
{
	return mTextMessage;
}

void stonex::amqp::TextMessage::setText(const char* msg)
{
	mTextMessage = msg;
}

void stonex::amqp::TextMessage::setText(const std::string& msg)
{
	mTextMessage = msg;
}

