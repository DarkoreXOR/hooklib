#include "ipc_global.h"


// channel

BOOL
GlobalCreateChannel(LPCWSTR ChannelName,
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
                                  IPC_GLOBAL_CHANNEL,
                                  Size,
                                  *SharedMemory);
}

BOOL
GlobalDestroyChannel(PSHARED_MEMORY SharedMemory)
{
    BOOL Result;

    Result = IpcDestroySharedMemory(SharedMemory);

    free(SharedMemory);

    return Result;
}

BOOL
GlobalOpenChannel(LPCWSTR ChannelName,
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
                                  ChannelName,
                                  IPC_GLOBAL_CHANNEL,
                                  SharedMemory);

    return Result;
}

BOOL
GlobalCloseChannel(PSHARED_MEMORY SharedMemory)
{
    return IpcCloseSharedMemory(SharedMemory);
}

// header read / write

BOOL
GlobalReadChannelHeaderData(PSHARED_MEMORY SharedMemory,
                            PCHANNEL_HEADER ChannelHeader)
{
    return IpcReadFromSharedMemory(SharedMemory,
                                   ChannelHeader,
                                   sizeof(CHANNEL_HEADER));
}

BOOL
GlobalWriteChannelHeaderData(PSHARED_MEMORY SharedMemory,
                             PCHANNEL_HEADER ChannelHeader)
{
    return IpcWriteToSharedMemory(SharedMemory,
                                  ChannelHeader,
                                  sizeof(CHANNEL_HEADER));
}

// header event

BOOL
GlobalCreateChannelSendHeaderEvent(LPCWSTR ChannelName,
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
                           IPC_GLOBAL_CHANNEL_HEADER_SEND_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
GlobalDestroyChannelSendHeaderEvent(PEVENT Event)
{
    BOOL Result;

    Result = IpcDestroyEvent(Event);

    free(Event);

    return Result;
}

BOOL
GlobalSetChannelSendHeaderEvent(LPCWSTR ChannelName,
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
                           IPC_GLOBAL_CHANNEL_HEADER_SEND_EVENT,
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
GlobalWaitChannelSendHeaderEvent(LPCWSTR ChannelName,
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
                           ChannelName,
                           IPC_GLOBAL_CHANNEL_HEADER_SEND_EVENT,
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
GlobalCreateChannelReadyEvent(LPCWSTR ChannelName,
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
                           IPC_GLOBAL_CHANNEL_READY_EVENT,
                           FALSE,
                           TRUE,
                           *Event);
}

BOOL
GlobalDestroyChannelReadyEvent(PEVENT Event)
{
    BOOL Result;

    Result = IpcDestroyEvent(Event);

    free(Event);

    return Result;
}

BOOL
GlobalSetChannelReadyEvent(PEVENT Event)
{
    return IpcSetEvent(Event);
}

BOOL
GlobalWaitChannelReadyEvent(LPCWSTR ChannelName,
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
                           ChannelName,
                           IPC_GLOBAL_CHANNEL_READY_EVENT,
                           &Event);

    if (!Result)
    {
        return FALSE;
    }

    WaitStatus = IpcWaitEvent(&Event, Timeout);

    Result = IpcCloseEvent(&Event);

    return Result && WaitStatus == WAIT_OBJECT_0;
}

// local channel ready event

BOOL
GlobalCreateLocalChannelReadyEvent(LPCWSTR ChannelName,
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
                           IPC_GLOBAL_LOCAL_CHANNEL_READY_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
GlobalDestroyLocalChannelReadyEvent(PEVENT Event)
{
    BOOL Result;

    Result = IpcDestroyEvent(Event);

    free(Event);

    return Result;
}

BOOL
GlobalSetLocalChannelReadyEvent(PEVENT Event)
{
    return IpcSetEvent(Event);
}

BOOL
GlobalWaitLocalChannelReadyEvent(LPCWSTR ChannelName,
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
                           ChannelName,
                           IPC_GLOBAL_LOCAL_CHANNEL_READY_EVENT,
                           &Event);

    if (!Result)
    {
        return FALSE;
    }

    WaitStatus = IpcWaitEvent(&Event, Timeout);

    Result = IpcCloseEvent(&Event);

    return Result && WaitStatus == WAIT_OBJECT_0;
}
