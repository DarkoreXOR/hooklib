#ifndef __IPC_CLIENT_H
#define __IPC_CLIENT_H

#include <Windows.h>
#include "../defines.h"
#include "ipc_types.h"
#include "ipc_named_pipe.h"

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
