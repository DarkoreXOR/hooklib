#ifndef __IL_INJECT_H
#define __IL_INJECT_H

#include <Windows.h>
#include "..\dll_decls.h"
#include "il_utils.h"
#include "il_utils_x86.h"
#include "il_utils_x64.h"

BOOL
IlInjectLibrary(
    LPCWSTR DllFileName,
    DWORD ProcessId,
    DWORD Timeout
);

BOOL
IlUninjectLibrary(
    LPCWSTR DllFileName,
    DWORD ProcessId,
    DWORD Timeout
);

#endif
