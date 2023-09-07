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


#include <cms/BytesMessage.h>
#include "stonex-cms-amqp-lib-defines.h"

namespace proton
{
	class message;
	class sender;
	class receiver;
	class delivery;
}

AMQP_DEFINES

	class AMQPCMSMessageConverter;

	class CMS_API CMSBytesMessage :public ::cms::BytesMessage
	{
		friend class MessageProducerImpl;

	public:

		CMSBytesMessage();
		CMSBytesMessage(const CMSBytesMessage& other);
		CMSBytesMessage(const unsigned char* bytes, int bytesSize);
		CMSBytesMessage(proton::message* message, const proton::delivery* delivery, const proton::receiver* receiver);

		~CMSBytesMessage() override = default;

		void acknowledge() const override;
		void clearBody() override;
		void clearProperties() override;

		std::vector<std::string> getPropertyNames() const override;
		bool propertyExists(const std::string& name) const override;
		ValueType getPropertyValueType(const std::string& name) const override;

		bool getBooleanProperty(const std::string& name) const override;
		unsigned char getByteProperty(const std::string& name) const override;
		double getDoubleProperty(const std::string& name) const override;
		float getFloatProperty(const std::string& name) const override;
		int getIntProperty(const std::string& name) const override;
		long long getLongProperty(const std::string& name) const override;
		short getShortProperty(const std::string& name) const override;
		std::string getStringProperty(const std::string& name) const override;

		void setBooleanProperty(const std::string& name, bool value) override;
		void setByteProperty(const std::string& name, unsigned char value) override;
		void setDoubleProperty(const std::string& name, double value) override;
		void setFloatProperty(const std::string& name, float value) override;
		void setIntProperty(const std::string& name, int value) override;
		void setLongProperty(const std::string& name, long long value) override;
		void setShortProperty(const std::string& name, short value) override;
		void setStringProperty(const std::string& name, const std::string& value) override;



		std::string getCMSCorrelationID() const override;
		void setCMSCorrelationID(const std::string& correlationId) override;

		int getCMSDeliveryMode() const override;
		void setCMSDeliveryMode(int mode) override;

		const ::cms::Destination* getCMSDestination() const override;
		void setCMSDestination(const ::cms::Destination* destination) override;

		long long getCMSExpiration() const override;
		void setCMSExpiration(long long expireTime) override;

		std::string getCMSMessageID() const override;
		void setCMSMessageID(const std::string& id) override;

		int getCMSPriority() const override;
		void setCMSPriority(int priority) override;

		bool getCMSRedelivered() const override;
		void setCMSRedelivered(bool redelivered) override;

		const ::cms::Destination* getCMSReplyTo() const override;
		void setCMSReplyTo(const ::cms::Destination* destination) override;

		long long getCMSTimestamp() const override;
		void setCMSTimestamp(long long timeStamp) override;

		std::string getCMSType() const override;
		void setCMSType(const std::string& type) override;

	public: // BytesMessage methods
		void setBodyBytes(const unsigned char* buffer, int numBytes) override;
		unsigned char* getBodyBytes() const override;
		int getBodyLength() const override;
		void reset() override;
		bool readBoolean() const override;
		void writeBoolean(bool value) override;
		unsigned char readByte() const override;
		void writeByte(unsigned char value) override;
		int readBytes(std::vector<unsigned char>& value) const override;
		void writeBytes(const std::vector<unsigned char>& value) override;
		int readBytes(unsigned char* buffer, int length) const override;
		void writeBytes(const unsigned char* value, int offset, int length) override;
		char readChar() const override;
		void writeChar(char value) override;
		float readFloat() const override;
		void writeFloat(float value) override;
		double readDouble() const override;
		void writeDouble(double value) override;
		short readShort() const override;
		void writeShort(short value) override;
		unsigned short readUnsignedShort() const override;
		void writeUnsignedShort(unsigned short value) override;
		int readInt() const override;
		void writeInt(int value) override;
		long long readLong() const override;
		void writeLong(long long value) override;
		std::string readString() const override;
		void writeString(const std::string& value) override;
		std::string readUTF() const override;
		void writeUTF(const std::string& value) override;
		BytesMessage* clone() const override;

	private:
		std::shared_ptr<proton::message> message();

		template <typename T>
		void set(T value)
		{
			constexpr size_t size{ sizeof(T) };
			if (int space = mMessageBody.capacity() - mMessageBody.size(); space < size)
			{
				mMessageBody.reserve(size - space);
			}
			std::copy_n((uint8_t*)&value, size, std::back_inserter(mMessageBody));
		}

		template <typename T>
		T get() const
		{
			try
			{
				constexpr size_t size{ sizeof(T) };
				T value{ 0 };
				memcpy(&value, &mMessageBody.at(read_position), size);
				read_position+=size;
				return value;
			}
			catch (const std::out_of_range&)
			{
				throw ::cms::MessageEOFException();
			}
		}

		template<>
		void set(const std::string s)
		{
			std::copy(std::cbegin(s), std::cend(s), std::back_inserter(mMessageBody));
		}

		template<>
		std::string get() const
		{
			if (mMessageBody.size() <= read_position)
				throw ::cms::MessageEOFException();

			std::string output;
			std::copy(std::next(std::cbegin(mMessageBody), read_position), std::cend(mMessageBody), std::back_inserter(output));
			return output;
		}
	protected:
		std::shared_ptr<proton::message> mMessage;
		std::vector<uint8_t> mMessageBody;
		mutable const proton::delivery *mMessageDelivery;
		mutable long long read_position{ 0 };

	private:
		std::shared_ptr<const ::cms::Destination> mDestination{ nullptr };
		std::shared_ptr<const ::cms::Destination> mReplyTo{ nullptr };
	};

AMQP_DEFINES_CLOSE

