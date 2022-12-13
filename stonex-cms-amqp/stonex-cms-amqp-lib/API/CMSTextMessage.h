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

#pragma once

#include <cms/TextMessage.h>

#include "stonex-cms-amqp-lib-defines.h"

namespace proton
{
	class message;
	class sender;
	class receiver;
	class delivery;
}

AMQP_DEFINES

	class  AMQPCMSMessageConverter;

	class CMS_API CMSTextMessage :public ::cms::TextMessage
	{
		friend class MessageProducerImpl;
	public:
		CMSTextMessage(const CMSTextMessage& other);
		CMSTextMessage(CMSTextMessage&& other) = delete;

		explicit CMSTextMessage(const std::string& text);
		CMSTextMessage(proton::message* message, const proton::delivery* delivery, const proton::receiver* receiver);
		
		~CMSTextMessage() override;

		std::string getText() const  override;
		void setText(const char* msg)  override {};
		void setText(const std::string& msg)  override {};

	public:
		Message* clone() const override;
		void acknowledge() const override;
		void clearBody() override;
		void clearProperties() override;

		std::vector<std::string> getPropertyNames() const override;
		bool propertyExists(const std::string & name) const override;
		ValueType getPropertyValueType(const std::string & name) const override;

		bool getBooleanProperty(const std::string & name) const override;
		unsigned char getByteProperty(const std::string & name) const override;
		double getDoubleProperty(const std::string & name) const override;
		float getFloatProperty(const std::string & name) const override;
		int getIntProperty(const std::string & name) const override;
		long long getLongProperty(const std::string & name) const override;
		short getShortProperty(const std::string & name) const override;
		std::string getStringProperty(const std::string & name) const override;

		void setBooleanProperty(const std::string & name, bool value) override;
		void setByteProperty(const std::string & name, unsigned char value) override;
		void setDoubleProperty(const std::string & name, double value) override;
		void setFloatProperty(const std::string & name, float value) override;
		void setIntProperty(const std::string & name, int value) override;
		void setLongProperty(const std::string & name, long long value) override;
		void setShortProperty(const std::string & name, short value) override;
		void setStringProperty(const std::string & name, const std::string & value) override;

		std::string getCMSCorrelationID() const override;
		void setCMSCorrelationID(const std::string & correlationId) override;

		int getCMSDeliveryMode() const override;
		void setCMSDeliveryMode(int mode) override;

		const ::cms::Destination* getCMSDestination() const override;
		void setCMSDestination(const ::cms::Destination * destination) override;

		long long getCMSExpiration() const override;
		void setCMSExpiration(long long expireTime) override;

		std::string getCMSMessageID() const override;
		void setCMSMessageID(const std::string & id) override;

		int getCMSPriority() const override;
		void setCMSPriority(int priority) override;

		bool getCMSRedelivered() const override;
		void setCMSRedelivered(bool redelivered) override;

		const ::cms::Destination* getCMSReplyTo() const override;
		void setCMSReplyTo(const ::cms::Destination * destination) override;

		long long getCMSTimestamp() const override;
		void setCMSTimestamp(long long timeStamp) override;

		std::string getCMSType() const override;
		void setCMSType(const std::string & type) override;

	protected:
		void body(const std::string& message);

	protected:
		std::shared_ptr <proton::message> mMessage;
		std::shared_ptr <proton::delivery> mMessageDelivery{nullptr};

	private:
		std::shared_ptr<const ::cms::Destination> mDestination{ nullptr };
		std::shared_ptr<const ::cms::Destination> mReplyTo{ nullptr };
	};

AMQP_DEFINES_CLOSE

