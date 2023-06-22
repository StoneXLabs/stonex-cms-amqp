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

#include <array>

#include <cms/Destination.h>
#include <cms/Message.h>
#include <proton/sender.hpp>
#include <proton/receiver.hpp>
#include <proton/message.hpp>
#include "..\API\stonex-cms-amqp-lib-defines.h"

namespace cms::amqp
{
	constexpr std::string_view QUEUE_CAPABILITY = "queue";
	constexpr std::string_view TOPIC_CAPABILITY = "topic";
	constexpr std::string_view TEMPORARY_QUEUE_CAPABILITY = "temporary-queue";
	constexpr std::string_view TEMPORARY_TOPIC_CAPABILITY = "temporary-topic";

	constexpr std::string_view X_OPT_TO_TYPE = "x-opt-to-type"; 
	constexpr std::string_view X_OPT_JMS_DEST = "x-opt-jms-dest";

	constexpr std::string_view X_OPT_REPLY_TO_TYPE = "x-opt-reply-to-type";  
	constexpr std::string_view X_OPT_JMS_REPLY_TO = "x-opt-jms-reply-to";

	constexpr std::string_view X_OPT_JMS_MESSAGE_TYPE = "x-opt-jms-msg-type";

	constexpr std::string_view X_OPT_JMS_COTENT_TYPE = "x-opt-jms-content-type";

	//CMSX properties
	constexpr std::string_view CMSXGroupID = "CMSXGroupID";
	constexpr std::string_view CMSXGroupSeq = "CMSXGroupSeq";

	enum class MESSAGE_TYPE
	{
		MESSAGE,
		OBJECT_MESSAGE,
		MAP_MESSAGE,
		BYTES_MESSAGE,
		STREAM_MESSAGE,
		TEXT_MESSAGE
	};

	class  CMS_API AMQPCMSMessageConverter
	{
	public:
		~AMQPCMSMessageConverter() = default;
		static ::cms::Destination* createCMSDestination(const proton::sender* sender);
		static ::cms::Destination* createCMSDestination(const proton::receiver* receiver);
		static ::cms::Destination* createCMSDestination(const std::shared_ptr<proton::message> message);
		static ::cms::Destination* createCMSDestinationUsingMessageDestination(const proton::sender* sender, const std::shared_ptr<proton::message> message);
		static ::cms::Destination* createCMSDestinationUsingMessageDestination(const proton::receiver* receiver, const std::shared_ptr<proton::message> message);

		static ::cms::Destination* createCMSReplyToUsingMessageDestination(const std::shared_ptr<proton::message> message);

		static std::string destinationAddress(const ::cms::Message* message);
		static std::string replyToAddress(const ::cms::Message* message);

		static std::string destinationToAddress(const ::cms::Destination* destintion);

		static ::cms::Message::ValueType type_id_to_ValueType(proton::type_id t_id);
		static std::tuple<proton::type_id, int, bool> prefixToType(const std::string& s);
		static std::array<char, 16> hexStringToArray(const std::string& s);
		static std::string hexStringToBinaryArray(const std::string& s);
		static std::string binaryArrayToHexString(const std::vector<uint8_t>& b);
		static std::string destinationTypeToString(::cms::Destination::DestinationType dest_type);
		static int8_t destinationTypeToJMSType(::cms::Destination::DestinationType dest_type);

		static std::vector<std::string> getPropertyNames(const std::shared_ptr<proton::message> message);
		static bool propertyExists(const std::string& name, const std::shared_ptr<proton::message> message);
		
		static bool getCMSBooleanProperty(const std::string& name, const std::shared_ptr<proton::message> message);
		static unsigned char getCMSByteProperty(const std::string& name, const std::shared_ptr<proton::message> message);
		static double getCMSDoubleProperty(const std::string& name, const std::shared_ptr<proton::message> message);
		static float getCMSFloatProperty(const std::string& name, const std::shared_ptr<proton::message> message);
		static int getCMSIntProperty(const std::string& name, const std::shared_ptr<proton::message> message);
		static long long getCMSLongProperty(const std::string& name, const std::shared_ptr<proton::message> message);
		static short getCMSShortProperty(const std::string& name, const std::shared_ptr<proton::message> message);
		static std::string getCMSStringProperty(const std::string& name, const std::shared_ptr<proton::message> message);
		
		static void setAMQPBooleanProperty(const std::string& name, bool value, std::shared_ptr<proton::message> message);
		static void setAMQPByteProperty(const std::string& name, unsigned char value, std::shared_ptr<proton::message> message);
		static void setAMQPDoubleProperty(const std::string& name, double value, std::shared_ptr<proton::message> message);
		static void setAMQPFloatProperty(const std::string& name, float value, std::shared_ptr<proton::message> message);
		static void setAMQPIntProperty(const std::string& name, int value, std::shared_ptr<proton::message> message);
		static void setAMQPLongProperty(const std::string& name, long long value, std::shared_ptr<proton::message> message);
		static void setAMQPShortProperty(const std::string& name, short value, std::shared_ptr<proton::message> message);
		static void setAMQPStringProperty(const std::string& name, const std::string& value, std::shared_ptr<proton::message> message);
		
		static std::string getCMSCorrelationID(const std::shared_ptr<proton::message> message);
		static void setAMQPCorrelationID(const std::string& correlationId, std::shared_ptr<proton::message> message);
		
		static int getCMSDeliveryMode(const std::shared_ptr<proton::message> message);
		static void setAMQPDeliveryMode(int mode, std::shared_ptr<proton::message> message);
		
		static void setAMQPDestination(const ::cms::Destination* destination, std::shared_ptr<proton::message> message);
		
		static long long getCMSExpiration(const std::shared_ptr<proton::message> message);
		static void setAMQPExpiration(long long expireTime, std::shared_ptr<proton::message> message);
		
		static std::string getCMSMessageID(const std::shared_ptr<proton::message> message);
		static void setAMQPMessageID(const std::string& id, std::shared_ptr<proton::message> message);
		
		static int getCMSPriority(const std::shared_ptr<proton::message> message);
		static void setAMQPPriority(int priority, std::shared_ptr<proton::message> message);
		
		static bool getCMSRedelivered(const std::shared_ptr<proton::message> message);
		static void setAMQPRedelivered(bool redelivered, std::shared_ptr<proton::message> message);
		
		static void setAMQPReplyTo(const ::cms::Destination* destination, std::shared_ptr<proton::message> message);
		
		static long long getCMSTimestamp(const std::shared_ptr<proton::message> message);
		static void setAMQPTimestamp(long long timeStamp, std::shared_ptr<proton::message> message);

		static std::string getCMSType(const std::shared_ptr<proton::message> message);
		static void setAMQPType(const std::string& type, std::shared_ptr<proton::message> message);
	private:
		static ::cms::Destination::DestinationType capabilityToDestinationType(const std::vector<proton::symbol>& capabilities);
		static ::cms::Destination::DestinationType annotationsToDestinationType(const proton::message::annotation_map& annotations, const proton::symbol& annotation_key);
		static ::cms::Destination::DestinationType JMSTypeToDestinationType(const uint8_t type);
		static ::cms::Destination::DestinationType JMSTypeToDestinationType(const int8_t type);

	};

}