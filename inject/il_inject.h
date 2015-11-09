#ifndef __IL_INJECT_H
#define __IL_INJECT_H

#include <Windows.h>
#include "..\dll_decls.h"
#include "il_utils.h"
#include "il_utils_x86.h"
#include "il_utils_x64.h"
#include "..\ipc\ipc.h"
#include "..\logger\logger_utils.h"

#define MESSAGE_ID_INJECT 0x10
#define MESSAGE_ID_UNINJECT 0x11

BOOL
IlInjectLibrary(
    LPCWSTR DllFileName32,
    LPCWSTR DllFileName64,
    DWORD ProcessId,
    HANDLE ProcessHandle,
    BOOL IsProcessInitialized,
    DWORD Timeout
);

BOOL
IlUninjectLibrary(
    LPCWSTR ModuleName32,
    LPCWSTR ModuleName64,
    DWORD ProcessId,
    HANDLE ProcessHandle,
    BOOL IsProcessInitialized,
    DWORD Timeout
);

BOOL
IlInjectLibraryToAllProcesses(
    LPCWSTR DllFileName32,
    LPCWSTR DllFileName64,
    DWORD Timeout
);

BOOL
IlUninjectLibraryFromAllProcesses(
    LPCWSTR ModuleName32,
    LPCWSTR ModuleName64,
    DWORD Timeout
);

#endif
