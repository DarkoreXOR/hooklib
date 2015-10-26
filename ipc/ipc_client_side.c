#include "ipc_client_side.h"

BOOL
CsWaitReadyEvent(LPCWSTR ChannelName,
                 DWORD Timeout)
{
    BOOL Result;
    EVENT GlobReadyEvent;
    DWORD Counter;
    DWORD WaitStatus;

    if (Timeout == 0)
    {
        return FALSE;
    }

    Counter = Timeout;

    while (Counter--)
    {
        // open event
        Result = IpcOpenEventW(ChannelName,
                               GLOB_CHANNEL_READY_EVENT_POSTFIX,
                               &GlobReadyEvent);

        if (!Result)
        {
            return FALSE;
        }

        // wait event
        WaitStatus = IpcWaitEvent(&GlobReadyEvent, 1);

        // close event
        Result = IpcCloseEvent(&GlobReadyEvent);

        if (!Result)
        {
            return FALSE;
        }

        // check !timeout
        if (WaitStatus != WAIT_TIMEOUT)
        {
            break;
        }
    }

    if (WaitStatus == WAIT_FAILED)
    {
        return FALSE;
    }

    if (WaitStatus == WAIT_TIMEOUT && !Counter)
    {
        return FALSE;
    }

    if (WaitStatus == WAIT_OBJECT_0)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL
CsReadUniqueId(LPCWSTR ChannelName,
               PUINT64 UniqueId)
{
    BOOL Result;
    SHARED_MEMORY SharedMemory;
    PCHANNEL_HEADER Header;

    Result = IpcOpenSharedMemoryW(ChannelName,
                                  GLOB_SHARED_MEMORY_POSTFIX,
                                  &SharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    Header = SharedMemory.MemoryPointer;

    *UniqueId = ++Header->UniqueId;

    return IpcCloseSharedMemory(&SharedMemory);
}

BOOL
CsGenerateUniqueName(LPWSTR Dest,
                     LPCWSTR Postfix,
                     UINT64 UniqueId,
                     LPCWSTR ChannelName)
{
    BOOL Result;

    Result = IpcMakeWideString(Dest, ChannelName);
    Result = Result && IpcAppendPostfix(Dest, Postfix);
    return Result && IpcAppendNumValue(Dest, UniqueId);
}

VOID
CsMakeHeader(PCHANNEL_HEADER Header,
             UINT64 UniqueId,
             LPCWSTR LocalChannelName,
             PVOID MessageBuf,
             DWORD MessageSize,
             PVOID AnswerBuf,
             DWORD AnswerSize)
{
    Header->UniqueId = UniqueId;
    wcscpy_s(Header->LocalChannelName, 0x100, LocalChannelName);
    Header->HasMessage = MessageBuf && MessageSize;
    Header->HasAnswer = AnswerBuf && AnswerSize;
    Header->MessageSize = MessageSize;
    Header->AnswerSize = AnswerSize;
}

BOOL
CsSetSendEvent(LPCWSTR ChannelName)
{
    BOOL Result;
    EVENT SendEvent;

    Result = IpcOpenEventW(ChannelName,
                           GLOB_CHANNEL_SEND_EVENT_POSTFIX,
                           &SendEvent);

    if (!Result)
    {
        return FALSE;
    }

    Result = IpcSetEvent(&SendEvent);

    return IpcCloseEvent(&SendEvent) && Result;
}

BOOL
CsHandleLocalChannel(LPCWSTR GlobalChannelName,
                     LPCWSTR LocalChannelName,
                     PCHANNEL_HEADER Header,
                     PVOID MessageBuf,
                     DWORD MessageSize,
                     PVOID AnswerBuf,
                     DWORD AnswerSize,
                     DWORD WaitAnswerTimeout)
{
    BOOL Result;
    SHARED_MEMORY LocalSharedMemory;
    EVENT AnswerEvent;

    // create local shared memory
    Result = IpcCreateSharedMemoryW(LocalChannelName,
                                    LOC_SHARED_MEMORY_POSTFIX,
                                    MAX(MessageSize, AnswerSize),
                                    &LocalSharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    // copy messageBuf to localSharedMemory
    if (Header->HasMessage)
    {
        IpcWriteToSharedMemory(&LocalSharedMemory,
                               MessageBuf,
                               MessageSize);
    }

    // create answer event
    Result = IpcCreateEventW(LocalChannelName,
                             LOC_CHANNEL_ANSWER_EVENT_POSTFIX,
                             FALSE,
                             FALSE,
                             &AnswerEvent);

    if (!Result)
    {
        IpcDestroySharedMemory(&LocalSharedMemory);
        return FALSE;
    }

    // set send event
    Result = CsSetSendEvent(GlobalChannelName);

    if (!Result)
    {
        IpcDestroySharedMemory(&LocalSharedMemory);
        return FALSE;
    }

    // wait answer event
    Result = IpcWaitEvent(&AnswerEvent, WaitAnswerTimeout) == WAIT_OBJECT_0;

    if (!Result)
    {
        IpcDestroySharedMemory(&LocalSharedMemory);
        IpcDestroyEvent(&AnswerEvent);
        return FALSE;
    }

    // destroy event
    Result = IpcDestroyEvent(&AnswerEvent);

    if (!Result)
    {
        IpcDestroySharedMemory(&LocalSharedMemory);
    }

    // copy answer to answerBuf
    if (Header->HasAnswer)
    {
        // copy answer to answerBuf
        IpcReadFromSharedMemory(&LocalSharedMemory,
                                AnswerBuf,
                                AnswerSize);
    }

    // destroy local shared memory
    Result = IpcDestroySharedMemory(&LocalSharedMemory);

    return Result;
}
