#ifndef __IPC_CLIENT_H
#define __IPC_CLIENT_H

#include <Windows.h>
#include "../defines.h"
#include "ipc_types.h"
#include "ipc_named_pipe.h"
#include "ipc_event.h"

BOOL
IpcClientWaitChannel(
    LPCWSTR ChannelName,
    BOOL MultiSession,
    DWORD Timeout
);

BOOL
IpcClientCallIpcServer(
    LPCWSTR ChannelName,
    LPVOID MessageBuffer,
    DWORD MessageSize,
    LPVOID AnswerBuffer,
    DWORD AnswerSize,
    DWORD Timeout
);

#endif
