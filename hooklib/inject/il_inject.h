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

#ifndef __IL_INJECT_H
#define __IL_INJECT_H

#include <Windows.h>
#include "..\dll_decls.h"
#include "il_utils.h"
#include "il_utils_x86.h"
#include "il_utils_x64.h"
#include "..\fastipc\ipc.h"
#include "..\logger\logger_utils.h"

#define MESSAGE_ID_INJECT 0x10
#define MESSAGE_ID_UNINJECT 0x11

BOOL
IlInjectLibrary(
    LPCWSTR DllFileName32,
    LPCWSTR DllFileName64,
    DWORD ProcessId,
    HANDLE ProcessHandle,
    BOOL IsProcessInitialized,
    DWORD Timeout
);

BOOL
IlUninjectLibrary(
    LPCWSTR ModuleName32,
    LPCWSTR ModuleName64,
    DWORD ProcessId,
    HANDLE ProcessHandle,
    BOOL IsProcessInitialized,
    DWORD Timeout
);

BOOL
IlInjectLibraryToAllProcesses(
    LPCWSTR DllFileName32,
    LPCWSTR DllFileName64,
    DWORD Timeout
);

BOOL
IlUninjectLibraryFromAllProcesses(
    LPCWSTR ModuleName32,
    LPCWSTR ModuleName64,
    DWORD Timeout
);

#endif
