#include "ipc_server_side.h"

BOOL
SsCreateGlobalSharedMemory(LPCWSTR ChannelName,
                           PSHARED_MEMORY *SharedMemory)
{
    BOOL Result;

    if (!SharedMemory)
    {
        return FALSE;
    }

    *SharedMemory = (PSHARED_MEMORY)malloc(sizeof(SHARED_MEMORY));

    if (!*SharedMemory)
    {
        return FALSE;
    }

    Result = IpcCreateSharedMemoryW(ChannelName,
                                    GLOB_SHARED_MEMORY_POSTFIX,
                                    sizeof(CHANNEL_HEADER),
                                    *SharedMemory);

    if (!Result)
    {
        free(*SharedMemory);
        return FALSE;
    }

    return TRUE;
}

BOOL
SsDestroyGlobalSharedMemory(PSHARED_MEMORY SharedMemory)
{
    BOOL Result;

    if (!SharedMemory)
    {
        return FALSE;
    }

    Result = IpcDestroySharedMemory(SharedMemory);

    free(SharedMemory);

    return Result;
}

BOOL
SsCreateEvent(LPCWSTR EventName,
              LPCWSTR Postfix,
              BOOL ManualReset,
              BOOL InitialState,
              PEVENT *Event)
{
    if (!Event)
    {
        return FALSE;
    }

    *Event = (PEVENT)malloc(sizeof(EVENT));

    if (!*Event)
    {
        return FALSE;
    }

    BOOL result = IpcCreateEventW(EventName,
                                  Postfix,
                                  ManualReset,
                                  InitialState,
                                  *Event);

    if (!result)
    {
        free(*Event);
        return FALSE;
    }

    return TRUE;
}

BOOL
SsDestroyEvent(PEVENT Event)
{
    BOOL Result;

    if (!Event)
    {
        return FALSE;
    }

    Result = IpcDestroyEvent(Event);

    free(Event);

    return Result;
}

BOOL
SsHandleLocalChannel(LPCWSTR ChannelName,
                     PCHANNEL_HEADER Header,
                     IPC_ROUTINE Routine)
{
    BOOL Result;
    SHARED_MEMORY SharedMemory;

    // open local channel
    Result = IpcOpenSharedMemoryW(Header->LocalChannelName,
                                  LOC_SHARED_MEMORY_POSTFIX,
                                  &SharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    Result = SsCallIpcRoutine(ChannelName,
                              Header,
                              Routine,
                              &SharedMemory);

    if (!Result)
    {
        IpcCloseSharedMemory(&SharedMemory);
        return FALSE;
    }

    // close local channel
    Result = IpcCloseSharedMemory(&SharedMemory);

    return Result;
}

BOOL
SsCallIpcRoutine(LPCWSTR ChannelName,
                 PCHANNEL_HEADER Header,
                 IPC_ROUTINE Routine,
                 PSHARED_MEMORY SharedMemory)
{
    PVOID MessageBuf = NULL;
    PVOID AnswerBuf = NULL;

    if (Header->HasMessage)
    {
        MessageBuf = SharedMemory->MemoryPointer;
    }

    if (Header->HasAnswer)
    {
        AnswerBuf = malloc(Header->AnswerSize);
    }

    // call routine

    if (!Routine)
    {
        if (Header->HasAnswer)
        {
            free(AnswerBuf);
        }

        IpcCloseSharedMemory(SharedMemory);

        return FALSE;
    }

    Routine(ChannelName,
            MessageBuf,
            Header->MessageSize,
            AnswerBuf,
            Header->AnswerSize);

    // free answer buffer
    if (Header->HasAnswer)
    {
        IpcWriteToSharedMemory(SharedMemory,
                               AnswerBuf,
                               Header->AnswerSize);
        free(AnswerBuf);
    }

    return SsSetAnswerEvent(Header->LocalChannelName);
}

BOOL
SsSetAnswerEvent(LPCWSTR ChannelName)
{
    BOOL Result;
    EVENT AnswerEvent;

    Result = IpcOpenEventW(ChannelName,
                           LOC_CHANNEL_ANSWER_EVENT_POSTFIX,
                           &AnswerEvent);

    if (!Result)
    {
        return FALSE;
    }

    Result = IpcSetEvent(&AnswerEvent);

    return IpcCloseEvent(&AnswerEvent) && Result;
}
