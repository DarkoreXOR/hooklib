#include "ipc_global.h"


// channel

BOOL
GlobalCreateChannel(LPCWSTR ChannelName,
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
                                  IPC_GLOBAL_CHANNEL,
                                  Size,
                                  *SharedMemory);
}

BOOL
GlobalDestroyChannel(PSHARED_MEMORY SharedMemory)
{
    BOOL result;

    result = IpcDestroySharedMemory(SharedMemory);

    free(SharedMemory);

    return result;
}

BOOL
GlobalOpenChannel(LPCWSTR ChannelName,
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
                                  ChannelName,
                                  IPC_GLOBAL_CHANNEL,
                                  SharedMemory);

    return result;
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
                           IPC_GLOBAL_CHANNEL_HEADER_SEND_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
GlobalDestroyChannelSendHeaderEvent(PEVENT Event)
{
    BOOL result;

    result = IpcDestroyEvent(Event);

    free(Event);

    return result;
}

BOOL
GlobalSetChannelSendHeaderEvent(LPCWSTR ChannelName,
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
                           IPC_GLOBAL_CHANNEL_HEADER_SEND_EVENT,
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
GlobalWaitChannelSendHeaderEvent(LPCWSTR ChannelName,
                                 BOOL GlobalObject,
                                 DWORD Timeout)
{
    BOOL result;
    EVENT event;
    LPCWSTR objectNamespace = NULL;
    DWORD waitStatus;

    if (GlobalObject)
    {
        objectNamespace = L"Global";
    }

    result = IpcOpenEventW(objectNamespace,
                           ChannelName,
                           IPC_GLOBAL_CHANNEL_HEADER_SEND_EVENT,
                           &event);

    if (!result)
    {
        return FALSE;
    }

    waitStatus = IpcWaitEvent(&event, Timeout);

    result = IpcCloseEvent(&event);

    return result && waitStatus == WAIT_OBJECT_0;
}

// channel ready event

BOOL
GlobalCreateChannelReadyEvent(LPCWSTR ChannelName,
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
                           IPC_GLOBAL_CHANNEL_READY_EVENT,
                           FALSE,
                           TRUE,
                           *Event);
}

BOOL
GlobalDestroyChannelReadyEvent(PEVENT Event)
{
    BOOL result;

    result = IpcDestroyEvent(Event);

    free(Event);

    return result;
}

BOOL
GlobalSetChannelReadyEvent(PEVENT Event)
{
    return IpcSetEvent(Event);
}

BOOL
GlobalSetChannelReadyEvent2(LPCWSTR ChannelName,
                            BOOL GlobalObject,
                            DWORD Timeout)
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
                           IPC_GLOBAL_CHANNEL_READY_EVENT,
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
GlobalWaitChannelReadyEvent(LPCWSTR ChannelName,
                            BOOL GlobalObject,
                            DWORD Timeout)
{
    DWORD waitStatus;

    waitStatus = IpcWaitEventEx(ChannelName,
                                IPC_GLOBAL_CHANNEL_READY_EVENT,
                                GlobalObject,
                                Timeout);

    return waitStatus == WAIT_OBJECT_0;
}

// local channel ready event

BOOL
GlobalCreateLocalChannelReadyEvent(LPCWSTR ChannelName,
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
                           IPC_GLOBAL_LOCAL_CHANNEL_READY_EVENT,
                           FALSE,
                           FALSE,
                           *Event);
}

BOOL
GlobalDestroyLocalChannelReadyEvent(PEVENT Event)
{
    BOOL result;

    result = IpcDestroyEvent(Event);

    free(Event);

    return result;
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
    DWORD waitStatus;

    waitStatus = IpcWaitEventEx(ChannelName,
                                IPC_GLOBAL_LOCAL_CHANNEL_READY_EVENT,
                                GlobalObject,
                                Timeout);

    return waitStatus == WAIT_OBJECT_0;
}
