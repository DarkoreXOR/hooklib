#ifndef __THREAD_UTILS_H
#define __THREAD_UTILS_H

#include <Windows.h>
#include "..\dll_decls.h"
#include "..\native\native_utils.h"
#include "..\logger\logger_utils.h"

HANDLE
WINAPI
ThdNewCreateRemoteThread(
    HANDLE hProcess,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
);

#endif
