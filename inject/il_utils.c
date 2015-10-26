#include "il_utils.h"

BOOL
IlGetLoadLibraryWProcAddress(PVOID *LoadLibraryWPtr)
{
    if (LoadLibraryWPtr == NULL)
    {
        return FALSE;
    }

    HMODULE ModuleHandle = GetModuleHandleW(L"kernel32");

    if (!ModuleHandle)
    {
        return FALSE;
    }

    *LoadLibraryWPtr = GetProcAddress(ModuleHandle, "LoadLibraryW");

    return *LoadLibraryWPtr != NULL;
}

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
IlAllocateAndWrite(HANDLE ProcessHandle,
                   LPVOID Buffer,
                   SIZE_T Size,
                   DWORD Protect,
                   LPVOID *Address)
{
    BOOL Result;

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

    Result = WriteProcessMemory(ProcessHandle,
                                *Address,
                                Buffer,
                                Size,
                                NULL);

    if (!Result)
    {
        VirtualFreeEx(ProcessHandle, *Address, 0, MEM_RELEASE);
    }

    return Result;
}

BOOL
IlDeallocate(HANDLE ProcessHandle,
             LPVOID Address)
{
    return VirtualFreeEx(ProcessHandle, Address, 0, MEM_RELEASE);
}

BOOL
IlAllocateCode(HANDLE ProcessHandle,
               PBYTE CodeBuf,
               SIZE_T CodeSize,
               LPVOID *Address)
{
    return IlAllocateAndWrite(ProcessHandle,
                              CodeBuf,
                              CodeSize,
                              PAGE_EXECUTE_READWRITE,
                              Address);
}

BOOL
IlAllocateWideString(HANDLE ProcessHandle,
                     LPCWSTR String,
                     LPVOID *Address)
{
    SIZE_T StringLen;
    SIZE_T StringSize;

    StringLen = wcslen(String);
    StringSize = sizeof(WCHAR) * (StringLen + 1);

    return IlAllocateAndWrite(ProcessHandle,
                              (LPVOID)String,
                              StringSize,
                              PAGE_READWRITE,
                              Address);
}


