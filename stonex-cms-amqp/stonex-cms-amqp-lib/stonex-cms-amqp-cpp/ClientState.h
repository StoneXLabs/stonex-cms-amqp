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
#include <iostream>
#include <fmt/format.h>
#include <logger/StoneXLogger.h>

namespace cms::amqp
{
	enum class ClientState
	{
		UNNINITIALIZED,
		STARTED,
		STOPPED,
		CLOSED,
		DETATCHED

	};

	class StateMachine
	{
	public:
		bool checkState(ClientState state);
		ClientState getState() const;
		void setState(ClientState state);
		void setLogger(StonexLoggerPtr logger);

	private:
		ClientState mState{ ClientState::UNNINITIALIZED };
		StonexLoggerPtr mLogger;
	};
}

template <>
class fmt::formatter<cms::amqp::ClientState> {
public:
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename Context>
	constexpr auto format(cms::amqp::ClientState const& state, Context& ctx) const {
		switch (state)
		{
		case cms::amqp::ClientState::UNNINITIALIZED:
			return format_to(ctx.out(), "UNNINITIALIZED");
			break;
		case cms::amqp::ClientState::STARTED:
			return format_to(ctx.out(), "STARTED");
			break;
		case cms::amqp::ClientState::STOPPED:
			return format_to(ctx.out(), "STOPPED");
			break;
		case cms::amqp::ClientState::CLOSED:
			return format_to(ctx.out(), "CLOSED");
			break;
		case cms::amqp::ClientState::DETATCHED:
			return format_to(ctx.out(), "DETATCHED");
			break;
		default:
			return format_to(ctx.out(), "UNKNOWN");
			break;

		}
	}
};