#ifndef __IPC_H
#define __IPC_H

#include <Windows.h>
#include "../memory/heap.h"
#include "../defines.h"
#include "ipc_types.h"
#include "ipc_event.h"
#include "ipc_named_pipe.h"
#include "ipc_client.h"
#include "ipc_server.h"

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
    PVOID MessageBuffer,
    DWORD MessageSize,
    PVOID AnswerBuffer,
    DWORD AnswerSize,
    DWORD Timeout,
    BOOL MultiSession
);

#endif
