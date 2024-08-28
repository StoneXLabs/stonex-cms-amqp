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

#include "CMSQueue.h"

#include "CMSMessagePropertiesImpl.h"


cms::amqp::CMSQueue::CMSQueue(const std::string& queueName)
    :mQueueName{queueName},
    mDestinationType{DestinationType::QUEUE}
{
}

cms::amqp::CMSQueue::CMSQueue(const CMSQueue& other)
    : mQueueName{other.mQueueName},
    mDestinationType{other.mDestinationType}
{
}

cms::Destination::DestinationType cms::amqp::CMSQueue::getDestinationType() const
{
    return mDestinationType;
}

cms::Destination* cms::amqp::CMSQueue::clone() const
{
    return new CMSQueue(*this);
}

void cms::amqp::CMSQueue::copy(const Destination& source)
{
    throw cms::CMSException("illegal use - not implemented");
}

bool cms::amqp::CMSQueue::equals(const Destination& other) const
{
    if (mDestinationType != other.getDestinationType())
        return false;

    if (auto obj = dynamic_cast<const CMSQueue*>(&other); !obj || mQueueName != obj->getQueueName())
        return false;

    return true;
}

const cms::CMSProperties& cms::amqp::CMSQueue::getCMSProperties() const
{
    // TODO: insert return statement here
    return CMSMessagePropertiesImpl();
}

std::string cms::amqp::CMSQueue::getQueueName() const
{
    return mQueueName;
}

std::ostream & operator<<(std::ostream & os, const cms::amqp::CMSQueue & ob)
{
	os << "QUEUE " << ob.getQueueName();
	return os;
}
