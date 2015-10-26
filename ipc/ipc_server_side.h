#ifndef __IPC_SERVER_SIDE_H
#define __IPC_SERVER_SIDE_H

#include "ipc_utils.h"
#include "ipc_shared_memory.h"
#include "ipc_event.h"
#include "ipc_defs.h"
#include "ipc_types.h"

BOOL
SsCreateGlobalSharedMemory(
    LPCWSTR ChannelName,
    PSHARED_MEMORY *SharedMemory
);

BOOL
SsDestroyGlobalSharedMemory(PSHARED_MEMORY SharedMemory);

BOOL
SsCreateEvent(
    LPCWSTR eventName,
    LPCWSTR postfix,
    BOOL manualReset,
    BOOL initialState,
    PEVENT *Event
);

BOOL
SsDestroyEvent(PEVENT Event);

BOOL
SsHandleLocalChannel(
    LPCWSTR ChannelName,
    PCHANNEL_HEADER Header,
    IPC_ROUTINE Routine
);

BOOL
SsCallIpcRoutine(
    LPCWSTR ChannelName,
    PCHANNEL_HEADER Header,
    IPC_ROUTINE Routine,
    PSHARED_MEMORY SharedMemory
);

BOOL
SsSetAnswerEvent(LPCWSTR ChannelName);

#endif
