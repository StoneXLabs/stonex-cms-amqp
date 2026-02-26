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
	mMessage.properties().clear();
}

std::vector<std::string> stonex::amqp::Message::getPropertyNames() const
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

bool stonex::amqp::Message::propertyExists(const std::string& name) const
{
	return mMessage.properties().exists(name);
}

cms::Message::ValueType stonex::amqp::Message::getPropertyValueType(const std::string& name) const
{
	if (!propertyExists(name))
		throw cms::CMSException("property name " + name + "does not exist");

	return internal::ValueTypeConverter::amqpToCms(mMessage.properties().get(name).type());
}

//getters
bool stonex::amqp::Message::getBooleanProperty(const std::string& name) const
{
	try
	{
		return proton::get<bool>(mMessage.properties().get(name));
	}
	catch(const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

unsigned char stonex::amqp::Message::getByteProperty(const std::string& name) const
{
	
	try
	{
		return proton::get<unsigned char>(mMessage.properties().get(name));
	}
	catch(const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

double stonex::amqp::Message::getDoubleProperty(const std::string& name) const
{
	
	try
	{
		return proton::get<double>(mMessage.properties().get(name));
	}
	catch(const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

float stonex::amqp::Message::getFloatProperty(const std::string& name) const
{
	
	try
	{
		return proton::get<float>(mMessage.properties().get(name));
	}
	catch(const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

int stonex::amqp::Message::getIntProperty(const std::string& name) const
{
	
	try
	{
		return proton::get<int>(mMessage.properties().get(name));
	}
	catch(const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

long long stonex::amqp::Message::getLongProperty(const std::string& name) const
{
	
	try
	{
		return proton::get<long long>(mMessage.properties().get(name));
	}
	catch(const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

short stonex::amqp::Message::getShortProperty(const std::string& name) const
{
	
	try
	{
		return proton::get<short>(mMessage.properties().get(name));
	}
	catch(const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

std::string stonex::amqp::Message::getStringProperty(const std::string& name) const
{
	
	try
	{
		return proton::get<std::string>(mMessage.properties().get(name));
	}
	catch(const std::exception& e)
	{
		throw cms::MessageFormatException(e.what());
	}
}

//setters
void stonex::amqp::Message::setBooleanProperty(const std::string& name, bool value)
{
	if(name.empty())
		throw cms::CMSException("property name cannot be empty");
		
	mMessage.properties().put(name, value);
}

void stonex::amqp::Message::setByteProperty(const std::string& name, unsigned char value)
{
	
	if(name.empty())
		throw cms::CMSException("property name cannot be empty");
		
	mMessage.properties().put(name, value);
}

void stonex::amqp::Message::setDoubleProperty(const std::string& name, double value)
{
	
	if(name.empty())
		throw cms::CMSException("property name cannot be empty");
		
	mMessage.properties().put(name, value);
}

void stonex::amqp::Message::setFloatProperty(const std::string& name, float value)
{
	
	if(name.empty())
		throw cms::CMSException("property name cannot be empty");
		
	mMessage.properties().put(name, value);
}

void stonex::amqp::Message::setIntProperty(const std::string& name, int value)
{
	
	if(name.empty())
		throw cms::CMSException("property name cannot be empty");
		
	mMessage.properties().put(name, value);
}

void stonex::amqp::Message::setLongProperty(const std::string& name, long long value)
{
	
	if(name.empty())
		throw cms::CMSException("property name cannot be empty");
		
	mMessage.properties().put(name, value);
}

void stonex::amqp::Message::setShortProperty(const std::string& name, short value)
{
	
	if(name.empty())
		throw cms::CMSException("property name cannot be empty");
		
	mMessage.properties().put(name, value);
}

void stonex::amqp::Message::setStringProperty(const std::string& name, const std::string& value)
{
	
	if(name.empty())
		throw cms::CMSException("property name cannot be empty");
		
	mMessage.properties().put(name, value);
}

std::string stonex::amqp::Message::getCMSCorrelationID() const
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

void stonex::amqp::Message::setCMSCorrelationID(const std::string& correlationId)
{
	mMessage.correlation_id(correlationId);
}

int stonex::amqp::Message::getCMSDeliveryMode() const
{
	return mMessage.durable() ? cms::DeliveryMode::PERSISTENT : cms::DeliveryMode::NON_PERSISTENT;
}

void stonex::amqp::Message::setCMSDeliveryMode(int mode)
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

const cms::Destination* stonex::amqp::Message::getCMSDestination() const
{
	return internal::DestinationConverter::createCMSDestination(mMessage);
}

void stonex::amqp::Message::setCMSDestination(const cms::Destination* destination)
{
	mMessage.to(internal::DestinationConverter::address(destination));
	mMessage.message_annotations().put(internal::annotation::JMS_DESTINATION_TYPE, static_cast<int8_t>(internal::DestinationConverter::jmsDestinationType(destination)));

}

long long stonex::amqp::Message::getCMSExpiration() const
{
	return mMessage.expiry_time().milliseconds();
}

void stonex::amqp::Message::setCMSExpiration(long long expireTime)
{
	mMessage.expiry_time(proton::timestamp(expireTime));
}

std::string stonex::amqp::Message::getCMSMessageID() const
{
	return proton::get<std::string>(mMessage.id());
}


void stonex::amqp::Message::setCMSMessageID(const std::string& id)
{
	mMessage.id(id);
	mMessage.properties().put("message-id-string", "ID:"+id);
	mMessage.properties().put("JMSMessageID", "ID:AMQP_STRING:" + id);
}

int stonex::amqp::Message::getCMSPriority() const
{
	return static_cast<int>(mMessage.priority());
}

void stonex::amqp::Message::setCMSPriority(int priority)
{
	if (priority > std::numeric_limits<uint8_t>::max())
		throw cms::CMSException("Priority value cannot be greater than 255");

	mMessage.priority(static_cast<uint8_t>(priority));
}

bool stonex::amqp::Message::getCMSRedelivered() const
{
	//should use delivery annotations?
	return mMessage.delivery_count() > 0;
}

void stonex::amqp::Message::setCMSRedelivered(bool redelivered)
{
	if (redelivered)
		mMessage.delivery_count(mMessage.delivery_count() + 1);
	else
		mMessage.delivery_count(0);
}

const cms::Destination* stonex::amqp::Message::getCMSReplyTo() const
{
	return internal::DestinationConverter::createCMSReplyTo(mMessage);
}

void stonex::amqp::Message::setCMSReplyTo(const cms::Destination* destination)
{
	mMessage.reply_to(internal::DestinationConverter::address(destination));
	mMessage.message_annotations().put(internal::annotation::JMS_REPLY_TO_TYPE, static_cast<int8_t>(internal::DestinationConverter::jmsDestinationType(destination)));
}

long long stonex::amqp::Message::getCMSTimestamp() const
{
	return mMessage.creation_time().milliseconds();
}

void stonex::amqp::Message::setCMSTimestamp(long long timeStamp)
{
	mMessage.creation_time(proton::timestamp(timeStamp));
}

std::string stonex::amqp::Message::getCMSType() const
{
	return mMessage.subject();
}

void stonex::amqp::Message::setCMSType(const std::string& type)
{
	mMessage.subject(type);
}
