#include "ipc.h"
#include <stdio.h>

VOID
CopyThreadData(PTHREAD_DATA Dest,
               PTHREAD_DATA Source)
{
    CopyMemory(Dest, Source, sizeof(THREAD_DATA));
    free(Source);
}

DWORD
WINAPI
IpcThreadProc(PTHREAD_DATA ThreadDataPtr)
{
    BOOL Result;
    THREAD_DATA ThreadData;
    CHANNEL_HEADER Header;
    DWORD Counter;
    DWORD WaitStatus;

    if (!ThreadDataPtr)
    {
        return 1;
    }

    CopyThreadData(&ThreadData, ThreadDataPtr);

    do
    {
        // wait send event

        Counter = ThreadData.Timeout;

        while (Counter--)
        {
            WaitStatus = IpcWaitEvent(ThreadData.SendEvent, 1);

            if (WaitStatus != WAIT_TIMEOUT)
            {
                break;
            }
        }

        if (WaitStatus == WAIT_FAILED)
        {
            Result = FALSE;
            break;
        }

        if (WaitStatus == WAIT_TIMEOUT && !Counter)
        {
            Result = FALSE;
            break;
        }

        // read header
        IpcReadFromSharedMemory(ThreadData.SharedMemory,
                                &Header,
                                sizeof(CHANNEL_HEADER));

        // send ready event
        Result = IpcSetEvent(ThreadData.ReadyEvent);

        if (!Result)
        {
            break;
        }

        // handle local channel
        Result = SsHandleLocalChannel(ThreadData.ChannelName,
                                      &Header,
                                      ThreadData.Routine);

        if (!Result)
        {
            break;
        }
    }
    while (1);

    if (!Result)
    {
        return 1;
    }

    return 0;
}

BOOL
StartIpcThread(LPCWSTR ChannelName,
               IPC_ROUTINE Routine,
               PSHARED_MEMORY SharedMemory,
               PEVENT SendEvent,
               PEVENT ReadyEvent)
{
    BOOL Result;
    PTHREAD_DATA ThreadData;
    HANDLE ThreadHandle;

    ThreadData = (PTHREAD_DATA)malloc(sizeof(THREAD_DATA));

    if (!ThreadData)
    {
        return FALSE;
    }

    Result = wcscpy_s(ThreadData->ChannelName, 0x100, ChannelName) == 0;

    if (!Result)
    {
        free(ThreadData);
        return FALSE;
    }

    ThreadHandle = CreateThread(NULL,
                                0,
                                IpcThreadProc,
                                ThreadData,
                                CREATE_SUSPENDED,
                                &ThreadData->ThreadId);
    
    Result = ThreadHandle != 0;

    if (!Result)
    {
        free(ThreadData);
        return FALSE;
    }

    ThreadData->Routine = Routine;
    ThreadData->Timeout = INFINITE;
    ThreadData->SharedMemory = SharedMemory;
    ThreadData->SendEvent = SendEvent;
    ThreadData->ReadyEvent = ReadyEvent;
    ThreadData->ThreadHandle = ThreadHandle;

    ResumeThread(ThreadData->ThreadHandle);

    return TRUE;
}

BOOL
IpcCreateIpcChannel(LPCWSTR ChannelName,
                    IPC_ROUTINE Routine,
                    PVOID *ChannelData)
{
    BOOL Result;
    PSHARED_MEMORY SharedMemory;
    CHANNEL_HEADER Header;
    PEVENT SendEvent;
    PEVENT ReadyEvent;

    if (ChannelData == NULL)
    {
        return FALSE;
    }

    Result = SsCreateGlobalSharedMemory(ChannelName, &SharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    *ChannelData = malloc(sizeof(CHANNEL_DATA));

    if (*ChannelData == NULL)
    {
        SsDestroyGlobalSharedMemory(SharedMemory);
        return FALSE;
    }

    ((PCHANNEL_DATA)*ChannelData)->SharedMemory = SharedMemory;

    // initialize unique id
    Header.UniqueId = 0;

    IpcWriteToSharedMemory(SharedMemory,
                           &Header,
                           sizeof(CHANNEL_HEADER));

    Result = SsCreateEvent(ChannelName,
                           GLOB_CHANNEL_SEND_EVENT_POSTFIX,
                           FALSE,
                           FALSE,
                           &SendEvent);

    if (!Result)
    {
        free(*ChannelData);
        *ChannelData = NULL;
        SsDestroyGlobalSharedMemory(SharedMemory);
        return FALSE;
    }

    ((PCHANNEL_DATA)*ChannelData)->SendEvent = SendEvent;

    Result = SsCreateEvent(ChannelName,
                           GLOB_CHANNEL_READY_EVENT_POSTFIX,
                           FALSE,
                           TRUE,
                           &ReadyEvent);

    if (!Result)
    {
        free(*ChannelData);
        *ChannelData = NULL;
        SsDestroyGlobalSharedMemory(SharedMemory);
        SsDestroyEvent(SendEvent);
        return FALSE;
    }

    ((PCHANNEL_DATA)*ChannelData)->ReadyEvent = ReadyEvent;

    Result = StartIpcThread(ChannelName,
                            Routine,
                            SharedMemory,
                            SendEvent,
                            ReadyEvent);

    return Result;
}

BOOL
IpcDestroyIpcChannel(PVOID ChannelData)
{
    BOOL Result;

    if (!ChannelData)
    {
        return FALSE;
    }

    Result = SsDestroyGlobalSharedMemory(((PCHANNEL_DATA)ChannelData)->SharedMemory);
    Result = SsDestroyEvent(((PCHANNEL_DATA)ChannelData)->SendEvent) && Result;
    Result = SsDestroyEvent(((PCHANNEL_DATA)ChannelData)->ReadyEvent) && Result;
    free(ChannelData);

    return Result;
}

BOOL
IpcSendIpcMessage(LPCWSTR ChannelName,
                  PVOID MessageBuf,
                  DWORD MessageSize,
                  PVOID AnswerBuf,
                  DWORD AnswerSize,
                  DWORD Timeout)
{
    BOOL Result;
    UINT64 UniqueId;
    WCHAR LocalChannelName[0x100];
    CHANNEL_HEADER Header;
    SHARED_MEMORY GlobSharedMemory;

    Result = CsWaitReadyEvent(ChannelName, Timeout);

    if (!Result)
    {
        return FALSE;
    }

    // make unique local channel name
    Result = CsReadUniqueId(ChannelName, &UniqueId);

    if (!Result)
    {
        return FALSE;
    }

    Result = CsGenerateUniqueName(LocalChannelName,
                                  LOC_CHANNEL_POSTFIX,
                                  UniqueId,
                                  ChannelName);

    if (!Result)
    {
        return FALSE;
    }

    // make header
    CsMakeHeader(&Header,
                 UniqueId,
                 LocalChannelName,
                 MessageBuf,
                 MessageSize,
                 AnswerBuf,
                 AnswerSize);

    // open global channel
    Result = IpcOpenSharedMemoryW(ChannelName,
                                  GLOB_SHARED_MEMORY_POSTFIX,
                                  &GlobSharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    // write header
    CopyMemory(GlobSharedMemory.MemoryPointer,
               &Header,
               sizeof(CHANNEL_HEADER));

    Result = IpcCloseSharedMemory(&GlobSharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    Result = CsHandleLocalChannel(ChannelName,
                                  LocalChannelName,
                                  &Header,
                                  MessageBuf,
                                  MessageSize,
                                  AnswerBuf,
                                  AnswerSize,
                                  Timeout);

    return Result;
}

