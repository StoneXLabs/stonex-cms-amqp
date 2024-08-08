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

#include "CMSMessageConsumer.h"
#include "ConnectionContext.h"
#include "MessageConsumerImpl.h"
#include <API/CMSSession.h>

cms::amqp::CMSMessageConsumer::CMSMessageConsumer(std::shared_ptr<MessageConsumerImpl> impl)
	: mPimpl(impl),
	mLogger(LoggerFactory::getInstance().create("com.stonex.cms.CMSMessageConsumer"))
{
}

cms::amqp::CMSMessageConsumer::~CMSMessageConsumer()
{
}

::cms::Message*  cms::amqp::CMSMessageConsumer::receive()
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, "receive");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

::cms::Message*  cms::amqp::CMSMessageConsumer::receive(int milis)
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, fmt::format("receive. timeout: {} [ms]",milis));
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

::cms::Message*  cms::amqp::CMSMessageConsumer::receiveNoWait()
{
#if _DEBUG
	mLogger->log(SEVERITY::LOG_DEBUG, "receive no wait");
#endif
	throw ::cms::CMSException("illegal use - not implemented");
	return nullptr;
}

void  cms::amqp::CMSMessageConsumer::setMessageListener(::cms::MessageListener* listener)
{
	
	mLogger->log(SEVERITY::LOG_DEBUG, fmt::format("set message listener: {}", (void*)listener));
	mPimpl->setMessageListener(listener);
}

::cms::MessageListener*  cms::amqp::CMSMessageConsumer::getMessageListener() const
{
	return mPimpl->getMessageListener();
}

std::string  cms::amqp::CMSMessageConsumer::getMessageSelector() const
{
	return mPimpl->getMessageSelector();
}

void  cms::amqp::CMSMessageConsumer::setMessageTransformer(::cms::MessageTransformer* transformer)
{
	mLogger->log(SEVERITY::LOG_DEBUG, fmt::format("set message transformer: {}", (void*)transformer));
	mPimpl->setMessageTransformer(transformer);
}

::cms::MessageTransformer*  cms::amqp::CMSMessageConsumer::getMessageTransformer() const
{
	return mPimpl->getMessageTransformer();
}

void  cms::amqp::CMSMessageConsumer::setMessageAvailableListener(::cms::MessageAvailableListener* listener)
{
	mLogger->log(SEVERITY::LOG_DEBUG, fmt::format("set message available listener: {}", (void*)listener));
	mPimpl->setMessageAvailableListener(listener);
}

::cms::MessageAvailableListener*  cms::amqp::CMSMessageConsumer::getMessageAvailableListener() const
{
	return mPimpl->getMessageAvailableListener();
}

void  cms::amqp::CMSMessageConsumer::start()
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("starting consumer {}", mPimpl->getAddress()));
	mPimpl->start();
}

void  cms::amqp::CMSMessageConsumer::stop()
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("stopping consumer {}", mPimpl->getAddress()));
	mPimpl->stop();
}

void  cms::amqp::CMSMessageConsumer::close()
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("closing consumer {}", mPimpl->getAddress()));
	mPimpl->close();
}