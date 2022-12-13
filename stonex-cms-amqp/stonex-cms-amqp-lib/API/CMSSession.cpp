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

#include "CMSSession.h"

#include "ConnectionContext.h"

#include "SessionImpl.h"

#include "CMSQueue.h"
#include "CMSTopic.h"
#include "CMSTemporaryQueue.h"
#include "CMSTemporaryTopic.h"

#include "CMSMessageProducer.h"
#include "CMSMessageConsumer.h"

#include "CMSTextMessage.h"
#include "CMSBytesMessage.h"

cms::amqp::CMSSession::CMSSession(const ConnectionContext& cntx)
	:mPimpl{ std::make_shared<SessionImpl>(cntx.connection()) }
{
}

cms::amqp::CMSSession::CMSSession(const ConnectionContext& cntx, const ::cms::Session::AcknowledgeMode ackMode)
	: mPimpl{ std::make_shared<SessionImpl>(cntx.connection(),ackMode) }
{
}

void cms::amqp::CMSSession::close()
{
	mPimpl->close();
}

void cms::amqp::CMSSession::commit()
{
	mPimpl->commit();
}

void cms::amqp::CMSSession::rollback()
{
	mPimpl->rollback();
}

void cms::amqp::CMSSession::recover()
{
	mPimpl->recover();
}

void cms::amqp::CMSSession::start()
{
	mPimpl->start();
}

void cms::amqp::CMSSession::stop()
{
	throw ::cms::CMSException("illegal use - not implemented");
}

cms::MessageConsumer* cms::amqp::CMSSession::createConsumer(const ::cms::Destination* destination)
{
	return new CMSMessageConsumer(destination, std::make_shared<SessionContext>(mPimpl->session(), false, false, true));
}

cms::MessageConsumer* cms::amqp::CMSSession::createConsumer(const ::cms::Destination* destination, const std::string& selector)
{
	return new CMSMessageConsumer(destination, selector, std::make_shared<SessionContext>(mPimpl->session(), false, false, mPimpl->ackMode() == ::cms::Session::AUTO_ACKNOWLEDGE));
}

cms::MessageConsumer* cms::amqp::CMSSession::createConsumer(const ::cms::Destination* destination, const std::string& selector, bool noLocal)
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::MessageConsumer* cms::amqp::CMSSession::createDurableConsumer(const ::cms::Topic* destination, const std::string& name, const std::string& selector, bool noLocal)
{
	return new CMSMessageConsumer(destination, name, selector, std::make_shared<SessionContext>(mPimpl->session(), true, false, mPimpl->ackMode() == ::cms::Session::AUTO_ACKNOWLEDGE));
}

cms::MessageProducer* cms::amqp::CMSSession::createProducer(const ::cms::Destination* destination)
{
	return new CMSMessageProducer(destination, std::make_shared<SessionContext>(mPimpl->session(), false, false, true));
}

cms::QueueBrowser* cms::amqp::CMSSession::createBrowser(const ::cms::Queue* queue)
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::QueueBrowser* cms::amqp::CMSSession::createBrowser(const ::cms::Queue* queue, const std::string& selector)
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::Queue* cms::amqp::CMSSession::createQueue(const std::string& queueName)
{
	return new CMSQueue(queueName);
}

cms::Topic* cms::amqp::CMSSession::createTopic(const std::string& topicName)
{
	return new CMSTopic(topicName);
}

cms::TemporaryQueue* cms::amqp::CMSSession::createTemporaryQueue()
{
	return new CMSTemporaryQueue();
}

cms::TemporaryTopic* cms::amqp::CMSSession::createTemporaryTopic()
{
	
	return new CMSTemporaryTopic();
}

cms::Message* cms::amqp::CMSSession::createMessage()
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::BytesMessage* cms::amqp::CMSSession::createBytesMessage()
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::BytesMessage* cms::amqp::CMSSession::createBytesMessage(const unsigned char* bytes, int bytesSize)
{
	return new CMSBytesMessage(bytes, bytesSize);
}

cms::StreamMessage* cms::amqp::CMSSession::createStreamMessage()
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::TextMessage* cms::amqp::CMSSession::createTextMessage()
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::TextMessage* cms::amqp::CMSSession::createTextMessage(const std::string& text)
{
	return new cms::amqp::CMSTextMessage(text);
}

cms::MapMessage* cms::amqp::CMSSession::createMapMessage()
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

::cms::Session::AcknowledgeMode cms::amqp::CMSSession::getAcknowledgeMode() const
{
	return mPimpl->ackMode();
}

//AcknowledgeMode cms::amqp::CMSSession::getAcknowledgeMode() const
//{
//	return AcknowledgeMode();
//}

bool cms::amqp::CMSSession::isTransacted() const
{
	throw ::cms::CMSException("illegal use - not implemented");
	return false;
}

void cms::amqp::CMSSession::unsubscribe(const std::string& name)
{
	throw ::cms::CMSException("illegal use - not implemented");
}

void cms::amqp::CMSSession::setMessageTransformer(::cms::MessageTransformer* transformer)
{
	throw ::cms::CMSException("illegal use - not implemented");
}

cms::MessageTransformer* cms::amqp::CMSSession::getMessageTransformer() const
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

std::shared_ptr<cms::amqp::SessionContext> cms::amqp::CMSSession::createSessionContext(bool durable, bool shared, bool auto_ack) const
{
	return std::make_shared<cms::amqp::SessionContext>(mPimpl->session(), durable, shared, auto_ack);
}

