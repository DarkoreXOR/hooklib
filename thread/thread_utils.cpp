#include "thread_utils.h"

HANDLE
WINAPI
NewCreateRemoteThread(HANDLE hProcess,
                      LPSECURITY_ATTRIBUTES lpThreadAttributes,
                      SIZE_T dwStackSize,
                      LPTHREAD_START_ROUTINE lpStartAddress,
                      LPVOID lpParameter,
                      DWORD dwCreationFlags,
                      LPDWORD lpThreadId)
{
    RTL_CREATE_USER_THREAD RtlCreateUserThread;
    HMODULE ModuleHandle;
    HANDLE ThreadHandle;
    NT_STATUS Status;
    CLIENT_ID ClientId;

    ModuleHandle = GetModuleHandleW(L"ntdll");

    if (!ModuleHandle)
    {
        return NULL;
    }

    RtlCreateUserThread = (RTL_CREATE_USER_THREAD)GetProcAddress(ModuleHandle,
                                                                 "RtlCreateUserThread");;

    if (!RtlCreateUserThread)
    {
        return NULL;
    }

    Status = RtlCreateUserThread(hProcess,
                                 NULL,
                                 FALSE, 
                                 0,
                                 0,
                                 0,
                                 lpStartAddress,
                                 lpParameter,
                                 &ThreadHandle,
                                 &ClientId);

    if (Status != NT_STATUS_SUCCESS)
    {
        return NULL;
    }

    if (lpThreadId)
    {
        *lpThreadId = (DWORD)ClientId.UniqueThread;
    }

    return ThreadHandle;
}
