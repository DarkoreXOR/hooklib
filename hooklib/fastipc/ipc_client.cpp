#include "ipc_client.h"

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
                                 NULL,
                                 NULL))
        {
            LEAVE(FALSE);
        }

        if (!IpcReadFromNamedPipe(&Pipe,
                                  &Accepted,
                                  sizeof(BOOL),
                                  FALSE,
                                  NULL,
                                  NULL))
        {
            LEAVE(FALSE);
        }

        if (!IpcWriteToNamedPipe(&Pipe,
                                 MessageBuffer,
                                 MessageSize,
                                 FALSE,
                                 NULL,
                                 NULL))
        {
            LEAVE(FALSE);
        }

        if (Header.HasAnswer && !IpcReadFromNamedPipe(&Pipe,
                                                      AnswerBuffer,
                                                      AnswerSize,
                                                      FALSE,
                                                      NULL,
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

