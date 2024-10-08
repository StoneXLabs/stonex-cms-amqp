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
#include <algorithm>
#include <vector>

cms::amqp::CMSConnection::CMSConnection(std::shared_ptr<ConnectionImpl> impl)
	:mPimpl(impl),
	mLogger(LoggerFactory::getInstance().create("com.stonex.cms.CMSConnection"))
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("created connection"));
}


void cms::amqp::CMSConnection::close()
{
	mLogger->log(SEVERITY::LOG_INFO, "close");
	mPimpl->close();
}

void cms::amqp::CMSConnection::start()
{
	mPimpl->start();
}

void cms::amqp::CMSConnection::stop()
{
	mPimpl->stop();
}

const cms::ConnectionMetaData* cms::amqp::CMSConnection::getMetaData() const
{
	return new ConnectionMetadataImpl();
}

cms::Session* cms::amqp::CMSConnection::createSession()
{
	return createSession(cms::Session::AcknowledgeMode::AUTO_ACKNOWLEDGE);
}

cms::Session* cms::amqp::CMSConnection::createSession(cms::Session::AcknowledgeMode ackMode)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("createSession ACK_MODE {}", ackMode));
	config::SessionContext context(mPimpl->mContext, ackMode);
	std::shared_ptr<SessionImpl> session = std::make_shared<SessionImpl>(context);
	mPimpl->addSession(session);
	return new CMSSession(session);
}

std::string cms::amqp::CMSConnection::getClientID() const
{
	throw cms::CMSException("illegal use - not implemented");
	return {};// mPimpl->getClientID();
}

void cms::amqp::CMSConnection::setClientID(const std::string& clientID)
{
	mLogger->log(SEVERITY::LOG_INFO, fmt::format("set clientId {}", clientID));
	mPimpl->setClientID(clientID);

}

cms::ExceptionListener* cms::amqp::CMSConnection::getExceptionListener() const
{
	return mPimpl->getExceptionListener();
}

void cms::amqp::CMSConnection::setExceptionListener(cms::ExceptionListener* listener)
{
	mLogger->log(SEVERITY::LOG_INFO, "set Exception Listener");
	mPimpl->setExceptionListener(listener);
}

void cms::amqp::CMSConnection::setMessageTransformer(cms::MessageTransformer* transformer)
{
}

cms::MessageTransformer* cms::amqp::CMSConnection::getMessageTransformer() const
{
	return mPimpl->getMessageTransformer();
}




