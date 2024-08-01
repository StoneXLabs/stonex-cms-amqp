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
#include <LoggerFactory/LoggerFactory.h>

#include "SessionImpl.h"

#include <fmt/format.h>

 cms::amqp::ConnectionImpl::ConnectionImpl(const ConnectionContext& context)
	:mLogger(LoggerFactory::getInstance().create("com.stonex.cms.amqp.CMSConnectionFactory")),
	 mEXHandler(mLogger),
	 mContext(context)
{	
//	mLogger->log(SEVERITY::LOG_INFO, fmt::format("request amqp connection: {} failover {} user {} clientId {}", mContext.broker(), mContext.failoverAddresses(), mContext.user(), mConnectionId));

//	mEXHandler.SynchronizeCall(std::bind(&ConnectionContext::requestBrokerConnection, &mContext, std::placeholders::_1), *this);
}

// cms::amqp::ConnectionImpl::ConnectionImpl(const std::string& id, const FactoryContext& context)
//	 :mLogger(LoggerFactory::getInstance().create("com.stonex.cms.CMSConnectionFactory")),
//	 mEXHandler(mLogger),
//	 mConnectionId{id},
//	 mContext{context}
//{
//	mLogger->log(SEVERITY::LOG_INFO, fmt::format("request amqp connection: {} failover {} user {} clientId {}", mContext.broker(),mContext.failoverAddresses(), mContext.user(), mConnectionId));
//	mEXHandler.SynchronizeCall(std::bind(&FactoryContext::requestBrokerConnection, &mContext, std::placeholders::_1), *this);
//}



 cms::amqp::ConnectionImpl::~ConnectionImpl()
{
	close();
}

void  cms::amqp::ConnectionImpl::close()
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} closing connection: {} ", __func__, mContext.mPrimaryUrl));
	if(mState == ClientState::STARTED || mState == ClientState::STOPPED)
		mEXHandler.SynchronizeCall(std::bind(&ConnectionImpl::syncClose,this));

}
	
std::string  cms::amqp::ConnectionImpl::getClientID() const
{
	return mContext.mConnection->container_id();
}

void  cms::amqp::ConnectionImpl::setClientID(const std::string& clientID)
{
	
}



void  cms::amqp::ConnectionImpl::on_transport_open(proton::transport& transport)
{
#if _DEBUG
	if (auto err = transport.error(); err.empty())
		mLogger->log(SEVERITY::LOG_TRACE, fmt::format("{}", __func__));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));
#endif
}

void  cms::amqp::ConnectionImpl::on_transport_close(proton::transport& transport)
{
	if (auto err = transport.error(); err.empty())
		mLogger->log(SEVERITY::LOG_TRACE, fmt::format("{}", __func__));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));

	setState(ClientState::CLOSED);
	mEXHandler.onResourceUninitialized(transport.error());
}

void  cms::amqp::ConnectionImpl::on_transport_error(proton::transport& transport)
{

	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, transport.error().what()));

	if (mExceptionListener)
		mExceptionListener->onException(transport.error().what());
}

void  cms::amqp::ConnectionImpl::on_connection_open(proton::connection& connection)
{
	if (auto err = connection.error(); err.empty())
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} auto reconnected : {}", __func__, connection.reconnected()));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} auto reconnected : {} {}", __func__, connection.reconnected(), err.what()));

	mContext.mConnection  = std::make_shared<proton::connection>(connection);

	//connection is created in stopped state
	setState(ClientState::STOPPED);

	mEXHandler.onResourceInitialized();

}
void  cms::amqp::ConnectionImpl::on_connection_close(proton::connection& connection)
{
	if(auto err = connection.error(); err.empty())
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{}", __func__));
	else
		mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, err.what()));

}

void  cms::amqp::ConnectionImpl::on_connection_error(proton::connection& connection)
{
	mLogger->log(SEVERITY::LOG_ERROR, fmt::format("{} {}", __func__, connection.error().what()));
	if (mExceptionListener)
		mExceptionListener->onException(connection.error().what());
}

bool  cms::amqp::ConnectionImpl::syncClose()
{
	bool ok{ false };
	if (mContext.mConnection && mContext.mConnection->closed())
	{
		ok = mContext.mConnection->work_queue().add([=] {mContext.mConnection->close(); });
	}

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

cms::amqp::ClientState cms::amqp::ConnectionImpl::getState() const
{
	return mState;
}

void cms::amqp::ConnectionImpl::setState(ClientState state)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} last state {} current state {}", __func__, mState, state));
	mState = state;
}
