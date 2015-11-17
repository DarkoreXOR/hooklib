#include "ipc_server.h"

PIPC_CHANNEL_DATA
IpcServerAllocateChannelData()
{
    return (PIPC_CHANNEL_DATA)AllocMem(sizeof(IPC_CHANNEL_DATA));
}

BOOL
IpcServerDeallocateChannelData(PIPC_CHANNEL_DATA ChannelData)
{
    return DeallocMem(ChannelData);
}

BOOL
IpcServerTryCreateChannel(LPCWSTR ChannelName,
                          BOOL MultiSession)
{
    NAMED_PIPE Pipe;
    EVENT Event;

    TRY(BOOL, TRUE)
    {
        if (!IpcCreateNamedPipe(ChannelName,
                                TRUE,
                                0x1000,
                                0x1000,
                                NMPWAIT_WAIT_FOREVER,
                                &Pipe))
        {
            LEAVE(FALSE);
        }

        if (!IpcCreateEventW(MultiSession ? L"Global" : NULL,
                             ChannelName,
                             L"_CHRDYEVNT",
                             FALSE,
                             FALSE,
                             &Event))
        {
            LEAVE(FALSE);
        }
    }
    FINALLY
    {
        IpcDestroyNamedPipe(&Pipe);
        IpcDestroyEvent(&Event);
    }

    return TRY_VALUE;
}

BOOL
IpcServerCreateEvent(LPCWSTR ChannelName,
                     BOOL MultiSession,
                     PEVENT Event)
{
    return IpcCreateEventW(MultiSession ? L"Global" : NULL,
                           ChannelName,
                           L"_CHRDYEVNT",
                           FALSE,
                           FALSE,
                           Event);
}

BOOL
IpcServerDestroyEvent(PEVENT Event)
{
    return IpcDestroyEvent(Event);
}

DWORD
IpcServerThread(PTHREAD Thread,
                LPVOID UserData)
{
    PIPC_SERVER_THREAD_DATA ThreadData;
    EVENT ChannelReadyEvent;
    NAMED_PIPE Pipe;
    IPC_CHANNEL_HEADER Header;
    LPVOID InternalBuffer = NULL;
    BOOL Accepted;

    ThreadData = (PIPC_SERVER_THREAD_DATA)UserData;

    TRY(DWORD, 0)
    {
        if (!IpcOpenEventW(ThreadData->MultiSession ? L"Global" : NULL,
                           ThreadData->ChannelName,
                           L"_CHRDYEVNT",
                           &ChannelReadyEvent))
        {
            LEAVE(1);
        }

        if (!IpcCreateNamedPipe(ThreadData->ChannelName,
                                FALSE,
                                0x1000,
                                0x1000,
                                NMPWAIT_WAIT_FOREVER,
                                &Pipe))
        {
            LEAVE(1);
        }

        while (TRUE)
        {
            if (!IpcSetEvent(&ChannelReadyEvent))
            {
                MessageBoxA(0, "3", "event ok", 0);
                LEAVE(1);
            }

            if (!IpcWaitConnectClient(&Pipe,
                                      INFINITE,
                                      &Thread->DoSafeStop))
            {
                LEAVE(1);
            }

            if (!IpcReadFromNamedPipe(&Pipe,
                                      &Header,
                                      sizeof(IPC_CHANNEL_HEADER),
                                      TRUE,
                                      &Thread->DoSafeStop))
            {
                LEAVE(1);
            }

            Accepted = TRUE;

            if (!IpcWriteToNamedPipe(&Pipe,
                                     &Accepted,
                                     sizeof(BOOL),
                                     TRUE,
                                     &Thread->DoSafeStop))
            {
                LEAVE(1);
            }

            if (!(InternalBuffer = AllocMem(MAX(Header.MessageSize,
                                                Header.AnswerSize))))
            {
                LEAVE(1);
            }

            if (!IpcReadFromNamedPipe(&Pipe,
                                      InternalBuffer,
                                      Header.MessageSize,
                                      TRUE,
                                      &Thread->DoSafeStop))
            {
                LEAVE(1);
            }

            ThreadData->Routine(ThreadData->ChannelName,
                                InternalBuffer,
                                Header.MessageSize,
                                InternalBuffer,
                                Header.AnswerSize);

            if (Header.HasAnswer && !IpcWriteToNamedPipe(&Pipe,
                                                         InternalBuffer,
                                                         Header.AnswerSize,
                                                         TRUE,
                                                         &Thread->DoSafeStop))
            {
                LEAVE(1);
            }

            IpcFlushNamedPipe(&Pipe);
            IpcDisconnectClient(&Pipe);
            DeallocMem(InternalBuffer);
            InternalBuffer = NULL;

        }
    }
    FINALLY
    {
        IpcCloseEvent(&ChannelReadyEvent);
        IpcDisconnectClient(&Pipe);
        IpcDestroyNamedPipe(&Pipe);
        DeallocMem(InternalBuffer);
    }

    return TRY_VALUE;
}

BOOL
IpcServerCreateServerThread(LPCWSTR ChannelName,
                            IPC_ROUTINE Routine,
                            BOOL MultiSession,
                            PTHREAD *Thread)
{
    PIPC_SERVER_THREAD_DATA UserData;

    UserData = (PIPC_SERVER_THREAD_DATA)
        AllocMem(sizeof(IPC_SERVER_THREAD_DATA));

    if (!UserData)
    {
        return FALSE;
    }

    wcscpy_s(UserData->ChannelName,
             ChannelName);

    UserData->Routine = Routine;
    UserData->MultiSession = MultiSession;

    if (!IpcCreateThread(IpcServerThread,
                         TRUE,
                         UserData,
                         Thread))
    {
        DeallocMem(UserData);
        return FALSE;
    }

    return TRUE;
}

BOOL
IpcDestroyServerThread(PTHREAD *Thread)
{
    BOOL Result;
    LPVOID UserData;

    UserData = IpcGetThreadUserData(*Thread);
    Result = DeallocMem(UserData);
    Result = IpcTerminateThread(*Thread, 0) && Result;
    Result = IpcDestroyThread(Thread) && Result;

    return Result;
}

