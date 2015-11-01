#include "il_inject.h"

#pragma pack(push, 1)
typedef struct _InjectMessage
{
    BYTE MessageId;
    DWORD ProcessId;
    WCHAR DllFileName[MAX_PATH];
} InjectMessage, *PInjectMessage;
#pragma pack(pop)

BOOL SendInjectMessage32(LPCWSTR DllFileName,
                         DWORD ProcessId)
{
    BOOL Result;
    BOOL RetValue;
    InjectMessage msg;

    msg.MessageId = 0x10;
    wcscpy_s(msg.DllFileName, DllFileName);
    msg.ProcessId = ProcessId;

    Result = IpcSendIpcMessage(L"hlsrv_lib_injector32",
                               &msg,
                               sizeof(msg),
                               &RetValue,
                               sizeof(RetValue),
                               INFINITE,
                               TRUE);

    if (!Result)
    {
        return FALSE;
    }

    return RetValue;
}

BOOL SendInjectMessage64(LPCWSTR DllFileName,
                         DWORD ProcessId)
{
    BOOL Result;
    BOOL RetValue;
    InjectMessage msg;

    msg.MessageId = 0x10;
    wcscpy_s(msg.DllFileName, DllFileName);
    msg.ProcessId = ProcessId;

    Result = IpcSendIpcMessage(L"hlsrv_lib_injector64",
                               &msg,
                               sizeof(msg),
                               &RetValue,
                               sizeof(RetValue),
                               INFINITE,
                               TRUE);

    if (!Result)
    {
        return FALSE;
    }

    return RetValue;
}

BOOL
IlInjectLibrary(LPCWSTR DllFileName32,
                LPCWSTR DllFileName64,
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
        return SendInjectMessage32(DllFileName32,
                                   ProcessId);
    }

    // 64-bit OS
    // 64-bit process
    if (IsX64Process)
    {
        return SendInjectMessage64(DllFileName64,
                                   ProcessId);
    }

    return SendInjectMessage32(DllFileName32,
                               ProcessId);
}

BOOL
IlUninjectLibrary(LPCWSTR DllFileName,
                  DWORD ProcessId,
                  DWORD Timeout)
{
    return FALSE;
}
