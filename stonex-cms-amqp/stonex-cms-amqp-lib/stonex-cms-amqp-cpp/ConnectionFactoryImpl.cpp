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

#include "ConnectionFactoryImpl.h"

#include <iostream>

#include <proton/connection_options.hpp>
#include <proton/reconnect_options.hpp>
#include <proton/connection.hpp>

#include "ConnectionImpl.h"
#include "ProtonCppLibrary.h"




cms::amqp::ConnectionFactoryImpl::ConnectionFactoryImpl(const std::string& brokerURI, const std::string& user, const std::string& password)
{
	if(brokerURI.empty())
		throw ::cms::CMSException("Connection factory creation with EMPTY broker URL is forbidden");
}
