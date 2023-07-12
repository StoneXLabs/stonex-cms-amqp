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

#include "ConnectionMetadataImpl.h"

/// <summary>
/// CMS API info
/// </summary>
constexpr int CMSVersionMajor = 3;
constexpr int CMSVersionMinor = 2;

/// <summary>
/// CMS Provider info
/// </summary>
constexpr std::string_view CMSAPIProviderName = "activemq-cpp";
constexpr int CMSProviderVersionMajor = 0;
constexpr int CMSProviderVersionMinor = 0;
constexpr int CMSProviderVersionPatch = 0;

std::string cms::amqp::ConnectionMetadataImpl::getCMSVersion() const
{
	return std::to_string(CMSVersionMajor) + "." + std::to_string(CMSVersionMinor);
}

int cms::amqp::ConnectionMetadataImpl::getCMSMajorVersion() const
{
	return  CMSVersionMajor;
}

int cms::amqp::ConnectionMetadataImpl::getCMSMinorVersion() const
{
	return CMSVersionMinor;
}

std::string cms::amqp::ConnectionMetadataImpl::getCMSProviderName() const
{
	return std::string(CMSAPIProviderName);
}

std::string cms::amqp::ConnectionMetadataImpl::getProviderVersion() const
{
	return std::to_string(CMSProviderVersionMajor) + "." + std::to_string(CMSProviderVersionMinor)+"." + std::to_string(CMSProviderVersionPatch);
}

int cms::amqp::ConnectionMetadataImpl::getProviderMajorVersion() const
{
	return CMSProviderVersionMajor;
}

int cms::amqp::ConnectionMetadataImpl::getProviderMinorVersion() const
{
	return CMSProviderVersionMinor;
}

int cms::amqp::ConnectionMetadataImpl::getProviderPatchVersion() const
{
	return CMSProviderVersionPatch;
}

std::vector<std::string> cms::amqp::ConnectionMetadataImpl::getCMSXPropertyNames() const
{
	return std::vector<std::string>();
}
