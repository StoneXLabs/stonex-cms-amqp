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
#include "ProtonCppLibrary.h"

#include "SessionImpl.h"

#include <fmt/format.h>

 cms::amqp::ConnectionImpl::ConnectionImpl(const config::ConnectionContext& context)
	:mLogger(LoggerFactory::getInstance().create("com.stonex.cms.amqp.ConnectionImpl")),
	 mContext(context)
{
	 mContext.setLogger(mLogger);
	 std::unique_lock lk(mMutex);
	 proton::connection_options connectionOptions = mContext.config();
	 connectionOptions.handler(*this);
	 ProtonCppLibrary::getContainer()->connect(mContext.mPrimaryUrl, connectionOptions);
	 mCv.wait(lk, [this]() {return !mContext.checkState(ClientState::UNNINITIALIZED); });
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("request amqp connection: {} failover {} user {} clientId {}", mContext.mPrimaryUrl, "mContext.mFailoverUrls", mContext.mUser, mConnectionId));
}

cms::amqp::ConnectionImpl::~ConnectionImpl()
{
  close();
}


void cms::amqp::ConnectionImpl::start()
{
	if (mContext.checkState(ClientState::CLOSED))
	{
		throw cms::IllegalStateException(fmt::format("{} - Connection has already been closed!", __func__));
	}

	for (auto& session : mSessions)
	{
		if (auto sessionPtr = session.lock())
		{
			sessionPtr->start();
		}
	}
	mContext.setState(ClientState::STARTED);
}

void cms::amqp::ConnectionImpl::stop()
{
	for (auto& session : mSessions)
	{
		if (auto sessionPtr = session.lock())
		{
			sessionPtr->stop();
		}
	}
	mContext.setState(ClientState::STOPPED);
}

void cms::amqp::ConnectionImpl::close()
{
	if (mContext.checkState(ClientState::CLOSED))
	{
		mLogger->log(SEVERITY::LOG_WARNING, fmt::format("{} closing connection: {} connection allready closed", __func__, mContext.mPrimaryUrl));
	}
	else
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} closing connection: {} ", __func__, mContext.mPrimaryUrl));
	}

	if (auto queue = mContext.mWorkQueue; queue != nullptr)
	{
		std::unique_lock lk(mMutex);

		for (auto& session : mSessions)
		{
			if (auto sessionPtr = session.lock())
			{
				sessionPtr->close();
			}
		}

		queue->add([this]() {mContext.mConnection.close(); });
		mCv.wait(lk, [this]() {return mContext.checkState(ClientState::CLOSED); });
	}
}
	
std::string  cms::amqp::ConnectionImpl::getClientID() const
{
	return {};// mContext.mConnection->container_id();
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

	mContext.setState(ClientState::CLOSED);
	mContext.mWorkQueue = nullptr;
	mCv.notify_one();
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

	mContext.mConnection = connection;
	mContext.mWorkQueue  = &mContext.mConnection.work_queue();

	//connection is created in stopped state
	mContext.setState(ClientState::STOPPED);
	mCv.notify_one();

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

void cms::amqp::ConnectionImpl::addSession(std::shared_ptr<SessionImpl> session)
{
	mSessions.push_back(session);
}