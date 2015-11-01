#include "il_utils_x64.h"

BOOL
GenerateLdrLoadDllCall64(HANDLE ProcessHandle,
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
    PVOID LocalCodePtr;

    a.sub(host::rsp, 0x28);
    a.mov(host::rcx, (uint64_t)RemotePathToFile);
    a.mov(host::rdx, (uint64_t)RemoteFlags);
    a.mov(host::r8, (uint64_t)RemoteModuleFileName);
    a.mov(host::r9, (uint64_t)RemoteModuleHandle);
    a.mov(host::rax, (uint64_t)LdrLoadDllProc);
    a.call(host::rax);
    a.mov(host::rbx, (uint64_t)RemoteReturnValue);
    a.mov(X86Mem(host::rbx, 0), host::rax);
    a.add(host::rsp, 0x28);
    a.ret();

    CodePtr = a.make();

    if (!CodePtr)
    {
        return FALSE;
    }

    LocalCodePtr = malloc(a.getCodeSize());

    if (!LocalCodePtr)
    {
        runtime.release(CodePtr);
        return FALSE;
    }

    CopyMemory(LocalCodePtr, CodePtr, a.getCodeSize());

    a.relocCode(LocalCodePtr, Ptr(RemoteCode));

    Result = IlWrite(ProcessHandle,
                     RemoteCode,
                     LocalCodePtr,
                     a.getCodeSize());


    free(LocalCodePtr);
    runtime.release(CodePtr);

    return Result;
}

NTSTATUS
LdrLoadDllEx64(HANDLE ProcessHandle,
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

    Result = GenerateLdrLoadDllCall64(ProcessHandle,
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
IlInjectLibrary64(LPCWSTR DllFileName,
                  DWORD ProcessId,
                  DWORD Timeout)
{
    DWORD Access = 0;
    HANDLE ProcessHandle;
    NTSTATUS Status;
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

    Status = LdrLoadDllEx64(ProcessHandle,
                            NULL,
                            0,
                            &UnicodeString,
                            &ModuleHandle);

    CloseHandle(ProcessHandle);

    return Status != STATUS_UNSUCCESSFUL;
}
