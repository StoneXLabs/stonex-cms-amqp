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

#include "CMSTopic.h"

#include "CMSMessagePropertiesImpl.h"

cms::amqp::CMSTopic::CMSTopic(const std::string& topicName)
    :mTopicName{ topicName },
    mDestinationType{ DestinationType::TOPIC }
{
}

cms::amqp::CMSTopic::CMSTopic(const CMSTopic& other)
    : mTopicName{other.mTopicName},
    mDestinationType{other.mDestinationType}
{
}

cms::Destination::DestinationType cms::amqp::CMSTopic::getDestinationType() const
{
    return mDestinationType;
}

cms::Destination* cms::amqp::CMSTopic::clone() const
{
    return new CMSTopic(*this);
}

void cms::amqp::CMSTopic::copy(const ::cms::Destination& source)
{
    throw ::cms::CMSException("illegal use - not implemented");
}

bool cms::amqp::CMSTopic::equals(const ::cms::Destination& other) const
{
    if (mDestinationType != other.getDestinationType())
        return false;

    if (auto obj = dynamic_cast<const CMSTopic*>(&other); !obj || mTopicName != obj->getTopicName())
        return false;

    return true;
}

const cms::CMSProperties& cms::amqp::CMSTopic::getCMSProperties() const
{
    // TODO: insert return statement here
    return CMSMessagePropertiesImpl();
}

std::string cms::amqp::CMSTopic::getTopicName() const
{
    return mTopicName;
}