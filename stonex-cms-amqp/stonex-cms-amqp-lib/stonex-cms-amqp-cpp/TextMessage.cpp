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
:mMessage(text)
{
	mMessage.message_annotations().put(internal::annotation::JMS_MESSAGE_TYPE, static_cast<int8_t>(internal::annotation::MESSAGE_TYPE::TEXT_MESSAGE));
	// mMessage->durable(cms::DeliveryMode::PERSISTENT == 0); //default delivery mode PERISTENT -> durable(true)
	mMessage.priority(static_cast<uint8_t>(cms::Message::DEFAULT_MSG_PRIORITY));
	mMessage.body(text);
}


stonex::amqp::TextMessage::TextMessage(const proton::message& message)
:mMessage(message)
{
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
	mMessage.clear();
}


void stonex::amqp::TextMessage::clearProperties()
{
	mMessage.properties().clear();
	//mMessage.message_annotations().clear();
}

std::vector<std::string> stonex::amqp::TextMessage::getPropertyNames() const
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

bool stonex::amqp::TextMessage::propertyExists(const std::string& name) const
{
	return mMessage.properties().exists(name);
}

cms::Message::ValueType stonex::amqp::TextMessage::getPropertyValueType(const std::string& name) const
{
	if (!propertyExists(name))
		throw cms::CMSException("property name " + name + "does not exist");

	return internal::ValueTypeConverter::amqpToCms(mMessage.properties().get(name).type());
}


//getters
bool stonex::amqp::TextMessage::getBooleanProperty(const std::string& name) const
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

unsigned char stonex::amqp::TextMessage::getByteProperty(const std::string& name) const
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

double stonex::amqp::TextMessage::getDoubleProperty(const std::string& name) const
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

float stonex::amqp::TextMessage::getFloatProperty(const std::string& name) const
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

int stonex::amqp::TextMessage::getIntProperty(const std::string& name) const
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

long long stonex::amqp::TextMessage::getLongProperty(const std::string& name) const
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

short stonex::amqp::TextMessage::getShortProperty(const std::string& name) const
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

std::string stonex::amqp::TextMessage::getStringProperty(const std::string& name) const
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
void stonex::amqp::TextMessage::setBooleanProperty(const std::string& name, bool value)
{
	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::TextMessage::setByteProperty(const std::string& name, unsigned char value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::TextMessage::setDoubleProperty(const std::string& name, double value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::TextMessage::setFloatProperty(const std::string& name, float value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::TextMessage::setIntProperty(const std::string& name, int value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::TextMessage::setLongProperty(const std::string& name, long long value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::TextMessage::setShortProperty(const std::string& name, short value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

void stonex::amqp::TextMessage::setStringProperty(const std::string& name, const std::string& value)
{

	if (name.empty())
		throw cms::CMSException("property name cannot be empty");

	mMessage.properties().put(name, value);
}

//////
std::string stonex::amqp::TextMessage::getCMSCorrelationID() const
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

void stonex::amqp::TextMessage::setCMSCorrelationID(const std::string& correlationId)
{
	mMessage.correlation_id(correlationId);
}

int stonex::amqp::TextMessage::getCMSDeliveryMode() const
{
	return mMessage.durable() ? cms::DeliveryMode::PERSISTENT : cms::DeliveryMode::NON_PERSISTENT;
}

void stonex::amqp::TextMessage::setCMSDeliveryMode(int mode)
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

const cms::Destination* stonex::amqp::TextMessage::getCMSDestination() const
{
	return internal::DestinationConverter::createCMSDestination(mMessage);
}

void stonex::amqp::TextMessage::setCMSDestination(const cms::Destination* destination)
{
	mMessage.to(internal::DestinationConverter::address(destination));
	mMessage.message_annotations().put(internal::annotation::JMS_DESTINATION_TYPE, static_cast<int8_t>(internal::DestinationConverter::jmsDestinationType(destination)));
}

long long stonex::amqp::TextMessage::getCMSExpiration() const
{
	return mMessage.expiry_time().milliseconds();
}

void stonex::amqp::TextMessage::setCMSExpiration(long long expireTime)
{
	mMessage.expiry_time(proton::timestamp(expireTime));
}

std::string stonex::amqp::TextMessage::getCMSMessageID() const
{
	return proton::get<std::string>(mMessage.id());
}


void stonex::amqp::TextMessage::setCMSMessageID(const std::string& id)
{
	mMessage.id(id);
	mMessage.properties().put("message-id-string", "ID:"+id);
	mMessage.properties().put("JMSMessageID", "ID:AMQP_STRING:" + id);
}

int stonex::amqp::TextMessage::getCMSPriority() const
{
	return static_cast<int>(mMessage.priority());
}

void stonex::amqp::TextMessage::setCMSPriority(int priority)
{
	if (priority > std::numeric_limits<uint8_t>::max())
		throw cms::CMSException("Priority value cannot be greater than 255");

	mMessage.priority(static_cast<uint8_t>(priority));
}

bool stonex::amqp::TextMessage::getCMSRedelivered() const
{
	//should use delivery annotations?
	return mMessage.delivery_count() > 0;
}

void stonex::amqp::TextMessage::setCMSRedelivered(bool redelivered)
{
	if (redelivered)
		mMessage.delivery_count(mMessage.delivery_count() + 1);
	else
		mMessage.delivery_count(0);
}

const cms::Destination* stonex::amqp::TextMessage::getCMSReplyTo() const
{
	return internal::DestinationConverter::createCMSReplyTo(mMessage);
}

void stonex::amqp::TextMessage::setCMSReplyTo(const cms::Destination* destination)
{
	mMessage.reply_to(internal::DestinationConverter::address(destination));
	mMessage.message_annotations().put(internal::annotation::JMS_REPLY_TO_TYPE, static_cast<int8_t>(internal::DestinationConverter::jmsDestinationType(destination)));
}

long long stonex::amqp::TextMessage::getCMSTimestamp() const
{
	return mMessage.creation_time().milliseconds();
}

void stonex::amqp::TextMessage::setCMSTimestamp(long long timeStamp)
{
	mMessage.creation_time(proton::timestamp(timeStamp));
}

std::string stonex::amqp::TextMessage::getCMSType() const
{
	return mMessage.subject();
}


void stonex::amqp::TextMessage::setCMSType(const std::string& type)
{
	mMessage.subject(type);
}

std::string stonex::amqp::TextMessage::getText() const
{
	return proton::get<std::string>(mMessage.body());
}

void stonex::amqp::TextMessage::setText(const char* msg)
{
	mMessage.body(msg);
}

void stonex::amqp::TextMessage::setText(const std::string& msg)
{
	mMessage.body(msg);
}

