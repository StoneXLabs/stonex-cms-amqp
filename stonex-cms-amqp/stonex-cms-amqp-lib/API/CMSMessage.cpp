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

#include "CMSMessage.h"

#include <proton/message.hpp>
#include <proton/sender.hpp>
#include <proton/receiver.hpp>
#include <proton/value.hpp>

#include <cms/CMSException.h>

#include <AMQPCMSMessageConverter.h>

#include "MessageProducerImpl.h"

cms::amqp::CMSMessage::CMSMessage()
{
	// set default durable to true that maps to  cms::DeliveryMode::PERSISTENT
	mMessage = std::make_shared<proton::message>();
	mMessage->durable(true);
	mMessage->priority(cms::Message::DEFAULT_MSG_PRIORITY);
}


cms::amqp::CMSMessage::CMSMessage(const CMSMessage& other)
	:mMessage{other.mMessage},
	mDestination{other.mDestination},
	mReplyTo{other.mReplyTo}
{	
}

cms::amqp::CMSMessage::CMSMessage(proton::message* mes)
	: mMessage(std::make_shared<proton::message>(*mes))
{

	try
	{
		if (!mMessage->address().empty())
			setCMSDestination(AMQPCMSMessageConverter::createCMSDestination(mMessage));
	}
	catch (const cms::InvalidDestinationException& ex)
	{

	}

	try
	{
		if (!mMessage->reply_to().empty())
		{
			setCMSReplyTo(AMQPCMSMessageConverter::createCMSReplyToUsingMessageDestination(mMessage));
		}
	}
	catch (const cms::InvalidDestinationException& ex)
	{

	}
}

cms::amqp::CMSMessage::CMSMessage(proton::message* mes, const proton::sender* sender)
	: CMSMessage(mes)
{
	try
	{
		if (mMessage->address().empty())
			setCMSDestination(AMQPCMSMessageConverter::createCMSDestination(sender));

	}
	catch (const cms::InvalidDestinationException& ex)
	{

	}


}

cms::amqp::CMSMessage::CMSMessage(proton::message* mes, const proton::receiver* receiver)
	: CMSMessage(mes)
{
	try
	{
		if (mMessage->address().empty())
			setCMSDestination(AMQPCMSMessageConverter::createCMSDestination(receiver));
	}
	catch (const cms::InvalidDestinationException& ex)
	{

	}

}

cms::Message* cms::amqp::CMSMessage::clone() const
{
	return new CMSMessage(*this);
}

void cms::amqp::CMSMessage::clearBody()
{
	mMessage->clear();
}

void cms::amqp::CMSMessage::clearProperties()
{
	mMessage->properties().clear();
}

std::vector<std::string> cms::amqp::CMSMessage::getPropertyNames() const
{
	return AMQPCMSMessageConverter::getPropertyNames(mMessage);
}

bool cms::amqp::CMSMessage::propertyExists(const std::string& name) const
{
	return AMQPCMSMessageConverter::propertyExists(name, mMessage);
}

cms::Message::ValueType cms::amqp::CMSMessage::getPropertyValueType(const std::string& name) const
{
	return AMQPCMSMessageConverter::type_id_to_ValueType(mMessage->properties().get(name).type());
}

//getters
bool cms::amqp::CMSMessage::getBooleanProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSBooleanProperty(name, mMessage);
}

unsigned char cms::amqp::CMSMessage::getByteProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSByteProperty(name, mMessage);
}

double cms::amqp::CMSMessage::getDoubleProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSDoubleProperty(name, mMessage);
}

float cms::amqp::CMSMessage::getFloatProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSFloatProperty(name, mMessage);
}

int cms::amqp::CMSMessage::getIntProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSIntProperty(name, mMessage);
}
long long cms::amqp::CMSMessage::getLongProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSLongProperty(name, mMessage);
}

short cms::amqp::CMSMessage::getShortProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSShortProperty(name, mMessage);
}

std::string cms::amqp::CMSMessage::getStringProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSStringProperty(name, mMessage);
}




//setters
void cms::amqp::CMSMessage::setBooleanProperty(const std::string& name, bool value)
{
	AMQPCMSMessageConverter::setAMQPBooleanProperty(name, value, mMessage);
}

void cms::amqp::CMSMessage::setByteProperty(const std::string& name, unsigned char value)
{
	AMQPCMSMessageConverter::setAMQPByteProperty(name, value, mMessage);
}

void cms::amqp::CMSMessage::setDoubleProperty(const std::string& name, double value)
{
	AMQPCMSMessageConverter::setAMQPDoubleProperty(name, value, mMessage);
}

void cms::amqp::CMSMessage::setFloatProperty(const std::string& name, float value)
{
	AMQPCMSMessageConverter::setAMQPFloatProperty(name, value, mMessage);
}

void cms::amqp::CMSMessage::setIntProperty(const std::string& name, int value)
{
	AMQPCMSMessageConverter::setAMQPIntProperty(name, value, mMessage);
}

void cms::amqp::CMSMessage::setLongProperty(const std::string& name, long long value)
{
	AMQPCMSMessageConverter::setAMQPLongProperty(name, value, mMessage);
}

void cms::amqp::CMSMessage::setShortProperty(const std::string& name, short value)
{
	AMQPCMSMessageConverter::setAMQPShortProperty(name, value, mMessage);
}

void cms::amqp::CMSMessage::setStringProperty(const std::string& name, const std::string& value)
{
	AMQPCMSMessageConverter::setAMQPStringProperty(name, value, mMessage);
}

std::string cms::amqp::CMSMessage::getCMSCorrelationID() const
{
	return AMQPCMSMessageConverter::getCMSCorrelationID(mMessage);
}

void cms::amqp::CMSMessage::setCMSCorrelationID(const std::string& correlationId)
{
	return AMQPCMSMessageConverter::setAMQPCorrelationID(correlationId, mMessage);
}

int cms::amqp::CMSMessage::getCMSDeliveryMode() const
{
	return AMQPCMSMessageConverter::getCMSDeliveryMode(mMessage);
}

void cms::amqp::CMSMessage::setCMSDeliveryMode(int mode)
{
	return AMQPCMSMessageConverter::setAMQPDeliveryMode(mode, mMessage);
}

const cms::Destination* cms::amqp::CMSMessage::getCMSDestination() const
{
	return mDestination.get();
}

void cms::amqp::CMSMessage::setCMSDestination(const cms::Destination* destination)
{
	AMQPCMSMessageConverter::setAMQPDestination(destination, mMessage);
	if (destination)
		mDestination.reset(destination->clone());
	else
		mDestination.reset();
}

long long cms::amqp::CMSMessage::getCMSExpiration() const
{
	return AMQPCMSMessageConverter::getCMSExpiration(mMessage);
}

void cms::amqp::CMSMessage::setCMSExpiration(long long expireTime)
{
	return AMQPCMSMessageConverter::setAMQPExpiration(expireTime, mMessage);
}

std::string cms::amqp::CMSMessage::getCMSMessageID() const
{
	return AMQPCMSMessageConverter::getCMSMessageID(mMessage);
}


void cms::amqp::CMSMessage::setCMSMessageID(const std::string& id)
{
	return AMQPCMSMessageConverter::setAMQPMessageID(id, mMessage);
}

int cms::amqp::CMSMessage::getCMSPriority() const
{
	return AMQPCMSMessageConverter::getCMSPriority(mMessage);
}

void cms::amqp::CMSMessage::setCMSPriority(int priority)
{
	return AMQPCMSMessageConverter::setAMQPPriority(priority, mMessage);
}

bool cms::amqp::CMSMessage::getCMSRedelivered() const
{
	return AMQPCMSMessageConverter::getCMSRedelivered(mMessage);
}

void cms::amqp::CMSMessage::setCMSRedelivered(bool redelivered)
{
	return AMQPCMSMessageConverter::setAMQPRedelivered(redelivered, mMessage);
}

const cms::Destination* cms::amqp::CMSMessage::getCMSReplyTo() const
{
	return mReplyTo.get();
}

void cms::amqp::CMSMessage::setCMSReplyTo(const cms::Destination* destination)
{
	AMQPCMSMessageConverter::setAMQPReplyTo(destination, mMessage);
	if (destination)
		mReplyTo.reset(destination->clone());
	else
		mReplyTo.reset();
}

long long cms::amqp::CMSMessage::getCMSTimestamp() const
{
	return AMQPCMSMessageConverter::getCMSTimestamp(mMessage);
}

void cms::amqp::CMSMessage::setCMSTimestamp(long long timeStamp)
{
	return AMQPCMSMessageConverter::setAMQPTimestamp(timeStamp, mMessage);
}

std::string cms::amqp::CMSMessage::getCMSType() const
{
	return AMQPCMSMessageConverter::getCMSType(mMessage);
}


void cms::amqp::CMSMessage::setCMSType(const std::string& type)
{
	return AMQPCMSMessageConverter::setAMQPType(type, mMessage);
}

void cms::amqp::CMSMessage::body(const std::string& message)
{
	mMessage->body(message);
}
