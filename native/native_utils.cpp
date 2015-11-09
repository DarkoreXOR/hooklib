#include "native_utils.h"

LPVOID
__NtDllImport(LPCSTR ProcName)
{
    HMODULE ModuleHandle;

    ModuleHandle = GetModuleHandleW(L"ntdll");

    if (!ModuleHandle)
    {
        return NULL;
    }

    return GetProcAddress(ModuleHandle, ProcName);
}

NTSTATUS
NtQueryVirtualMemory(_In_ HANDLE ProcessHandle,
                     _In_ PVOID BaseAddress,
                     _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
                     _Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
                     _In_ SIZE_T MemoryInformationLength,
                     _Out_opt_ PSIZE_T ReturnLength)
{
    NT_QUERY_VIRTUAL_MEMORY proc;

    proc = NTDLL_IMPORT(NT_QUERY_VIRTUAL_MEMORY,
                        "NtQueryVirtualMemory");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(ProcessHandle,
                BaseAddress,
                MemoryInformationClass,
                MemoryInformation,
                MemoryInformationLength,
                ReturnLength);
}

NTSTATUS
RtlCreateUserThread(HANDLE ProcessHandle,
                    PSECURITY_DESCRIPTOR SecurityDescriptor,
                    BOOLEAN CreateSuspended,
                    ULONG StackZeroBits,
                    SIZE_T StackReserve,
                    SIZE_T StackCommit,
                    LPVOID StartAddress,
                    LPVOID Parameter,
                    PHANDLE ThreadHandle,
                    PNT_CLIENT_ID ClientId)
{
    RTL_CREATE_USER_THREAD proc;

    proc = NTDLL_IMPORT(RTL_CREATE_USER_THREAD,
                        "RtlCreateUserThread");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(ProcessHandle,
                SecurityDescriptor,
                CreateSuspended,
                StackZeroBits,
                StackReserve,
                StackCommit,
                StartAddress,
                Parameter,
                ThreadHandle,
                ClientId);
}
