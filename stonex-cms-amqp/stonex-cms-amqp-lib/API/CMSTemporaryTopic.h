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

#pragma once
#include "cms/TemporaryTopic.h"

#include <logger/StonexLogSource.h>

#include "stonex-cms-amqp-lib-defines.h"

AMQP_DEFINES


    class CMSMessageConsumer;
    class CMSMessageProducer;

    class CMS_API CMSTemporaryTopic : public ::cms::TemporaryTopic
    {
        friend class MessageConsumerImpl;//remove
        friend class MessageProducerImpl;//remove
        friend class AMQPCMSMessageConverter;
    public:
        explicit CMSTemporaryTopic();
        CMSTemporaryTopic(const CMSTemporaryTopic& other);
        CMSTemporaryTopic(CMSTemporaryTopic&& other) = delete;

        ::cms::Destination::DestinationType getDestinationType() const override;

        ::cms::Destination* clone() const override;

        void copy(const ::cms::Destination& source) override;

        bool equals(const ::cms::Destination& other) const override;

        const ::cms::CMSProperties& getCMSProperties() const override;

        std::string getTopicName() const override;

		friend std::ostream& operator << (std::ostream& os, const cms::amqp::CMSTemporaryTopic& ob);

    private:
        mutable std::string mTopicName;
        const ::cms::Destination::DestinationType mDestinationType;
        void destroy() override;
    };

AMQP_DEFINES_CLOSE
