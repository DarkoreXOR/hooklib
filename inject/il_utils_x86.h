#ifndef __IL_UTILS_X86
#define __IL_UTILS_X86

#include <Windows.h>
#include "il_utils.h"
#include "..\asmjit\asmjit.h"
#include "..\thread\thread_utils.h"

BOOL
IlInjectLibrary32(
    LPCWSTR DllFileName,
    DWORD ProcessId,
    DWORD Timeout
);

#endif
