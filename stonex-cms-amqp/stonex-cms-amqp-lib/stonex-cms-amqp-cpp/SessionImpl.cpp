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

#include "SessionImpl.h"

#include "proton/work_queue.hpp"
#include <proton/session.hpp>
#include <proton/session_options.hpp>

#include "../API/CMSTextMessage.h"
#include "MessageConsumerImpl.h"
#include "MessageProducerImpl.h"

class MessageConsumer : public cms::MessageConsumer{};
class MessageProducer : public cms::MessageProducer {};
class QueueBrowser : public cms::QueueBrowser {};
class Queue : public cms::Queue {};
class Topic : public cms::Topic {};
class TemporaryQueue : public cms::TemporaryQueue {};
class TemporaryTopic : public cms::TemporaryTopic {};
class Message : public cms::Message {};
class BytesMessage : public cms::BytesMessage {};
class StreamMessage : public cms::StreamMessage {};
//class TextMessage : public cms::TextMessage {};
//class BytesMessage : public cms::BytesMessage {};






cms::amqp::SessionImpl::SessionImpl(std::shared_ptr<proton::connection>  connection, ::cms::Session::AcknowledgeMode ack_mode)
	:mACKMode{ack_mode}
{

	mEXHandler.SynchronizeCall(std::bind(&SessionImpl::syncStart, this, std::placeholders::_1), connection);
}

cms::amqp::SessionImpl::~SessionImpl()
{
	close();
}

void cms::amqp::SessionImpl::close()
{
	if(mState == ClientState::STARTED)
		mEXHandler.SynchronizeCall(std::bind(&SessionImpl::syncClose, this));
}

void cms::amqp::SessionImpl::commit()
{
}

void cms::amqp::SessionImpl::rollback()
{
}

void cms::amqp::SessionImpl::recover()
{
}

void cms::amqp::SessionImpl::start()
{
}

void cms::amqp::SessionImpl::stop()
{
}

::cms::Session::AcknowledgeMode cms::amqp::SessionImpl::ackMode()
{
	return mACKMode;
}


void cms::amqp::SessionImpl::on_session_open(proton::session& session)
{
	mSession = std::make_shared<proton::session>(session);
	mState = ClientState::STARTED;
	mEXHandler.onResourceInitialized();
}

void cms::amqp::SessionImpl::on_session_close(proton::session& session)
{
	mState = ClientState::CLOSED;
	mEXHandler.onResourceInitialized();
}

void cms::amqp::SessionImpl::on_session_error(proton::session& session)
{
	mEXHandler.onResourceUninitialized(session.error()); // move to close??

}

std::shared_ptr<proton::session> cms::amqp::SessionImpl::session()
{
	return mSession;
}

bool cms::amqp::SessionImpl::syncClose()
{
	return mSession->connection().work_queue().add([=] {mSession->close(); });
	

}

bool cms::amqp::SessionImpl::syncStart(std::shared_ptr<proton::connection>  connection)
{
	proton::session_options sopt;
	sopt.handler(*this);
	return connection->work_queue().add([=] {connection->open_session(sopt); });
}

bool cms::amqp::SessionImpl::syncStop()
{
	return false;
}
