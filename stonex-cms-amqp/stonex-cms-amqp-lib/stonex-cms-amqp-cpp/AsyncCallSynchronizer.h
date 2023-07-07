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

#pragma once
#include <mutex>
#include <functional>
#include <condition_variable>

#include <proton/messaging_handler.hpp>
#include <proton/error_condition.hpp>
#include <proton/receiver_options.hpp>
#include <proton/sender_options.hpp>

#include <cms/CMSException.h>


namespace cms::internal 
{
	class AsyncCallSynchronizer
	{
	public:
		void SynchronizeCall(std::function<void(proton::messaging_handler* handler)> asyncCall, proton::messaging_handler& parameter);

		void SynchronizeCall(std::function<bool()> asyncCall);

		void SynchronizeCall(std::function<bool(std::shared_ptr<proton::connection>)> asyncCall, std::shared_ptr<proton::connection> param);

		void SynchronizeCall(std::function<bool(const std::string&,const proton::receiver_options&, std::shared_ptr<proton::session>)> asyncCall,const std::string& address,const proton::receiver_options& options, std::shared_ptr<proton::session> param);

		void SynchronizeCall(std::function<bool(const std::string&, const proton::sender_options&, std::shared_ptr<proton::session>)> asyncCall, const std::string& address, const proton::sender_options& options, std::shared_ptr<proton::session> param);

		void onResourceInitialized();
		void onResourceUninitialized(const proton::error_condition& error);

		void waitForResource();

	private:
		std::mutex mMutex;
		std::condition_variable mCV;
		bool mIdle = true;
		bool mSuccess{ false };
		proton::error_condition mLastError;

	};
};
