/*
 *  [ BSD License: http://opensource.org/licenses/bsd-license.php ]
 *  ===========================================================================
 *  Copyright (c) 2015, Lakutin Ivan
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

    processes = (PROCESS_INFO*)AllocMem(sizeof(PROCESS_INFO) * 0x1000);

    if (!processes)
    {
        return FALSE;
    }

    result = PsGetProcesses(processes, 0x1000, &numOfProcesses);

    if (!result)
    {
        DeallocMem(processes);
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

    DeallocMem(processes);

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

    processes = (PROCESS_INFO*)AllocMem(sizeof(PROCESS_INFO) * 0x1000);

    if (!processes)
    {
        return FALSE;
    }

    result = PsGetProcesses(processes, 0x1000, &numOfProcesses);

    if (!result)
    {
        DeallocMem(processes);
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

    DeallocMem(processes);

    return TRUE;
}

