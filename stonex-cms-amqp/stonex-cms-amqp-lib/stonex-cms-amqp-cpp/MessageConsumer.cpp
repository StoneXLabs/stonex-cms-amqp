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

#include "MessageConsumer.h"

#include <proton/source_options.hpp>
#include <proton/connection.hpp>
#include <proton/work_queue.hpp>
#include <proton/annotation_key.hpp>
#include <proton/delivery.hpp>
#include <proton/message_id.hpp>


#include "TextMessage.h"
#include "BytesMessage.h"
#include "Queue.h"
#include "Topic.h"
#include "TemporaryQueue.h"
#include "TemporaryTopic.h"

#include <format>

#include "Protocol/utils.h"

stonex::amqp::MessageConsumer::MessageConsumer(proton::session& session, const cms::Destination* destination, const std::string& selector)
	:mSession(session)
{
	if (!destination)
		throw cms::IllegalStateException("destination must not be null");

	auto capabilities = internal::DestinationConverter::capabilities(destination);
	const auto address = internal::DestinationConverter::address(destination);
	
	proton::source_options source_options;
	source_options.capabilities(capabilities);

	if(!selector.empty())
	{
		proton::source::filter_map filters;
		proton::symbol filter_key("selector");
		proton::value filter_value;
		proton::codec::encoder enc(filter_value);
		enc << proton::codec::start::described()
			<< proton::symbol("apache.org:selector-filter:string")
			<< selector
			<< proton::codec::finish();
		filters.put(filter_key, filter_value);
		source_options.filters(filters);

	}

	proton::receiver_options ropts;
	ropts.source(source_options);
	ropts.handler(*this);
		
	mSession.work_queue().add([=]() {mSession.open_receiver(address, ropts); });

	std::unique_lock lk(mMutex);
	mCv.wait(lk, [this]() { return mWorkQueue; });
}



void stonex::amqp::MessageConsumer::start()
{
}

void stonex::amqp::MessageConsumer::stop()
{
}

void stonex::amqp::MessageConsumer::close()
{
	mWorkQueue->add([=] { 
		mReceiver.close(); 
	});
	std::unique_lock lk(mMutex);
	mCv.wait(lk, [this]() { return !mWorkQueue; });
}

stonex::amqp::MessageConsumer::~MessageConsumer()
{
	close();
}

cms::Message* stonex::amqp::MessageConsumer::receive()
{

	return nullptr;
}

cms::Message* stonex::amqp::MessageConsumer::receive(int milis)
{

	return nullptr;
}

cms::Message* stonex::amqp::MessageConsumer::receiveNoWait()
{

	return nullptr;
}

void stonex::amqp::MessageConsumer::setMessageListener(cms::MessageListener* listener)
{
	if(!listener)
		throw cms::CMSException("MessageListener cannot be null");

	mListener = listener;

	onMessageCallback = [this](const cms::Message* message) {mListener->onMessage(message); };
}

cms::MessageListener* stonex::amqp::MessageConsumer::getMessageListener() const
{
	return mListener;
}


std::string stonex::amqp::MessageConsumer::getMessageSelector() const
{
	return std::string();
}


void stonex::amqp::MessageConsumer::setMessageTransformer(cms::MessageTransformer* transformer)
{
	//TO DO implement message transformer management
}

cms::MessageTransformer* stonex::amqp::MessageConsumer::getMessageTransformer() const
{
	//TO DO implement message transformer management
	return nullptr;
}

void stonex::amqp::MessageConsumer::setMessageAvailableListener(cms::MessageAvailableListener* listener)
{
	//TO DO implement message available listener management
}

cms::MessageAvailableListener* stonex::amqp::MessageConsumer::getMessageAvailableListener() const
{
	//TO DO implement message available listener management
	return nullptr;
}

void stonex::amqp::MessageConsumer::on_receiver_open(proton::receiver& receiver)
{
	std::unique_lock lk(mMutex);
	mWorkQueue = &receiver.work_queue();
	mReceiver = receiver;
	LOG4CXX_INFO(mLogger, std::format("Consumer open {}", receiver.source().address()));
	mCv.notify_one();
}

void stonex::amqp::MessageConsumer::on_receiver_close(proton::receiver& receiver)
{
	std::unique_lock lk(mMutex);
	mWorkQueue = nullptr;
	LOG4CXX_INFO(mLogger, std::format("Consumer close {}", receiver.source().address()));
	mCv.notify_one();
}

void stonex::amqp::MessageConsumer::on_receiver_detach(proton::receiver& receiver)
{
	LOG4CXX_INFO(mLogger, std::format("Consumer detatch {}", receiver.source().address()));
}

void stonex::amqp::MessageConsumer::on_receiver_error(proton::receiver& receiver)
{
	LOG4CXX_ERROR(mLogger, std::format("Consumer error {} {}", receiver.source().address(), receiver.error().what()));
}

void stonex::amqp::MessageConsumer::on_receiver_drain_finish(proton::receiver& receiver)
{
	LOG4CXX_INFO(mLogger, std::format("Consumer drain finish {}", receiver.source().address()));
}


void stonex::amqp::MessageConsumer::on_message(proton::delivery& delivery, proton::message& message)
{
	if (!onMessageCallback)
		return;

	onMessageCallback(internal::MessageConverter::fromProtonMessage(message));
}
