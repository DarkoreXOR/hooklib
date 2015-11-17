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
NtQueryVirtualMemory(IN HANDLE ProcessHandle,
                     IN PVOID BaseAddress,
                     IN NT_MEMORY_INFORMATION_CLASS MemoryInformationClass,
                     OUT PVOID MemoryInformation,
                     IN SIZE_T MemoryInformationLength,
                     OUT PSIZE_T ReturnLength OPTIONAL)
{
    typedef
    NTSTATUS
    (NTAPI *NT_QUERY_VIRTUAL_MEMORY)(
        IN HANDLE ProcessHandle,
        IN PVOID BaseAddress,
        IN NT_MEMORY_INFORMATION_CLASS MemoryInformationClass,
        OUT PVOID MemoryInformation,
        IN SIZE_T MemoryInformationLength,
        OUT PSIZE_T ReturnLength OPTIONAL
    );

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

VOID
NTAPI
RtlInitUnicodeString(PNT_UNICODE_STRING DestinationString,
                     PCWSTR SourceString)
{
    typedef
    VOID
    (NTAPI *RTL_INIT_UNICODE_STRING)(
        PNT_UNICODE_STRING DestinationString,
        PCWSTR SourceString
    );

    RTL_INIT_UNICODE_STRING proc;
    
    proc = NTDLL_IMPORT(RTL_INIT_UNICODE_STRING,
                        "RtlInitUnicodeString");

    if (!proc)
    {
        return;
    }

    return proc(DestinationString,
                SourceString);
}

NTSTATUS
NTAPI
NtCreatePort(OUT PHANDLE PortHandle,
             IN PNT_OBJECT_ATTRIBUTES ObjectAttributes,
             IN ULONG MaxConnectionInfoLength,
             IN ULONG MaxMessageLength,
             IN ULONG MaxPoolUsage)
{
    typedef
    NTSTATUS
    (NTAPI *NT_CREATE_PORT)(
        OUT PHANDLE PortHandle,
        IN PNT_OBJECT_ATTRIBUTES ObjectAttributes,
        IN ULONG MaxConnectionInfoLength,
        IN ULONG MaxMessageLength,
        IN ULONG MaxPoolUsage
    );

    NT_CREATE_PORT proc;
    
    proc = NTDLL_IMPORT(NT_CREATE_PORT,
                        "NtCreatePort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle,
                ObjectAttributes,
                MaxConnectionInfoLength,
                MaxMessageLength,
                MaxPoolUsage);
}

NTSTATUS
NTAPI
NtListenPort(IN HANDLE PortHandle,
             OUT PPORT_MESSAGE RequestMessage)
{
    typedef
    NTSTATUS
    (NTAPI *NT_LISTEN_PORT)(
        IN HANDLE PortHandle,
        OUT PPORT_MESSAGE RequestMessage
    );

    NT_LISTEN_PORT proc;
    
    proc = NTDLL_IMPORT(NT_LISTEN_PORT,
                        "NtListenPort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle,
                RequestMessage);
}

NTSTATUS
NTAPI
NtConnectPort(OUT PHANDLE PortHandle,
              IN PNT_UNICODE_STRING PortName,
              IN PNT_SECURITY_QUALITY_OF_SERVICE SecurityQos,
              IN OUT PNT_PORT_VIEW ClientView OPTIONAL,
              OUT PNT_REMOTE_PORT_VIEW ServerView OPTIONAL,
              OUT PULONG MaxMessageLength OPTIONAL,
              IN OUT PVOID ConnectionInformation OPTIONAL,
              IN OUT PULONG ConnectionInformationLength OPTIONAL)
{
    typedef
    NTSTATUS
    (NTAPI *NT_CONNECT_PORT)(
        OUT PHANDLE PortHandle,
        IN PNT_UNICODE_STRING PortName,
        IN PNT_SECURITY_QUALITY_OF_SERVICE SecurityQos,
        IN OUT PNT_PORT_VIEW ClientView OPTIONAL,
        OUT PNT_REMOTE_PORT_VIEW ServerView OPTIONAL,
        OUT PULONG MaxMessageLength OPTIONAL,
        IN OUT PVOID ConnectionInformation OPTIONAL,
        IN OUT PULONG ConnectionInformationLength OPTIONAL
    );

    NT_CONNECT_PORT proc;
    
    proc = NTDLL_IMPORT(NT_CONNECT_PORT,
                        "NtConnectPort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle,
                PortName,
                SecurityQos,
                ClientView,
                ServerView,
                MaxMessageLength,
                ConnectionInformation,
                ConnectionInformationLength);
}

NTSTATUS
NTAPI
NtAcceptConnectPort(OUT PHANDLE PortHandle,
                    IN PVOID PortContext OPTIONAL,
                    IN PPORT_MESSAGE ConnectionRequest,
                    IN BOOLEAN AcceptConnection,
                    IN OUT PNT_PORT_VIEW ServerView OPTIONAL,
                    OUT PNT_REMOTE_PORT_VIEW ClientView OPTIONAL)
{
    typedef 
    NTSTATUS
    (NTAPI *NT_ACCEPT_CONNECT_PORT)(
        OUT PHANDLE PortHandle,
        IN PVOID PortContext OPTIONAL,
        IN PPORT_MESSAGE ConnectionRequest,
        IN BOOLEAN AcceptConnection,
        IN OUT PNT_PORT_VIEW ServerView OPTIONAL,
        OUT PNT_REMOTE_PORT_VIEW ClientView OPTIONAL
    );

    NT_ACCEPT_CONNECT_PORT proc;
    
    proc = NTDLL_IMPORT(NT_ACCEPT_CONNECT_PORT,
                        "NtAcceptConnectPort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle,
                PortContext,
                ConnectionRequest,
                AcceptConnection,
                ServerView,
                ClientView);
}

NTSTATUS
NTAPI
NtRequestPort(IN HANDLE PortHandle,
              IN PPORT_MESSAGE RequestMessage)
{
    typedef
    NTSTATUS
    (NTAPI *NT_REQUEST_PORT)(
        IN HANDLE PortHandle,
        IN PPORT_MESSAGE RequestMessage
    );

    NT_REQUEST_PORT proc;
    
    proc = NTDLL_IMPORT(NT_REQUEST_PORT,
                        "NtRequestPort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle,
                RequestMessage);
}

NTSTATUS
NTAPI
NtRequestWaitReplyPort(IN HANDLE PortHandle,
                       IN PPORT_MESSAGE RequestMessage,
                       OUT PPORT_MESSAGE ReplyMessage)
{
    typedef
    NTSTATUS
    (NTAPI *NT_REQUEST_WAIT_REPLY_PORT)(
        IN HANDLE PortHandle,
        IN PPORT_MESSAGE RequestMessage,
        OUT PPORT_MESSAGE ReplyMessage
    );

    NT_REQUEST_WAIT_REPLY_PORT proc;
    
    proc = NTDLL_IMPORT(NT_REQUEST_WAIT_REPLY_PORT,
                        "NtRequestWaitReplyPort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle,
                RequestMessage,
                ReplyMessage);
}

NTSTATUS
NTAPI
NtClose(IN HANDLE Handle)
{
    typedef
    NTSTATUS
    (NTAPI *NT_CLOSE)(
        IN HANDLE Handle
    );

    NT_CLOSE proc;
    
    proc = NTDLL_IMPORT(NT_CLOSE,
                        "NtClose");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(Handle);
}

NTSTATUS
NTAPI
NtCompleteConnectPort(IN HANDLE PortHandle)
{
    typedef
    NTSTATUS
    (NTAPI *NT_COMPLETE_CONNECT_PORT)(
        IN HANDLE PortHandle
    );

    NT_COMPLETE_CONNECT_PORT proc;
    
    proc = NTDLL_IMPORT(NT_COMPLETE_CONNECT_PORT,
                        "NtCompleteConnectPort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle);
}

NTSTATUS
NTAPI
NtReplyWaitReceivePort(IN HANDLE PortHandle,
                       OUT PVOID *PortContext OPTIONAL,
                       IN PPORT_MESSAGE ReplyMessage OPTIONAL,
                       OUT PPORT_MESSAGE ReceiveMessage)
{
    typedef
    NTSTATUS
    (NTAPI *NT_REPLY_WAIT_RECEIVE_PORT)(
        IN HANDLE PortHandle,
        OUT PVOID *PortContext OPTIONAL,
        IN PPORT_MESSAGE ReplyMessage OPTIONAL,
        OUT PPORT_MESSAGE ReceiveMessage
    );

    NT_REPLY_WAIT_RECEIVE_PORT proc;
    
    proc = NTDLL_IMPORT(NT_REPLY_WAIT_RECEIVE_PORT,
                        "NtReplyWaitReceivePort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle,
                PortContext,
                ReplyMessage,
                ReceiveMessage);
}

NTSTATUS
NTAPI
NtReplyPort(IN HANDLE PortHandle,
            IN PPORT_MESSAGE ReplyMessage)
{
    typedef
    NTSTATUS
    (NTAPI *NT_REPLY_PORT)(
        IN HANDLE PortHandle,
        IN PPORT_MESSAGE ReplyMessage
    );

    NT_REPLY_PORT proc;
    
    proc = NTDLL_IMPORT(NT_REPLY_PORT,
                        "NtReplyPort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle,
                ReplyMessage);
}

NTSTATUS
NTAPI
NtReplyWaitReplyPort(IN HANDLE PortHandle,
                     IN OUT PPORT_MESSAGE Reply)
{
    typedef NTSTATUS(NTAPI *NT_REPLY_WAIT_REPLY_PORT)(
        IN HANDLE PortHandle,
        IN OUT PPORT_MESSAGE Reply
    );

    NT_REPLY_WAIT_REPLY_PORT proc = NTDLL_IMPORT(NT_REPLY_WAIT_REPLY_PORT,
                                                 "NtReplyWaitReplyPort");

    if (!proc)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return proc(PortHandle,
                Reply);
}
