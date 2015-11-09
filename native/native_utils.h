#ifndef __NATIVE_UTILS_H
#define __NATIVE_UTILS_H

#include <Windows.h>
#include "..\logger\logger_utils.h"

LPVOID
__NtDllImport(
    LPCSTR ProcName
);

#define NTDLL_IMPORT(cast, func) ((cast)__NtDllImport(func))

#define STATUS_SUCCESS ((NTSTATUS)0x00000000)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xc0000001)
#define STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005L)

#define WOW64_PTR(type) ULONG32

typedef struct _NT_UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    WCHAR *Buffer;
} NT_UNICODE_STRING, *PNT_UNICODE_STRING;

typedef struct _NT_UNICODE_STRING32
{
    USHORT Length;
    USHORT MaximumLength;
    WOW64_PTR(WCHAR *) Buffer;
} NT_UNICODE_STRING32, *PNT_UNICODE_STRING32;

typedef struct _NT_CLIENT_ID
{
    LPVOID UniqueProcess;
    LPVOID UniqueThread;
} NT_CLIENT_ID, *PNT_CLIENT_ID;

enum MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation = 0,
    MemoryMappedFilenameInformation = 2
};

typedef
NTSTATUS
(NTAPI *NT_QUERY_VIRTUAL_MEMORY)(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
    _Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
    _In_ SIZE_T MemoryInformationLength,
    _Out_opt_ PSIZE_T ReturnLength
);

typedef
NTSTATUS
(NTAPI *LDR_LOAD_DLL)(
    PWCHAR PathToFile,
    ULONG Flags,
    PNT_UNICODE_STRING ModuleFileName,
    HMODULE *ModuleHandle
);

typedef
NTSTATUS
(NTAPI *RTL_CREATE_USER_THREAD)(
    HANDLE ProcessHandle,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    BOOLEAN CreateSuspended,
    ULONG StackZeroBits,
    SIZE_T StackReserve,
    SIZE_T StackCommit,
    LPVOID StartAddress,
    LPVOID Parameter,
    PHANDLE ThreadHandle,
    PNT_CLIENT_ID ClientId
);

NTSTATUS
NtQueryVirtualMemory(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
    _Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
    _In_ SIZE_T MemoryInformationLength,
    _Out_opt_ PSIZE_T ReturnLength
);

NTSTATUS
RtlCreateUserThread(
    HANDLE ProcessHandle,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    BOOLEAN CreateSuspended,
    ULONG StackZeroBits,
    SIZE_T StackReserve,
    SIZE_T StackCommit,
    LPVOID StartAddress,
    LPVOID Parameter,
    PHANDLE ThreadHandle,
    PNT_CLIENT_ID ClientId
);

#endif
