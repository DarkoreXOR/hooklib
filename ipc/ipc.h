#ifndef __IPC_H
#define __IPC_H

#include <Windows.h>
#include <tchar.h>
#include "..\dll_decls.h"
#include "ipc_types.h"
#include "ipc_defs.h"
#include "ipc_event.h"
#include "ipc_shared_memory.h"
#include "ipc_utils.h"
#include "ipc_client_side.h"
#include "ipc_server_side.h"

typedef struct _THREAD_DATA
{
    HANDLE ThreadHandle;
    DWORD ThreadId;
    IPC_ROUTINE Routine;
    WCHAR ChannelName[0x100];
    PSHARED_MEMORY SharedMemory;
    PEVENT SendEvent;
    PEVENT ReadyEvent;
    DWORD Timeout;
} THREAD_DATA, *PTHREAD_DATA;

typedef struct _CHANNEL_DATA
{
    PSHARED_MEMORY SharedMemory;
    PEVENT SendEvent;
    PEVENT ReadyEvent;
} CHANNEL_DATA, *PCHANNEL_DATA;

BOOL
IpcCreateIpcChannel(
    LPCWSTR ChannelName,
    IPC_ROUTINE Routine,
    PVOID *ChannelData
);

BOOL
IpcDestroyIpcChannel(PVOID ChannelData);

BOOL
IpcSendIpcMessage(
    LPCWSTR ChannelName,
    PVOID MessageBuf,
    DWORD MessageSize,
    PVOID AnswerBuf,
    DWORD AnswerSize,
    DWORD Timeout
);

#endif
