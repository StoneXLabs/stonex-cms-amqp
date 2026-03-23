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

#include "Session.h"

#include <proton/session.hpp>
#include <proton/session_options.hpp>
#include <proton/work_queue.hpp>

#include "Queue.h"
#include "TemporaryQueue.h"
#include "Topic.h"
#include "TemporaryTopic.h"

#include "TextMessage.h"
#include "BytesMessage.h"

#include "MessageProducer.h"
#include "MessageConsumer.h"


stonex::amqp::Session::Session::Session(proton::connection& connection, cms::Session::AcknowledgeMode ackMode)
	:mConnection{connection}
{
}

stonex::amqp::Session::Session::~Session()
{
	close();
}

void stonex::amqp::Session::Session::start()
{
	mConnection.work_queue().add([this](){
		
		proton::session_options sessionOptions;
		sessionOptions.handler(*this);
		mConnection.open_session(sessionOptions); 
		});
	
	std::unique_lock<std::mutex> lk(mMutex);
	mCv.wait(lk, [this]() { return mWorkQueue; });
}

void stonex::amqp::Session::Session::stop()
{
	//AMQP session has no Stopabble interface
	//TO DO implement Stopable behaviour like in CMS
	close();
}

void stonex::amqp::Session::Session::close()
{
	mWorkQueue->add([=] {mSession.close(); });
	std::unique_lock lk(mMutex);
	mCv.wait(lk, [this]() { return !mWorkQueue; });
}

void stonex::amqp::Session::Session::commit()
{
	//TO DO implement Transactions
}

void stonex::amqp::Session::Session::rollback()
{
	//TO DO implement Transactions
}

void stonex::amqp::Session::Session::recover()
{
	//TO DO implement Transactions
}


cms::MessageConsumer* stonex::amqp::Session::createConsumer(const cms::Destination* destination)
{
	return createConsumer(destination, "");
}

cms::MessageConsumer* stonex::amqp::Session::createConsumer(const cms::Destination* destination, const std::string& selector)
{
	return new stonex::amqp::MessageConsumer(mSession, destination, selector);
}

cms::MessageConsumer* stonex::amqp::Session::createConsumer(const cms::Destination* destination, const std::string& selector, bool noLocal)
{
	
	//TO DO implement noLocal behaviour
    return nullptr;
}

cms::MessageConsumer* stonex::amqp::Session::createDurableConsumer(const cms::Topic* destination, const std::string& name, const std::string& selector, bool noLocal )
{
	//TO DO implement noLocal behaviour
    return nullptr;
}

cms::MessageProducer* stonex::amqp::Session::createProducer(const cms::Destination* destination)
{
     return new stonex::amqp::MessageProducer(mSession, destination);
}

cms::QueueBrowser* stonex::amqp::Session::createBrowser(const cms::Queue* queue)
{
	//TO DO implement Browser
    return nullptr;
}

cms::QueueBrowser* stonex::amqp::Session::createBrowser(const cms::Queue* queue, const std::string& selector)
{
	//TO DO implement Browser
    return nullptr;
}

cms::Queue* stonex::amqp::Session::createQueue(const std::string& queueName)
{
    return new Queue(queueName);
}

cms::Topic* stonex::amqp::Session::createTopic(const std::string& topicName)
{
    return new Topic(topicName);
}

cms::TemporaryQueue* stonex::amqp::Session::createTemporaryQueue()
{
    return new TemporaryQueue();
}

cms::TemporaryTopic* stonex::amqp::Session::createTemporaryTopic()
{
    return new TemporaryTopic();
}

cms::Message* stonex::amqp::Session::createMessage()
{
    return nullptr;
}

cms::BytesMessage* stonex::amqp::Session::createBytesMessage()
{
     return new BytesMessage();
}

cms::BytesMessage* stonex::amqp::Session::createBytesMessage(const unsigned char* bytes, int bytesSize)
{
     return  new BytesMessage(bytes, bytesSize);
}

cms::StreamMessage* stonex::amqp::Session::createStreamMessage()
{
	//TO DO implement Stream Message
    return nullptr;
}

cms::TextMessage* stonex::amqp::Session::createTextMessage()
{
    return new TextMessage("");
}

cms::TextMessage* stonex::amqp::Session::createTextMessage(const std::string& text)
{
	return new TextMessage(text);
}

cms::MapMessage* stonex::amqp::Session::createMapMessage()
{
	//TO DO implement Map Message
    return nullptr;
}

cms::Session::AcknowledgeMode stonex::amqp::Session::getAcknowledgeMode() const
{
	//not_implemented<>();
	return {};
}

bool stonex::amqp::Session::isTransacted() const
{	
	//TO DO implement Transactions
    return false;
}

void stonex::amqp::Session::unsubscribe(const std::string& name)
{
	//TO DO implement Transactions
}

void stonex::amqp::Session::setMessageTransformer(cms::MessageTransformer* transformer)
{
	//TO DO implement message transformer management
}

cms::MessageTransformer* stonex::amqp::Session::getMessageTransformer() const
{
	//TO DO implement message transformer management
	return nullptr;
}



void stonex::amqp::Session::Session::on_session_open(proton::session& session)
{
	
	std::unique_lock<std::mutex> lk(mMutex);
	mSession = session;
	mWorkQueue = &session.work_queue();

	LOG4CXX_INFO(mLogger, std::format("Session open"));
	mCv.notify_all();
}

void stonex::amqp::Session::Session::on_session_close(proton::session& session)
{
	std::unique_lock lk(mMutex);
	mWorkQueue = nullptr;
	LOG4CXX_INFO(mLogger, std::format("Session close"));
	mCv.notify_one();
}

void stonex::amqp::Session::Session::on_session_error(proton::session& session)
{
	LOG4CXX_ERROR(mLogger, std::format("Session error {}", session.error().what()));
}