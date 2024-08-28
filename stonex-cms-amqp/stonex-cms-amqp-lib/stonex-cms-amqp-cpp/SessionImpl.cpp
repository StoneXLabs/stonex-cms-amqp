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

#include "SessionImpl.h"

#include "proton/work_queue.hpp"
#include <proton/session.hpp>
#include <proton/session_options.hpp>


#include "../API/CMSTextMessage.h"
#include "MessageConsumerImpl.h"
#include "MessageProducerImpl.h"

#include <fmt/format.h>

#include <LoggerFactory/LoggerFactory.h>

class MessageConsumer : public cms::MessageConsumer{};
class MessageProducer : public cms::MessageProducer {};
class QueueBrowser : public cms::QueueBrowser {};
class Queue : public cms::Queue {};
class Topic : public cms::Topic {};
class TemporaryQueue : public cms::TemporaryQueue {};
class TemporaryTopic : public cms::TemporaryTopic {};
class Message : public cms::Message {};
class BytesMessage : public cms::BytesMessage {};
class StreamMessage : public cms::StreamMessage {};
//class TextMessage : public cms::TextMessage {};
//class BytesMessage : public cms::BytesMessage {};






cms::amqp::SessionImpl::SessionImpl(const config::SessionContext& context)
	:mLogger(LoggerFactory::getInstance().create("com.stonex.cms.amqp.SessionImpl")),
	mContext(context)
{
	mContext.setLogger(mLogger);
	std::unique_lock lk(mMutex);
	proton::session_options sessionOptions;
	sessionOptions.handler(*this);
	if (mContext.mWorkQueue)
	{
		mContext.mWorkQueue->add([=]() {mContext.mConnection.open_session(sessionOptions); });
		mCv.wait(lk, [this]() {return !mContext.checkState(ClientState::UNNINITIALIZED); });
	}
	else
	{
		throw CMSException("Connection uninitiaized"); //change to cms::IllegalStateException("ActiveMQConnection::enforceConnected - Connection has already been closed!");
	}
}

cms::amqp::SessionImpl::~SessionImpl()
{
	close();
}

void cms::amqp::SessionImpl::start()
{
	for (auto& consumer : mConsumers)
	{
		if (auto consumerPtr = consumer.lock())
		{
			consumerPtr->start();
		}
	}
	mContext.setState(ClientState::STARTED);
}

void cms::amqp::SessionImpl::stop()
{

	for (auto& consumer : mConsumers)
	{
		if (auto consumerPtr = consumer.lock())
		{
			consumerPtr->stop();
		}
	}
	mContext.setState(ClientState::STOPPED);
}

void cms::amqp::SessionImpl::close()
{
	if (auto queue = mContext.mWorkQueue; queue != nullptr)
	{

		std::unique_lock lk(mMutex);

		for (auto& consumer : mConsumers)
		{
			if (auto consumerPtr = consumer.lock())
			{
				consumerPtr->close();
			}
		}

		for (auto& producer : mProducers)
		{
			if (auto producerPtr = producer.lock())
			{
				producerPtr->close();
			}
		}
		queue->add([this]() {mContext.mSession.close(); });
		mCv.wait(lk, [this]() {return mContext.checkState(ClientState::CLOSED); });
	}
}

void cms::amqp::SessionImpl::commit()
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, "method not implemented"));
}

void cms::amqp::SessionImpl::rollback()
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, "method not implemented"));
}

void cms::amqp::SessionImpl::recover()
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, "method not implemented"));
}

cms::Session::AcknowledgeMode cms::amqp::SessionImpl::ackMode()
{
	return {};
}

void cms::amqp::SessionImpl::on_session_open(proton::session& session)
{
	if (auto err = session.error(); err.empty())
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} {}", __func__, err.what()));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));

	mContext.mSession = session;
	mContext.mWorkQueue = &mContext.mSession.work_queue();
	if (mContext.checkState(ClientState::UNNINITIALIZED))
	{
		mContext.setState(ClientState::STOPPED);
	}
	else
	{
		mContext.setState(ClientState::STARTED);
	}

	mCv.notify_one();
}

void cms::amqp::SessionImpl::on_session_close(proton::session& session)
{
	if (auto err = session.error(); err.empty())
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} {}", __func__, err.what()));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));

	mContext.setState(ClientState::CLOSED);
	mContext.mWorkQueue = nullptr;
	mCv.notify_one();
}

void cms::amqp::SessionImpl::on_session_error(proton::session& session)
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, session.error().what()));
}



void cms::amqp::SessionImpl::addConsumer(std::shared_ptr<MessageConsumerImpl> consumer)
{
	mConsumers.push_back(consumer);
}

void cms::amqp::SessionImpl::addProducer(std::shared_ptr<MessageProducerImpl> producer)
{
	mProducers.push_back(producer);
}


void cms::amqp::SessionImpl::check()
{
	if (mContext.checkState(ClientState::CLOSED))
	{
		throw cms::IllegalStateException("Session allready closed");
	}
}