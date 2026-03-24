#pragma once

#include <proton/type_id.hpp>
#include <proton/symbol.hpp>
#include <proton/source.hpp>
#include <proton/target.hpp>
#include <proton/message.hpp>

#include <cms/Message.h>

#include <variant>
#include <map>

#include "Destination.h"

#include "stonex-cms-amqp-lib-defines.h"

AMQP_DEFINES

namespace internal {

	namespace capability
	{
		constexpr const char *QUEUE = "queue";
		constexpr const char *TOPIC = "topic";
		constexpr const char *TEMPORARY_QUEUE = "temporary-queue";
		constexpr const char *TEMPORARY_TOPIC = "temporary-topic";
		constexpr const char *DELETE_ON_CLOSE = "delete-on-close";
	}

	namespace properties
	{
		constexpr const char* JMSX_GROUP_ID = "JMSXGroupID";
		constexpr const char* CMSX_GROUP_ID = "CMSXGroupID";
		constexpr const char* JMSX_MESSAGE_ID = "JMSMessageId";
		constexpr const char* AMQP_MESSAGE_ID = "message-id-string";
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
		static internal::annotation::DESTINATION_TYPE jmsDestinationType(const cms::Destination::DestinationType& destinationType);
		static std::string address(const cms::Destination *destination);
		static cms::Destination* createCMSDestination(const internal::Destination& destination);
		static cms::Destination* createCMSDestination(const std::string& address ,const proton::target& target);
		static cms::Destination* createCMSDestination(const std::string& address ,const proton::source& target);
		static cms::Destination* createCMSDestination(const proton::message& message);
		static cms::Destination* createCMSReplyTo(const proton::message& message);
		static internal::Destination* createProtonDestination(const cms::Destination* destination);

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
	
	private:
		static void transformProperties(const proton::message& message, cms::Message* cmsMessage);
	};

	
	class MessageProperties
	{
		using PropertyValue = std::variant<bool, unsigned char, double, float, int, long long, short, std::string>;

	public:
		MessageProperties() = default;
		MessageProperties(const MessageProperties& other);
		
		void clear();
		std::vector<std::string> getNames() const;
		cms::Message::ValueType getType(const std::string& name) const;
		bool exists(const std::string& name) const;
		void set(const std::string& name, PropertyValue value);
		

		template<typename T>
		T get(const std::string& name) const
		{
			auto it = mProperties.find(name);
			if (it == mProperties.end())
				throw std::out_of_range("Property not found");

			return std::visit([](auto&& arg) -> T {
				if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, T>)
					return arg;
				else
					throw std::bad_variant_access();
			}, it->second);
		}

		PropertyValue get(const std::string& name) const
		{
			auto it = mProperties.find(name);
			if (it == mProperties.end())
				throw std::out_of_range("Property not found");

			return it->second;
		}

		std::string correlationId;
		int deliveryMode{cms::Message::DEFAULT_DELIVERY_MODE};
		std::unique_ptr<cms::Destination> destination;
		long long expiration{0};
		std::string messageId;
		int priority{cms::Message::DEFAULT_MSG_PRIORITY};
		bool redelivered{false};
		std::unique_ptr<cms::Destination> replyTo;
		long long timeStamp{0};
		std::string type;

	private:
		std::map<std::string, PropertyValue> mProperties;
	};
}

AMQP_DEFINES_CLOSE