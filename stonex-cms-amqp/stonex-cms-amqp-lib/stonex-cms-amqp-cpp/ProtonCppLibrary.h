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

#include <thread>

#include <proton/container.hpp>


namespace activemq::library::ActiveMQCPP
{
	void initialize_library();
};

namespace cms::amqp
{

	//!ProtonCppLibrary
	/*!Singleton containing proton::container as top proton handling object used as reference object for Connection/Session/Producer?Consumer creation and library messages handling
	   Object contains Thread in which proton::container runs
	*/
	class ProtonCppLibrary
	{
	private:
		ProtonCppLibrary();
	public:
		~ProtonCppLibrary();

		static std::shared_ptr<proton::container> getContainer();
		static ProtonCppLibrary& getInstance();

	private:
		static ProtonCppLibrary* mInstance;

		std::shared_ptr<proton::container> mContainer;
		std::unique_ptr<std::thread> mContainerThread;

	};
}




