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

#include "ConnectionFactory.h"

#include <iostream>

#include <proton/connection_options.hpp>
#include <proton/reconnect_options.hpp>
#include <proton/connection.hpp>

#include "Connection.h"
#include "ProtonCppLibrary.h"

#include <algorithm>
#include <regex>
#include <memory>
#include <iterator>

namespace
{
	std::vector<std::string> getBrokerURI(const std::string& url)
	{
	std::vector<std::string> urls;
    std::regex failoverRegExpr("^failover:(.*)");
    std::string url_copy;

    if (std::regex_match(url, failoverRegExpr))
    {
        url_copy = (url.substr(10, url.length() - 11));
        auto it = std::find_if(std::begin(url_copy), std::end(url_copy), [](const char elem) {return elem == ')'; });
        url_copy.resize(std::distance(std::begin(url_copy), it));
    }
    else
    {
        auto it = std::find_if(std::begin(url), std::end(url), [](const char elem) {return elem == '?'; });
        url_copy = url.substr(0, std::distance(std::begin(url), it));
    }

        

    auto start = std::begin(url_copy);
    auto it = std::find_if(start, std::end(url_copy), [](const char elem) {return elem == ','; });
    while (start != it)
    {
        urls.emplace_back(std::string(start, it));
        if (it != std::end(url_copy))
        {
            start = std::next(it);
            it = std::find_if(start, std::end(url_copy), [](const char elem) {return elem == ','; });
        }
        else
            start = it;
    }

	return urls;
	}

    std::string getPrimaryUrl(std::vector<std::string> urls)
    {
        if (urls.empty())
            throw cms::CMSException("No broker URL provided");

        return urls.front();
    }

    std::vector<std::string> getFailoverUrls(std::vector<std::string> urls)
    {
        if (urls.empty())
            throw cms::CMSException("No broker URL provided");

        urls.erase(std::begin(urls));
        return urls;
    }

    proton::connection_options getConnectionOptions(const std::string& username, const std::string& password, const std::string& clientId, const std::vector<std::string>& mFailoverUrl)
    {
        proton::connection_options co;

	    if (!username.empty() && !password.empty())
	    {
	    	co.user(username);
	    	co.password(password);
	    }


	    co.sasl_allow_insecure_mechs(true);
	    co.sasl_allowed_mechs("PLAIN");

	    if (!clientId.empty())
	    	co.container_id(clientId);

	// no idea how to map
	//	co.idle_timeout(proton::duration(1 * 2));

		//from URL


	    proton::reconnect_options rco;

	    rco.failover_urls(mFailoverUrl);

	//fromURL
	    rco.delay(proton::duration(5));
	    rco.max_delay(proton::duration(5));
	    rco.max_attempts(5);


	    co.reconnect(rco);
	    co.desired_capabilities({ "ANONYMOUS-RELAY" });

        return co;
    }

}


stonex::amqp::ConnectionFactory::ConnectionFactory(const std::string& brokerURI)
:mPrimaryUrl{getPrimaryUrl(getBrokerURI(brokerURI))},
mFailoverUrl{getFailoverUrls(getBrokerURI(brokerURI))},
mContainer(ProtonCppLibrary::getInstance().getContainer())
{
	if(brokerURI.empty())
		throw cms::CMSException("Connection factory creation with EMPTY broker URL is forbidden");

    LOG4CXX_INFO(mLogger, "Creating connection factory " << brokerURI);
}



cms::Connection* stonex::amqp::ConnectionFactory::createConnection()
{
	return createConnection("", "", "");
}

cms::Connection* stonex::amqp::ConnectionFactory::createConnection(const std::string& username, const std::string& password)
{
	return createConnection(username, password, "");
}

cms::Connection* stonex::amqp::ConnectionFactory::createConnection(const std::string& username, const std::string& password, const std::string& clientId)
{
    auto connectionOptions = getConnectionOptions(username, password, clientId, mFailoverUrl);
    
    std::string failoverHosts(mPrimaryUrl);

    for (const auto& url : mFailoverUrl)
    {
        failoverHosts += ",";
        failoverHosts += url;
    }

	LOG4CXX_INFO(mLogger, "Creating connection to brokers: "<<failoverHosts <<" with username: " << username);
	auto cmsConnection = new stonex::amqp::Connection();

	connectionOptions.handler(*cmsConnection);

    mContainer.connect(mPrimaryUrl, connectionOptions);
    return cmsConnection;
}

void stonex::amqp::ConnectionFactory::setExceptionListener(cms::ExceptionListener* listener)
{
}

cms::ExceptionListener* stonex::amqp::ConnectionFactory::getExceptionListener() const
{
	return nullptr;
}

void stonex::amqp::ConnectionFactory::setMessageTransformer(cms::MessageTransformer* transformer)
{
	//TO DO implement message transformer management
}

cms::MessageTransformer* stonex::amqp::ConnectionFactory::getMessageTransformer() const
{
	//TO DO implement message transformer management
	return nullptr;
}

cms::ConnectionFactory* stonex::amqp::ConnectionFactory::createCMSConnectionFactory(const std::string& brokerURI)
{
	return new ConnectionFactory(brokerURI);
}




