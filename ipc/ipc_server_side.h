#ifndef __IPC_SERVER_SIDE_H
#define __IPC_SERVER_SIDE_H

#include <Windows.h>
#include "ipc_types.h"
#include "ipc_shared_memory.h"
#include "ipc_event.h"
#include "ipc_global.h"

BOOL
SsCreateThreads(
    LPCWSTR ChannelName,
    LPTHREAD_START_ROUTINE ThreadProc,
    IPC_ROUTINE Routine,
    BOOL CreateSuspended,
    PSHARED_MEMORY GlobalSharedMemory,
    PEVENT GlobalChannelReadyEvent,
    PEVENT GlobalChannelSendHeaderEvent,
    PEVENT GlobalLocalChannelReadyEvent,
    DWORD Timeout,
    BOOL MultiSession,
    HANDLE **ThreadHandles,
    DWORD NumOfThreads
);

VOID
SsDestroyThreads(
    HANDLE *ThreadHandles,
    DWORD NumOfThreads
);

VOID
SsSuspendThreads(
    HANDLE *ThreadHandles,
    DWORD NumOfThreads
);

VOID
SsResumeThreads(
    HANDLE *ThreadHandles,
    DWORD NumOfThreads
);

BOOL
SsCreateGlobalObjects(
    LPCWSTR ChannelName,
    BOOL MultiSession,
    PSHARED_MEMORY *GlobalSharedMemory,
    PEVENT *GlobalChannelReadyEvent,
    PEVENT *GlobalChannelSendHeaderEvent,
    PEVENT *GlobalLocalChannelReadyEvent
);

BOOL
SsDestroyGlobalObjects(
    PSHARED_MEMORY GlobalSharedMemory,
    PEVENT GlobalChannelReadyEvent,
    PEVENT GlobalChannelSendHeaderEvent,
    PEVENT GlobalLocalChannelReadyEvent
);

VOID
SsInitializeChannelData(
    PCHANNEL_DATA ChannelData,
    PSHARED_MEMORY GlobalSharedMemory,
    PEVENT GlobalChannelReadyEvent,
    PEVENT GlobalChannelSendHeaderEvent,
    PEVENT GlobalLocalChannelReadyEvent,
    DWORD NumOfThreads,
    HANDLE *ThreadHandles
);

#endif
