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

#include "FactoryContext.h"
#include "ProtonCppLibrary.h"

cms::amqp::config::FactoryContext::FactoryContext(const std::string& url)
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


std::string cms::amqp::config::FactoryContext::mainBroker()
{
	return mBroker;
}

std::vector<std::string> cms::amqp::config::FactoryContext::failoverUrl()
{
	return mFailoverAddresses;
}

int cms::amqp::config::FactoryContext::initialReconnectDelay()
{
	return mParameters.failoverOptrions.initialReconnectDelay;
}

int cms::amqp::config::FactoryContext::maxReconnectDelay()
{
	return mParameters.failoverOptrions.maxReconnectDelay;
}

int cms::amqp::config::FactoryContext::maxReconnectAttempts()
{
	return mParameters.failoverOptrions.maxReconnectAttempts;
}
