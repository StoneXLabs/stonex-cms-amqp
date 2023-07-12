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

#include "ConnectionImpl.h"

#include <mutex>
#include <iostream>

#include <proton/connection_options.hpp>
#include <proton/container.hpp>
#include <proton/transport.hpp>
#include <proton/work_queue.hpp>

#include "SessionImpl.h"

 cms::amqp::ConnectionImpl::ConnectionImpl(const FactoryContext& context)
	:mContext{ context }
{
	mEXHandler.SynchronizeCall(std::bind(&FactoryContext::requestBrokerConnection, &mContext, std::placeholders::_1), *this);
}

 cms::amqp::ConnectionImpl::ConnectionImpl(const std::string& id, const FactoryContext& context)
	 : mConnectionId{id},
	 mContext{context}
{
	mEXHandler.SynchronizeCall(std::bind(&FactoryContext::requestBrokerConnection, &mContext, std::placeholders::_1), *this);
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
//	std::cout << __FUNCTION__ << " " << transport.error().what() << std::endl;
}

void  cms::amqp::ConnectionImpl::on_transport_close(proton::transport& transport)
{
	mState = ClientState::CLOSED;
//	std::cout << __FUNCTION__ << " " << transport.error().what() << std::endl;
	mEXHandler.onResourceUninitialized(transport.error());
}

void  cms::amqp::ConnectionImpl::on_transport_error(proton::transport& transport)
{
//	std::cout << __FUNCTION__<<" "<<transport.error().what() << std::endl;
	if (mExceptionListener)
		mExceptionListener->onException(transport.error().what());
}

void  cms::amqp::ConnectionImpl::on_connection_open(proton::connection& connection)
{
//	std::cout << __FUNCTION__ << " " << connection.error().what() << std::endl;

	mConnection  = std::make_shared<proton::connection>(connection);
	mState = ClientState::STARTED;
	mEXHandler.onResourceInitialized();

}
void  cms::amqp::ConnectionImpl::on_connection_close(proton::connection& connection)
{
//	std::cout << __FUNCTION__ << std::endl;
}

void  cms::amqp::ConnectionImpl::on_connection_error(proton::connection& connection)
{
//	std::cout << __FUNCTION__ << std::endl;
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
