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

#include "Topic.h"

#include "MessageProperties.h"

stonex::amqp::Topic::Topic(const std::string& topicName)
    :mTopicName{ topicName },
    mDestinationType{ DestinationType::TOPIC }
{
}

stonex::amqp::Topic::Topic(const Topic& other)
    : mTopicName{other.mTopicName},
    mDestinationType{other.mDestinationType}
{
}

cms::Destination::DestinationType stonex::amqp::Topic::getDestinationType() const
{
    return mDestinationType;
}

cms::Destination* stonex::amqp::Topic::clone() const
{
    return new Topic(*this);
}

void stonex::amqp::Topic::copy(const cms::Destination& source)
{
	//TO DO handle copy, should  throw if source is not Topic?
}

bool stonex::amqp::Topic::equals(const cms::Destination& other) const
{
    if (mDestinationType != other.getDestinationType())
        return false;

    if (auto obj = dynamic_cast<const Topic*>(&other); !obj || mTopicName != obj->getTopicName())
        return false;

    return true;
}

const cms::CMSProperties& stonex::amqp::Topic::getCMSProperties() const
{
    return MessageProperties();
}

std::string stonex::amqp::Topic::getTopicName() const
{
    return mTopicName;
}