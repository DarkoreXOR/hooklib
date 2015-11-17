#ifndef __IPC_TYPES_H
#define __IPC_TYPES_H

#include "ipc_event.h"
#include "ipc_thread.h"
#include "ipc_named_pipe.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef
BOOL
(WINAPI *IPC_ROUTINE)(
    LPCWSTR ChannelName,
    LPVOID MessageBuffer,
    DWORD MessageSize,
    LPVOID AnswerBuffer,
    DWORD AnswerSize
);

typedef struct _IPC_CHANNEL_DATA
{
    PTHREAD ServerThread;
    EVENT ChannelReadyEvent;
    NAMED_PIPE ServerNamedPipe;
} IPC_CHANNEL_DATA, *PIPC_CHANNEL_DATA;

typedef struct _IPC_SERVER_THREAD_DATA
{
    WCHAR ChannelName[0x100];
    IPC_ROUTINE Routine;
    BOOL MultiSession;
} IPC_SERVER_THREAD_DATA, *PIPC_SERVER_THREAD_DATA;

typedef struct _IPC_CHANNEL_HEADER
{
    BOOL HasMessage;
    BOOL HasAnswer;
    DWORD MessageSize;
    DWORD AnswerSize;
} IPC_CHANNEL_HEADER, *PIPC_CHANNEL_HEADER;

#endif
