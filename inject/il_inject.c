#include "il_inject.h"

BOOL
IlInjectLibrary(LPCWSTR DllFileName,
                DWORD ProcessId,
                DWORD Timeout)
{
    BOOL Result;
    BOOL IsX64System;
    BOOL IsX64Process;

    IsX64System = IlIsX64System();

    Result = IlIsX64Process(ProcessId, &IsX64Process);

    if (!Result)
    {
        return FALSE;
    }

    // 32-bit OS
    if (!IsX64System)
    {
        return IlInjectLibrary32(DllFileName,
                                 ProcessId,
                                 Timeout);
    }

    // 64-bit OS
    // 64-bit process
    if (IsX64Process)
    {
        return IlInjectLibrary64(DllFileName,
                                 ProcessId,
                                 Timeout);
    }

    return IlInjectLibrary32(DllFileName,
                             ProcessId,
                             Timeout);
}

BOOL
IlUninjectLibrary(LPCWSTR DllFileName,
                  DWORD ProcessId,
                  DWORD Timeout)
{
    return FALSE;
}
