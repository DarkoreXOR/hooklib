#ifndef __IPC_H
#define __IPC_H

#include <Windows.h>
#include <tchar.h>
#include "ipc_event.h"
#include "ipc_shared_memory.h"
#include "ipc_types.h"
#include "ipc_global.h"
#include "ipc_local.h"
#include "ipc_server_side.h"

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
