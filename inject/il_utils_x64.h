#ifndef __IL_UTILS_X64_H
#define __IL_UTILS_X64_H

#include <Windows.h>
#include "il_utils.h"
#include "..\asmjit\asmjit.h"
#include "..\thread\thread_utils.h"

BOOL
IlInjectLibrary64(
    LPCWSTR DllFileName,
    DWORD ProcessId,
    DWORD Timeout
);

#endif
