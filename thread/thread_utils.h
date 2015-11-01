#ifndef __THREAD_UTILS_H
#define __THREAD_UTILS_H

#include <Windows.h>

typedef DWORD NT_STATUS;

typedef struct _THREAD_ATTRIBUTES
{
    SIZE_T TotalLength;
    ULONG Attribute;
    SIZE_T ValueSize;
    LPVOID ValuePtr;
    PSIZE_T RetValueSize;
} THREAD_ATTRIBUTES, *PTHREAD_ATTRIBUTES;

typedef struct _CLIENT_ID
{
    LPVOID UniqueProcess;
    LPVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

#define NT_STATUS_SUCCESS 0x00000000

#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED 0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004
#define THREAD_CREATE_FLAGS_HAS_SECURITY_DESCRIPTOR 0x00000010
#define THREAD_CREATE_FLAGS_ACCESS_CHECK_IN_TARGET 0x00000020
#define THREAD_CREATE_FLAGS_INITIAL_THREAD 0x00000080

typedef
NT_STATUS
(NTAPI * NT_CREATE_THRED_EX)(
    PHANDLE ThreadHandle,
    ACCESS_MASK DesiredAccess,
    LPVOID ObjectAttributes,
    HANDLE ProcessHandle,
    PVOID StartRoutine,
    PVOID Argument,
    DWORD dwCreationFlags,
    SIZE_T ZeroBits,
    SIZE_T StackSize,
    SIZE_T MaximumStackSize,
    PTHREAD_ATTRIBUTES AttributeList
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
    PCLIENT_ID ClientId
);

HANDLE
WINAPI
NewCreateRemoteThread(
    HANDLE hProcess,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
);

#endif
