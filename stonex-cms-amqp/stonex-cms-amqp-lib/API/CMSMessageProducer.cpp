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

#include "CMSMessageProducer.h"

#include <cms/MessageTransformer.h>

#include "MessageProducerImpl.h"

#include "ConnectionContext.h"

 cms::amqp::CMSMessageProducer::CMSMessageProducer(const ::cms::Destination* destination, std::shared_ptr<SessionContext> context)
	:mPimpl(std::make_shared<MessageProducerImpl>(destination,context->connection()))
{
}

void  cms::amqp::CMSMessageProducer::send(::cms::Message* mes)
{
	mPimpl->send(mes);
}

void  cms::amqp::CMSMessageProducer::send(::cms::Message* mes, ::cms::AsyncCallback* callback)
{
	mPimpl->send(mes, callback);
}

void  cms::amqp::CMSMessageProducer::send(::cms::Message* mes, int deliveryMode, int priority, long long timeToLive)
{
	mPimpl->send(mes, deliveryMode, priority,timeToLive);
}

void  cms::amqp::CMSMessageProducer::send(::cms::Message* mes, int deliveryMode, int priority, long long timeToLive, ::cms::AsyncCallback* callback)
{
	mPimpl->send(mes, deliveryMode, priority, timeToLive, callback);
}

void  cms::amqp::CMSMessageProducer::send(const ::cms::Destination* destination, ::cms::Message* mes, int deliveryMode, int priority, long long timeToLive)
{
	mPimpl->send(destination, mes, deliveryMode, priority, timeToLive);
}

void  cms::amqp::CMSMessageProducer::send(const ::cms::Destination* destination, ::cms::Message* mes, int deliveryMode, int priority, long long timeToLive, ::cms::AsyncCallback* callback)
{
	mPimpl->send(destination, mes, deliveryMode, priority, timeToLive, callback);
}

void  cms::amqp::CMSMessageProducer::send(const ::cms::Destination* destination, ::cms::Message* mes, ::cms::AsyncCallback* callback)
{
	mPimpl->send(destination, mes, callback);
}

void  cms::amqp::CMSMessageProducer::send(const ::cms::Destination* destination, ::cms::Message* mes)
{
	mPimpl->send(destination, mes);
}



void  cms::amqp::CMSMessageProducer::setDeliveryMode(int mode)
{
	mPimpl->setDeliveryMode(mode);
}

int  cms::amqp::CMSMessageProducer::getDeliveryMode() const
{
	return mPimpl->getDeliveryMode();
}

void  cms::amqp::CMSMessageProducer::setDisableMessageID(bool value)
{
	mPimpl->setDisableMessageID(value);
}

bool  cms::amqp::CMSMessageProducer::getDisableMessageID() const
{
	return getDisableMessageID();
}

void  cms::amqp::CMSMessageProducer::setDisableMessageTimeStamp(bool value)
{
	mPimpl->setDisableMessageTimeStamp(value);
}

bool  cms::amqp::CMSMessageProducer::getDisableMessageTimeStamp() const
{
	return mPimpl->getDisableMessageTimeStamp();
}

void  cms::amqp::CMSMessageProducer::setPriority(int priority)
{
	mPimpl->setPriority(priority);
}

int  cms::amqp::CMSMessageProducer::getPriority() const
{
	return mPimpl->getPriority();
}

void  cms::amqp::CMSMessageProducer::setTimeToLive(long long time)
{
	mPimpl->setTimeToLive(time);
}

long long  cms::amqp::CMSMessageProducer::getTimeToLive() const
{
	return mPimpl->getTimeToLive();
}

void  cms::amqp::CMSMessageProducer::setMessageTransformer(::cms::MessageTransformer* transformer)
{
	mPimpl->setMessageTransformer(transformer);
}

::cms::MessageTransformer*  cms::amqp::CMSMessageProducer::getMessageTransformer() const
{
	return mPimpl->getMessageTransformer();
}

void cms::amqp::CMSMessageProducer::close()
{
	mPimpl->close();
}
