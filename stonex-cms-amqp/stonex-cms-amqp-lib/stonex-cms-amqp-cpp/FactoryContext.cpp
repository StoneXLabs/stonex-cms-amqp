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
	mContainer = ProtonCppLibrary::getContainer();
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

std::shared_ptr<proton::container> cms::amqp::config::FactoryContext::container()
{
	return mContainer;
}

std::string cms::amqp::config::FactoryContext::broker() const
{
	return mBroker;
}

std::vector<std::string> cms::amqp::config::FactoryContext::failoverAddresses() const
{
	return mFailoverAddresses;
}

std::string cms::amqp::config::FactoryContext::user() const
{
	return mUser;
}

int cms::amqp::config::FactoryContext::reconnectAttempts() const
{
	return mReconnectAttempts;
}