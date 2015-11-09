#include "ipc_local.h"

BOOL
LocalCreateChannel(LPCWSTR ChannelName,
                   BOOL GlobalObject,
                   DWORD Size,
                   PSHARED_MEMORY *SharedMemory)
{
    LPCWSTR objectNamespace = NULL;

    if (GlobalObject)
    {
        objectNamespace = L"Global";
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

    return IpcCreateSharedMemoryW(objectNamespace,
                                  ChannelName,
                                  L"",
                                  Size,
                                  *SharedMemory);
}

BOOL
LocalDestroyChannel(PSHARED_MEMORY SharedMemory)
{
    BOOL result;

    result = IpcDestroySharedMemory(SharedMemory);

    free(SharedMemory);

    return result;
}

BOOL
LocalOpenChannel(LPCWSTR LocalChannelName,
                 BOOL GlobalObject,
                 PSHARED_MEMORY SharedMemory)
{
    BOOL result;
    LPCWSTR objectNamespace = NULL;

    if (GlobalObject)
    {
        objectNamespace = L"Global";
    }

    result = IpcOpenSharedMemoryW(objectNamespace,
                                  LocalChannelName,
                                  L"",
                                  SharedMemory);

    return result;
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
                        DWORD MessageSize)
{
    return IpcReadFromSharedMemory(SharedMemory,
                                   MessageBuf,
                                   MessageSize);
}

BOOL
LocalWriteChannelMessage(PSHARED_MEMORY SharedMemory,
                         LPVOID MessageBuf,
                         DWORD MessageSize)
{
    return IpcWriteToSharedMemory(SharedMemory,
                                  MessageBuf,
                                  MessageSize);
}

// answer read / write

BOOL
LocalReadChannelAnswer(PSHARED_MEMORY SharedMemory,
                       LPVOID AnswerBuf,
                       DWORD AnswerSize)
{
    return IpcReadFromSharedMemory(SharedMemory,
                                   AnswerBuf,
                                   AnswerSize);
}

BOOL
LocalWriteChannelAnswer(PSHARED_MEMORY SharedMemory,
                        LPVOID AnswerBuf,
                        DWORD AnswerSize)
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
    LPCWSTR objectNamespace = NULL;

    if (GlobalObject)
    {
        objectNamespace = L"Global";
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

    return IpcCreateEventW(objectNamespace,
                           ChannelName,
                           IPC_LOCAL_CHANNEL_MESSAGE_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
LocalDestroyChannelMessageEvent(PEVENT Event)
{
    BOOL result;

    result = IpcDestroyEvent(Event);

    free(Event);

    return result;
}

BOOL
LocalSetChannelMessageEvent(LPCWSTR LocalChannelName,
                            BOOL GlobalObject)
{
    BOOL result;
    EVENT event;
    LPCWSTR objectNamespace = NULL;

    if (GlobalObject)
    {
        objectNamespace = L"Global";
    }

    result = IpcOpenEventW(objectNamespace,
                           LocalChannelName,
                           IPC_LOCAL_CHANNEL_MESSAGE_EVENT,
                           &event);

    if (!result)
    {
        return FALSE;
    }

    result = IpcSetEvent(&event);

    result = IpcCloseEvent(&event) && result;

    return result;
}

BOOL
LocalWaitChannelMessageEvent(PEVENT Event,
                             DWORD Timeout)
{
    DWORD waitStatus;

    waitStatus = IpcWaitEvent(Event, Timeout);

    return waitStatus == WAIT_OBJECT_0;
}

// channel answer event

BOOL
LocalCreateChannelAnswerEvent(LPCWSTR ChannelName,
                              BOOL GlobalObject,
                              PEVENT *Event)
{
    LPCWSTR objectNamespace = NULL;

    if (GlobalObject)
    {
        objectNamespace = L"Global";
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

    return IpcCreateEventW(objectNamespace,
                           ChannelName,
                           IPC_LOCAL_CHANNEL_ANSWER_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
LocalDestroyChannelAnswerEvent(PEVENT Event)
{
    BOOL result;

    result = IpcDestroyEvent(Event);

    free(Event);

    return result;
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
    DWORD waitStatus;

    waitStatus = IpcWaitEventEx(LocalChannelName,
                                IPC_LOCAL_CHANNEL_ANSWER_EVENT,
                                GlobalObject,
                                Timeout);

    return waitStatus == WAIT_OBJECT_0;
}

// channel ready event

BOOL
LocalCreateChannelReadyEvent(LPCWSTR ChannelName,
                             BOOL GlobalObject,
                             PEVENT *Event)
{
    LPCWSTR objectNamespace = NULL;

    if (GlobalObject)
    {
        objectNamespace = L"Global";
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

    return IpcCreateEventW(objectNamespace,
                           ChannelName,
                           IPC_LOCAL_CHANNEL_READY_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
LocalDestroyChannelReadyEvent(PEVENT Event)
{
    BOOL result;

    result = IpcDestroyEvent(Event);

    free(Event);

    return result;
}

BOOL
LocalSetChannelReadyEvent(LPCWSTR ChannelName,
                          BOOL GlobalObject)
{
    BOOL result;
    EVENT event;
    LPCWSTR objectNamespace = NULL;

    if (GlobalObject)
    {
        objectNamespace = L"Global";
    }

    result = IpcOpenEventW(objectNamespace,
                           ChannelName,
                           IPC_LOCAL_CHANNEL_READY_EVENT,
                           &event);

    if (!result)
    {
        return FALSE;
    }

    result = IpcSetEvent(&event);

    result = IpcCloseEvent(&event) && result;

    return result;
}

BOOL
LocalWaitChannelReadyEvent(PEVENT Event,
                           DWORD Timeout)
{
    DWORD waitStatus;

    waitStatus = IpcWaitEvent(Event, Timeout);

    return waitStatus == WAIT_OBJECT_0;
}

