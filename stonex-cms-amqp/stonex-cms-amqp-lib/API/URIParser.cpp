/*
 * Copyright 2022 StoneX Financial Ltd.
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

#include "URIParser.h"

#include <iostream>
#include <algorithm>
#include <regex>
#include <memory>
#include <iterator>

URIParser::Parameters URIParser::parseURI(const std::string& url)
{
    
    parameters = Parameters();
    
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
        parameters.url_vec.emplace_back(std::string(start, it));
        if (it != std::end(url_copy))
        {
            start = std::next(it);
            it = std::find_if(start, std::end(url_copy), [](const char elem) {return elem == ','; });
        }
        else
            start = it;
    }

    const char delimeter{ '?' };
    auto optionsChar = std::find(std::cbegin(url), std::cend(url), delimeter);
    if (optionsChar != std::cend(url))
    {
        getOptionsString(&*std::next(optionsChar),false);
    }

    return parameters;
}

std::string URIParser::getOptionsString(const std::string& uri, bool compound)
{
    auto start = std::cbegin(uri);
    auto stop = std::cend(uri);

    return getOptionsString(start, stop, compound);
}

std::string URIParser::getOptionsString(const std::string::const_iterator& uri_begin, const std::string::const_iterator& uri_end, bool compound)
{

    const char optionsSeparator{ '&' };
    const char compoundOptionsSeparator{ '.' };


    auto option = std::find_if(uri_begin, uri_end, [=](const char& elem) { return elem == optionsSeparator; });

    auto space = std::next(option, -1);

    while (space != uri_begin)
    {
        if (*space == compoundOptionsSeparator)
        {
            option = space;
            break;
        }
        space--;
    }

    if (option == uri_end)
    {
        if (!compound)
            updateOption(std::string(uri_begin, option));
        else
            return std::string(uri_begin, uri_end);
    }
    else
    {
        if (!compound)
            compound = *option == compoundOptionsSeparator;

        if (*option == optionsSeparator && !compound)
        {
            updateOption(std::string(uri_begin, option));
            getOptionsString(std::next(option), uri_end, compound);
        }
        else if (*option == compoundOptionsSeparator && !compound)
        {
            updateOption(std::string(uri_begin, option), getOptionsString(std::next(option), uri_end, compound));
        }
        else if (*option == optionsSeparator && compound)
        {
            updateOption(getOptionsString(std::next(option), uri_end, compound));
            return std::string(uri_begin, option);
        }
        else if (*option == compoundOptionsSeparator && compound)
        {
            updateOption(std::string(uri_begin, option), getOptionsString(std::next(option), uri_end, compound));
            return "";
        }
        else
            updateOption(std::string(uri_begin, option), getOptionsString(std::next(option), uri_end, compound));
    }
    return "";

}


void URIParser::updateOption(const std::string& option_group,const std::string& option)
{


    if(option.empty())
        std::cout << "initialize compound option ERROR : " << option_group << "." << option << std::endl;
    else
    {
        std::string::const_iterator compound = std::find(std::cbegin(option), std::cend(option), '.'); compound != std::cend(option);
        if (std::string::const_iterator separator = std::find(std::cbegin(option), std::cend(option), '='); separator != std::cend(option))
        {
            std::string _option_group = option_group;// std::string(std::cbegin(option), compound);
            std::string _option = std::string(std::cbegin(option), separator);

            if (_option_group == "transport")
            {
                if (_option == "commandTracingEnabled")
                    parameters.transportOptions.commandTracingEnabled = (std::string(std::next(separator), std::cend(option)) == "true");
                else if (_option == "tcpTracingEnabled")
                    parameters.transportOptions.tcpTracingEnabled = (std::string(std::next(separator), std::cend(option)) == "true");
                else if (_option == "useAsyncSend")
                    parameters.transportOptions.useAsyncSend = (std::string(std::next(separator), std::cend(option)) == "true");
                else if (_option == "responseCorrelator_MaxResponseWaitTime")
                    parameters.transportOptions.responseCorrelator_MaxResponseWaitTime = std::stoi(std::string(std::next(separator), std::cend(option)));

            }
            else if (_option_group == "transaction")
            {
                if (_option == "maxRedeliveryCount")
                    parameters.transactionOptions.maxRedeliveryCount = std::stoi(std::string(std::next(separator), std::cend(option)));

            }
            else if (_option_group == "connection")
            {
                if (_option == "sendTimeout")
                    parameters.connectionOptions.sendTimeout = std::stoi(std::string(std::next(separator), std::cend(option)));
                else if (_option == "producerWindowSize")
                    parameters.connectionOptions.producerWindowSize = std::stoi(std::string(std::next(separator), std::cend(option)));
                else if (_option == "closeTimeout")
                    parameters.connectionOptions.closeTimeout = std::stoi(std::string(std::next(separator), std::cend(option)));
                else if (_option == "useAsyncSend")
                    parameters.connectionOptions.useAsyncSend = (std::string(std::next(separator), std::cend(option)) == "true");
                else if (_option == "alwaysSyncSend")
                    parameters.connectionOptions.alwaysSyncSend = (std::string(std::next(separator), std::cend(option)) == "true");
                else if (_option == "useCompression")
                    parameters.connectionOptions.useCompression = (std::string(std::next(separator), std::cend(option)) == "true");
                else if (_option == "dispatchAsync")
                    parameters.connectionOptions.dispatchAsync = (std::string(std::next(separator), std::cend(option)) == "true");

            }
            else if (_option_group == "cms")
            {

                auto compound_start = compound;

                if (compound = std::find(std::next(compound), std::cend(option), '.'); compound != std::cend(option))
                {
                    _option_group = std::string(std::next(compound_start), compound);

                    if (_option_group == "Prefetch")
                    {
                        if (_option == "durableTopicPrefetch")
                            parameters.connectionPolicyOptions.prefetchOptons.durableTopicPrefetch = std::stoi(std::string(std::next(separator), std::cend(option)));
                        else if (_option == "queueBrowserPrefetch")
                            parameters.connectionPolicyOptions.prefetchOptons.queueBrowserPrefetch = std::stoi(std::string(std::next(separator), std::cend(option)));
                        else if (_option == "queuePrefetch")
                            parameters.connectionPolicyOptions.prefetchOptons.queuePrefetch = std::stoi(std::string(std::next(separator), std::cend(option)));
                        else if (_option == "topicPrefetch")
                            parameters.connectionPolicyOptions.prefetchOptons.topicPrefetch = std::stoi(std::string(std::next(separator), std::cend(option)));
                    }
                    else if (_option_group == "Redelivery")
                    {
                        if (_option == "backOffMultiplier")
                            parameters.connectionPolicyOptions.redeliveryOptions.backOffMultiplier = std::stof(std::string(std::next(separator), std::cend(option)));
                        else if (_option == "collisionAvoidancePercent")
                            parameters.connectionPolicyOptions.redeliveryOptions.collisionAvoidancePercent = std::stoi(std::string(std::next(separator), std::cend(option)));
                        else if (_option == "initialRedeliveryDelay")
                            parameters.connectionPolicyOptions.redeliveryOptions.initialRedeliveryDelay = std::stoi(std::string(std::next(separator), std::cend(option)));
                        else if (_option == "maximumRedeliveries")
                            parameters.connectionPolicyOptions.redeliveryOptions.maximumRedeliveries = std::stoi(std::string(std::next(separator), std::cend(option)));
                        else if (_option == "useCollisionAvoidance")
                            parameters.connectionPolicyOptions.redeliveryOptions.useCollisionAvoidance = (std::string(std::next(separator), std::cend(option)) == "true");
                        else if (_option == "useExponentialBackOff")
                            parameters.connectionPolicyOptions.redeliveryOptions.useExponentialBackOff = (std::string(std::next(separator), std::cend(option)) == "true");
                    }
                }

                if (_option == "sendTimeout")
                    parameters.connectionOptions.sendTimeout = std::stoi(std::string(std::next(separator), std::cend(option)));
                else if (_option == "producerWindowSize")
                    parameters.connectionOptions.producerWindowSize = std::stoi(std::string(std::next(separator), std::cend(option)));
                else if (_option == "closeTimeout")
                    parameters.connectionOptions.closeTimeout = std::stoi(std::string(std::next(separator), std::cend(option)));
                else if (_option == "useAsyncSend")
                    parameters.connectionOptions.useAsyncSend = (std::string(std::next(separator), std::cend(option)) == "true");
                else if (_option == "alwaysSyncSend")
                    parameters.connectionOptions.alwaysSyncSend = (std::string(std::next(separator), std::cend(option)) == "true");
                else if (_option == "useCompression")
                    parameters.connectionOptions.useCompression = (std::string(std::next(separator), std::cend(option)) == "true");
                else if (_option == "dispatchAsync")
                    parameters.connectionOptions.dispatchAsync = (std::string(std::next(separator), std::cend(option)) == "true");

            }
            else
            {
                std::string::const_iterator compound = std::find(std::cbegin(option_group), std::cend(option_group), '.');
                compound != std::cend(option_group);
                if (std::string(std::begin(option_group), compound) == "cms")
                {

                    auto compound_start = compound;

                    if (compound = std::find(std::next(compound), std::cend(option_group), '.'); compound == std::cend(option_group))
                    {
                        _option_group = std::string(std::next(compound_start), compound);

                        if (_option_group == "Prefetch")
                        {
                            if (_option == "durableTopicPrefetch")
                                parameters.connectionPolicyOptions.prefetchOptons.durableTopicPrefetch = std::stoi(std::string(std::next(separator), std::cend(option)));
                            else if (_option == "queueBrowserPrefetch")
                                parameters.connectionPolicyOptions.prefetchOptons.queueBrowserPrefetch = std::stoi(std::string(std::next(separator), std::cend(option)));
                            else if (_option == "queuePrefetch")
                                parameters.connectionPolicyOptions.prefetchOptons.queuePrefetch = std::stoi(std::string(std::next(separator), std::cend(option)));
                            else if (_option == "topicPrefetch")
                                parameters.connectionPolicyOptions.prefetchOptons.topicPrefetch = std::stoi(std::string(std::next(separator), std::cend(option)));
                        }
                        else if (_option_group == "Redelivery")
                        {
                            if (_option == "backOffMultiplier")
                                parameters.connectionPolicyOptions.redeliveryOptions.backOffMultiplier = std::stof(std::string(std::next(separator), std::cend(option)));
                            else if (_option == "collisionAvoidancePercent")
                                parameters.connectionPolicyOptions.redeliveryOptions.collisionAvoidancePercent = std::stoi(std::string(std::next(separator), std::cend(option)));
                            else if (_option == "initialRedeliveryDelay")
                                parameters.connectionPolicyOptions.redeliveryOptions.initialRedeliveryDelay = std::stoi(std::string(std::next(separator), std::cend(option)));
                            else if (_option == "maximumRedeliveries")
                                parameters.connectionPolicyOptions.redeliveryOptions.maximumRedeliveries = std::stoi(std::string(std::next(separator), std::cend(option)));
                            else if (_option == "useCollisionAvoidance")
                                parameters.connectionPolicyOptions.redeliveryOptions.useCollisionAvoidance = (std::string(std::next(separator), std::cend(option)) == "true");
                            else if (_option == "useExponentialBackOff")
                                parameters.connectionPolicyOptions.redeliveryOptions.useExponentialBackOff = (std::string(std::next(separator), std::cend(option)) == "true");
                        }
                    }
                }
            }
             
        }
    }



}

void URIParser::updateOption(const std::string& option)
{
    if(!option.empty())
    {
        if (std::string::const_iterator separator = std::find(std::cbegin(option), std::cend(option), '='); separator != std::cend(option))
        {
            std::string _option = std::string(std::cbegin(option), separator);

            if(_option == "inputBufferSize")
                parameters.socketOptions.inputBufferSize = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "outputBufferSize")
                parameters.socketOptions.outputBufferSize = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "soLinger")
                parameters.socketOptions.soLinger = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "soKeepAlive")
                parameters.socketOptions.soKeepAlive = (std::string(std::next(separator), std::cend(option)) == "true");
            else if(_option == "soReceiveBufferSize")
                parameters.socketOptions.soReceiveBufferSize = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "soConnectionTimeout")
                parameters.socketOptions.soConnectionTimeout = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "soSendBufferSize")
                parameters.socketOptions.soSendBufferSize = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "tcpNoDelay")
                parameters.socketOptions.tcpNoDelay = (std::string(std::next(separator), std::cend(option)) == "true");
            else if(_option == "initialReconnectDelay")
                parameters.failoverOptrions.initialReconnectDelay = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "maxReconnectDelay")
                parameters.failoverOptrions.maxReconnectDelay =  std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "useExpotentialBackOff")
                parameters.failoverOptrions.useExpotentialBackOff = (std::string(std::next(separator), std::cend(option)) == "true");
            else if(_option == "startupMaxReconnectAttempts")
                parameters.failoverOptrions.startupMaxReconnectAttempts = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if (_option == "maxReconnectAttempts")
                parameters.failoverOptrions.maxReconnectAttempts = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "randomize")
                parameters.failoverOptrions.randomize = (std::string(std::next(separator), std::cend(option)) == "true");
            else if(_option == "backup")
                parameters.failoverOptrions.backup = (std::string(std::next(separator), std::cend(option)) == "true");
            else if(_option == "backupPoolSize")
                parameters.failoverOptrions.backupPoolSize  = std::stoi(std::string(std::next(separator), std::cend(option)));
            else if(_option == "timeout")
                parameters.failoverOptrions.timeout  = std::stoi(std::string(std::next(separator), std::cend(option)));
        }
        
    }
     
        
}
