#include "il_inject.h"

#pragma pack(push, 1)
typedef struct _InjectMessage
{
    BYTE MessageId;
    DWORD ProcessId;
    DWORD Timeout;
    BOOL IsProcessInitialized;
    WCHAR DllFileName32[MAX_PATH];
    WCHAR DllFileName64[MAX_PATH];
} InjectMessage, *PInjectMessage;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _UninjectMessage
{
    BYTE MessageId;
    DWORD ProcessId;
    DWORD Timeout;
    BOOL IsProcessInitialized;
    WCHAR ModuleName32[MAX_PATH];
    WCHAR ModuleName64[MAX_PATH];
} UninjectMessage, *PUninjectMessage;
#pragma pack(pop)

BOOL SendInjectMessage(LPCWSTR DllFileName32,
                       LPCWSTR DllFileName64,
                       DWORD ProcessId,
                       BOOL IsProcessInitialized,
                       DWORD Timeout)
{
    BOOL result;
    BOOL retValue;
    InjectMessage messagePacket;

    messagePacket.MessageId = MESSAGE_ID_INJECT;
    wcscpy_s(messagePacket.DllFileName32, DllFileName32);
    wcscpy_s(messagePacket.DllFileName64, DllFileName64);
    messagePacket.ProcessId = ProcessId;
    messagePacket.Timeout = Timeout;
    messagePacket.IsProcessInitialized = IsProcessInitialized;

    result = IpcSendIpcMessage(L"hlsrv_lib_injector",
                               &messagePacket,
                               sizeof(messagePacket),
                               &retValue,
                               sizeof(retValue),
                               INFINITE,
                               TRUE);

    if (!result)
    {
        return FALSE;
    }

    return retValue;
}

BOOL SendUninjectMessage(LPCWSTR ModuleName32,
                         LPCWSTR ModuleName64,
                         DWORD ProcessId,
                         BOOL IsProcessInitialized,
                         DWORD Timeout)
{
    BOOL result;
    BOOL retValue;
    UninjectMessage messagePacket;

    messagePacket.MessageId = MESSAGE_ID_UNINJECT;
    wcscpy_s(messagePacket.ModuleName32, ModuleName32);
    wcscpy_s(messagePacket.ModuleName64, ModuleName64);
    messagePacket.ProcessId = ProcessId;
    messagePacket.Timeout = Timeout;
    messagePacket.IsProcessInitialized = IsProcessInitialized;

    result = IpcSendIpcMessage(L"hlsrv_lib_injector",
                               &messagePacket,
                               sizeof(messagePacket),
                               &retValue,
                               sizeof(retValue),
                               INFINITE,
                               TRUE);

    if (!result)
    {
        return FALSE;
    }

    return retValue;
}

BOOL
IlInjectLibrary(LPCWSTR DllFileName32,
                LPCWSTR DllFileName64,
                DWORD ProcessId,
                HANDLE ProcessHandle,
                BOOL IsProcessInitialized,
                DWORD Timeout)
{
    if (!ProcessId)
    {
        ProcessId = PsProcessHandleToId(ProcessHandle);
    }

    if (!ProcessId)
    {
        return FALSE;
    }

    return SendInjectMessage(DllFileName32,
                             DllFileName64,
                             ProcessId,
                             IsProcessInitialized,
                             Timeout);
}

BOOL
IlUninjectLibrary(LPCWSTR ModuleName32,
                  LPCWSTR ModuleName64,
                  DWORD ProcessId,
                  HANDLE ProcessHandle,
                  BOOL IsProcessInitialized,
                  DWORD Timeout)
{
    if (!ProcessId)
    {
        ProcessId = PsProcessHandleToId(ProcessHandle);
    }

    if (!ProcessId)
    {
        return FALSE;
    }

    return SendUninjectMessage(ModuleName32,
                               ModuleName64,
                               ProcessId,
                               IsProcessInitialized,
                               Timeout);
}

BOOL
IlInjectLibraryToAllProcesses(LPCWSTR DllFileName32,
                              LPCWSTR DllFileName64,
                              DWORD Timeout)
{
    BOOL result;
    PROCESS_INFO *processes;
    SIZE_T numOfProcesses;

    processes = (PROCESS_INFO*)malloc(sizeof(PROCESS_INFO) * 0x1000);

    if (!processes)
    {
        return FALSE;
    }

    result = PsGetProcesses(processes, 0x1000, &numOfProcesses);

    if (!result)
    {
        free(processes);
        return FALSE;
    }

    for (DWORD i = 0; i < numOfProcesses; ++i)
    {
        IlInjectLibrary(DllFileName32,
                        DllFileName64,
                        processes[i].ProcessId,
                        NULL,
                        TRUE,
                        Timeout);
    }

    free(processes);

    return TRUE;
}

BOOL
IlUninjectLibraryFromAllProcesses(LPCWSTR ModuleName32,
                                  LPCWSTR ModuleName64,
                                  DWORD Timeout)
{
    BOOL result;
    PROCESS_INFO *processes;
    SIZE_T numOfProcesses;

    processes = (PROCESS_INFO*)malloc(sizeof(PROCESS_INFO) * 0x1000);

    if (!processes)
    {
        return FALSE;
    }

    result = PsGetProcesses(processes, 0x1000, &numOfProcesses);

    if (!result)
    {
        free(processes);
        return FALSE;
    }

    for (DWORD i = 0; i < numOfProcesses; ++i)
    {
        IlUninjectLibrary(ModuleName32,
                          ModuleName64,
                          processes[i].ProcessId,
                          NULL,
                          TRUE,
                          Timeout);
    }

    free(processes);

    return TRUE;
}

