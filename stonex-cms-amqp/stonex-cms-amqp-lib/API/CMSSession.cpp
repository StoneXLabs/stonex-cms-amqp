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
#include <API/CMSConnection.h>
#include "SessionImpl.h"
#include "MessageConsumerImpl.h"
#include "MessageProducerImpl.h"

#include "CMSQueue.h"
#include "CMSTopic.h"
#include "CMSTemporaryQueue.h"
#include "CMSTemporaryTopic.h"

#include "CMSMessageProducer.h"
#include "CMSMessageConsumer.h"

#include "CMSTextMessage.h"
#include "CMSBytesMessage.h"

//cms::amqp::CMSSession::CMSSession(cms::amqp::CMSConnection& parent, const ConnectionContext& cntx)
//	:CMSSession(parent, cntx, cms::Session::AcknowledgeMode::AUTO_ACKNOWLEDGE)
//{
//	if (mParent->getState() == ClientState::STOPPED)
//	{
//		stop();
//	}
//}

cms::amqp::CMSSession::CMSSession(std::shared_ptr<SessionImpl> impl)
	: mPimpl(impl),
	mLogger(LoggerFactory::getInstance().create("com.stonex.cms.CMSSession"))
{

}

cms::amqp::CMSSession::~CMSSession()
{
	
}

void cms::amqp::CMSSession::close()
{
	mLogger->log(SEVERITY::LOG_INFO, "closing session");
	mPimpl->close();
}

void cms::amqp::CMSSession::commit()
{
	mLogger->log(SEVERITY::LOG_INFO, "commit");
	mPimpl->commit();
}

void cms::amqp::CMSSession::rollback()
{
	mLogger->log(SEVERITY::LOG_INFO, "rollback");
	mPimpl->rollback();
}

void cms::amqp::CMSSession::recover()
{
	mLogger->log(SEVERITY::LOG_INFO, "recover");
	mPimpl->recover();
}

void cms::amqp::CMSSession::start()
{
	mLogger->log(SEVERITY::LOG_INFO, "starting session");
	//for (auto consumer : mConsumers)
	//{
	//	consumer->start();
	//}
	//setState(ClientState::STARTED);

	//for (auto producer : mProducers)
	//{
	//	producer->start();
	//}
}

void cms::amqp::CMSSession::stop()
{
	mLogger->log(SEVERITY::LOG_INFO, "stopping session");
//	for (auto consumer : mConsumers)
//	{
//		consumer->stop();
//	}
//	setState(ClientState::STOPPED);
}

cms::MessageConsumer* cms::amqp::CMSSession::createConsumer(const ::cms::Destination* destination)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("creating consumer. destination: {}",destination->getDestinationType()));
	config::ConsumerContext context(mPimpl->mContext,false, false, destination);
	std::shared_ptr<MessageConsumerImpl> consumer = std::make_shared<MessageConsumerImpl>(context);
	mConsumers.emplace_back(consumer);
	return new CMSMessageConsumer(consumer);

}

cms::MessageConsumer* cms::amqp::CMSSession::createConsumer(const ::cms::Destination* destination, const std::string& selector)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("creating consumer. destination: {} selector: {}", destination->getDestinationType(), selector));
	config::ConsumerContext context(mPimpl->mContext, false, false, destination);
	std::shared_ptr<MessageConsumerImpl> consumer = std::make_shared<MessageConsumerImpl>(context);
	mConsumers.emplace_back(consumer);
	return new CMSMessageConsumer(consumer);
//	return mConsumers.emplace_back(new CMSMessageConsumer(*this, destination, selector, std::make_shared<SessionContext>(mPimpl->session(), false, false, mPimpl->ackMode() == ::cms::Session::AUTO_ACKNOWLEDGE)));
}

cms::MessageConsumer* cms::amqp::CMSSession::createConsumer(const ::cms::Destination* destination, const std::string& selector, bool noLocal)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("creating consumer. destination: {} selector: {} noLocal: {}", destination->getDestinationType(), selector, noLocal));
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::MessageConsumer* cms::amqp::CMSSession::createDurableConsumer(const ::cms::Topic* destination, const std::string& name, const std::string& selector, bool noLocal)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("creating durable consumer. destination: {} selector: {} noLocal: {}", destination->getDestinationType(), selector, noLocal));

	config::ConsumerContext context(mPimpl->mContext, false, false, destination);
	std::shared_ptr<MessageConsumerImpl> consumer = std::make_shared<MessageConsumerImpl>(context);
	mConsumers.emplace_back(consumer);
	return new CMSMessageConsumer(consumer);
//	return mConsumers.emplace_back(new CMSMessageConsumer(*this, destination, name, selector, std::make_shared<SessionContext>(mPimpl->session(), true, false, mPimpl->ackMode() == ::cms::Session::AUTO_ACKNOWLEDGE)));
}

cms::MessageProducer* cms::amqp::CMSSession::createProducer(const ::cms::Destination* destination)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("creating producer. destination: {}", destination->getDestinationType()));

	config::ProducerContext context(mPimpl->mContext, destination);
	std::shared_ptr<MessageProducerImpl> producer = std::make_shared<MessageProducerImpl>(context);
	mProducers.emplace_back(producer);
	return new CMSMessageProducer(producer);
//	return new CMSMessageProducer(destination, std::make_shared<SessionContext>(mPimpl->session(), false, false, true));
}

cms::QueueBrowser* cms::amqp::CMSSession::createBrowser(const ::cms::Queue* queue)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("creating browser. queue: {}", queue->getQueueName()));
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::QueueBrowser* cms::amqp::CMSSession::createBrowser(const ::cms::Queue* queue, const std::string& selector)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("creating browser. queue: {} selector: {}", queue->getQueueName(), selector));
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::Queue* cms::amqp::CMSSession::createQueue(const std::string& queueName)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("creating queue {}", queueName));
	return new CMSQueue(queueName);
}

cms::Topic* cms::amqp::CMSSession::createTopic(const std::string& topicName)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("creating topic {}", topicName));
	return new CMSTopic(topicName);
}

cms::TemporaryQueue* cms::amqp::CMSSession::createTemporaryQueue()
{
	mLogger->log(SEVERITY::LOG_INFO, "creating temporary queue");
	return new CMSTemporaryQueue();
}

cms::TemporaryTopic* cms::amqp::CMSSession::createTemporaryTopic()
{
	mLogger->log(SEVERITY::LOG_INFO, "creating temporary topic");
	return new CMSTemporaryTopic();
}

cms::Message* cms::amqp::CMSSession::createMessage()
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, "creating cms message");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::BytesMessage* cms::amqp::CMSSession::createBytesMessage()
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, "creating bytes message");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::BytesMessage* cms::amqp::CMSSession::createBytesMessage(const unsigned char* bytes, int bytesSize)
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, fmt::format("creating bytes message. byte size {}",bytesSize));
#endif
	return new CMSBytesMessage(bytes, bytesSize);
}

cms::StreamMessage* cms::amqp::CMSSession::createStreamMessage()
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, "creating stream message");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::TextMessage* cms::amqp::CMSSession::createTextMessage()
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, "creating text message");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

cms::TextMessage* cms::amqp::CMSSession::createTextMessage(const std::string& text)
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, "creating text message");
#endif
	return new cms::amqp::CMSTextMessage(text);
}

cms::MapMessage* cms::amqp::CMSSession::createMapMessage()
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, "creating map message");
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
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("unsubscribe {}", name));
	throw ::cms::CMSException("illegal use - not implemented");
}

void cms::amqp::CMSSession::setMessageTransformer(::cms::MessageTransformer* transformer)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("set message transformer: {}", (void*)transformer));
	throw ::cms::CMSException("illegal use - not implemented");
}

cms::MessageTransformer* cms::amqp::CMSSession::getMessageTransformer() const
{
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}


//cms::amqp::SessionContext cms::amqp::CMSSession::createSessionContext(bool durable, bool shared, bool auto_ack) const
//{
//	mLogger->log(SEVERITY::LOG_DEBUG, fmt::format("create session context: durable: {} shared: {} auto_ack: {}", durable, shared, auto_ack));
//	return cms::amqp::SessionContext(mPimpl->mContext, auto_ack);
//}
