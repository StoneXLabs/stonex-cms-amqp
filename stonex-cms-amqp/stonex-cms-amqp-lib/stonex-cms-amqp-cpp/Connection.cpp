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

#include "Connection.h"


#include <proton/connection_options.hpp>
#include <proton/container.hpp>
#include <proton/transport.hpp>
#include <proton/work_queue.hpp>

#include "ProtonCppLibrary.h"
#include "Session.h"

#include "ConnectionMetadata.h"

#include <mutex>
#include <format>

stonex::amqp::Connection::~Connection()
{
  close();
}


void stonex::amqp::Connection::start()
{
	std::unique_lock<std::mutex> lk(mMutex);
	mCv.wait(lk, [this]() { return mWorkQueue; });
}

void stonex::amqp::Connection::stop()
{
	//AMQP connection has no Stopabble interface
	//TO DO implement Stopable behaviour like in CMS
	close();
}

void stonex::amqp::Connection::close()
{
	mWorkQueue->add([this]() { mConnection.close(); });
	std::unique_lock lk(mMutex);
	mCv.wait(lk, [this]() { return !mWorkQueue; });
}

const cms::ConnectionMetaData* stonex::amqp::Connection::getMetaData() const
{
	return new stonex::amqp::ConnectionMetaData();
}

cms::Session* stonex::amqp::Connection::createSession()
{
	std::unique_lock<std::mutex> lk(mMutex);
	mCv.wait(lk, [this]() { return mWorkQueue; });
	return new stonex::amqp::Session(mConnection, Session::AcknowledgeMode::AUTO_ACKNOWLEDGE);
}

cms::Session* stonex::amqp::Connection::createSession(Session::AcknowledgeMode ackMode)
{
	std::unique_lock<std::mutex> lk(mMutex);
	mCv.wait(lk, [this]() { return mWorkQueue; });
	return new stonex::amqp::Session(mConnection, ackMode);
}


std::string  stonex::amqp::Connection::getClientID() const
{
	//TO DO implement clientID management
	return {};
}

void  stonex::amqp::Connection::setClientID(const std::string& clientID)
{
	//TO DO implement clientID management
}

cms::ExceptionListener*  stonex::amqp::Connection::getExceptionListener() const
{
	return mExceptionListener;
}

void  stonex::amqp::Connection::setExceptionListener(cms::ExceptionListener* listener)
{
	mExceptionListener = listener;
}

void  stonex::amqp::Connection::setMessageTransformer(cms::MessageTransformer* transformer)
{
	//TO DO implement message transformer management
}

cms::MessageTransformer*  stonex::amqp::Connection::getMessageTransformer() const
{
	return nullptr;
}

void  stonex::amqp::Connection::on_transport_open(proton::transport& transport)
{
	LOG4CXX_INFO(mLogger, std::format("Transport open {}", transport.error().empty() ? "" : "error " + transport.error().what()));
}

void  stonex::amqp::Connection::on_transport_close(proton::transport& transport)
{
	std::unique_lock lk(mMutex);
	mWorkQueue = nullptr;
	LOG4CXX_INFO(mLogger, std::format("Transport close"));
	mCv.notify_one();
}

void  stonex::amqp::Connection::on_transport_error(proton::transport& transport)
{
	LOG4CXX_INFO(mLogger, std::format("Transport error {}", transport.error().what()));
}

void  stonex::amqp::Connection::on_connection_open(proton::connection& connection)
{
	std::unique_lock<std::mutex> lk(mMutex);
	mConnection = connection;
	mWorkQueue = &connection.work_queue();
	LOG4CXX_INFO(mLogger, "Connection open");
	mCv.notify_all();
}
void  stonex::amqp::Connection::on_connection_close(proton::connection& connection)
{
	LOG4CXX_INFO(mLogger, "Connection closed");
}

void  stonex::amqp::Connection::on_connection_error(proton::connection& connection)
{
	LOG4CXX_ERROR(mLogger, std::format("Connection error {}", connection.error().what()));
}