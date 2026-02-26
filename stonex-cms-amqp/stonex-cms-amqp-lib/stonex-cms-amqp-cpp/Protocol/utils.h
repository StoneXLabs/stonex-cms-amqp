#pragma once

#include <proton/type_id.hpp>
#include <proton/symbol.hpp>
#include <proton/source.hpp>
#include <proton/target.hpp>
#include <proton/message.hpp>

#include <cms/Message.h>

#include "Destination.h"

namespace internal {

	namespace capability
	{
		constexpr const char *QUEUE = "queue";
		constexpr const char *TOPIC = "topic";
		constexpr const char *TEMPORARY_QUEUE = "temporary-queue";
		constexpr const char *TEMPORARY_TOPIC = "temporary-topic";
		constexpr const char *DELETE_ON_CLOSE = "delete-on-close";
	}


	namespace annotation
	{
		constexpr const char* JMS_MESSAGE_TYPE = "x-opt-jms-msg-type";
		enum class MESSAGE_TYPE : int8_t
		{
			MESSAGE = 0,
			OBJECT_MESSAGE = 1,
			MAP_MESSAGE = 2,
			BYTES_MESSAGE = 3,
			STREAM_MESSAGE = 4,
			TEXT_MESSAGE = 5,
		};

		constexpr const char* JMS_DESTINATION_TYPE = "x-opt-jms-dest";
		constexpr const char* JMS_REPLY_TO_TYPE = "x-opt-jms-reply-to";

		enum class DESTINATION_TYPE : int8_t
		{
			QUEUE = 0,
			TOPIC = 1,
			TEMPORARY_QUEUE = 2,
			TEMPORARY_TOPIC = 3
		};


	}

	class DestinationConverter
	{
	public:
		static std::vector<proton::symbol> capabilities(const cms::Destination *destination);
		static internal::annotation::DESTINATION_TYPE jmsDestinationType(const cms::Destination* destination);
		static std::string address(const cms::Destination *destination);
		static cms::Destination* createCMSDestination(const internal::Destination& destination);
		static cms::Destination* createCMSDestination(const std::string& address ,const proton::target& target);
		static cms::Destination* createCMSDestination(const std::string& address ,const proton::source& target);
		static cms::Destination* createCMSDestination(const proton::message& message);
		static cms::Destination* createCMSReplyTo(const proton::message& message);

	};

	class ValueTypeConverter
	{
		public:
		static cms::Message::ValueType amqpToCms(proton::type_id t_id);
	};

	class MessageConverter
	{
	public:
		static cms::Message* fromProtonMessage(const proton::message& message);
	//	static proton::message fromCmsMessage(const cms::Message* message);

	//	private:
	//	static proton::message fromCmsMessage(const cms::TextMessage *destination);
	//	static proton::message fromCmsMessage(const cms::BytesMessage *destination);
	};

}