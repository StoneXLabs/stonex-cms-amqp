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

#include "ConnectionImpl.h"

#include <mutex>
#include <iostream>

#include <proton/connection_options.hpp>
#include <proton/container.hpp>
#include <proton/transport.hpp>
#include <proton/work_queue.hpp>

#include "SessionImpl.h"

#include <fmt/format.h>

 cms::amqp::ConnectionImpl::ConnectionImpl(const FactoryContext& context, std::shared_ptr<StonexLogger> logger)
	:mContext{ context }
{	
	setLogger(logger);
	mEXHandler.SynchronizeCall(std::bind(&FactoryContext::requestBrokerConnection, &mContext, std::placeholders::_1), *this);
	setLogger(nullptr);
}

 cms::amqp::ConnectionImpl::ConnectionImpl(const std::string& id, const FactoryContext& context, std::shared_ptr<StonexLogger> logger)
	 : mConnectionId{id},
	 mContext{context}
{
	setLogger(logger);
	mEXHandler.SynchronizeCall(std::bind(&FactoryContext::requestBrokerConnection, &mContext, std::placeholders::_1), *this);
	setLogger(nullptr);
}



 cms::amqp::ConnectionImpl::~ConnectionImpl()
{
	close();
}

void  cms::amqp::ConnectionImpl::close()
{
	if(mState == ClientState::STARTED)
		mEXHandler.SynchronizeCall(std::bind(&ConnectionImpl::syncClose,this));

}
	

void  cms::amqp::ConnectionImpl::start()
{
	std::cout << "connection status : " << std::boolalpha << !mConnection->closed() << " starting connection " << &mConnection << std::endl;
	mEXHandler.SynchronizeCall(std::bind(&FactoryContext::requestBrokerConnection, &mContext, std::placeholders::_1), *this);

}

void  cms::amqp::ConnectionImpl::stop()
{
	//mEXHandler.SynchronizeCall(std::bind(&ConnectionContext::requestBrokerConnection, &mContext, std::placeholders::_1), *this);
}

std::string  cms::amqp::ConnectionImpl::getClientID() const
{
	return mConnection->container_id();
}

void  cms::amqp::ConnectionImpl::setClientID(const std::string& clientID)
{
	
}



void  cms::amqp::ConnectionImpl::on_transport_open(proton::transport& transport)
{
	trace("connection implementation", fmt::format("{} {}", __func__, transport.error().what()));
}

void  cms::amqp::ConnectionImpl::on_transport_close(proton::transport& transport)
{
	trace("connection implementation", fmt::format("{} {}", __func__, transport.error().what()));
	mState = ClientState::CLOSED;
	mEXHandler.onResourceUninitialized(transport.error());
}

void  cms::amqp::ConnectionImpl::on_transport_error(proton::transport& transport)
{
	trace("connection implementation", fmt::format("{} {}", __func__, transport.error().what()));
	if (mExceptionListener)
		mExceptionListener->onException(transport.error().what());
}

void  cms::amqp::ConnectionImpl::on_connection_open(proton::connection& connection)
{
	trace("connection implementation", fmt::format("{} {}", __func__, connection.error().what()));
	mConnection  = std::make_shared<proton::connection>(connection);
	mState = ClientState::STARTED;
	mEXHandler.onResourceInitialized();

}
void  cms::amqp::ConnectionImpl::on_connection_close(proton::connection& connection)
{
	trace("connection implementation", fmt::format("{} {}", __func__, connection.error().what()));
}

void  cms::amqp::ConnectionImpl::on_connection_error(proton::connection& connection)
{
	trace("connection implementation", fmt::format("{} {}", __func__, connection.error().what()));
	if (mExceptionListener)
		mExceptionListener->onException(connection.error().what());
}

bool  cms::amqp::ConnectionImpl::syncClose()
{
	bool ok{ false };
	if (mConnection && !mConnection->closed())
	{
		ok = mConnection->work_queue().add([=] {mConnection->close(); });
	}

	return ok;
}

bool  cms::amqp::ConnectionImpl::syncStart()
{
	bool ok{ false };


	return ok;
}

bool  cms::amqp::ConnectionImpl::syncStop()
{
	bool ok{ false };


	return ok;
}

cms::ExceptionListener*  cms::amqp::ConnectionImpl::getExceptionListener() const
{
	return mExceptionListener;
}

void  cms::amqp::ConnectionImpl::setExceptionListener(::cms::ExceptionListener* listener)
{
	mExceptionListener = listener;
}

void  cms::amqp::ConnectionImpl::setMessageTransformer(::cms::MessageTransformer* transformer)
{

}

cms::MessageTransformer*  cms::amqp::ConnectionImpl::getMessageTransformer() const
{
	return nullptr;
}
