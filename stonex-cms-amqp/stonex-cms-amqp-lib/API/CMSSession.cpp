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

#include <fmt/format.h>

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

cms::amqp::CMSSession::CMSSession(const ConnectionContext& cntx, std::shared_ptr<StonexLogger> logger)
	:mPimpl{ std::make_shared<SessionImpl>(cntx.connection(), cms::Session::AcknowledgeMode::AUTO_ACKNOWLEDGE,logger) }
{
}

cms::amqp::CMSSession::CMSSession(const ConnectionContext& cntx, const ::cms::Session::AcknowledgeMode ackMode, std::shared_ptr<StonexLogger> logger)
	: mPimpl{ std::make_shared<SessionImpl>(cntx.connection(),ackMode, logger) }
{
}

void cms::amqp::CMSSession::close()
{
	info("com.stonex.cms.CMSSession", "closing session");
	mPimpl->close();
}

void cms::amqp::CMSSession::commit()
{
	info("com.stonex.cms.CMSSession", "commit");
	mPimpl->commit();
}

void cms::amqp::CMSSession::rollback()
{
	info("com.stonex.cms.CMSSession", "rollback");
	mPimpl->rollback();
}

void cms::amqp::CMSSession::recover()
{
	info("com.stonex.cms.CMSSession", "recover");
	mPimpl->recover();
}

void cms::amqp::CMSSession::start()
{
	info("com.stonex.cms.CMSSession", "starting session");
	mPimpl->start();
}

void cms::amqp::CMSSession::stop()
{
	info("com.stonex.cms.CMSSession", "stopping session");
	throw ::cms::CMSException("illegal use - not implemented");
}

cms::MessageConsumer* cms::amqp::CMSSession::createConsumer(const ::cms::Destination* destination)
{
	info("com.stonex.cms.CMSSession", fmt::format("creating consumer. destination: {}",destination->getDestinationType()));
	return new CMSMessageConsumer(destination, std::make_shared<SessionContext>(mPimpl->session(), false, false, true), mLogSink);
}

cms::MessageConsumer* cms::amqp::CMSSession::createConsumer(const ::cms::Destination* destination, const std::string& selector)
{
	info("com.stonex.cms.CMSSession", fmt::format("creating consumer. destination: {} selector: {}", destination->getDestinationType(), selector));
	return new CMSMessageConsumer(destination, selector, std::make_shared<SessionContext>(mPimpl->session(), false, false, mPimpl->ackMode() == ::cms::Session::AUTO_ACKNOWLEDGE), mLogSink);
}

cms::MessageConsumer* cms::amqp::CMSSession::createConsumer(const ::cms::Destination* destination, const std::string& selector, bool noLocal)
{
	info("com.stonex.cms.CMSSession", fmt::format("creating consumer. destination: {} selector: {} noLocal: {}", destination->getDestinationType(), selector, noLocal));
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::MessageConsumer* cms::amqp::CMSSession::createDurableConsumer(const ::cms::Topic* destination, const std::string& name, const std::string& selector, bool noLocal)
{
	info("com.stonex.cms.CMSSession", fmt::format("creating durable consumer. destination: {} selector: {} noLocal: {}", destination->getDestinationType(), selector, noLocal));
	return new CMSMessageConsumer(destination, name, selector, std::make_shared<SessionContext>(mPimpl->session(), true, false, mPimpl->ackMode() == ::cms::Session::AUTO_ACKNOWLEDGE), mLogSink);
}

cms::MessageProducer* cms::amqp::CMSSession::createProducer(const ::cms::Destination* destination)
{
	info("com.stonex.cms.CMSSession", fmt::format("creating producer. destination: {}", destination->getDestinationType()));
	return new CMSMessageProducer(destination, std::make_shared<SessionContext>(mPimpl->session(), false, false, true), mLogSink);
}

cms::QueueBrowser* cms::amqp::CMSSession::createBrowser(const ::cms::Queue* queue)
{
	info("com.stonex.cms.CMSSession", fmt::format("creating browser. queue: {}", queue->getQueueName()));
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::QueueBrowser* cms::amqp::CMSSession::createBrowser(const ::cms::Queue* queue, const std::string& selector)
{
	info("com.stonex.cms.CMSSession", fmt::format("creating browser. queue: {} selector: {}", queue->getQueueName(), selector));
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::Queue* cms::amqp::CMSSession::createQueue(const std::string& queueName)
{
	info("com.stonex.cms.CMSSession", fmt::format("creating queue {}", queueName));
	return new CMSQueue(queueName);
}

cms::Topic* cms::amqp::CMSSession::createTopic(const std::string& topicName)
{
	info("com.stonex.cms.CMSSession", fmt::format("creating topic {}", topicName));
	return new CMSTopic(topicName);
}

cms::TemporaryQueue* cms::amqp::CMSSession::createTemporaryQueue()
{
	info("com.stonex.cms.CMSSession", "creating temporary queue");
	return new CMSTemporaryQueue();
}

cms::TemporaryTopic* cms::amqp::CMSSession::createTemporaryTopic()
{
	info("com.stonex.cms.CMSSession", "creating temporary topic");
	return new CMSTemporaryTopic();
}

cms::Message* cms::amqp::CMSSession::createMessage()
{
#if _DEBUG
	debug("com.stonex.cms.CMSSession", "creating cms message");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::BytesMessage* cms::amqp::CMSSession::createBytesMessage()
{
#if _DEBUG
	debug("com.stonex.cms.CMSSession", "creating bytes message");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::BytesMessage* cms::amqp::CMSSession::createBytesMessage(const unsigned char* bytes, int bytesSize)
{
#if _DEBUG
	debug("com.stonex.cms.CMSSession", fmt::format("creating bytes message. byte size {}",bytesSize));
#endif
	return new CMSBytesMessage(bytes, bytesSize);
}

cms::StreamMessage* cms::amqp::CMSSession::createStreamMessage()
{
#if _DEBUG
	debug("com.stonex.cms.CMSSession", "creating stream message");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::TextMessage* cms::amqp::CMSSession::createTextMessage()
{
#if _DEBUG
	debug("com.stonex.cms.CMSSession", "creating text message");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::TextMessage* cms::amqp::CMSSession::createTextMessage(const std::string& text)
{
#if _DEBUG
	debug("com.stonex.cms.CMSSession", "creating text message");
#endif
	return new cms::amqp::CMSTextMessage(text);
}

cms::MapMessage* cms::amqp::CMSSession::createMapMessage()
{
#if _DEBUG
	debug("com.stonex.cms.CMSSession", "creating map message");
#endif
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
	info("com.stonex.cms.CMSSession", fmt::format("unsubscribe {}", name));
	throw ::cms::CMSException("illegal use - not implemented");
}

void cms::amqp::CMSSession::setMessageTransformer(::cms::MessageTransformer* transformer)
{
	info("com.stonex.cms.CMSSession", fmt::format("set message transformer: {}", (void*)transformer));
	throw ::cms::CMSException("illegal use - not implemented");
}

cms::MessageTransformer* cms::amqp::CMSSession::getMessageTransformer() const
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}


std::shared_ptr<cms::amqp::SessionContext> cms::amqp::CMSSession::createSessionContext(bool durable, bool shared, bool auto_ack) const
{
	debug("com.stonex.cms.CMSSession", fmt::format("create session context: durable: {} shared: {} auto_ack: {}", durable, shared, auto_ack));
	return std::make_shared<cms::amqp::SessionContext>(mPimpl->session(), durable, shared, auto_ack);
}

void cms::amqp::CMSSession::setLogger(std::shared_ptr<StonexLogger> sink)
{
	StonexLogSource::setLogger(sink);
	mPimpl->setLogger(sink);
};