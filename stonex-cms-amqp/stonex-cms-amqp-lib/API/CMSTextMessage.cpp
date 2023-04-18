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

#include "CMSTextMessage.h"

#include <AMQPCMSMessageConverter.h>

#include <regex>
#include <string_view>

#include <proton/delivery.hpp>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/annotation_key.hpp>

#include <cms/InvalidDestinationException.h>

cms::amqp::CMSTextMessage::CMSTextMessage(const CMSTextMessage& other)
	:mMessage{ std::make_shared<proton::message>(*other.mMessage) },
	mDestination{ other.mDestination },
	mReplyTo{ other.mReplyTo }
{
}

cms::amqp::CMSTextMessage::CMSTextMessage(const std::string& text)
{
	mMessage = std::make_shared<proton::message>();
	mMessage->durable(cms::DeliveryMode::PERSISTENT == 0); //default delivery mode PERISTENT -> durable(true)
	mMessage->priority(::cms::Message::DEFAULT_MSG_PRIORITY);
	mMessage->body(text);
	mMessage->message_annotations().put(X_OPT_JMS_MESSAGE_TYPE.data(), static_cast<int8_t>(MESSAGE_TYPE::TEXT_MESSAGE));
}

cms::amqp::CMSTextMessage::CMSTextMessage(proton::message* message, const proton::delivery* delivery, const proton::receiver* receiver)
:mMessage(std::make_shared<proton::message>(*message)),
mMessageDelivery{std::make_shared<proton::delivery>(*delivery)}
{
	try
	{
		mDestination.reset(AMQPCMSMessageConverter::createCMSDestination(mMessage));
	}
	catch (const ::cms::InvalidDestinationException &ex)
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

cms::amqp::CMSTextMessage::~CMSTextMessage()

{
//	if (mMessageDelivery)
//		delete mMessageDelivery;
}


std::string cms::amqp::CMSTextMessage::getText() const
{
	if (!mMessage->body().empty())
		return proton::to_string(mMessage->body());
	else
		return {};
}


::cms::Message* cms::amqp::CMSTextMessage::clone() const
{
	return new CMSTextMessage(*this);
}

void cms::amqp::CMSTextMessage::acknowledge() const
{ 
	//mMessageDelivery.accept();
	mMessageDelivery->accept();
}

void cms::amqp::CMSTextMessage::clearBody()
{
	mMessage->clear();
	//throw ::cms::CMSException("illegal use - not implemented");
}


void cms::amqp::CMSTextMessage::clearProperties()
{
	mMessage->properties().clear();
}

std::vector<std::string> cms::amqp::CMSTextMessage::getPropertyNames() const
{
	return AMQPCMSMessageConverter::getPropertyNames(mMessage);
}

bool cms::amqp::CMSTextMessage::propertyExists(const std::string& name) const
{
	return AMQPCMSMessageConverter::propertyExists(name, mMessage);
}

::cms::Message::ValueType cms::amqp::CMSTextMessage::getPropertyValueType(const std::string& name) const
{
	return AMQPCMSMessageConverter::type_id_to_ValueType(mMessage->properties().get(name).type());
}


//getters
bool cms::amqp::CMSTextMessage::getBooleanProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSBooleanProperty(name, mMessage);
}

unsigned char cms::amqp::CMSTextMessage::getByteProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSByteProperty(name, mMessage);
}

double cms::amqp::CMSTextMessage::getDoubleProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSDoubleProperty(name, mMessage);
}

float cms::amqp::CMSTextMessage::getFloatProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSFloatProperty(name, mMessage);
}

int cms::amqp::CMSTextMessage::getIntProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSIntProperty(name, mMessage);
}
long long cms::amqp::CMSTextMessage::getLongProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSLongProperty(name, mMessage);
}

short cms::amqp::CMSTextMessage::getShortProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSShortProperty(name, mMessage);
}

std::string cms::amqp::CMSTextMessage::getStringProperty(const std::string& name) const
{
	return AMQPCMSMessageConverter::getCMSStringProperty(name, mMessage);
}




//setters
void cms::amqp::CMSTextMessage::setBooleanProperty(const std::string& name, bool value)
{
	AMQPCMSMessageConverter::setAMQPBooleanProperty(name, value, mMessage);
}

void cms::amqp::CMSTextMessage::setByteProperty(const std::string& name, unsigned char value)
{
	AMQPCMSMessageConverter::setAMQPByteProperty(name, value, mMessage);
}

void cms::amqp::CMSTextMessage::setDoubleProperty(const std::string& name, double value)
{
	AMQPCMSMessageConverter::setAMQPDoubleProperty(name, value, mMessage);
}

void cms::amqp::CMSTextMessage::setFloatProperty(const std::string& name, float value)
{
	AMQPCMSMessageConverter::setAMQPFloatProperty(name, value, mMessage);
}

void cms::amqp::CMSTextMessage::setIntProperty(const std::string& name, int value)
{
	AMQPCMSMessageConverter::setAMQPIntProperty(name, value, mMessage);
}

void cms::amqp::CMSTextMessage::setLongProperty(const std::string& name, long long value)
{
	AMQPCMSMessageConverter::setAMQPLongProperty(name, value, mMessage);
}

void cms::amqp::CMSTextMessage::setShortProperty(const std::string& name, short value)
{
	AMQPCMSMessageConverter::setAMQPShortProperty(name, value, mMessage);
}

void cms::amqp::CMSTextMessage::setStringProperty(const std::string& name, const std::string& value)
{
	AMQPCMSMessageConverter::setAMQPStringProperty(name, value, mMessage);
}
//////
std::string cms::amqp::CMSTextMessage::getCMSCorrelationID() const
{
	return AMQPCMSMessageConverter::getCMSCorrelationID(mMessage);
}

void cms::amqp::CMSTextMessage::setCMSCorrelationID(const std::string& correlationId)
{
	return AMQPCMSMessageConverter::setAMQPCorrelationID(correlationId, mMessage);
}

int cms::amqp::CMSTextMessage::getCMSDeliveryMode() const
{
	return AMQPCMSMessageConverter::getCMSDeliveryMode(mMessage);
}

void cms::amqp::CMSTextMessage::setCMSDeliveryMode(int mode)
{
	return AMQPCMSMessageConverter::setAMQPDeliveryMode(mode, mMessage);
}

const::cms::Destination* cms::amqp::CMSTextMessage::getCMSDestination() const
{
	return mDestination.get();
}

void cms::amqp::CMSTextMessage::setCMSDestination(const::cms::Destination* destination)
{
	AMQPCMSMessageConverter::setAMQPDestination(destination, mMessage);
	if (destination)
		mDestination.reset(destination->clone());
	else
		mDestination.reset();
}

long long cms::amqp::CMSTextMessage::getCMSExpiration() const
{
	return AMQPCMSMessageConverter::getCMSExpiration(mMessage);
}

void cms::amqp::CMSTextMessage::setCMSExpiration(long long expireTime)
{
	return AMQPCMSMessageConverter::setAMQPExpiration(expireTime, mMessage);
}

std::string cms::amqp::CMSTextMessage::getCMSMessageID() const
{
	return AMQPCMSMessageConverter::getCMSMessageID(mMessage);
}


void cms::amqp::CMSTextMessage::setCMSMessageID(const std::string& id)
{
	return AMQPCMSMessageConverter::setAMQPMessageID(id, mMessage);
}

int cms::amqp::CMSTextMessage::getCMSPriority() const
{
	return AMQPCMSMessageConverter::getCMSPriority(mMessage);
}

void cms::amqp::CMSTextMessage::setCMSPriority(int priority)
{
	return AMQPCMSMessageConverter::setAMQPPriority(priority, mMessage);
}

bool cms::amqp::CMSTextMessage::getCMSRedelivered() const
{
	return AMQPCMSMessageConverter::getCMSRedelivered(mMessage);
}

void cms::amqp::CMSTextMessage::setCMSRedelivered(bool redelivered)
{
	return AMQPCMSMessageConverter::setAMQPRedelivered(redelivered, mMessage);
}

const::cms::Destination* cms::amqp::CMSTextMessage::getCMSReplyTo() const
{
	return mReplyTo.get();
}

void cms::amqp::CMSTextMessage::setCMSReplyTo(const::cms::Destination* destination)
{
	AMQPCMSMessageConverter::setAMQPReplyTo(destination, mMessage);
	if (destination)
		mReplyTo.reset(destination->clone());
	else
		mReplyTo.reset();
}

long long cms::amqp::CMSTextMessage::getCMSTimestamp() const
{
	return AMQPCMSMessageConverter::getCMSTimestamp(mMessage);
}

void cms::amqp::CMSTextMessage::setCMSTimestamp(long long timeStamp)
{
	return AMQPCMSMessageConverter::setAMQPTimestamp(timeStamp, mMessage);
}

std::string cms::amqp::CMSTextMessage::getCMSType() const
{
	return AMQPCMSMessageConverter::getCMSType(mMessage);
}


void cms::amqp::CMSTextMessage::setCMSType(const std::string& type)
{
	return AMQPCMSMessageConverter::setAMQPType(type, mMessage);
}

void cms::amqp::CMSTextMessage::body(const std::string& message)
{
	mMessage->body(message); 
}