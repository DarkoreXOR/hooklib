#ifndef __IPC_GLOBAL_H
#define __IPC_GLOBAL_H

#include <Windows.h>
#include <tchar.h>
#include "ipc_event.h"
#include "ipc_shared_memory.h"
#include "ipc_types.h"
#include "ipc_utils.h"

#define IPC_GLOBAL_CHANNEL_READY_EVENT L"__GLBL_CH_RDY_EVNT"
#define IPC_GLOBAL_CHANNEL L"__GLBL_CH"
#define IPC_GLOBAL_CHANNEL_HEADER_SEND_EVENT L"__GLBL_CH_HDR_SND_EVNT"
#define IPC_GLOBAL_LOCAL_CHANNEL_READY_EVENT L"__GLBL_LOC_CH_RDY_EVNT"

// channel functions

BOOL
GlobalCreateChannel(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    DWORD Size,
    PSHARED_MEMORY *SharedMemory
);

BOOL
GlobalDestroyChannel(
    PSHARED_MEMORY SharedMemory
);

BOOL
GlobalOpenChannel(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    PSHARED_MEMORY SharedMemory
);

BOOL
GlobalCloseChannel(
    PSHARED_MEMORY SharedMemory
);

// header read / write functions

BOOL
GlobalReadChannelHeaderData(
    PSHARED_MEMORY SharedMemory,
    PCHANNEL_HEADER ChannelHeader
);

BOOL
GlobalWriteChannelHeaderData(
    PSHARED_MEMORY SharedMemory,
    PCHANNEL_HEADER ChannelHeader
);

// header event functions

BOOL
GlobalCreateChannelSendHeaderEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    PEVENT *Event
);

BOOL
GlobalDestroyChannelSendHeaderEvent(
    PEVENT Event
);

BOOL
GlobalSetChannelSendHeaderEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject
);

BOOL
GlobalWaitChannelSendHeaderEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    DWORD Timeout
);

// channel ready event functions

BOOL
GlobalCreateChannelReadyEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    PEVENT *Event
);

BOOL
GlobalDestroyChannelReadyEvent(
    PEVENT Event
);

BOOL
GlobalSetChannelReadyEvent(
    PEVENT Event
);

BOOL
GlobalSetChannelReadyEvent2(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    DWORD Timeout
);

BOOL
GlobalWaitChannelReadyEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    DWORD Timeout
);

// local channel ready event functions

BOOL
GlobalCreateLocalChannelReadyEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    PEVENT *Event
);

BOOL
GlobalDestroyLocalChannelReadyEvent(
    PEVENT Event
);

BOOL
GlobalSetLocalChannelReadyEvent(
    PEVENT Event
);

BOOL
GlobalWaitLocalChannelReadyEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    DWORD Timeout
);

#endif
