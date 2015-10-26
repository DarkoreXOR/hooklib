#include "dll_entry.h"

BOOL
EXPORT
WINAPI
IsX64System()
{
    return IlIsX64System();
}

BOOL
EXPORT
WINAPI
IsX64Process(DWORD ProcessId,
             PBOOL Result)
{
    return IlIsX64Process(ProcessId, Result);
}

BOOL
EXPORT
WINAPI
CreateIpcChannel(LPCWSTR ChannelName,
                 IPC_ROUTINE Routine,
                 PVOID *ChannelData)
{
    return IpcCreateIpcChannel(ChannelName,
                               Routine,
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
               DWORD Timeout)
{
    return IpcSendIpcMessage(ChannelName,
                             MessageBuf,
                             MessageSize,
                             AnswerBuf,
                             AnswerSize,
                             Timeout);
}

BOOL
EXPORT
WINAPI
InjectLibrary(LPCWSTR DllFileName,
              DWORD ProcessId,
              DWORD Timeout)
{
    return IlInjectLibrary(DllFileName,
                           ProcessId,
                           Timeout);
}

BOOL
EXPORT
WINAPI
InjectLibrary32(LPCWSTR DllFileName,
                DWORD ProcessId,
                DWORD Timeout)
{
    return IlInjectLibrary32(DllFileName,
                             ProcessId,
                             Timeout);
}

BOOL
EXPORT
WINAPI
InjectLibrary64(LPCWSTR DllFileName,
                DWORD ProcessId,
                DWORD Timeout)
{
    return IlInjectLibrary64(DllFileName,
                             ProcessId,
                             Timeout);
}

BOOL
EXPORT
WINAPI
UninjectLibrary(LPCWSTR DllFileName,
                DWORD ProcessId,
                DWORD Timeout)
{
    return IlUninjectLibrary(DllFileName,
                             ProcessId,
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

VOID
InitializeLibrary()
{
    EnableDebugPrivilege(TRUE);
    HkInitialize();
}

VOID
UninitializeLibrary()
{
    HkUninitialize();
    EnableDebugPrivilege(FALSE);
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
            InitializeLibrary();
            break;
        case DLL_PROCESS_DETACH:
            UninitializeLibrary();
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}
