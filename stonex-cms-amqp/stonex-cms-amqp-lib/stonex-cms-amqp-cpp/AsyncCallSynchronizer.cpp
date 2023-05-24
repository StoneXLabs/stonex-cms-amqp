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

#include "AsyncCallSynchronizer.h"
#include <fmt/format.h>

cms::internal::AsyncCallSynchronizer::AsyncCallSynchronizer(std::shared_ptr<StonexLogger> logger)
{
	setLogger(logger);
}

void cms::internal::AsyncCallSynchronizer::SynchronizeCall(std::function<void(proton::messaging_handler* handler)> asyncCall, proton::messaging_handler& parameter)
{
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquire mutex", __func__));
#endif

	std::unique_lock lk(mMutex);
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquired mutex", __func__));
#endif

	mIdle = false;
	mSuccess = false;
	asyncCall(&parameter);
	mCV.wait(lk, [this] {return mIdle; });
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} done status {}", __func__, mSuccess));
#endif
	if (!mSuccess)
		throw cms::CMSException(mLastError.what());
}

void cms::internal::AsyncCallSynchronizer::SynchronizeCall(std::function<bool()> asyncCall)
{
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquire mutex", __func__));
#endif

	std::unique_lock lk(mMutex);
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquired mutex", __func__));
#endif

	if (asyncCall())
	{
		mIdle = false;
		mSuccess = false;
		mCV.wait(lk, [this] {return mIdle; });
#if _DEBUG
		trace("async call synchronizer", fmt::format("{} done status {}", __func__, mSuccess));
#endif
		if (!mSuccess)
			throw cms::CMSException(mLastError.what());
	}

}

void cms::internal::AsyncCallSynchronizer::SynchronizeCall(std::function<bool(std::shared_ptr<proton::connection> connection)> asyncCall, std::shared_ptr<proton::connection> param)
{
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquire mutex", __func__));
#endif

	std::unique_lock lk(mMutex);
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquired mutex", __func__));
#endif

	if (asyncCall(param))
	{
		mIdle = false;
		mSuccess = false;
		mCV.wait(lk, [this] {return mIdle; });
#if _DEBUG
		trace("async call synchronizer", fmt::format("{} done status {}", __func__, mSuccess));
#endif
		if (!mSuccess)
			throw cms::CMSException(mLastError.what());
	}
}

void cms::internal::AsyncCallSynchronizer::SynchronizeCall(std::function<bool(const std::string&, const proton::receiver_options&, std::shared_ptr<proton::session>)> asyncCall, const std::string& address, const proton::receiver_options& options, std::shared_ptr<proton::session> param)
{
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquire mutex", __func__));
#endif

	std::unique_lock lk(mMutex);
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquired mutex", __func__));
#endif

	if (asyncCall(address,options,param))
	{
		mIdle = false;
		mSuccess = false;
		mCV.wait(lk, [this] {return mIdle; });
#if _DEBUG
		trace("async call synchronizer", fmt::format("{} done status {}", __func__, mSuccess));
#endif
		if (!mSuccess)
			throw cms::CMSException(mLastError.what());
	}
}

void cms::internal::AsyncCallSynchronizer::SynchronizeCall(std::function<bool(const std::string&, const proton::sender_options&, std::shared_ptr<proton::session>)> asyncCall, const std::string& address, const proton::sender_options& options, std::shared_ptr<proton::session> param)
{
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquire mutex", __func__));
#endif

	std::unique_lock lk(mMutex);
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquired mutex", __func__));
#endif

	if (asyncCall(address, options, param))
	{
		mIdle = false;
		mSuccess = false;
		mCV.wait(lk, [this] {return mIdle; });
#if _DEBUG
		trace("async call synchronizer", fmt::format("{} done status {}", __func__, mSuccess));
#endif
		if (!mSuccess)
			throw cms::CMSException(mLastError.what());
	}
}

void cms::internal::AsyncCallSynchronizer::onResourceInitialized()
{
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquire mutex", __func__));
#endif

	std::unique_lock lk(mMutex);
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} notify one", __func__));
#endif

	mIdle = true;
	mSuccess = true;
	lk.unlock();
	mCV.notify_one();
}

void cms::internal::AsyncCallSynchronizer::onResourceUninitialized(const proton::error_condition& error)
{
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquire mutex", __func__));
#endif

	std::unique_lock lk(mMutex);
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} notify all", __func__));
#endif

	mIdle = true;
	mSuccess = error.empty();
	if (!mSuccess)
		mLastError = error;
	lk.unlock();
	mCV.notify_all();
}

void cms::internal::AsyncCallSynchronizer::waitForResource()
{
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquire mutex", __func__));
#endif

	std::unique_lock lk(mMutex);
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} acquired mutex", __func__));
#endif

	mCV.wait(lk, [this] {return mIdle; });
#if _DEBUG
	trace("async call synchronizer", fmt::format("{} done status {}", __func__, mSuccess));
#endif
	mIdle = false;
}
