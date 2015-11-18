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

#include "dll_entry.h"

BOOL
EXPORT
WINAPI
IsX64System()
{
    return SysIsX64System();
}

BOOL
EXPORT
WINAPI
IsX64Process(HANDLE ProcessHandle,
             PBOOL Result)
{
    return PsIsX64Process(ProcessHandle, Result);
}

BOOL
EXPORT
WINAPI
CreateIpcChannel(LPCWSTR ChannelName,
                 IPC_ROUTINE Routine,
                 BOOL MultiSession,
                 PVOID *ChannelData)
{
    return IpcCreateIpcChannel(ChannelName,
                               Routine,
                               MultiSession,
                               ChannelData);
}

BOOL
EXPORT
WINAPI
DestroyIpcChannel(PVOID ChannelData)
{
    return IpcDestroyIpcChannel(ChannelData);
}

BOOL
EXPORT
WINAPI
SendIpcMessage(LPCWSTR ChannelName,
               PVOID MessageBuf,
               DWORD MessageSize,
               PVOID AnswerBuf,
               DWORD AnswerSize,
               DWORD Timeout,
               BOOL MultiSession)
{
    return IpcSendIpcMessage(ChannelName,
                             MessageBuf,
                             MessageSize,
                             AnswerBuf,
                             AnswerSize,
                             Timeout,
                             MultiSession);
}

BOOL
EXPORT
WINAPI
InjectLibrary(LPCWSTR DllFileName32,
              LPCWSTR DllFileName64,
              DWORD ProcessId,
              HANDLE ProcessHandle,
              BOOL IsProcessInitialized,
              DWORD Timeout)
{
    return IlInjectLibrary(DllFileName32,
                           DllFileName64,
                           ProcessId,
                           ProcessHandle,
                           IsProcessInitialized,
                           Timeout);
}

BOOL
EXPORT
WINAPI
InjectLibrary32(LPCWSTR DllFileName,
                HANDLE ProcessHandle,
                BOOL IsProcessInitialized,
                DWORD Timeout)
{
    return IlInjectLibrary32(DllFileName,
                             ProcessHandle,
                             IsProcessInitialized,
                             Timeout);
}

BOOL
EXPORT
WINAPI
InjectLibrary64(LPCWSTR DllFileName,
                HANDLE ProcessHandle,
                BOOL IsProcessInitialized,
                DWORD Timeout)
{
    return IlInjectLibrary64(DllFileName,
                             ProcessHandle,
                             IsProcessInitialized,
                             Timeout);
}

BOOL
EXPORT
WINAPI
UninjectLibrary(LPCWSTR ModuleName32,
                LPCWSTR ModuleName64,
                DWORD ProcessId,
                HANDLE ProcessHandle,
                BOOL IsProcessInitialized,
                DWORD Timeout)
{
    return IlUninjectLibrary(ModuleName32,
                             ModuleName64,
                             ProcessId,
                             ProcessHandle,
                             IsProcessInitialized,
                             Timeout);
}

BOOL
EXPORT
WINAPI
UninjectLibrary32(LPCWSTR ModuleName,
                  HANDLE ProcessHandle,
                  BOOL IsProcessInitialized,
                  DWORD Timeout)
{
    return IlUninjectLibrary32(ModuleName,
                               ProcessHandle,
                               IsProcessInitialized,
                               Timeout);
}

BOOL
EXPORT
WINAPI
UninjectLibrary64(LPCWSTR ModuleName,
                  HANDLE ProcessHandle,
                  BOOL IsProcessInitialized,
                  DWORD Timeout)
{
    return IlUninjectLibrary64(ModuleName,
                               ProcessHandle,
                               IsProcessInitialized,
                               Timeout);
}

BOOL
EXPORT
WINAPI
HookCode(LPVOID Target,
         LPVOID Detour,
         LPVOID *Original,
         BOOL SetEnabled)
{
    return HkHookCode(Target,
                      Detour,
                      Original,
                      SetEnabled);
}

BOOL
EXPORT
WINAPI
UnhookCode(LPVOID Target)
{
    return HkUnhookCode(Target);
}

BOOL
EXPORT
WINAPI
HookApi(LPCWSTR ModuleName,
        LPCSTR ProcName,
        LPVOID Detour,
        LPVOID *Original,
        BOOL SetEnabled)
{
    return HkHookApi(ModuleName,
                     ProcName,
                     Detour,
                     Original,
                     SetEnabled);
}

BOOL
EXPORT
WINAPI
UnhookApi(LPCWSTR ModuleName,
          LPCSTR ProcName)
{
    return HkUnhookApi(ModuleName,
                       ProcName);
}

BOOL
EXPORT
WINAPI
EnableHook(LPVOID Target)
{
    return HkEnableHook(Target);
}

BOOL
EXPORT
WINAPI
DisableHook(LPVOID Target)
{
    return HkDisableHook(Target);
}

BOOL
EXPORT
WINAPI
EnableAllHooks()
{
    return HkEnableAllHooks();
}

BOOL
EXPORT
WINAPI
DisableAllHooks()
{
    return HkDisableAllHooks();
}

DWORD
EXPORT
WINAPI
ProcessHandleToId(HANDLE ProcessHandle)
{
    return PsProcessHandleToId(ProcessHandle);
}

BOOL
InjectLibraryToAllProcesses(LPCWSTR DllFileName32,
                            LPCWSTR DllFileName64,
                            DWORD Timeout)
{
    return IlInjectLibraryToAllProcesses(DllFileName32,
                                         DllFileName64,
                                         Timeout);
}

BOOL
UninjectLibraryFromAllProcesses(LPCWSTR ModuleName32,
                                LPCWSTR ModuleName64,
                                DWORD Timeout)
{
    return IlUninjectLibraryFromAllProcesses(ModuleName32,
                                             ModuleName64,
                                             Timeout);
}

VOID
EXPORT
WINAPI
InitializeLibrary()
{

}

VOID
EXPORT
WINAPI
FinalizeLibrary()
{

}

VOID
EntryInitializeLibrary()
{
    HeapInitialize();
    EnableDebugPrivilege(TRUE);
    EnableCreateGlobalPrivilege(TRUE);
    HkInitialize();
    LOG(L"PID [%d] : %s", GetCurrentProcessId(), L"hooklib initialized");
}

VOID
EntryFinalizeLibrary()
{
    HkUninitialize();
    //EnableDebugPrivilege(FALSE);
    //EnableCreateGlobalPrivilege(FALSE);
    LOG(L"PID [%d] : %s", GetCurrentProcessId(), L"hooklib uninitialized");
}

BOOL
WINAPI
DllMain(_In_ HINSTANCE hinstDLL,
        _In_ DWORD fdwReason,
        _In_ LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            EntryInitializeLibrary();
            break;
        case DLL_PROCESS_DETACH:
            EntryFinalizeLibrary();
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}
