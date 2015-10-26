#ifndef __IL_UTILS_X64_H
#define __IL_UTILS_X64_H

#include <Windows.h>
#include "il_utils.h"

#define INJECT_CODE_X64_SIZE 51

BYTE InjectCodeX64[INJECT_CODE_X64_SIZE];

BOOL
IlInjectLibrary64(
    LPCWSTR DllFileName,
    DWORD ProcessId,
    DWORD Timeout
);

#endif
