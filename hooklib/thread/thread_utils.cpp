#include "thread_utils.h"

HANDLE
WINAPI
ThdNewCreateRemoteThread(HANDLE hProcess,
                         LPSECURITY_ATTRIBUTES lpThreadAttributes,
                         SIZE_T dwStackSize,
                         LPTHREAD_START_ROUTINE lpStartAddress,
                         LPVOID lpParameter,
                         DWORD dwCreationFlags,
                         LPDWORD lpThreadId)
{
    HANDLE threadHandle;
    NTSTATUS status;
    NT_CLIENT_ID clientId;

    status = RtlCreateUserThread(hProcess,
                                 NULL,
                                 FALSE, 
                                 0,
                                 0,
                                 0,
                                 lpStartAddress,
                                 lpParameter,
                                 &threadHandle,
                                 &clientId);

    if (status != STATUS_SUCCESS)
    {
        return NULL;
    }

    if (lpThreadId)
    {
        *lpThreadId = (DWORD)clientId.UniqueThread;
    }

    return threadHandle;
}
