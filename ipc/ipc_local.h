#ifndef __IPC_LOCAL_H
#define __IPC_LOCAL_H

#include <Windows.h>
#include <tchar.h>
#include "ipc_event.h"
#include "ipc_shared_memory.h"
#include "ipc_utils.h"

#define IPC_LOCAL_CHANNEL L"__LOC_CH"
#define IPC_LOCAL_CHANNEL_MESSAGE_EVENT L"__LOC_CH_MSG_EVNT"
#define IPC_LOCAL_CHANNEL_ANSWER_EVENT L"__LOC_CH_ANSWR_EVNT"
#define IPC_LOCAL_CHANNEL_READY_EVENT L"__LOC_CH_RDY_EVNT"

// channel functions

BOOL
LocalCreateChannel(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    DWORD Size,
    PSHARED_MEMORY *SharedMemory
);

BOOL
LocalDestroyChannel(
    PSHARED_MEMORY SharedMemory
);

BOOL
LocalOpenChannel(
    LPCWSTR LocalChannelName,
    BOOL GlobalObject,
    PSHARED_MEMORY SharedMemory
);

BOOL
LocalCloseChannel(
    PSHARED_MEMORY SharedMemory
);

// message read / write functions

BOOL
LocalReadChannelMessage(
    PSHARED_MEMORY SharedMemory,
    LPVOID MessageBuf,
    DWORD MessageSize
);

BOOL
LocalWriteChannelMessage(
    PSHARED_MEMORY SharedMemory,
    LPVOID MessageBuf,
    DWORD MessageSize
);

// answer read / write functions

BOOL
LocalReadChannelAnswer(
    PSHARED_MEMORY SharedMemory,
    LPVOID AnswerBuf,
    DWORD AnswerSize
);

BOOL
LocalWriteChannelAnswer(
    PSHARED_MEMORY SharedMemory,
    LPVOID AnswerBuf,
    DWORD AnswerSize
);

// channel message event functions

BOOL
LocalCreateChannelMessageEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    PEVENT *Event
);

BOOL
LocalDestroyChannelMessageEvent(
    PEVENT Event
);

BOOL
LocalSetChannelMessageEvent(
    LPCWSTR LocalChannelName,
    BOOL GlobalObject
);

BOOL
LocalWaitChannelMessageEvent(
    PEVENT Event,
    DWORD Timeout
);

// channel answer event functions

BOOL
LocalCreateChannelAnswerEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    PEVENT *Event
);

BOOL
LocalDestroyChannelAnswerEvent(
    PEVENT Event
);

BOOL
LocalSetChannelAnswerEvent(
    PEVENT Event
);

BOOL
LocalWaitChannelAnswerEvent(
    LPCWSTR LocalChannelName,
    BOOL GlobalObject,
    DWORD Timeout
);

// channel answer event functions

BOOL
LocalCreateChannelReadyEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    PEVENT *Event
);

BOOL
LocalDestroyChannelReadyEvent(
    PEVENT Event
);

BOOL
LocalSetChannelReadyEvent(
    LPCWSTR ChannelName,
    BOOL GlobalObject
);

BOOL
LocalWaitChannelReadyEvent(
    PEVENT Event,
    DWORD Timeout
);

#endif
