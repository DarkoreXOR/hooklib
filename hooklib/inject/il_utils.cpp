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

#include "il_utils.h"

BOOL
IlAllocate(HANDLE ProcessHandle,
           SIZE_T Size,
           DWORD Protect,
           LPVOID *Address)
{
    if (!Address)
    {
        return FALSE;
    }

    *Address = VirtualAllocEx(ProcessHandle,
                              NULL,
                              Size,
                              MEM_COMMIT,
                              Protect);

    if (!*Address)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL
IlWrite(HANDLE ProcessHandle,
        LPVOID Address,
        LPVOID Buffer,
        SIZE_T Size)
{
    BOOL result;

    result = WriteProcessMemory(ProcessHandle,
                                Address,
                                Buffer,
                                Size,
                                NULL);

    return result;
}

BOOL
IlRead(HANDLE ProcessHandle,
       LPVOID Address,
       LPVOID Buffer,
       SIZE_T Size)
{
    BOOL result;

    result = ReadProcessMemory(ProcessHandle,
                               Address,
                               Buffer,
                               Size,
                               NULL);

    return result;
}

BOOL
IlAllocateAndWrite(HANDLE ProcessHandle,
                   LPVOID Buffer,
                   SIZE_T Size,
                   DWORD Protect,
                   LPVOID *Address)
{
    BOOL result;

    result = IlAllocate(ProcessHandle,
                        Size,
                        Protect,
                        Address);

    if (!result)
    {
        return FALSE;
    }

    result = IlWrite(ProcessHandle,
                     *Address,
                     Buffer,
                     Size);

    if (!result)
    {
        IlDeallocate(ProcessHandle,
                     *Address);
    }

    return result;
}

BOOL
IlDeallocate(HANDLE ProcessHandle,
             LPVOID Address)
{
    if (!Address)
    {
        return FALSE;
    }

    return VirtualFreeEx(ProcessHandle,
                         Address,
                         0,
                         MEM_RELEASE);
}

BOOL
IlAllocateWideString(HANDLE ProcessHandle,
                     LPCWSTR String,
                     LPVOID *WideStringAddress)
{
    SIZE_T stringLen;
    SIZE_T stringSize;

    stringLen = wcslen(String);
    stringSize = sizeof(WCHAR) * (stringLen + 1);

    return IlAllocateAndWrite(ProcessHandle,
                              (LPVOID)String,
                              stringSize,
                              PAGE_READWRITE,
                              WideStringAddress);
}


BOOL
IlAllocateUnicodeString(HANDLE ProcessHandle,
                        LPCWSTR WideString,
                        LPVOID *UnicodeStringAddress)
{
    BOOL result;
    LPVOID allocatedDllFileName;
    NT_UNICODE_STRING unicodeTemplate;

    result = IlAllocateWideString(ProcessHandle,
                                  WideString,
                                  &allocatedDllFileName);

    if (!result)
    {
        return FALSE;
    }

    result = IlAllocate(ProcessHandle,
                        sizeof(NT_UNICODE_STRING),
                        PAGE_READWRITE,
                        UnicodeStringAddress);

    if (!result)
    {
        IlDeallocate(ProcessHandle, allocatedDllFileName);
        return FALSE;
    }

    unicodeTemplate.Buffer = (LPWSTR)allocatedDllFileName;
    unicodeTemplate.Length = (USHORT)wcslen(WideString) * 2;
    unicodeTemplate.MaximumLength = unicodeTemplate.Length + 2;

    result = IlWrite(ProcessHandle,
                     *UnicodeStringAddress,
                     &unicodeTemplate,
                     sizeof(NT_UNICODE_STRING));

    return result;
}

BOOL
IlAllocateUnicodeString32(HANDLE ProcessHandle,
                          LPCWSTR WideString,
                          LPVOID *UnicodeStringAddress)
{
    BOOL result;
    LPVOID allocatedDllFileName;
    NT_UNICODE_STRING32 unicodeTemplate;

    result = IlAllocateWideString(ProcessHandle,
                                  WideString,
                                  &allocatedDllFileName);

    if (!result)
    {
        return FALSE;
    }

    result = IlAllocate(ProcessHandle,
                        sizeof(NT_UNICODE_STRING32),
                        PAGE_READWRITE,
                        UnicodeStringAddress);

    if (!result)
    {
        IlDeallocate(ProcessHandle, allocatedDllFileName);
        return FALSE;
    }

    unicodeTemplate.Buffer = (WIN32_PTR(LPWSTR))allocatedDllFileName;
    unicodeTemplate.Length = (USHORT)wcslen(WideString) * 2;
    unicodeTemplate.MaximumLength = unicodeTemplate.Length + 2;

    result = IlWrite(ProcessHandle,
                     *UnicodeStringAddress,
                     &unicodeTemplate,
                     sizeof(NT_UNICODE_STRING32));

    return result;
}

BOOL
IlDeallocateUnicodeString(HANDLE ProcessHandle,
                          LPVOID UnicodeStringAddress)
{
    BOOL result;

    NT_UNICODE_STRING unicodeTemplate;

    result = IlRead(ProcessHandle,
                    UnicodeStringAddress,
                    &unicodeTemplate,
                    sizeof(NT_UNICODE_STRING));

    if (!result)
    {
        return FALSE;
    }

    result = IlDeallocate(ProcessHandle, UnicodeStringAddress);
    result = IlDeallocate(ProcessHandle, unicodeTemplate.Buffer) && result;

    return result;
}

BOOL
IlDeallocateUnicodeString32(HANDLE ProcessHandle,
                            LPVOID UnicodeStringAddress)
{
    BOOL result;

    NT_UNICODE_STRING32 unicodeTemplate;

    result = IlRead(ProcessHandle,
                    UnicodeStringAddress,
                    &unicodeTemplate,
                    sizeof(NT_UNICODE_STRING32));

    if (!result)
    {
        return FALSE;
    }

    result = IlDeallocate(ProcessHandle, UnicodeStringAddress);
    result = IlDeallocate(ProcessHandle, (LPVOID)(unicodeTemplate.Buffer)) && result;

    return result;
}

LPVOID
IlGetRemoteModuleProcAddress(HANDLE ProcessHandle,
                             BOOL IsProcessInitialized,
                             BOOL IsModule32,
                             LPCWSTR ModuleName,
                             LPCSTR ProcName)
{
    BOOL result;
    MODULE_INFO moduleInfo;
    LPVOID procAddress;

    if (!ModuleName || !ProcName)
    {
        return NULL;
    }

    result = PsGetModuleInfo(ProcessHandle,
                             IsProcessInitialized,
                             IsModule32,
                             ModuleName,
                             &moduleInfo);

    if (!result)
    {
        return FALSE;
    }

    procAddress = PsGetRemoteProcAddress(ProcessHandle,
                                         moduleInfo.ModuleHandle,
                                         ProcName);

    return procAddress;
}
