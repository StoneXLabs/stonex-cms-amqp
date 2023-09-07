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

#include "CMSConnectionFactory.h"

#include "CMSConnection.h"
#include "ConnectionContext.h"
#include "ConnectionFactoryImpl.h"
#include "ProtonCppLibrary.h"
#include <fmt/format.h>

cms::amqp::CMSConnectionFactory::CMSConnectionFactory(const std::string& brokerURI, const std::string& user, const std::string& password)
	:mPimpl(std::make_shared<ConnectionFactoryImpl>(brokerURI, user, password)),
	mContext(std::make_shared<FactoryContext>(brokerURI, user, password, 0, ProtonCppLibrary::getContainer()))
{
}

::cms::Connection* cms::amqp::CMSConnectionFactory::createConnection()
{
	try
	{
		return new CMSConnection(mContext, mLogSink);
	}
	catch (const ::cms::CMSException& ex)
	{
		if (mExceptionListener)
			mExceptionListener->onException(ex);
		throw;
	}
}

::cms::Connection* cms::amqp::CMSConnectionFactory::createConnection(const std::string& username, const std::string& password)
{
	try
	{
		info("com.stonex.cms.ConnectionFactory", fmt::format("creating connection user {}",username));
		return new CMSConnection(mContext, username, password, mLogSink);
	}
	catch (const ::cms::CMSException& ex)
	{
		if (mExceptionListener)
			mExceptionListener->onException(ex);
		error("com.stonex.cms.ConnectionFactory", ex.what());
		throw;
	}
}

::cms::Connection* cms::amqp::CMSConnectionFactory::createConnection(const std::string& username, const std::string& password, const std::string& clientId)
{
	try
	{

		info("com.stonex.cms.ConnectionFactory", fmt::format("creating connection user {} clientId {}", username, clientId));
		mContext->updateCotainerId(clientId);
		return new CMSConnection(mContext, username, password, mLogSink);
	}
	catch (const ::cms::CMSException& ex)
	{
		if (mExceptionListener)
			mExceptionListener->onException(ex);
		error("com.stonex.cms.ConnectionFactory", ex.what());
		throw;
	}
}

void cms::amqp::CMSConnectionFactory::setExceptionListener(::cms::ExceptionListener* listener)
{
	debug("com.stonex.cms.ConnectionFactory", fmt::format("set exception listener: {}", (void*)listener));
	mExceptionListener = listener;
}

cms::ExceptionListener* cms::amqp::CMSConnectionFactory::getExceptionListener() const
{
	return mExceptionListener;
}

void cms::amqp::CMSConnectionFactory::setMessageTransformer(::cms::MessageTransformer* transformer)
{
	debug("com.stonex.cms.ConnectionFactory", fmt::format("set message transformer: {}", (void*)transformer));
	mMessageTransformer = transformer;
}

cms::MessageTransformer* cms::amqp::CMSConnectionFactory::getMessageTransformer() const
{
	return mMessageTransformer;
}



::cms::ConnectionFactory* cms::amqp::CMSConnectionFactory::createCMSConnectionFactory(const std::string& brokerURI)
{
	return new CMSConnectionFactory(brokerURI);
}

std::shared_ptr<cms::amqp::FactoryContext> cms::amqp::CMSConnectionFactory::context() const
{
	return mContext;
};