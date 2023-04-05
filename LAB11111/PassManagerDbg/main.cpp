#include <conio.h>
#include <iostream>
#include <windows.h>
#include <debugapi.h>

#pragma comment(lib, "advapi32.lib")

int main()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t cmdLine[] = L"../../PassManager/bin/PassMang.exe";

    std::cout << "***Current process: " << GetProcessId(GetCurrentProcess()) << std::endl;
    if (CreateProcess(
        cmdLine, NULL,
        NULL, NULL,
        TRUE, NULL,
        NULL, NULL,
        &si, &pi))
    {
        std::cout << "***CreateProcessW() success!" << std::endl;
        std::cout << "***CreateProcessW() pid = " << std::dec << pi.dwProcessId << std::endl;
    }
    else
    {
        std::cout << "***CreateProcessW() FAILED!" << std::endl;
        return -1;
    }

    bool isAttached = DebugActiveProcess(pi.dwProcessId);
    if (!isAttached)
    {
        DWORD lastError = GetLastError();
        std::cout << std::hex << "***DebugActiveProcess() FAILED, GetlastError() = " << lastError << std::endl;
    }
    else
    {
        std::cout << std::hex << "***DebugActiveProcess() success " << std::endl;
    }

    DEBUG_EVENT debugEvent;
    while (true)
    {
        bool result1 = WaitForDebugEvent(&debugEvent, INFINITE);
        bool result2 = ContinueDebugEvent(debugEvent.dwProcessId,
            debugEvent.dwThreadId,
            DBG_CONTINUE);

        if(debugEvent.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
        {
            break;
        }
    }

    std::cout << "***Dbg end...***\n";

    _getch();
    return 0;
}
