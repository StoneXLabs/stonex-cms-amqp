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
#include "FactoryContext.h"
#include <proton/connection_options.hpp>
#include <proton/reconnect_options.hpp>

cms::amqp::config::ConnectionContext::ConnectionContext(FactoryContext& context, const std::string& username, const std::string& password, const std::string& clientId)
	:mPrimaryUrl(context.broker()),
	mFailoverUrls(context.failoverAddresses()),
	mUser(username),
	mPassword(password),
	mClientId(clientId),
	mInitialReconnectDelay(context.mParameters.failoverOptrions.initialReconnectDelay),
	mMaxReconnectDelay(context.mParameters.failoverOptrions.maxReconnectDelay),
	mMaxReconnectAttempts(context.mParameters.failoverOptrions.maxReconnectAttempts)
{
}

proton::connection_options cms::amqp::config::ConnectionContext::config()
{
	proton::connection_options co;

	if (!mUser.empty() && !mPassword.empty())
	{
		co.user(mUser);
		co.password(mPassword);
	}


	co.sasl_allow_insecure_mechs(true);
	co.sasl_allowed_mechs("PLAIN");

	if (!mClientId.empty())
		co.container_id(mClientId);

	// no idea how to map
	//	co.idle_timeout(proton::duration(1 * 2));

		//from URL


	proton::reconnect_options rco;

	rco.failover_urls(mFailoverUrls);

	//fromURL
	rco.delay(proton::duration(mInitialReconnectDelay));
	rco.max_delay(proton::duration(mMaxReconnectDelay));
	rco.max_attempts(mMaxReconnectAttempts);


	co.reconnect(rco);
	co.desired_capabilities({ "ANONYMOUS-RELAY" });

	return co;
}

