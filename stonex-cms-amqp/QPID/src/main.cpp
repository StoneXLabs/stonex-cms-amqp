#include "proto_example.h"
#include <dbghelp.h>


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

int main()
{

    SetUnhandledExceptionFilter(UnhandledExceptionHandler);
	spdlog_setup::from_file("spdlog.conf");
	ProtonHandler handler;
	std::thread container_thread([&]() {
		proton::container c(handler);
		c.run();

	});

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
		proton::message msg("Hello World!");
		handler.send(msg);
	}
}