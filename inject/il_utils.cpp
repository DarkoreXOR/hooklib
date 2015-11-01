#include "il_utils.h"

BOOL
IlIsX64System()
{
#ifndef _M_X64
    HMODULE ModuleHandle;
    GET_NATIVE_SYSTEM_INFO GetNativeSystemInfo;
    SYSTEM_INFO SysInfo;

    ModuleHandle = GetModuleHandleW(L"kernel32");

    if (ModuleHandle == NULL)
    {
        return FALSE;
    }

    GetNativeSystemInfo = (GET_NATIVE_SYSTEM_INFO)GetProcAddress(ModuleHandle, "GetNativeSystemInfo");

    if (GetNativeSystemInfo == NULL)
    {
        return FALSE;
    }

    GetNativeSystemInfo(&SysInfo);

    if (SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
    {
        return FALSE;
    }

#endif
    return TRUE;
}

BOOL
IlIsX64Process(DWORD ProcessId,
               PBOOL Result)
{
    HMODULE ModuleHandle;
    BOOL IsTarget64Bit = FALSE;
    HANDLE ProcessHandle = NULL;
    IS_WOW64_PROCESS IsWow64ProcessProc;

    if (!Result)
    {
        return FALSE;
    }

#ifndef _M_X64
    GET_NATIVE_SYSTEM_INFO GetNativeSystemInfoProc;
    SYSTEM_INFO SysInfo;
#endif

    ModuleHandle = GetModuleHandleW(L"kernel32");

    if (ModuleHandle == NULL)
    {
        return FALSE;
    }

    ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ProcessId);

    if (ProcessHandle == NULL)
    {
        return FALSE;
    }

    IsWow64ProcessProc = (IS_WOW64_PROCESS)GetProcAddress(ModuleHandle, "IsWow64Process");

#ifdef _M_X64
    if (!IsWow64ProcessProc(ProcessHandle, &IsTarget64Bit))
    {
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    IsTarget64Bit = !IsTarget64Bit;
#else
    IsTarget64Bit = FALSE;

    if (IsWow64ProcessProc == NULL)
    {
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    GetNativeSystemInfoProc =
        (GET_NATIVE_SYSTEM_INFO)GetProcAddress(ModuleHandle, "GetNativeSystemInfo");

    if (GetNativeSystemInfoProc == NULL)
    {
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    GetNativeSystemInfoProc(&SysInfo);

    if (SysInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL)
    {
        if (!IsWow64ProcessProc(ProcessHandle, &IsTarget64Bit))
        {
            CloseHandle(ProcessHandle);
            return FALSE;
        }
        IsTarget64Bit = !IsTarget64Bit;
    }
#endif

    *Result = IsTarget64Bit;
    CloseHandle(ProcessHandle);
    return TRUE;
}

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
    BOOL Result;

    Result = WriteProcessMemory(ProcessHandle,
                                Address,
                                Buffer,
                                Size,
                                NULL);

    return Result;
}

BOOL
IlRead(HANDLE ProcessHandle,
       LPVOID Address,
       LPVOID Buffer,
       SIZE_T Size)
{
    BOOL Result;

    Result = ReadProcessMemory(ProcessHandle,
                               Address,
                               Buffer,
                               Size,
                               NULL);

    return Result;
}

BOOL
IlAllocateAndWrite(HANDLE ProcessHandle,
                   LPVOID Buffer,
                   SIZE_T Size,
                   DWORD Protect,
                   LPVOID *Address)
{
    BOOL Result;

    Result = IlAllocate(ProcessHandle, Size, Protect, Address);

    if (!Result)
    {
        return FALSE;
    }

    Result = IlWrite(ProcessHandle, *Address, Buffer, Size);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, *Address);
    }

    return Result;
}

BOOL
IlDeallocate(HANDLE ProcessHandle,
             LPVOID Address)
{
    if (!Address)
    {
        return FALSE;
    }
    return VirtualFreeEx(ProcessHandle, Address, 0, MEM_RELEASE);
}

BOOL
IlAllocateWideString(HANDLE ProcessHandle,
                     LPCWSTR String,
                     LPVOID *WideStringAddress)
{
    SIZE_T StringLen;
    SIZE_T StringSize;

    StringLen = wcslen(String);
    StringSize = sizeof(WCHAR) * (StringLen + 1);

    return IlAllocateAndWrite(ProcessHandle,
                              (LPVOID)String,
                              StringSize,
                              PAGE_READWRITE,
                              WideStringAddress);
}


BOOL
IlAllocateUnicodeString(HANDLE ProcessHandle,
                        LPCWSTR WideString,
                        LPVOID *UnicodeStringAddress)
{
    BOOL Result;
    LPVOID AllocDllFileName;
    UNICODE_STRING UnicodeTemplate;

    Result = IlAllocateWideString(ProcessHandle,
                                  WideString,
                                  &AllocDllFileName);

    if (!Result)
    {
        return FALSE;
    }

    Result = IlAllocate(ProcessHandle,
                        sizeof(UNICODE_STRING),
                        PAGE_READWRITE,
                        UnicodeStringAddress);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, AllocDllFileName);
        return FALSE;
    }

    UnicodeTemplate.Buffer = (LPWSTR)AllocDllFileName;
    UnicodeTemplate.Length = (USHORT)wcslen(WideString) * 2;
    UnicodeTemplate.MaximumLength = UnicodeTemplate.Length + 2;

    Result = IlWrite(ProcessHandle,
                     *UnicodeStringAddress,
                     &UnicodeTemplate,
                     sizeof(UNICODE_STRING));

    return Result;
}

BOOL
IlDeallocateUnicodeString(HANDLE ProcessHandle,
                          LPVOID UnicodeStringAddress)
{
    BOOL Result;

    UNICODE_STRING UnicodeTemplate;

    Result = IlRead(ProcessHandle,
                    UnicodeStringAddress,
                    &UnicodeTemplate,
                    sizeof(UNICODE_STRING));

    if (!Result)
    {
        return FALSE;
    }

    Result = IlDeallocate(ProcessHandle, UnicodeStringAddress);
    Result = IlDeallocate(ProcessHandle, UnicodeTemplate.Buffer) && Result;
    
    return Result;
}

LPVOID
GetModuleProcAddress(LPCWSTR ModuleName,
                     LPCSTR ProcName)
{
    HMODULE ModuleHandle;

    if (!ModuleName || !ProcName)
    {
        return NULL;
    }

    ModuleHandle = GetModuleHandle(ModuleName);

    if (!ModuleHandle)
    {
        return NULL;
    }

    return GetProcAddress(ModuleHandle, ProcName);
}