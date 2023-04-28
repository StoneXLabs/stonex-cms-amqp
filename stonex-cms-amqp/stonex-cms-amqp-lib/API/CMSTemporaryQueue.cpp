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

#include "CMSTemporaryQueue.h"

#include "CMSMessagePropertiesImpl.h"

#include <chrono>

cms::amqp::CMSTemporaryQueue::CMSTemporaryQueue()
    :mDestinationType{ DestinationType::TEMPORARY_QUEUE }
{
}

cms::amqp::CMSTemporaryQueue::CMSTemporaryQueue(const CMSTemporaryQueue& other)
    : mQueueName{other.mQueueName},
    mDestinationType{other.mDestinationType}
{
}

cms::amqp::CMSTemporaryQueue& cms::amqp::CMSTemporaryQueue::operator=(const cms::amqp::CMSTemporaryQueue& other)
{
    this->mDestinationType = other.mDestinationType;
    this->mQueueName = other.mQueueName;

    return *this;
}

::cms::Destination::DestinationType cms::amqp::CMSTemporaryQueue::getDestinationType() const
{
    return mDestinationType;
}

::cms::Destination* cms::amqp::CMSTemporaryQueue::clone() const
{
    return new CMSTemporaryQueue(*this);
}

void cms::amqp::CMSTemporaryQueue::copy(const Destination& source)
{
    throw ::cms::CMSException("illegal use - not implemented");
}

bool cms::amqp::CMSTemporaryQueue::equals(const Destination& other) const
{
    if (mDestinationType != other.getDestinationType())
        return false;

    if (auto obj = dynamic_cast<const CMSTemporaryQueue*>(&other); !obj || mQueueName != obj->getQueueName())
        return false;

    return true;
}

const ::cms::CMSProperties& cms::amqp::CMSTemporaryQueue::getCMSProperties() const
{
    // TODO: insert return statement here
    return CMSMessagePropertiesImpl();
}

std::string cms::amqp::CMSTemporaryQueue::getQueueName() const
{
    return mQueueName;
}

void cms::amqp::CMSTemporaryQueue::destroy()
{
    throw ::cms::CMSException("illegal use - not implemented");
}

std::ostream & operator<<(std::ostream & os, const cms::amqp::CMSTemporaryQueue & ob)
{
	os << "TEMPORARY QUEUE " << ob.getQueueName();
	return os;
}
