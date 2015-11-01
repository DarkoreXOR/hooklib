#ifndef __PROCESS_UTILS_H
#define __PROCESS_UTILS_H

#include <Windows.h>
#include <TlHelp32.h>
#include "..\string\str_utils.h"

typedef HMODULE *PHMODULE;

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
GetModuleInfoByProcessId(
    DWORD ProcessId,
    LPCWSTR ModuleName,
    PMODULE_INFO ModuleHandle
);

#endif
