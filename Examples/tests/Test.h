#pragma once

#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <cms/TextMessage.h>

#include <cms/Destination.h>
#include <LoggerFactory/LoggerFactory.h>
#include "../common/ParameterParser .h"

void devTest();

void createConnection(const std::string& user, const std::string& password, const std::string& broker, cms::ExceptionListener* exListener = nullptr);
void createConnection2(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener = nullptr);
void createAddress(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener = nullptr);
void createConsumer(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener = nullptr);
void createConsumerWithSelector(const std::string& user, const std::string& password, const std::string& broker, const std::string& address, cms::ExceptionListener* exListener = nullptr);
void sendAndReceive(const std::string& producer_user, const std::string& producer_password, const std::string& consumer_user, const std::string& consumer_password, const std::string& broker, const std::string& prodcer_address, const std::string& consumer_address, cms::Destination::DestinationType destType = cms::Destination::QUEUE, cms::ExceptionListener* exListener = nullptr);