#ifndef __IPC_H
#define __IPC_H

#include <Windows.h>
#include <tchar.h>
#include "..\dll_decls.h"
#include "ipc_event.h"
#include "ipc_shared_memory.h"
#include "ipc_types.h"
#include "ipc_global.h"
#include "ipc_local.h"
#include "ipc_defs.h"

typedef
BOOL
(WINAPI *IPC_ROUTINE)(
    LPCWSTR ChannelName,
    PVOID MessageBuf,
    DWORD MessageSize,
    PVOID AnswerBuf,
    DWORD AnswerSize
);

typedef struct _THREAD_DATA
{
    HANDLE ThreadHandle;
    DWORD ThreadId;
    IPC_ROUTINE Routine;
    WCHAR GlobalChannelName[0x200];
    PSHARED_MEMORY GlobalSharedMemory;
    PEVENT GlobalChannelReadyEvent;
    PEVENT GlobalChannelSendHeaderEvent;
    PEVENT GlobalLocalChannelReadyEvent;
    BOOL MultiSession;
    DWORD Timeout;
} THREAD_DATA, *PTHREAD_DATA;

typedef struct _CHANNEL_DATA
{
    PSHARED_MEMORY GlobalSharedMemory;
    PEVENT GlobalChannelReadyEvent;
    PEVENT GlobalChannelSendHeaderEvent;
    PEVENT GlobalLocalChannelReadyEvent;
} CHANNEL_DATA, *PCHANNEL_DATA;

BOOL
IpcCreateIpcChannel(
    LPCWSTR ChannelName,
    IPC_ROUTINE Routine,
    BOOL MultiSession,
    PVOID *ChannelData
);

BOOL
IpcDestroyIpcChannel(
    PVOID ChannelData
);

BOOL
IpcSendIpcMessage(
    LPCWSTR ChannelName,
    PVOID MessageBuf,
    DWORD MessageSize,
    PVOID AnswerBuf,
    DWORD AnswerSize,
    DWORD Timeout,
    BOOL MultiSession
);

#endif
