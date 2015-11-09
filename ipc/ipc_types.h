#ifndef __IPC_TYPES_H
#define __IPC_TYPES_H

#include <Windows.h>
#include "ipc_shared_memory.h"
#include "ipc_event.h"

#define IPC_CHANNEL_NAME_LENGTH 0x100

typedef
BOOL
(WINAPI *IPC_ROUTINE)(
    LPCWSTR ChannelName,
    PVOID MessageBuf,
    SIZE_T MessageSize,
    PVOID AnswerBuf,
    SIZE_T AnswerSize
);

typedef struct _THREAD_DATA
{
    HANDLE ThreadHandle;
    IPC_ROUTINE Routine;
    WCHAR GlobalChannelName[IPC_CHANNEL_NAME_LENGTH];
    PSHARED_MEMORY GlobalSharedMemory;
    PEVENT GlobalChannelReadyEvent;
    PEVENT GlobalChannelSendHeaderEvent;
    PEVENT GlobalLocalChannelReadyEvent;
    BOOL MultiSession;
    DWORD Timeout;
} THREAD_DATA, *PTHREAD_DATA;

typedef struct _CHANNEL_HEADER
{
    UINT64 UniqueId;
    BOOL HasMessage;
    BOOL HasAnswer;
    DWORD MessageSize;
    DWORD AnswerSize;
    WCHAR LocalChannelName[IPC_CHANNEL_NAME_LENGTH];
} CHANNEL_HEADER, *PCHANNEL_HEADER;

typedef struct _CHANNEL_DATA
{
    DWORD NumOfIpcThreads;
    HANDLE *IpcThreads;
    PSHARED_MEMORY GlobalSharedMemory;
    PEVENT GlobalChannelReadyEvent;
    PEVENT GlobalChannelSendHeaderEvent;
    PEVENT GlobalLocalChannelReadyEvent;
} CHANNEL_DATA, *PCHANNEL_DATA;

#endif
