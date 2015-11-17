#include "ipc_client.h"

DWORD
IpcWaitEventEx(LPCWSTR ChannelName,
               LPCWSTR Postfix,
               BOOL GlobalObject,
               DWORD Timeout)
{
    EVENT EventObject;
    LPCWSTR ObjectNamespace = NULL;
    DWORD WaitStatus;
    DWORD TimeoutEx;

    if (Timeout == 0)
    {
        return FALSE;
    }

    if (GlobalObject)
    {
        ObjectNamespace = L"Global";
    }

    TimeoutEx = Timeout;

    while (TRUE)
    {
        if (!IpcOpenEventW(ObjectNamespace,
                           ChannelName,
                           Postfix,
                           &EventObject))
        {
            return WAIT_FAILED;
        }

        WaitStatus = IpcWaitEvent(&EventObject, 1);

        TimeoutEx--;

        IpcCloseEvent(&EventObject);

        if (WaitStatus != WAIT_TIMEOUT)
        {
            break;
        }

        if (!TimeoutEx && Timeout != INFINITE)
        {
            break;
        }
    }

    return WaitStatus;
}


BOOL
IpcClientWaitChannel(LPCWSTR ChannelName,
                     BOOL MultiSession,
                     DWORD Timeout)
{
    DWORD WaitStatus;

    WaitStatus = IpcWaitEventEx(ChannelName,
                                L"_CHRDYEVNT",
                                MultiSession,
                                Timeout);

    return WaitStatus == WAIT_OBJECT_0;
}

BOOL
IpcClientCallIpcServer(LPCWSTR ChannelName,
                       LPVOID MessageBuffer,
                       DWORD MessageSize,
                       LPVOID AnswerBuffer,
                       DWORD AnswerSize,
                       DWORD Timeout)
{
    NAMED_PIPE Pipe;
    IPC_CHANNEL_HEADER Header;
    BOOL Accepted;

    TRY(BOOL, TRUE)
    {
        if (!IpcOpenNamedPipe(ChannelName,
                              TRUE,
                              &Pipe))
        {
            LEAVE(FALSE);
        }

        Header.HasMessage = MessageSize && MessageBuffer;
        Header.HasAnswer = AnswerSize && AnswerBuffer;
        Header.MessageSize = MessageSize;
        Header.AnswerSize = AnswerSize;

        if (!IpcWriteToNamedPipe(&Pipe,
                                 &Header,
                                 sizeof(IPC_CHANNEL_HEADER),
                                 FALSE,
                                 NULL))
        {
            LEAVE(FALSE);
        }

        if (!IpcReadFromNamedPipe(&Pipe,
                                  &Accepted,
                                  sizeof(BOOL),
                                  FALSE,
                                  NULL))
        {
            LEAVE(FALSE);
        }

        if (!IpcWriteToNamedPipe(&Pipe,
                                 MessageBuffer,
                                 MessageSize,
                                 FALSE,
                                 NULL))
        {
            LEAVE(FALSE);
        }

        if (Header.HasAnswer && !IpcReadFromNamedPipe(&Pipe,
                                                      AnswerBuffer,
                                                      AnswerSize,
                                                      FALSE,
                                                      NULL))
        {
            LEAVE(FALSE);
        }
    }
    FINALLY
    {
        IpcCloseNamedPipe(&Pipe);
    }

    return TRY_VALUE;
}

