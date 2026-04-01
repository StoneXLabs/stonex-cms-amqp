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

#include "Queue.h"



stonex::amqp::Queue::Queue(const std::string& queueName)
    :mQueueName{queueName},
    mDestinationType{DestinationType::QUEUE}
{
}

stonex::amqp::Queue::Queue(const Queue& other)
    : mQueueName{other.mQueueName},
    mDestinationType{other.mDestinationType}
{
}

cms::Destination::DestinationType stonex::amqp::Queue::getDestinationType() const
{
    return mDestinationType;
}

cms::Destination* stonex::amqp::Queue::clone() const
{
    return new Queue(*this);
}

void stonex::amqp::Queue::copy(const Destination& source)
{
	//TO DO handle copy, should  throw if source is not Queue?
}

bool stonex::amqp::Queue::equals(const Destination& other) const
{
    if (mDestinationType != other.getDestinationType())
        return false;

    if (auto obj = dynamic_cast<const Queue*>(&other); !obj || mQueueName != obj->getQueueName())
        return false;

    return true;
}

const cms::CMSProperties& stonex::amqp::Queue::getCMSProperties() const
{
    return mProperties;
}

std::string stonex::amqp::Queue::getQueueName() const
{
    return mQueueName;
}
