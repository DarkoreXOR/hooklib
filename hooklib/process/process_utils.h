#ifndef __PROCESS_UTILS_H
#define __PROCESS_UTILS_H

#include <Windows.h>
#include <TlHelp32.h>
#include "../memory/heap.h"
#include "../string/string_utils.h"
#include "../system/system_utils.h"
#include "../native/native_utils.h"
#include "../logger/logger_utils.h"
#include "../defines.h"
#include <Psapi.h>

#define MAX_EXPORT_SYMBOL_NAME_LENGTH 0x100

typedef struct _MODULE_INFO
{
    HMODULE ModuleHandle;
    WCHAR ModuleName[MAX_PATH];
    WCHAR ModulePath[MAX_PATH];
} MODULE_INFO, *PMODULE_INFO;


typedef struct _PROCESS_INFO
{
    DWORD ParentProcessId;
    DWORD ProcessId;
} PROCESS_INFO, *PPROCESS_INFO;

typedef
BOOL
(WINAPI *IS_WOW64_PROCESS)(
    HANDLE ProcessHandle,
    PBOOL Result
);

BOOL
PsIsX64Process(
    HANDLE ProcessHandle,
    PBOOL Result
);

BOOL
PsGetProcesses(
    PROCESS_INFO *Entries,
    SIZE_T NumOfEntries,
    PSIZE_T RealNumOfEntries
);

BOOL
PsGetModules(
    DWORD ProcessId,
    MODULE_INFO *Entries,
    SIZE_T NumOfEntries,
    PSIZE_T RealNumOfEntries
);

BOOL
PsGetModulesByProcessHandle(
    HANDLE ProcessHandle,
    MODULE_INFO *Entries,
    SIZE_T NumOfEntries,
    PSIZE_T RealNumOfEntries
);

BOOL
PsGetProcessMappedModules(
    HANDLE ProcessHandle,
    MODULE_INFO *Entries,
    SIZE_T NumOfEntries,
    PSIZE_T RealNumOfEntries
);

BOOL
PsGetModuleInfo(
    HANDLE ProcessHandle,
    BOOL IsProcessInitialized,
    BOOL IsModule32,
    LPCWSTR ModuleName,
    PMODULE_INFO ModuleInfo
);

DWORD
PsProcessHandleToId(
    HANDLE ProcessHandle
);

HMODULE
PsGetRemoteModuleHandle(
    HANDLE hProcess,
    LPCSTR lpModuleName
);

LPVOID
PsGetRemoteProcAddress(
    HANDLE ProcessHandle,
    HMODULE ModuleHandle,
    LPCSTR ProcName
);

BOOL
PsCopyHandle(
    HANDLE ProcessHandle,
    HANDLE SourceHandle,
    PHANDLE DestinationHandle,
    DWORD DesiredAccess,
    BOOL InheritHandle
);

#endif
