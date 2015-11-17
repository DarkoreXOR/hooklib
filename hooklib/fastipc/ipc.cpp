#include "ipc.h"

BOOL
IpcCreateIpcChannel(LPCWSTR ChannelName,
                    IPC_ROUTINE Routine,
                    BOOL MultiSession,
                    PVOID *ChannelData)
{
    PIPC_CHANNEL_DATA ChannelDataBuffer = NULL;
    PTHREAD Thread;

    if (!ChannelData)
    {
        return FALSE;
    }

    if (!(ChannelDataBuffer = IpcServerAllocateChannelData()))
    {
        return FALSE;
    }

    if (!IpcServerTryCreateChannel(ChannelName,
                                   MultiSession))
    {
        IpcServerDeallocateChannelData(ChannelDataBuffer);
        return FALSE;
    }

    if (!IpcServerCreateEvent(ChannelName,
                              MultiSession,
                              &ChannelDataBuffer->ChannelReadyEvent))
    {
        IpcServerDeallocateChannelData(ChannelDataBuffer);
        return FALSE;
    }

    if (!IpcServerCreateServerThread(ChannelName,
                                     Routine,
                                     MultiSession,
                                     &Thread))
    {
        IpcServerDeallocateChannelData(ChannelDataBuffer);
        IpcServerDestroyEvent(&ChannelDataBuffer->ChannelReadyEvent);
        return FALSE;
    }

    ChannelDataBuffer->ServerThread = Thread;

    IpcResumeThread(Thread);

    *ChannelData = ChannelDataBuffer;

    return TRUE;
}

BOOL
IpcDestroyIpcChannel(PVOID ChannelData)
{
    BOOL Result;
    PIPC_CHANNEL_DATA ChannelDataBuffer;

    if (!ChannelData)
    {
        return FALSE;
    }

    ChannelDataBuffer = (PIPC_CHANNEL_DATA)ChannelData;

    IpcSafeStopThread(ChannelDataBuffer->ServerThread,
                      TRUE,
                      INFINITE);

    Result = IpcDestroyServerThread(&ChannelDataBuffer->ServerThread);
    Result = IpcServerDestroyEvent(&ChannelDataBuffer->ChannelReadyEvent) && Result;
    Result = IpcServerDeallocateChannelData(ChannelDataBuffer) && Result;

    return Result;
}

BOOL
IpcSendIpcMessage(LPCWSTR ChannelName,
                  PVOID MessageBuffer,
                  DWORD MessageSize,
                  PVOID AnswerBuffer,
                  DWORD AnswerSize,
                  DWORD Timeout,
                  BOOL MultiSession)
{
    if (!IpcClientWaitChannel(ChannelName,
                              MultiSession,
                              Timeout))
    {
        return FALSE;
    }

    if (!IpcClientCallIpcServer(ChannelName,
                                MessageBuffer,
                                MessageSize,
                                AnswerBuffer,
                                AnswerSize,
                                Timeout))
    {
        return FALSE;
    }

    return TRUE;
}
