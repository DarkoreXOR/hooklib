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

#ifndef __NATIVE_UTILS_H
#define __NATIVE_UTILS_H

#include <Windows.h>

// DLL IMPORT HELPER

LPVOID
__NtDllImport(
    LPCSTR ProcName
);

#define NTDLL_IMPORT(cast, func) ((cast)__NtDllImport(func))

// NTDLL

#define STATUS_SUCCESS ((NTSTATUS)0x00000000)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xc0000001)
#define STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005L)

#define NT_SUCCESS(status) (((NTSTATUS)(status)) >= 0)

#define WIN32_PTR(type) ULONG32
#define WIN64_PTR(type) ULONG64

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
    WIN32_PTR(WCHAR *) Buffer;
} NT_UNICODE_STRING32, *PNT_UNICODE_STRING32;

typedef struct _NT_CLIENT_ID
{
    LPVOID UniqueProcess;
    LPVOID UniqueThread;
} NT_CLIENT_ID, *PNT_CLIENT_ID;

typedef struct _NT_CLIENT_ID32
{
    WIN32_PTR(LPVOID) UniqueProcess;
    WIN32_PTR(LPVOID) UniqueThread;
} NT_CLIENT_ID32, *PNT_CLIENT_ID32;

typedef struct _NT_CLIENT_ID64
{
    WIN64_PTR(LPVOID) UniqueProcess;
    WIN64_PTR(LPVOID) UniqueThread;
} NT_CLIENT_ID64, *PNT_CLIENT_ID64;

typedef struct _NT_PORT_MESSAGE
{
    union
    {
        struct
        {
            INT16 DataLength;
            INT16 TotalLength;
        };
        UINT32 Length;
    };

    union
    {
        struct
        {
            INT16 Type;
            INT16 DataInfoOffset;
        };
        UINT32 ZeroInit;
    };

    union
    {
        NT_CLIENT_ID ClientId;
        long double DoNotUseThisField;
    };

    UINT MessageId;

    union
    {
        SIZE_T ClientViewSize;
        UINT32 CallbackId;
    };
} NT_PORT_MESSAGE, *PNT_PORT_MESSAGE;

typedef struct _NT_PORT_MESSAGE32
{
    union
    {
        struct
        {
            INT16 DataLength;
            INT16 TotalLength;
        };
        UINT32 Length;
    };

    union
    {
        struct
        {
            INT16 Type;
            INT16 DataInfoOffset;
        };
        UINT32 ZeroInit;
    };

    union
    {
        NT_CLIENT_ID32 ClientId;
        long double DoNotUseThisField;
    };

    UINT MessageId;

    union
    {
        UINT32 ClientViewSize;
        UINT32 CallbackId;
    };
} NT_PORT_MESSAGE32, *PNT_PORT_MESSAGE32;

typedef struct _NT_PORT_MESSAGE64
{
    union
    {
        struct
        {
            INT16 DataLength;
            INT16 TotalLength;
        };
        UINT32 Length;
    };

    union
    {
        struct
        {
            INT16 Type;
            INT16 DataInfoOffset;
        };
        UINT32 ZeroInit;
    };

    union
    {
        NT_CLIENT_ID64 ClientId;
        double DoNotUseThisField;
    };

    UINT MessageId;

    union
    {
        UINT64 ClientViewSize;
        UINT CallbackId;
    };
} NT_PORT_MESSAGE64, *PNT_PORT_MESSAGE64;

typedef LPVOID *PPORT_MESSAGE;

#define InitializeMessageHeader32(ph, pms, l, t) { \
    ((PNT_PORT_MESSAGE32)ph)->TotalLength = (INT16)(l + pms); \
    ((PNT_PORT_MESSAGE32)ph)->DataLength = (INT16)(l); \
    ((PNT_PORT_MESSAGE32)ph)->Type = (INT16)(t); \
    ((PNT_PORT_MESSAGE32)ph)->DataInfoOffset = 0; \
    ((PNT_PORT_MESSAGE32)ph)->ClientId.UniqueProcess = NULL; \
    ((PNT_PORT_MESSAGE32)ph)->ClientId.UniqueThread = NULL; \
    ((PNT_PORT_MESSAGE32)ph)->MessageId = 0; \
    ((PNT_PORT_MESSAGE32)ph)->ClientViewSize = 0; } \

#define InitializeMessageHeader64(ph, pms, l, t) { \
    ((PNT_PORT_MESSAGE64)ph)->TotalLength = (INT16)(l + pms); \
    ((PNT_PORT_MESSAGE64)ph)->DataLength = (INT16)(l); \
    ((PNT_PORT_MESSAGE64)ph)->Type = (INT16)(t); \
    ((PNT_PORT_MESSAGE64)ph)->DataInfoOffset = 0; \
    ((PNT_PORT_MESSAGE64)ph)->ClientId.UniqueProcess = NULL; \
    ((PNT_PORT_MESSAGE64)ph)->ClientId.UniqueThread = NULL; \
    ((PNT_PORT_MESSAGE64)ph)->MessageId = 0; \
    ((PNT_PORT_MESSAGE64)ph)->ClientViewSize = 0; } \

typedef struct _NT_OBJECT_ATTRIBUTES
{
    ULONG Length;
    HANDLE RootDirectory;
    PNT_UNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} NT_OBJECT_ATTRIBUTES, *PNT_OBJECT_ATTRIBUTES;

#define InitializeObjectAttributes(p, n, a, r, s) { \
    (p)->Length = sizeof(NT_OBJECT_ATTRIBUTES); \
    (p)->RootDirectory = r; \
    (p)->Attributes = a; \
    (p)->ObjectName = n; \
    (p)->SecurityDescriptor = s; \
    (p)->SecurityQualityOfService = NULL; } \

typedef struct _NT_SECURITY_QUALITY_OF_SERVICE
{
    DWORD Length;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    SECURITY_CONTEXT_TRACKING_MODE ContextTrackingMode;
    BOOLEAN EffectiveOnly;
} NT_SECURITY_QUALITY_OF_SERVICE, *PNT_SECURITY_QUALITY_OF_SERVICE;

typedef struct _NT_PORT_VIEW
{
    ULONG Length;
    HANDLE SectionHandle;
    ULONG SectionOffset;
    SIZE_T ViewSize;
    PVOID ViewBase;
    PVOID ViewRemoteBase;
} NT_PORT_VIEW, *PNT_PORT_VIEW;

typedef struct _NT_REMOTE_PORT_VIEW
{
    ULONG Length;
    SIZE_T ViewSize;
    PVOID ViewBase;
} NT_REMOTE_PORT_VIEW, *PNT_REMOTE_PORT_VIEW;

enum NT_MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation = 0,
    MemoryMappedFilenameInformation = 2
};

/*
typedef
NTSTATUS
(NTAPI *LDR_LOAD_DLL)(
    PWCHAR PathToFile,
    ULONG Flags,
    PNT_UNICODE_STRING ModuleFileName,
    HMODULE *ModuleHandle
);
*/

NTSTATUS
NtQueryVirtualMemory(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress,
    IN NT_MEMORY_INFORMATION_CLASS MemoryInformationClass,
    OUT PVOID MemoryInformation,
    IN SIZE_T MemoryInformationLength,
    OUT PSIZE_T ReturnLength OPTIONAL
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

NTSTATUS
NTAPI
NtCreatePort(
    OUT PHANDLE PortHandle,
    IN PNT_OBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG MaxConnectionInfoLength,
    IN ULONG MaxMessageLength,
    IN ULONG MaxPoolUsage
);

VOID
NTAPI
RtlInitUnicodeString(
    PNT_UNICODE_STRING DestinationString,
    PCWSTR SourceString
);

NTSTATUS
NTAPI
NtListenPort(
    IN HANDLE PortHandle,
    OUT PPORT_MESSAGE RequestMessage
);

NTSTATUS
NTAPI
NtConnectPort(
    OUT PHANDLE PortHandle,
    IN PNT_UNICODE_STRING PortName,
    IN PNT_SECURITY_QUALITY_OF_SERVICE SecurityQos,
    IN OUT PNT_PORT_VIEW ClientView OPTIONAL,
    OUT PNT_REMOTE_PORT_VIEW ServerView OPTIONAL,
    OUT PULONG MaxMessageLength OPTIONAL,
    IN OUT PVOID ConnectionInformation OPTIONAL,
    IN OUT PULONG ConnectionInformationLength OPTIONAL
);

NTSTATUS
NTAPI
NtAcceptConnectPort(
    OUT PHANDLE PortHandle,
    IN PVOID PortContext OPTIONAL,
    IN PPORT_MESSAGE ConnectionRequest,
    IN BOOLEAN AcceptConnection,
    IN OUT PNT_PORT_VIEW ServerView OPTIONAL,
    OUT PNT_REMOTE_PORT_VIEW ClientView OPTIONAL
);

NTSTATUS
NTAPI
NtRequestPort(
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE RequestMessage
);

NTSTATUS
NTAPI
NtRequestWaitReplyPort(
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE RequestMessage,
    OUT PPORT_MESSAGE ReplyMessage
);

NTSTATUS
NTAPI
NtClose(
    IN HANDLE Handle
);

NTSTATUS
NTAPI
NtCompleteConnectPort(
    IN HANDLE PortHandle
);

NTSTATUS
NTAPI
NtReplyWaitReceivePort(
    IN HANDLE PortHandle,
    OUT PVOID *PortContext OPTIONAL,
    IN PPORT_MESSAGE ReplyMessage OPTIONAL,
    OUT PPORT_MESSAGE ReceiveMessage
);

NTSTATUS
NTAPI
NtReplyPort(
    IN HANDLE PortHandle,
    IN PPORT_MESSAGE ReplyMessage
);

NTSTATUS
NTAPI
NtReplyWaitReplyPort(
    IN HANDLE PortHandle,
    IN OUT PPORT_MESSAGE Reply
);

#endif
