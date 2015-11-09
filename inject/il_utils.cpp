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

    unicodeTemplate.Buffer = (WOW64_PTR(LPWSTR)) allocatedDllFileName;
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
