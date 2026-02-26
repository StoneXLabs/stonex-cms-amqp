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

#include <mutex>
#include <iostream>

#include <proton/connection_options.hpp>
#include <proton/container.hpp>
#include <proton/transport.hpp>
#include <proton/work_queue.hpp>

#include "ProtonCppLibrary.h"
#include "Session.h"

#include "ConnectionMetadata.h"

#include <fmt/format.h>
#include <iostream>

stonex::amqp::Connection::Connection(const std::string& primaryUrl, proton::connection_options& connectionOptions)
:mPrimaryUrl{primaryUrl},
mConnectionOptions{std::move(connectionOptions.handler(*this))}
{
}

stonex::amqp::Connection::~Connection()
{
  close();
}


void stonex::amqp::Connection::start()
{
	std::unique_lock<std::mutex> lk(mMutex);
	ProtonCppLibrary::getContainer()->connect(mPrimaryUrl, mConnectionOptions);
	mCv.wait(lk, [this]() { return mWorkQueue; });
}

void stonex::amqp::Connection::stop()
{
}

void stonex::amqp::Connection::close()
{
	mWorkQueue->add([this]() { mConnection.close(); });
}

const cms::ConnectionMetaData* stonex::amqp::Connection::getMetaData() const
{
	return new stonex::amqp::ConnectionMetaData();
}

cms::Session* stonex::amqp::Connection::createSession()
{
	return new stonex::amqp::Session(mConnection, Session::AcknowledgeMode::AUTO_ACKNOWLEDGE);
}

cms::Session* stonex::amqp::Connection::createSession(Session::AcknowledgeMode ackMode)
{
	return new stonex::amqp::Session(mConnection, ackMode);
}


std::string  stonex::amqp::Connection::getClientID() const
{
	return {};
}

void  stonex::amqp::Connection::setClientID(const std::string& clientID)
{
	
}

cms::ExceptionListener*  stonex::amqp::Connection::getExceptionListener() const
{
	return nullptr;
}

void  stonex::amqp::Connection::setExceptionListener(cms::ExceptionListener* listener)
{
	mExceptionListener = listener;
}

void  stonex::amqp::Connection::setMessageTransformer(cms::MessageTransformer* transformer)
{

}

cms::MessageTransformer*  stonex::amqp::Connection::getMessageTransformer() const
{
	return nullptr;
}

void  stonex::amqp::Connection::on_transport_open(proton::transport& transport)
{
}

void  stonex::amqp::Connection::on_transport_close(proton::transport& transport)
{
	std::cout << transport.error() << std::endl;
}

void  stonex::amqp::Connection::on_transport_error(proton::transport& transport)
{
}

void  stonex::amqp::Connection::on_connection_open(proton::connection& connection)
{
	std::unique_lock<std::mutex> lk(mMutex);
	auto x = connection.active();
	mConnection = connection;
	mWorkQueue = &connection.work_queue();
	mCv.notify_all();
}
void  stonex::amqp::Connection::on_connection_close(proton::connection& connection)
{
}

void  stonex::amqp::Connection::on_connection_error(proton::connection& connection)
{
}