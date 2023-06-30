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

#include "CMSMessageProducer.h"

#include <cms/MessageTransformer.h>

#include "MessageProducerImpl.h"

#include "ConnectionContext.h"

 cms::amqp::CMSMessageProducer::CMSMessageProducer(const ::cms::Destination* destination, std::shared_ptr<SessionContext> context, std::shared_ptr<StonexLogger> logger)
	:mPimpl(std::make_shared<MessageProducerImpl>(destination,context->connection(), logger))
{
}

void  cms::amqp::CMSMessageProducer::send(::cms::Message* mes)
{
	debug("producer", "sending message");
	mPimpl->send(mes);
}

void  cms::amqp::CMSMessageProducer::send(::cms::Message* mes, ::cms::AsyncCallback* callback)
{
	debug("producer", "sending message with callback");
	mPimpl->send(mes, callback);
}

void  cms::amqp::CMSMessageProducer::send(::cms::Message* mes, int deliveryMode, int priority, long long timeToLive)
{
	debug("producer", "sending message");
	mPimpl->send(mes, deliveryMode, priority,timeToLive);
}

void  cms::amqp::CMSMessageProducer::send(::cms::Message* mes, int deliveryMode, int priority, long long timeToLive, ::cms::AsyncCallback* callback)
{
	debug("producer", "sending message with callback");
	mPimpl->send(mes, deliveryMode, priority, timeToLive, callback);
}

void  cms::amqp::CMSMessageProducer::send(const ::cms::Destination* destination, ::cms::Message* mes, int deliveryMode, int priority, long long timeToLive)
{
	debug("producer", "sending message");
	mPimpl->send(destination, mes, deliveryMode, priority, timeToLive);
}

void  cms::amqp::CMSMessageProducer::send(const ::cms::Destination* destination, ::cms::Message* mes, int deliveryMode, int priority, long long timeToLive, ::cms::AsyncCallback* callback)
{
	debug("producer", "sending message with callback");
	mPimpl->send(destination, mes, deliveryMode, priority, timeToLive, callback);
}

void  cms::amqp::CMSMessageProducer::send(const ::cms::Destination* destination, ::cms::Message* mes, ::cms::AsyncCallback* callback)
{
	debug("producer", "sending message");
	mPimpl->send(destination, mes, callback);
}

void  cms::amqp::CMSMessageProducer::send(const ::cms::Destination* destination, ::cms::Message* mes)
{
	debug("producer", "sending message");
	mPimpl->send(destination, mes);
}



void  cms::amqp::CMSMessageProducer::setDeliveryMode(int mode)
{
	info("producer", fmt::format("set delivery mode {}",mode));
	mPimpl->setDeliveryMode(mode);
}

int  cms::amqp::CMSMessageProducer::getDeliveryMode() const
{
	return mPimpl->getDeliveryMode();
}

void  cms::amqp::CMSMessageProducer::setDisableMessageID(bool value)
{
	info("producer", fmt::format("disable message id {}", value));
	mPimpl->setDisableMessageID(value);
}

bool  cms::amqp::CMSMessageProducer::getDisableMessageID() const
{
	return getDisableMessageID();
}

void  cms::amqp::CMSMessageProducer::setDisableMessageTimeStamp(bool value)
{
	info("producer", fmt::format("disable message timestamp id {}", value));
	mPimpl->setDisableMessageTimeStamp(value);
}

bool  cms::amqp::CMSMessageProducer::getDisableMessageTimeStamp() const
{
	return mPimpl->getDisableMessageTimeStamp();
}

void  cms::amqp::CMSMessageProducer::setPriority(int priority)
{
	info("producer", fmt::format("set priority {}", priority));
	mPimpl->setPriority(priority);
}

int  cms::amqp::CMSMessageProducer::getPriority() const
{
	return mPimpl->getPriority();
}

void  cms::amqp::CMSMessageProducer::setTimeToLive(long long time)
{
	info("producer", fmt::format("set TTL {} [ms]", time));
	mPimpl->setTimeToLive(time);
}

long long  cms::amqp::CMSMessageProducer::getTimeToLive() const
{
	return mPimpl->getTimeToLive();
}

void  cms::amqp::CMSMessageProducer::setMessageTransformer(::cms::MessageTransformer* transformer)
{
	info("producer", fmt::format("set message transformer: {}", (void*)transformer));
	mPimpl->setMessageTransformer(transformer);
}

::cms::MessageTransformer*  cms::amqp::CMSMessageProducer::getMessageTransformer() const
{
	return mPimpl->getMessageTransformer();
}

void cms::amqp::CMSMessageProducer::close()
{
	info("producer", "closing");
	mPimpl->close();
}

void cms::amqp::CMSMessageProducer::setLogger(std::shared_ptr<StonexLogger> sink)
{
	StonexLogSource::setLogger(sink);
	mPimpl->setLogger(sink);
};