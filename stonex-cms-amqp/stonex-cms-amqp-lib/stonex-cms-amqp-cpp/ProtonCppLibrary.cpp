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
#include <cms/IllegalStateException.h>
#include <dbghelp.h>
#include <fstream>
#include <log4cxx/logmanager.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>


namespace
{
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

        }
    }

    LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* pExceptionPointers) {
        WriteMiniDump(pExceptionPointers);
        return EXCEPTION_CONTINUE_SEARCH;
    }
};




stonex::amqp::ProtonCppLibrary::ProtonCppLibrary()
	:mLogger(log4cxx::Logger::getLogger("com.stonex.cms.ProtonCppLibrary")),
    mContainer(proton::container(*this)),
    mThread([this]() {
    mContainer.auto_stop(false);
	mContainer.run();
    })
{
	std::unique_lock<std::mutex> lk(mMutex);
	mCv.wait(lk);
    SetUnhandledExceptionFilter(UnhandledExceptionHandler);
}

stonex::amqp::ProtonCppLibrary::~ProtonCppLibrary()
{
    mContainer.stop();
    if (mThread.joinable())
        mThread.join();
}

stonex::amqp::ProtonCppLibrary &stonex::amqp::ProtonCppLibrary::getInstance()
{
    static ProtonCppLibrary mInstance;
	return mInstance;
}


proton::container& stonex::amqp::ProtonCppLibrary::getContainer()
{
    return mContainer;
}

void stonex::amqp::ProtonCppLibrary::on_container_start(proton::container& container)
{
    LOG4CXX_INFO(mLogger, "Container started "<<container.id());
    mCv.notify_all();
}

void stonex::amqp::ProtonCppLibrary::on_container_stop(proton::container& container)
{
    LOG4CXX_INFO(mLogger, "Container stoped " << container.id());

}