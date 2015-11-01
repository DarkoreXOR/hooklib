#ifndef __IPC_LOCAL_H
#define __IPC_LOCAL_H

#include <Windows.h>
#include <tchar.h>
#include "ipc_event.h"
#include "ipc_shared_memory.h"

#define IPC_LOCAL_CHANNEL L"__LOC_CH"
#define IPC_LOCAL_CHANNEL_MESSAGE_EVENT L"__LOC_CH_MSG_EVNT"
#define IPC_LOCAL_CHANNEL_ANSWER_EVENT L"__LOC_CH_ANSWR_EVNT"
#define IPC_LOCAL_CHANNEL_READY_EVENT L"__LOC_CH_RDY_EVNT"

// channel

BOOL
LocalCreateChannel(
    LPCWSTR ChannelName,
    BOOL GlobalObject,
    SIZE_T Size,
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

// message read / write

BOOL
LocalReadChannelMessage(
    PSHARED_MEMORY SharedMemory,
    LPVOID MessageBuf,
    SIZE_T MessageSize
);

BOOL
LocalWriteChannelMessage(
    PSHARED_MEMORY SharedMemory,
    LPVOID MessageBuf,
    SIZE_T MessageSize
);

// answer read / write

BOOL
LocalReadChannelAnswer(
    PSHARED_MEMORY SharedMemory,
    LPVOID AnswerBuf,
    SIZE_T AnswerSize
);

BOOL
LocalWriteChannelAnswer(
    PSHARED_MEMORY SharedMemory,
    LPVOID AnswerBuf,
    SIZE_T AnswerSize
);

// channel message event

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

// channel answer event

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

// channel answer event

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

