#include "ipc_local.h"

BOOL
LocalCreateChannel(LPCWSTR ChannelName,
                   BOOL GlobalObject,
                   SIZE_T Size,
                   PSHARED_MEMORY *SharedMemory)
{
    LPCWSTR ObjectNamespace = NULL;

    if (GlobalObject)
    {
        ObjectNamespace = L"Global";
    }

    if (!SharedMemory)
    {
        return FALSE;
    }

    *SharedMemory = (PSHARED_MEMORY)malloc(sizeof(SHARED_MEMORY));

    if (!(*SharedMemory))
    {
        return FALSE;
    }

    return IpcCreateSharedMemoryW(ObjectNamespace,
                                  ChannelName,
                                  L"",
                                  Size,
                                  *SharedMemory);
}

BOOL
LocalDestroyChannel(PSHARED_MEMORY SharedMemory)
{
    BOOL Result;

    Result = IpcDestroySharedMemory(SharedMemory);

    free(SharedMemory);

    return Result;
}

BOOL
LocalOpenChannel(LPCWSTR LocalChannelName,
                 BOOL GlobalObject,
                 PSHARED_MEMORY SharedMemory)
{
    BOOL Result;
    LPCWSTR ObjectNamespace = NULL;

    if (GlobalObject)
    {
        ObjectNamespace = L"Global";
    }

    Result = IpcOpenSharedMemoryW(ObjectNamespace,
                                  LocalChannelName,
                                  L"",
                                  SharedMemory);

    return Result;
}

BOOL
LocalCloseChannel(PSHARED_MEMORY SharedMemory)
{
    return IpcCloseSharedMemory(SharedMemory);
}

// message read / write

BOOL
LocalReadChannelMessage(PSHARED_MEMORY SharedMemory,
                        LPVOID MessageBuf,
                        SIZE_T MessageSize)
{
    return IpcReadFromSharedMemory(SharedMemory,
                                   MessageBuf,
                                   MessageSize);
}

BOOL
LocalWriteChannelMessage(PSHARED_MEMORY SharedMemory,
                         LPVOID MessageBuf,
                         SIZE_T MessageSize)
{
    return IpcWriteToSharedMemory(SharedMemory,
                                  MessageBuf,
                                  MessageSize);
}

// answer read / write

BOOL
LocalReadChannelAnswer(PSHARED_MEMORY SharedMemory,
                       LPVOID AnswerBuf,
                       SIZE_T AnswerSize)
{
    return IpcReadFromSharedMemory(SharedMemory,
                                   AnswerBuf,
                                   AnswerSize);
}

BOOL
LocalWriteChannelAnswer(PSHARED_MEMORY SharedMemory,
                        LPVOID AnswerBuf,
                        SIZE_T AnswerSize)
{
    return IpcWriteToSharedMemory(SharedMemory,
                                  AnswerBuf,
                                  AnswerSize);
}

// channel message event

BOOL
LocalCreateChannelMessageEvent(LPCWSTR ChannelName,
                               BOOL GlobalObject,
                               PEVENT *Event)
{
    LPCWSTR ObjectNamespace = NULL;

    if (GlobalObject)
    {
        ObjectNamespace = L"Global";
    }

    if (!Event)
    {
        return FALSE;
    }

    *Event = (PEVENT)malloc(sizeof(EVENT));

    if (!(*Event))
    {
        return FALSE;
    }

    return IpcCreateEventW(ObjectNamespace,
                           ChannelName,
                           IPC_LOCAL_CHANNEL_MESSAGE_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
LocalDestroyChannelMessageEvent(PEVENT Event)
{
    BOOL Result;

    Result = IpcDestroyEvent(Event);

    free(Event);

    return Result;
}

BOOL
LocalSetChannelMessageEvent(LPCWSTR LocalChannelName,
                            BOOL GlobalObject)
{
    BOOL Result;
    EVENT Event;
    LPCWSTR ObjectNamespace = NULL;

    if (GlobalObject)
    {
        ObjectNamespace = L"Global";
    }

    Result = IpcOpenEventW(ObjectNamespace,
                           LocalChannelName,
                           IPC_LOCAL_CHANNEL_MESSAGE_EVENT,
                           &Event);

    if (!Result)
    {
        return FALSE;
    }

    Result = IpcSetEvent(&Event);

    Result = IpcCloseEvent(&Event) && Result;

    return Result;
}

BOOL
LocalWaitChannelMessageEvent(PEVENT Event,
                             DWORD Timeout)
{
    DWORD WaitStatus;

    WaitStatus = IpcWaitEvent(Event, Timeout);

    return WaitStatus == WAIT_OBJECT_0;
}

// channel answer event

BOOL
LocalCreateChannelAnswerEvent(LPCWSTR ChannelName,
                              BOOL GlobalObject,
                              PEVENT *Event)
{
    LPCWSTR ObjectNamespace = NULL;

    if (GlobalObject)
    {
        ObjectNamespace = L"Global";
    }

    if (!Event)
    {
        return FALSE;
    }

    *Event = (PEVENT)malloc(sizeof(EVENT));

    if (!(*Event))
    {
        return FALSE;
    }

    return IpcCreateEventW(ObjectNamespace,
                           ChannelName,
                           IPC_LOCAL_CHANNEL_ANSWER_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
LocalDestroyChannelAnswerEvent(PEVENT Event)
{
    BOOL Result;

    Result = IpcDestroyEvent(Event);

    free(Event);

    return Result;
}

BOOL
LocalSetChannelAnswerEvent(PEVENT Event)
{
    return IpcSetEvent(Event);
}

BOOL
LocalWaitChannelAnswerEvent(LPCWSTR LocalChannelName,
                            BOOL GlobalObject,
                            DWORD Timeout)
{
    BOOL Result;
    EVENT Event;
    LPCWSTR ObjectNamespace = NULL;
    DWORD WaitStatus;

    if (GlobalObject)
    {
        ObjectNamespace = L"Global";
    }

    Result = IpcOpenEventW(ObjectNamespace,
                           LocalChannelName,
                           IPC_LOCAL_CHANNEL_ANSWER_EVENT,
                           &Event);

    if (!Result)
    {
        return FALSE;
    }

    WaitStatus = IpcWaitEvent(&Event, Timeout);

    Result = IpcCloseEvent(&Event);

    return Result && WaitStatus == WAIT_OBJECT_0;
}

// channel ready event

BOOL
LocalCreateChannelReadyEvent(LPCWSTR ChannelName,
                             BOOL GlobalObject,
                             PEVENT *Event)
{
    LPCWSTR ObjectNamespace = NULL;

    if (GlobalObject)
    {
        ObjectNamespace = L"Global";
    }

    if (!Event)
    {
        return FALSE;
    }

    *Event = (PEVENT)malloc(sizeof(EVENT));

    if (!(*Event))
    {
        return FALSE;
    }

    return IpcCreateEventW(ObjectNamespace,
                           ChannelName,
                           IPC_LOCAL_CHANNEL_READY_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
LocalDestroyChannelReadyEvent(PEVENT Event)
{
    BOOL Result;

    Result = IpcDestroyEvent(Event);

    free(Event);

    return Result;
}

BOOL
LocalSetChannelReadyEvent(LPCWSTR ChannelName,
                          BOOL GlobalObject)
{
    BOOL Result;
    EVENT Event;
    LPCWSTR ObjectNamespace = NULL;

    if (GlobalObject)
    {
        ObjectNamespace = L"Global";
    }

    Result = IpcOpenEventW(ObjectNamespace,
                           ChannelName,
                           IPC_LOCAL_CHANNEL_READY_EVENT,
                           &Event);

    if (!Result)
    {
        return FALSE;
    }

    Result = IpcSetEvent(&Event);

    Result = IpcCloseEvent(&Event) && Result;

    return Result;
}

BOOL
LocalWaitChannelReadyEvent(PEVENT Event,
                           DWORD Timeout)
{
    DWORD WaitStatus;

    WaitStatus = IpcWaitEvent(Event, Timeout);

    return WaitStatus == WAIT_OBJECT_0;
}

