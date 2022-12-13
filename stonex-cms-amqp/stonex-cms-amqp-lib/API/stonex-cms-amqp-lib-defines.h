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

#pragma once

#include "cms/Config.h"

#ifndef AMQP_DEFINES
#if __cplusplus > 201703L
#define AMQP_DEFINES namespace cms::amqp {
#define AMQP_DEFINES_CLOSE  }
#else
#define AMQP_DEFINES namespace cms{ namespace amqp {
#define AMQP_DEFINES_CLOSE  }}
#endif
#endif


#ifdef STX_EXPORTS
#define STX_API __declspec(dllexport)
#else
#define STX_API __declspec(dllimport)
#endif