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
#include <cms/Topic.h>

#include <logger/StonexLogSource.h>

#include "stonex-cms-amqp-lib-defines.h"

AMQP_DEFINES


	class CMS_API CMSTopic : public ::cms::Topic
	{
    public:
        explicit CMSTopic(const std::string& topicName);
        CMSTopic(const CMSTopic& other);
        CMSTopic(CMSTopic&& topicName) = delete;

        DestinationType getDestinationType() const override;

        Destination* clone() const override;

        void copy(const Destination& source) override;

        bool equals(const Destination& other) const override;

        const ::cms::CMSProperties& getCMSProperties() const override;

        std::string getTopicName() const override;

		friend std::ostream& operator << (std::ostream& os, const cms::amqp::CMSTopic& ob);

    private:
        const std::string mTopicName;
        const Destination::DestinationType mDestinationType;
	};



AMQP_DEFINES_CLOSE

