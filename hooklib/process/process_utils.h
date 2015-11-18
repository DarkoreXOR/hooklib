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
