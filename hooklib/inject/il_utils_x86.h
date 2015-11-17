#ifndef __IL_UTILS_X86
#define __IL_UTILS_X86

#include <Windows.h>
#include "il_utils.h"
#include "..\asmjit\asmjit.h"
#include "..\thread\thread_utils.h"
#include "..\process\process_utils.h"
#include "..\logger\logger_utils.h"

BOOL
IlInjectLibrary32(
    LPCWSTR DllFileName,
    HANDLE ProcessHandle,
    BOOL IsProcessInitialized,
    DWORD Timeout
);

BOOL
IlUninjectLibrary32(
    LPCWSTR ModuleName,
    HANDLE ProcessHandle,
    BOOL IsProcessInitialized,
    DWORD Timeout
);

#endif
