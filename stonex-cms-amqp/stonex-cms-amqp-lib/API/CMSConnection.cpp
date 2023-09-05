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

#include "CMSConnection.h"
#include "CMSSession.h"

#include "ConnectionImpl.h"
#include "SessionImpl.h"

#include "ConnectionContext.h"
#include "ConnectionMetadataImpl.h"

#include <fmt/format.h>

cms::amqp::CMSConnection::CMSConnection(std::shared_ptr<FactoryContext> context, std::shared_ptr<StonexLogger> logger)
	:mPimpl{ std::make_shared<ConnectionImpl>(*context, logger)}
{
}

cms::amqp::CMSConnection::CMSConnection(std::shared_ptr<FactoryContext> context, const std::string& username, const std::string& password, std::shared_ptr<StonexLogger> logger)
	: mPimpl{ std::make_shared<ConnectionImpl>((*context).updateUser(username).updatePassword(password), logger) }
{
}

cms::amqp::CMSConnection::CMSConnection(std::shared_ptr<FactoryContext> context, const std::string& username, const std::string& password, const std::string& clientId, std::shared_ptr<StonexLogger> logger)
	: mPimpl{ std::make_shared<ConnectionImpl>(clientId, (*context).updateUser(username).updatePassword(password),logger)}
{
}

void cms::amqp::CMSConnection::close()
{
	info("com.stonex.cms.CMSConnection", "close");
	mPimpl->close();
}

void cms::amqp::CMSConnection::start()
{
	info("com.stonex.cms.CMSConnection", "start");
	mPimpl->start();
}

void cms::amqp::CMSConnection::stop()
{
	info("com.stonex.cms.CMSConnection", "stop");
	mPimpl->stop();
}

const ::cms::ConnectionMetaData* cms::amqp::CMSConnection::getMetaData() const
{
	return new ConnectionMetadataImpl();
}

cms::Session* cms::amqp::CMSConnection::createSession()
{
	info("com.stonex.cms.CMSConnection", "createSession");
	return new CMSSession(ConnectionContext(mPimpl->connection()), cms::Session::AcknowledgeMode::AUTO_ACKNOWLEDGE,mLogSink);
}

cms::Session* cms::amqp::CMSConnection::createSession(::cms::Session::AcknowledgeMode ackMode)
{

	info("com.stonex.cms.CMSConnection", "close");
	return new CMSSession(ConnectionContext(mPimpl->connection()), ackMode, mLogSink);
}

std::string cms::amqp::CMSConnection::getClientID() const
{
	throw ::cms::CMSException("illegal use - not implemented");
	return {};// mPimpl->getClientID();
}

void cms::amqp::CMSConnection::setClientID(const std::string& clientID)
{
	info("com.stonex.cms.CMSConnection", "close");
	mPimpl->setClientID(clientID);

}

cms::ExceptionListener* cms::amqp::CMSConnection::getExceptionListener() const
{
	return mPimpl->getExceptionListener();
}

void cms::amqp::CMSConnection::setExceptionListener(::cms::ExceptionListener* listener)
{
	info("com.stonex.cms.CMSConnection", "close");
	mPimpl->setExceptionListener(listener);
}

void cms::amqp::CMSConnection::setMessageTransformer(::cms::MessageTransformer* transformer)
{
}

cms::MessageTransformer* cms::amqp::CMSConnection::getMessageTransformer() const
{
	return mPimpl->getMessageTransformer();
}

std::shared_ptr <cms::amqp::ConnectionContext> cms::amqp::CMSConnection::connectionContext() const
{
	info("com.stonex.cms.CMSConnection", "close");
	return std::make_shared<cms::amqp::ConnectionContext>(mPimpl->connection());
}

void cms::amqp::CMSConnection::setLogger(std::shared_ptr<StonexLogger> sink)
{
	StonexLogSource::setLogger(sink);
	mPimpl->setLogger(sink);
};



