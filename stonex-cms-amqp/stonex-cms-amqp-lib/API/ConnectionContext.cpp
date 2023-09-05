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

#include "ConnectionContext.h"

#include <regex>

#include <proton/container.hpp>
#include <proton/connection_options.hpp>
#include <proton/reconnect_options.hpp>

#include <cms/CMSException.h>

#include <fmt/format.h>

#include <sstream>

cms::amqp::FactoryContext::FactoryContext(const std::string& url, const std::string& user, const std::string& password, const std::vector<std::string>& failover_urls, int reconnect_attempts, std::shared_ptr<proton::container> container)
	: mBroker{ url }, mUser{ user }, mPassword{ password }, mFailoverAddresses{ failover_urls }, mReconnectAttempts{ reconnect_attempts }, mContainer{ container }
{
}

cms::amqp::FactoryContext::FactoryContext(const std::string& url, const std::string& user, const std::string& password, int reconnect_attempts, std::shared_ptr<proton::container> container)
	: mUser{ user }, mPassword{ password }, mReconnectAttempts{reconnect_attempts}, mContainer{ container }
{
	URIParser parser;
	mParameters = parser.parseURI(url);
	mBroker = mParameters.url_vec[0];
	if (mParameters.url_vec.size() > 0)
	{
		//change layout
		mFailoverAddresses.insert(std::end(mFailoverAddresses),
		std::make_move_iterator(std::next(mParameters.url_vec.begin())),
		std::make_move_iterator(std::end(mParameters.url_vec)));
	}
}

cms::amqp::FactoryContext& cms::amqp::FactoryContext::updateUser(const std::string& user)
{

#if _DEBUG
	trace("com.stonex.cms.FactoryContext", fmt::format("set update user: {}", user));
#endif
	mUser = user;
	return *this;
}

cms::amqp::FactoryContext& cms::amqp::FactoryContext::updatePassword(const std::string& password)
{
#if _DEBUG
	trace("com.stonex.cms.FactoryContext", "set update password");
#endif
	mPassword = password;
	return *this;
}

cms::amqp::FactoryContext& cms::amqp::FactoryContext::updateCotainerId(const std::string& connectionId)
{
#if _DEBUG
	trace("com.stonex.cms.FactoryContext", fmt::format("set connection id: {}", connectionId));
#endif
	mConnectionId = connectionId;
	return *this;
}

void cms::amqp::FactoryContext::requestBrokerConnection(proton::messaging_handler *handler)
{
	proton::connection_options co;
	co.user(mUser);
	co.password(mPassword);
	co.sasl_allow_insecure_mechs(true);
	co.sasl_allowed_mechs("PLAIN");
	co.handler(*handler);

	if (!mConnectionId.empty())
		co.container_id(mConnectionId);
	
// no idea how to map
//	co.idle_timeout(proton::duration(1 * 2));

	//from URL
	
	
	proton::reconnect_options rco;
	
	rco.failover_urls(mFailoverAddresses);

	//fromURL
	rco.delay(proton::duration(mParameters.failoverOptrions.initialReconnectDelay));
	rco.max_delay(proton::duration(mParameters.failoverOptrions.maxReconnectDelay));
	rco.max_attempts(mParameters.failoverOptrions.maxReconnectAttempts);
	
	
	co.reconnect(rco);
	co.desired_capabilities({ "ANONYMOUS-RELAY" });
#if _DEBUG
	trace("com.stonex.cms.FactoryContext", fmt::format("request amqp connection: {} failover {}", mBroker, fmt::join(mFailoverAddresses,", ")));
#endif
	mContainer->connect(mBroker, co);

}

std::shared_ptr<proton::container> cms::amqp::FactoryContext::container() 
{
	return mContainer;
}

std::string cms::amqp::FactoryContext::broker() const
{
	return mBroker;
}

std::string cms::amqp::FactoryContext::failoverAddresses() const
{
	std::ostringstream ss;
	std::copy(std::cbegin(mFailoverAddresses), std::cend(mFailoverAddresses), std::ostream_iterator<std::string>(ss, ","));
	return ss.str();
}

std::string cms::amqp::FactoryContext::user() const
{
	return mUser;
}

int cms::amqp::FactoryContext::reconnectAttempts() const
{
	return mReconnectAttempts;
}

void cms::amqp::ConnectionOptions::FailoverTransportOptions::setParametersFromString(const std::string& parameter)
{
	std::size_t  pos = parameter.find("=");
	if (pos == std::string::npos)
		return;

	std::string param = parameter.substr(pos);

	if (param == "initialReconnectDelay")
	{
		mInitialReconnectDelay = std::stoi(parameter.substr(pos + 1));
	}
	else if (param == "maxReconnectDelay")
	{
		mMaxReconnectDelay = std::stoi(parameter.substr(pos + 1));
	}
	else if(param == "useExpotentialBackOff")
	{
		mUseExpotentialBackOff = param == "true" ? true : false;
	}
	else if(param == "maxReconnectAttempts")
	{
		mMaxReconnectAttempts = std::stoi(parameter.substr(pos + 1));
	}
	else if(param == "randomize")
	{
		mRandomize = param == "true" ? true : false;
	}
	else if(param == "timeout")
	{
		mTimeout = std::stoi(parameter.substr(pos + 1));
	}
}

