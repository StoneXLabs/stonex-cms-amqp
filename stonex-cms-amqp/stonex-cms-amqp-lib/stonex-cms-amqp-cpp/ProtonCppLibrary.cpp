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

#include "ProtonCppLibrary.h"
#include <LoggerFactory/LoggerFactory.h>

#include <fmt/format.h>
#include <iostream>


cms::amqp::ProtonCppLibrary::ProtonCppLibrary()
    :mLogger(LoggerFactory::getInstance().create("com.stonex.cms.amqp.ProtonContainer"))
{
    mContainer = std::make_shared<proton::container>(*this);
    mContainer->auto_stop(false);
    mContainerThread = std::make_unique<std::thread>(std::thread([this] {
        try
        {
            mLogger->log(SEVERITY::LOG_INFO, fmt::format("starting proton container"));
            mContainer->run();
            mLogger->log(SEVERITY::LOG_INFO, fmt::format("proton container work done"));
            std::cout << "proton container work done" << std::endl;
        }
        catch (const std::exception& ex)
        {
            mLogger->log(SEVERITY::LOG_ERROR, fmt::format("container exception : {}", ex.what()));
        }
        }));
}

cms::amqp::ProtonCppLibrary::~ProtonCppLibrary()
{

    mLogger->log(SEVERITY::LOG_INFO, fmt::format("stopping proton container"));
    mContainer->stop();
    mContainerThread->join();
}

std::shared_ptr<proton::container> cms::amqp::ProtonCppLibrary::getContainer()
{

    return ProtonCppLibrary::getInstance().mContainer;
}


cms::amqp::ProtonCppLibrary &cms::amqp::ProtonCppLibrary::getInstance()
{
    static ProtonCppLibrary sInstance;

    return sInstance;
}

void cms::amqp::ProtonCppLibrary::on_container_start(proton::container& container)
{
    mLogger->log(SEVERITY::LOG_INFO, fmt::format("{}", __func__));
}

void cms::amqp::ProtonCppLibrary::on_container_stop(proton::container& container)
{
    mLogger->log(SEVERITY::LOG_INFO, fmt::format("{}", __func__));
}

void activemq::library::ActiveMQCPP::initialize_library()
{
    cms::amqp::ProtonCppLibrary::getInstance();

}
