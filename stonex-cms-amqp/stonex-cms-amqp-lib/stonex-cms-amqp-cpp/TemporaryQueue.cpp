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

#include "TemporaryQueue.h"

#include "MessageProperties.h"

#include <chrono>

stonex::amqp::TemporaryQueue::TemporaryQueue(const std::string& queueName)
    :mQueueName{queueName},
    mDestinationType{ DestinationType::TEMPORARY_QUEUE }
{
}

stonex::amqp::TemporaryQueue::TemporaryQueue(const TemporaryQueue& other)
    : mQueueName{other.mQueueName},
    mDestinationType{other.mDestinationType}
{
}

stonex::amqp::TemporaryQueue& stonex::amqp::TemporaryQueue::operator=(const stonex::amqp::TemporaryQueue& other)
{
    this->mDestinationType = other.mDestinationType;
    this->mQueueName = other.mQueueName;

    return *this;
}

cms::Destination::DestinationType stonex::amqp::TemporaryQueue::getDestinationType() const
{
    return mDestinationType;
}

cms::Destination* stonex::amqp::TemporaryQueue::clone() const
{
    return new TemporaryQueue(*this);
}

void stonex::amqp::TemporaryQueue::copy(const Destination& source)
{
	//TO DO handle copy, should  throw if source is not TemporaryQueue?
}

bool stonex::amqp::TemporaryQueue::equals(const Destination& other) const
{
    if (mDestinationType != other.getDestinationType())
        return false;

    if (auto obj = dynamic_cast<const TemporaryQueue*>(&other); !obj || mQueueName != obj->getQueueName())
        return false;

    return true;
}

const cms::CMSProperties& stonex::amqp::TemporaryQueue::getCMSProperties() const
{
    return MessageProperties();
}

std::string stonex::amqp::TemporaryQueue::getQueueName() const
{
    return mQueueName;
}

void stonex::amqp::TemporaryQueue::destroy()
{
	//TO DO implement temporary queue
}
