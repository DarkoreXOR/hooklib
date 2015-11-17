#include "il_utils_x64.h"

BOOL
MakeLdrLoadDllCall64(HANDLE ProcessHandle,
                     PVOID RemoteCode,
                     PVOID LdrLoadDllProc,
                     PVOID RemotePathToFile,
                     PVOID RemoteFlags,
                     PVOID RemoteModuleFileName,
                     PVOID RemoteModuleHandle,
                     PVOID RemoteReturnValue)
{
    using namespace asmjit;
    BOOL result;
    JitRuntime runtime;
    X86Assembler a(&runtime, kArchX64);
    PVOID codeBase;

    a.sub(x86::rsp, 0x28);
    a.mov(x86::rcx, (uint64_t)RemotePathToFile);
    a.mov(x86::rdx, (uint64_t)RemoteFlags);
    a.mov(x86::r8, (uint64_t)RemoteModuleFileName);
    a.mov(x86::r9, (uint64_t)RemoteModuleHandle);
    a.mov(x86::rax, (uint64_t)LdrLoadDllProc);
    a.call(x86::rax);
    a.mov(x86::rbx, (uint64_t)RemoteReturnValue);
    a.mov(x86::qword_ptr(x86::rbx), x86::rax);
    a.add(x86::rsp, 0x28);
    a.ret();

    codeBase = a.make();

    if (!codeBase)
    {
        return FALSE;
    }

    result = IlWrite(ProcessHandle,
                     RemoteCode,
                     codeBase,
                     a.getCodeSize());

    runtime.release(codeBase);

    return result;
}

NTSTATUS
LdrLoadDllEx64(HANDLE ProcessHandle,
               BOOL IsProcessInitialized,
               PWCHAR PathToFile,
               ULONG Flags,
               PNT_UNICODE_STRING ModuleFileName,
               HMODULE *ModuleHandle,
               DWORD Timeout)
{
    BOOL result;
    LPVOID ldrLoadDllProc;
    // handle
    HANDLE threadHandle;
    // remote args
    LPVOID remoteCode;
    LPVOID remotePathToFile = NULL;
    LPVOID remoteFlags = NULL;
    LPVOID remoteModuleFileName = NULL;
    LPVOID remoteModuleHandle = NULL;
    // return
    LPVOID remoteReturnValue;
    NTSTATUS returnValue;

    ldrLoadDllProc = IlGetRemoteModuleProcAddress(ProcessHandle,
                                                  IsProcessInitialized,
                                                  FALSE,
                                                  L"ntdll.dll",
                                                  "LdrLoadDll");

    if (!ldrLoadDllProc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (PathToFile)
    {
        result = IlAllocateWideString(ProcessHandle,
                                      PathToFile,
                                      &remotePathToFile);

        if (!result)
        {
            return STATUS_UNSUCCESSFUL;
        }
    }

    result = IlAllocateAndWrite(ProcessHandle,
                                &Flags,
                                sizeof(Flags),
                                PAGE_READWRITE,
                                &remoteFlags);

    if (!result)
    {
        IlDeallocate(ProcessHandle, remotePathToFile);
        return STATUS_UNSUCCESSFUL;
    }

    result = IlAllocateUnicodeString(ProcessHandle,
                                     ModuleFileName->Buffer,
                                     &remoteModuleFileName);

    if (!result)
    {
        IlDeallocate(ProcessHandle, remotePathToFile);
        IlDeallocate(ProcessHandle, remoteFlags);
        return STATUS_UNSUCCESSFUL;
    }

    result = IlAllocate(ProcessHandle,
                        sizeof(HMODULE),
                        PAGE_READWRITE,
                        &remoteModuleHandle);

    if (!result)
    {
        IlDeallocate(ProcessHandle, remotePathToFile);
        IlDeallocate(ProcessHandle, remoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, remoteModuleFileName);
        return STATUS_UNSUCCESSFUL;
    }

    result = IlAllocate(ProcessHandle,
                        sizeof(NTSTATUS),
                        PAGE_READWRITE,
                        &remoteReturnValue);

    if (!result)
    {
        IlDeallocate(ProcessHandle, remotePathToFile);
        IlDeallocate(ProcessHandle, remoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, remoteModuleFileName);
        IlDeallocate(ProcessHandle, remoteModuleHandle);
        return STATUS_UNSUCCESSFUL;
    }

    result = IlAllocate(ProcessHandle,
                        0x1000,
                        PAGE_EXECUTE_READWRITE,
                        &remoteCode);

    if (!result)
    {
        IlDeallocate(ProcessHandle, remotePathToFile);
        IlDeallocate(ProcessHandle, remoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, remoteModuleFileName);
        IlDeallocate(ProcessHandle, remoteModuleHandle);
        IlDeallocate(ProcessHandle, remoteReturnValue);
        return STATUS_UNSUCCESSFUL;
    }

    result = MakeLdrLoadDllCall64(ProcessHandle,
                                  remoteCode,
                                  ldrLoadDllProc,
                                  remotePathToFile,
                                  remoteFlags,
                                  remoteModuleFileName,
                                  remoteModuleHandle,
                                  remoteReturnValue);

    threadHandle = ThdNewCreateRemoteThread(ProcessHandle,
                                            NULL,
                                            0,
                                            (LPTHREAD_START_ROUTINE)remoteCode,
                                            NULL,
                                            0,
                                            NULL);

    if (!threadHandle)
    {
        IlDeallocate(ProcessHandle, remotePathToFile);
        IlDeallocate(ProcessHandle, remoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, remoteModuleFileName);
        IlDeallocate(ProcessHandle, remoteModuleHandle);
        IlDeallocate(ProcessHandle, remoteReturnValue);
        IlDeallocate(ProcessHandle, remoteCode);
        return STATUS_UNSUCCESSFUL;
    }

    ResumeThread(threadHandle);

    result = WaitForSingleObject(threadHandle, Timeout) == WAIT_OBJECT_0;

    CloseHandle(threadHandle);

    if (!result)
    {
        IlDeallocate(ProcessHandle, remotePathToFile);
        IlDeallocate(ProcessHandle, remoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, remoteModuleFileName);
        IlDeallocate(ProcessHandle, remoteModuleHandle);
        IlDeallocate(ProcessHandle, remoteReturnValue);
        IlDeallocate(ProcessHandle, remoteCode);
        return STATUS_UNSUCCESSFUL;
    }

    result = IlRead(ProcessHandle,
                    remoteModuleHandle,
                    ModuleHandle,
                    sizeof(HMODULE));

    if (!result)
    {
        IlDeallocate(ProcessHandle, remotePathToFile);
        IlDeallocate(ProcessHandle, remoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, remoteModuleFileName);
        IlDeallocate(ProcessHandle, remoteModuleHandle);
        IlDeallocate(ProcessHandle, remoteReturnValue);
        IlDeallocate(ProcessHandle, remoteCode);
        return STATUS_UNSUCCESSFUL;
    }

    result = IlRead(ProcessHandle,
                    remoteReturnValue,
                    &returnValue,
                    sizeof(returnValue));

    if (!result)
    {
        IlDeallocate(ProcessHandle, remotePathToFile);
        IlDeallocate(ProcessHandle, remoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, remoteModuleFileName);
        IlDeallocate(ProcessHandle, remoteModuleHandle);
        IlDeallocate(ProcessHandle, remoteReturnValue);
        IlDeallocate(ProcessHandle, remoteCode);

        return STATUS_UNSUCCESSFUL;
    }

    IlDeallocate(ProcessHandle, remotePathToFile);
    IlDeallocate(ProcessHandle, remoteFlags);
    IlDeallocateUnicodeString(ProcessHandle, remoteModuleFileName);
    IlDeallocate(ProcessHandle, remoteModuleHandle);
    IlDeallocate(ProcessHandle, remoteReturnValue);
    IlDeallocate(ProcessHandle, remoteCode);

    return returnValue;
}

BOOL
MakeLdrUnloadDllCall64(HANDLE ProcessHandle,
                       PVOID RemoteCode,
                       PVOID LdrUnloadDllProc,
                       HMODULE RemoteModuleHandle,
                       PVOID RemoteReturnValue)
{
    using namespace asmjit;
    BOOL result;
    JitRuntime runtime;
    X86Assembler a(&runtime, kArchX64);
    PVOID codeBase;

    a.sub(x86::rsp, 0x28);
    a.mov(x86::rcx, (uint64_t)RemoteModuleHandle);
    a.mov(x86::rax, (uint64_t)LdrUnloadDllProc);
    a.call(x86::rax);
    a.mov(x86::rbx, (uint64_t)RemoteReturnValue);
    a.mov(x86::qword_ptr(x86::rbx), x86::rax);
    a.add(x86::rsp, 0x28);
    a.ret();

    codeBase = a.make();

    if (!codeBase)
    {
        return FALSE;
    }

    result = IlWrite(ProcessHandle,
                     RemoteCode,
                     codeBase,
                     a.getCodeSize());

    runtime.release(codeBase);

    return result;
}

NTSTATUS
LdrUnloadDllEx64(HANDLE ProcessHandle,
                 BOOL IsProcessInitialized,
                 HMODULE ModuleHandle,
                 DWORD Timeout)
{
    BOOL result;
    LPVOID ldrUnloadDllProc;
    // handle
    HANDLE threadHandle;
    // remote args
    LPVOID remoteCode;
    HMODULE remoteModuleHandle = NULL;
    // return
    LPVOID remoteReturnValue;
    NTSTATUS returnValue;

    ldrUnloadDllProc = IlGetRemoteModuleProcAddress(ProcessHandle,
                                                    IsProcessInitialized,
                                                    FALSE,
                                                    L"ntdll.dll",
                                                    "LdrUnloadDll");

    if (!ldrUnloadDllProc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    remoteModuleHandle = ModuleHandle;

    // allocate ReturnValue
    result = IlAllocate(ProcessHandle,
                        sizeof(NTSTATUS),
                        PAGE_READWRITE,
                        &remoteReturnValue);

    if (!result)
    {
        return STATUS_UNSUCCESSFUL;
    }

    // build code

    result = IlAllocate(ProcessHandle,
                        0x1000,
                        PAGE_EXECUTE_READWRITE,
                        &remoteCode);

    if (!result)
    {
        IlDeallocate(ProcessHandle, remoteReturnValue);
        return STATUS_UNSUCCESSFUL;
    }

    result = MakeLdrUnloadDllCall64(ProcessHandle,
                                    remoteCode,
                                    ldrUnloadDllProc,
                                    remoteModuleHandle,
                                    remoteReturnValue);

    threadHandle = ThdNewCreateRemoteThread(ProcessHandle,
                                         NULL,
                                         0,
                                         (LPTHREAD_START_ROUTINE)remoteCode,
                                         NULL,
                                         0,
                                         NULL);

    if (!threadHandle)
    {
        IlDeallocate(ProcessHandle, remoteReturnValue);
        IlDeallocate(ProcessHandle, remoteCode);
        return STATUS_UNSUCCESSFUL;
    }

    result = ResumeThread(threadHandle) != (DWORD)(-1);

    if (!result)
    {
        TerminateProcess(threadHandle, 1);
        CloseHandle(threadHandle);
        IlDeallocate(ProcessHandle, remoteReturnValue);
        IlDeallocate(ProcessHandle, remoteCode);
        return STATUS_UNSUCCESSFUL;
    }

    result = WaitForSingleObject(threadHandle, Timeout) == WAIT_OBJECT_0;

    CloseHandle(threadHandle);

    if (!result)
    {
        IlDeallocate(ProcessHandle, remoteReturnValue);
        IlDeallocate(ProcessHandle, remoteCode);
        return STATUS_UNSUCCESSFUL;
    }

    // read return value
    result = IlRead(ProcessHandle,
                    remoteReturnValue,
                    &returnValue,
                    sizeof(returnValue));

    if (!result)
    {
        IlDeallocate(ProcessHandle, remoteReturnValue);
        IlDeallocate(ProcessHandle, remoteCode);

        return STATUS_UNSUCCESSFUL;
    }

    IlDeallocate(ProcessHandle, remoteReturnValue);
    IlDeallocate(ProcessHandle, remoteCode);

    return returnValue;
}

BOOL
IlInjectLibrary64(LPCWSTR DllFileName,
                  HANDLE ProcessHandle,
                  BOOL IsProcessInitialized,
                  DWORD Timeout)
{
    NTSTATUS status;
    NT_UNICODE_STRING unicodeString;
    HMODULE moduleHandle;

    if (!ProcessHandle)
    {
        return FALSE;
    }

    unicodeString.Buffer = (PWSTR)DllFileName;

    status = LdrLoadDllEx64(ProcessHandle,
                            IsProcessInitialized,
                            NULL,
                            0,
                            &unicodeString,
                            &moduleHandle,
                            Timeout);

    return status != STATUS_UNSUCCESSFUL;
}

BOOL
IlUninjectLibrary64(LPCWSTR ModuleName,
                    HANDLE ProcessHandle,
                    BOOL IsProcessInitialized,
                    DWORD Timeout)
{
    BOOL result;
    NTSTATUS status;
    MODULE_INFO moduleInfo;

    if (!ProcessHandle)
    {
        return FALSE;
    }

    result = PsGetModuleInfo(ProcessHandle,
                             IsProcessInitialized,
                             FALSE,
                             ModuleName,
                             &moduleInfo);

    if (!result)
    {
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    status = LdrUnloadDllEx64(ProcessHandle,
                              IsProcessInitialized,
                              moduleInfo.ModuleHandle,
                              Timeout);

    return status != STATUS_UNSUCCESSFUL;
}
