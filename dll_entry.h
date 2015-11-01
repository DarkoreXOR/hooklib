#ifndef __DLL_ENTRY_H
#define __DLL_ENTRY_H

#include <Windows.h>
#include "dll_decls.h"
#include "dll_utils.h"
#include "inject\il_utils.h"
#include "inject\il_inject.h"
#include "ipc\ipc.h"
#include "hook\hook_wrapper.h"

#if defined _M_X64
#pragma comment(lib, "libs\\minhook\\libMinHook.x64.lib")
#pragma comment(lib, "libs\\asmjit\\asmjit64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libs\\minhook\\libMinHook.x86.lib")
#pragma comment(lib, "libs\\asmjit\\asmjit32.lib")
#endif

BOOL
EXPORT
WINAPI
IsX64System();

BOOL
EXPORT
WINAPI
IsX64Process(
    DWORD ProcessId,
    PBOOL Result
);

BOOL
EXPORT
WINAPI
CreateIpcChannel(
    LPCWSTR ChannelName,
    IPC_ROUTINE Routine,
    BOOL MultiSession,
    PVOID *ChannelData
);

BOOL
EXPORT
WINAPI
DestroyIpcChannel(PVOID ChannelData);

BOOL
EXPORT
WINAPI
SendIpcMessage(
    LPCWSTR ChannelName,
    PVOID MessageBuf,
    DWORD MessageSize,
    PVOID AnswerBuf,
    DWORD AnswerSize,
    DWORD Timeout,
    BOOL MultiSession
);

BOOL
EXPORT
WINAPI
InjectLibrary(
    LPCWSTR DllFileName32,
    LPCWSTR DllFileName64,
    DWORD ProcessId,
    DWORD Timeout
);

BOOL
EXPORT
WINAPI
InjectLibrary32(
    LPCWSTR DllFileName,
    DWORD ProcessId,
    DWORD Timeout
);

BOOL
EXPORT
WINAPI
InjectLibrary64(
    LPCWSTR DllFileName,
    DWORD ProcessId,
    DWORD Timeout
);

BOOL
EXPORT
WINAPI
UninjectLibrary(
    LPCWSTR DllFileName,
    DWORD ProcessId,
    DWORD Timeout
);

BOOL
EXPORT
WINAPI
HookCode(
    LPVOID Target,
    LPVOID Detour,
    LPVOID *Original,
    BOOL SetEnabled
);

BOOL
EXPORT
WINAPI
UnhookCode(LPVOID Target);

BOOL
EXPORT
WINAPI
HookApi(
    LPCWSTR ModuleName,
    LPCSTR ProcName,
    LPVOID Detour,
    LPVOID *Original,
    BOOL SetEnabled
);

BOOL
EXPORT
WINAPI
UnhookApi(
    LPCWSTR ModuleName,
    LPCSTR ProcName
);

BOOL
EXPORT
WINAPI
EnableHook(LPVOID Target);

BOOL
EXPORT
WINAPI
DisableHook(LPVOID Target);

BOOL
EXPORT
WINAPI
EnableAllHooks();

DWORD
EXPORT
WINAPI
ProcessHandleToId(HANDLE ProcessHandle);

BOOL
EXPORT
WINAPI
DisableAllHooks();

VOID
InitializeLibrary();

#endif
