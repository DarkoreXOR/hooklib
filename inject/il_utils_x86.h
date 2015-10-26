#ifndef __IL_UTILS_X86
#define __IL_UTILS_X86

#include <Windows.h>
#include "il_utils.h"

#define INJECT_CODE_X86_SIZE 11

BYTE InjectCodeX86[INJECT_CODE_X86_SIZE];

BOOL
IlInjectLibrary32(
    LPCWSTR DllFileName,
    DWORD ProcessId,
    DWORD Timeout
);

#endif
