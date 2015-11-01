#include "il_utils_x86.h"

BOOL
GenerateLdrLoadDllCall32(HANDLE ProcessHandle,
                         PVOID RemoteCode,
                         PVOID LdrLoadDllProc,
                         PVOID RemotePathToFile,
                         PVOID RemoteFlags,
                         PVOID RemoteModuleFileName,
                         PVOID RemoteModuleHandle,
                         PVOID RemoteReturnValue)
{
    using namespace asmjit;
    BOOL Result;
    JitRuntime runtime;
    X86Assembler a(&runtime);
    X86Mem ReturnVarPtr;
    PVOID CodePtr;

    a.push((uint32_t)RemoteModuleHandle);
    a.push((uint32_t)RemoteModuleFileName);
    a.push((uint32_t)0);
    a.push((uint32_t)RemotePathToFile);
    a.mov(host::eax, (uint32_t)LdrLoadDllProc);
    a.call(host::eax);
    a.mov(host::ebx, (uint32_t)RemoteReturnValue);
    a.mov(X86Mem(host::ebx, 0), host::eax);
    a.ret();

    CodePtr = a.make();

    if (!CodePtr)
    {
        return FALSE;
    }

    Result = IlWrite(ProcessHandle,
                     RemoteCode,
                     CodePtr,
                     a.getCodeSize());

    runtime.release(CodePtr);

    return Result;
}

NTSTATUS
LdrLoadDllEx32(HANDLE ProcessHandle,
               PWCHAR PathToFile,
               ULONG Flags,
               PUNICODE_STRING ModuleFileName,
               HMODULE *ModuleHandle)
{
    BOOL Result;
    LPVOID LdrLoadDllProc;
    // handle
    HANDLE ThreadHandle;
    // remote args
    LPVOID RemoteCode;
    LPVOID RemotePathToFile = NULL;
    LPVOID RemoteFlags = NULL;
    LPVOID RemoteModuleFileName = NULL;
    LPVOID RemoteModuleHandle = NULL;
    // return
    LPVOID RemoteReturnValue;
    NTSTATUS ReturnValue;

    LdrLoadDllProc = GetModuleProcAddress(L"ntdll", "LdrLoadDll");

    if (!LdrLoadDllProc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (PathToFile)
    {
        // allocate PathToFile
        Result = IlAllocateWideString(ProcessHandle,
                                      PathToFile,
                                      &RemotePathToFile);

        if (!Result)
        {
            return STATUS_UNSUCCESSFUL;
        }
    }

    // allocate Flags
    Result = IlAllocateAndWrite(ProcessHandle,
                                &Flags,
                                sizeof(Flags),
                                PAGE_READWRITE,
                                &RemoteFlags);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, RemotePathToFile);
        return STATUS_UNSUCCESSFUL;
    }

    // allocate ModuleFileName
    Result = IlAllocateUnicodeString(ProcessHandle,
                                     ModuleFileName->Buffer,
                                     &RemoteModuleFileName);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, RemotePathToFile);
        IlDeallocate(ProcessHandle, RemoteFlags);
        return STATUS_UNSUCCESSFUL;
    }

    // allocate ModuleHandle
    Result = IlAllocate(ProcessHandle,
                        sizeof(HMODULE),
                        PAGE_READWRITE,
                        &RemoteModuleHandle);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, RemotePathToFile);
        IlDeallocate(ProcessHandle, RemoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, RemoteModuleFileName);
        return STATUS_UNSUCCESSFUL;
    }

    // allocate ReturnValue
    Result = IlAllocate(ProcessHandle,
                        sizeof(NTSTATUS),
                        PAGE_READWRITE,
                        &RemoteReturnValue);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, RemotePathToFile);
        IlDeallocate(ProcessHandle, RemoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, RemoteModuleFileName);
        IlDeallocate(ProcessHandle, RemoteModuleHandle);
        return STATUS_UNSUCCESSFUL;
    }

    // build code

    Result = IlAllocate(ProcessHandle,
                        0x1000,
                        PAGE_EXECUTE_READWRITE,
                        &RemoteCode);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, RemotePathToFile);
        IlDeallocate(ProcessHandle, RemoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, RemoteModuleFileName);
        IlDeallocate(ProcessHandle, RemoteModuleHandle);
        IlDeallocate(ProcessHandle, RemoteReturnValue);
        return STATUS_UNSUCCESSFUL;
    }

    Result = GenerateLdrLoadDllCall32(ProcessHandle,
                                      RemoteCode,
                                      LdrLoadDllProc,
                                      RemotePathToFile,
                                      RemoteFlags,
                                      RemoteModuleFileName,
                                      RemoteModuleHandle,
                                      RemoteReturnValue);

    ThreadHandle = NewCreateRemoteThread(ProcessHandle,
                                        NULL,
                                        0,
                                        (LPTHREAD_START_ROUTINE)RemoteCode,
                                        NULL,
                                        0,
                                        NULL);

    if (!ThreadHandle)
    {
        IlDeallocate(ProcessHandle, RemotePathToFile);
        IlDeallocate(ProcessHandle, RemoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, RemoteModuleFileName);
        IlDeallocate(ProcessHandle, RemoteModuleHandle);
        IlDeallocate(ProcessHandle, RemoteReturnValue);
        IlDeallocate(ProcessHandle, RemoteCode);
        return STATUS_UNSUCCESSFUL;
    }

    Result = WaitForSingleObject(ThreadHandle, INFINITE) == WAIT_OBJECT_0;

    CloseHandle(ThreadHandle);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, RemotePathToFile);
        IlDeallocate(ProcessHandle, RemoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, RemoteModuleFileName);
        IlDeallocate(ProcessHandle, RemoteModuleHandle);
        IlDeallocate(ProcessHandle, RemoteReturnValue);
        IlDeallocate(ProcessHandle, RemoteCode);
        return STATUS_UNSUCCESSFUL;
    }

    // read ModuleHandle
    Result = IlRead(ProcessHandle,
                    RemoteModuleHandle,
                    ModuleHandle,
                    sizeof(HMODULE));

    if (!Result)
    {
        IlDeallocate(ProcessHandle, RemotePathToFile);
        IlDeallocate(ProcessHandle, RemoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, RemoteModuleFileName);
        IlDeallocate(ProcessHandle, RemoteModuleHandle);
        IlDeallocate(ProcessHandle, RemoteReturnValue);
        IlDeallocate(ProcessHandle, RemoteCode);
        return STATUS_UNSUCCESSFUL;
    }

    // read return value
    Result = IlRead(ProcessHandle,
                    RemoteReturnValue,
                    &ReturnValue,
                    sizeof(ReturnValue));

    if (!Result)
    {
        IlDeallocate(ProcessHandle, RemotePathToFile);
        IlDeallocate(ProcessHandle, RemoteFlags);
        IlDeallocateUnicodeString(ProcessHandle, RemoteModuleFileName);
        IlDeallocate(ProcessHandle, RemoteModuleHandle);
        IlDeallocate(ProcessHandle, RemoteReturnValue);
        IlDeallocate(ProcessHandle, RemoteCode);

        return STATUS_UNSUCCESSFUL;
    }

    IlDeallocate(ProcessHandle, RemotePathToFile);
    IlDeallocate(ProcessHandle, RemoteFlags);
    IlDeallocateUnicodeString(ProcessHandle, RemoteModuleFileName);
    IlDeallocate(ProcessHandle, RemoteModuleHandle);
    IlDeallocate(ProcessHandle, RemoteReturnValue);
    IlDeallocate(ProcessHandle, RemoteCode);

    return ReturnValue;
}

BOOL
IlInjectLibrary32(LPCWSTR DllFileName,
                  DWORD ProcessId,
                  DWORD Timeout)
{
    DWORD Access = 0;
    HANDLE ProcessHandle;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    UNICODE_STRING UnicodeString;
    HMODULE ModuleHandle;

    Access |= PROCESS_VM_OPERATION;
    Access |= PROCESS_VM_READ;
    Access |= PROCESS_VM_WRITE;
    Access |= PROCESS_CREATE_THREAD;
    Access |= SYNCHRONIZE;
    Access |= PROCESS_QUERY_INFORMATION;

    ProcessHandle = OpenProcess(Access, FALSE, ProcessId);

    if (!ProcessHandle)
    {
        return FALSE;
    }

    UnicodeString.Buffer = (PWSTR)DllFileName;

    Status = LdrLoadDllEx32(ProcessHandle,
                            NULL,
                            0,
                            &UnicodeString,
                            &ModuleHandle);

    CloseHandle(ProcessHandle);

    return Status != STATUS_UNSUCCESSFUL;
}
