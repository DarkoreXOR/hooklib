#ifndef __IPC_SERVER_H
#define __IPC_SERVER_H

#include <Windows.h>
#include "../defines.h"
#include "ipc_types.h"
#include "ipc_named_pipe.h"
#include "ipc_thread.h"

PIPC_CHANNEL_DATA
IpcServerAllocateChannelData();

BOOL
IpcServerDeallocateChannelData(
    PIPC_CHANNEL_DATA ChannelData
);

BOOL
IpcServerTryCreateChannel(
    LPCWSTR ChannelName,
    BOOL MultiSession
);

BOOL
IpcServerCreateServerThread(
    LPCWSTR ChannelName,
    IPC_ROUTINE Routine,
    BOOL MultiSession,
    PTHREAD *Thread
);

BOOL
IpcDestroyServerThread(
    PTHREAD *Thread
);



#endif
