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

#include "ProtonCppLibrary.h"
#include <LoggerFactory/LoggerFactory.h>

#include <fmt/format.h>
#include <iostream>

#include <dbghelp.h>
#include <fstream>

void WriteMiniDump(EXCEPTION_POINTERS* pExceptionPointers)
{

    SYSTEMTIME st;
    GetSystemTime(&st);
    char dumpFileName[MAX_PATH];
    sprintf_s(dumpFileName, MAX_PATH, "CrashDump_amq_lib%04d-%02d-%02d_%02d-%02d-%02d.dmp",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    HANDLE hFile = CreateFileA(
        dumpFileName,
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ExceptionPointers = pExceptionPointers;
        dumpInfo.ClientPointers = TRUE;

        // Write the dump
        MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hFile,
            MiniDumpWithFullMemory,
            &dumpInfo,
            nullptr,
            nullptr
        );

        CloseHandle(hFile);
        std::wcout << L"Crash dump saved to CrashDump.dmp\n";
    }
    else {
        std::wcerr << L"Failed to create dump file\n";
    }
}




cms::amqp::ProtonCppLibrary::ProtonCppLibrary()
    :mLogger(LoggerFactory::getInstance().create("com.stonex.cms.amqp.ProtonContainer"))
{
    SetUnhandledExceptionFilter(UnhandledExceptionHandler);
    mContainer = std::make_shared<proton::container>(*this);
    mContainer->auto_stop(false);
    mContainerThread = std::make_unique<std::thread>(std::thread([this] {
        try
        {
            mLogger->log(SEVERITY::LOG_INFO, fmt::format("starting proton container"));
            mContainer->run();
            mLogger->log(SEVERITY::LOG_INFO, fmt::format("proton container work done"));
            std::cout << "proton container work done" << std::endl;
        }
        catch (const std::exception& ex)
        {
            mLogger->log(SEVERITY::LOG_ERROR, fmt::format("container exception : {}", ex.what()));
        }
        }));
}

cms::amqp::ProtonCppLibrary::~ProtonCppLibrary()
{

    mLogger->log(SEVERITY::LOG_INFO, fmt::format("stopping proton container"));
    mContainer->stop();
    mContainerThread->join();
}

std::shared_ptr<proton::container> cms::amqp::ProtonCppLibrary::getContainer()
{

    return ProtonCppLibrary::getInstance().mContainer;
}


LONG WINAPI cms::amqp::ProtonCppLibrary::UnhandledExceptionHandler(EXCEPTION_POINTERS* pExceptionPointers) {
    WriteMiniDump(pExceptionPointers);
    return EXCEPTION_CONTINUE_SEARCH;
}

cms::amqp::ProtonCppLibrary &cms::amqp::ProtonCppLibrary::getInstance()
{
    static ProtonCppLibrary sInstance;

    return sInstance;
}

void cms::amqp::ProtonCppLibrary::on_container_start(proton::container& container)
{
    mLogger->log(SEVERITY::LOG_INFO, fmt::format("{}", __func__));
}

void cms::amqp::ProtonCppLibrary::on_container_stop(proton::container& container)
{
    mLogger->log(SEVERITY::LOG_INFO, fmt::format("{}", __func__));
}

void activemq::library::ActiveMQCPP::initialize_library()
{
    cms::amqp::ProtonCppLibrary::getInstance();

}
