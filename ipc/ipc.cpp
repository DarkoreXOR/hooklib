#include "ipc.h"

VOID
CopyThreadData(PTHREAD_DATA Dest,
               PTHREAD_DATA Source)
{
    CopyMemory(Dest, Source, sizeof(THREAD_DATA));
    free(Source);
}

BOOL
ThreadLoop(PTHREAD_DATA ThreadData)
{
    BOOL result;
    CHANNEL_HEADER channelHeader;
    PSHARED_MEMORY localSharedMemory = NULL;
    PEVENT localChannelMessageEvent = NULL;
    PEVENT localChannelAnswerEvent = NULL;
    PEVENT localChannelReadyEvent = NULL;
    LPVOID messageBuffer;
    DWORD messageSize;
    LPVOID answerBuffer;
    DWORD answerSize;
    DWORD localChannelSize;

    result = GlobalWaitChannelSendHeaderEvent(ThreadData->GlobalChannelName,
                                              ThreadData->MultiSession,
                                              ThreadData->Timeout);

    if (!result)
    {
        GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);
        return FALSE;
    }

    result = GlobalReadChannelHeaderData(ThreadData->GlobalSharedMemory,
                                         &channelHeader);

    if (!result)
    {
        GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);
        return FALSE;
    }

    messageSize = 0;
    answerSize = 0;

    if (channelHeader.HasMessage)
    {
        messageSize = channelHeader.MessageSize;
    }

    if (channelHeader.HasAnswer)
    {
        answerSize = channelHeader.AnswerSize;
    }

    localChannelSize = MAX(messageSize, answerSize);

    if (localChannelSize == 0)
    {
        GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);
        return FALSE;
    }

    result = LocalCreateChannel(channelHeader.LocalChannelName,
                                ThreadData->MultiSession,
                                localChannelSize,
                                &localSharedMemory);

    if (!result)
    {
        GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);
        return FALSE;
    }



    result = LocalCreateChannelMessageEvent(channelHeader.LocalChannelName,
                                            ThreadData->MultiSession,
                                            &localChannelMessageEvent);

    if (!result)
    {
        LocalDestroyChannel(localSharedMemory);
        GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);
        return FALSE;
    }


    result = LocalCreateChannelAnswerEvent(channelHeader.LocalChannelName,
                                           ThreadData->MultiSession,
                                           &localChannelAnswerEvent);

    if (!result)
    {
        LocalDestroyChannel(localSharedMemory);
        LocalDestroyChannelMessageEvent(localChannelMessageEvent);
        GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);
        return FALSE;
    }



    result = LocalCreateChannelReadyEvent(channelHeader.LocalChannelName,
                                          ThreadData->MultiSession,
                                          &localChannelReadyEvent);

    if (!result)
    {
        LocalDestroyChannel(localSharedMemory);
        LocalDestroyChannelMessageEvent(localChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(localChannelAnswerEvent);
        GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);
        return FALSE;
    }

    result = GlobalSetLocalChannelReadyEvent(ThreadData->GlobalLocalChannelReadyEvent);

    if (!result)
    {
        LocalDestroyChannel(localSharedMemory);
        LocalDestroyChannelMessageEvent(localChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(localChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(localChannelReadyEvent);
        GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);
        return FALSE;
    }

    result = LocalWaitChannelMessageEvent(localChannelMessageEvent,
                                          ThreadData->Timeout);

    if (!result)
    {
        LocalDestroyChannel(localSharedMemory);
        LocalDestroyChannelMessageEvent(localChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(localChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(localChannelReadyEvent);
        GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);
        return FALSE;
    }

    result = GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);

    if (!result)
    {
        LocalDestroyChannel(localSharedMemory);
        LocalDestroyChannelMessageEvent(localChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(localChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(localChannelReadyEvent);
        return FALSE;
    }

    messageBuffer = malloc(messageSize);

    if (!messageBuffer)
    {
        LocalDestroyChannel(localSharedMemory);
        LocalDestroyChannelMessageEvent(localChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(localChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(localChannelReadyEvent);
        return FALSE;
    }

    result = LocalReadChannelMessage(localSharedMemory,
                                     messageBuffer,
                                     messageSize);

    if (!result)
    {
        LocalDestroyChannel(localSharedMemory);
        LocalDestroyChannelMessageEvent(localChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(localChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(localChannelReadyEvent);
        free(messageBuffer);
        return FALSE;
    }

    answerBuffer = localSharedMemory->MemoryPointer;

    ThreadData->Routine(ThreadData->GlobalChannelName,
                        messageBuffer,
                        messageSize,
                        answerBuffer,
                        answerSize);

    free(messageBuffer);

    result = LocalSetChannelAnswerEvent(localChannelAnswerEvent);

    if (!result)
    {
        LocalDestroyChannel(localSharedMemory);
        LocalDestroyChannelMessageEvent(localChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(localChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(localChannelReadyEvent);
        return FALSE;
    }

    result = LocalWaitChannelReadyEvent(localChannelReadyEvent,
                                        ThreadData->Timeout);

    if (!result)
    {
        LocalDestroyChannel(localSharedMemory);
        LocalDestroyChannelMessageEvent(localChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(localChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(localChannelReadyEvent);
        return FALSE;
    }

    LocalDestroyChannel(localSharedMemory);
    LocalDestroyChannelMessageEvent(localChannelMessageEvent);
    LocalDestroyChannelAnswerEvent(localChannelAnswerEvent);
    LocalDestroyChannelReadyEvent(localChannelReadyEvent);

    return TRUE;
}

DWORD
WINAPI
IpcThreadProc(PTHREAD_DATA td)
{
    BOOL result;
    THREAD_DATA threadData;

    if (!td)
    {
        return 1;
    }

    CopyThreadData(&threadData, td);

    do
    {
        result = ThreadLoop(&threadData);

        if (!result)
        {
            break;
        }
    }
    while (TRUE);

    if (!result)
    {
        return 1;
    }

    return 0;
}

BOOL
IpcCreateIpcChannel(LPCWSTR ChannelName,
                    IPC_ROUTINE Routine,
                    BOOL MultiSession,
                    PVOID *ChannelData)
{
    BOOL result;
    PSHARED_MEMORY globalSharedMemory;
    PEVENT globalChannelReadyEvent;
    PEVENT globalChannelSendHeaderEvent;
    PEVENT globalLocalChannelReadyEvent;
    PCHANNEL_DATA channelDataStruct;
    DWORD numOfThreads = 4;
    HANDLE *threadHandles;

    if (!ChannelName || !Routine || !ChannelData)
    {
        return FALSE;
    }

    result = SsCreateGlobalObjects(ChannelName,
                                   MultiSession,
                                   &globalSharedMemory,
                                   &globalChannelReadyEvent,
                                   &globalChannelSendHeaderEvent,
                                   &globalLocalChannelReadyEvent);

    if (!result)
    {
        return FALSE;
    }

    result = SsCreateThreads(ChannelName,
                             (LPTHREAD_START_ROUTINE)IpcThreadProc,
                             Routine,
                             TRUE,
                             globalSharedMemory,
                             globalChannelReadyEvent,
                             globalChannelSendHeaderEvent,
                             globalLocalChannelReadyEvent,
                             INFINITE,
                             MultiSession,
                             &threadHandles,
                             numOfThreads);

    if (!result)
    {
        *ChannelData = NULL;
        SsDestroyGlobalObjects(globalSharedMemory,
                               globalChannelReadyEvent,
                               globalChannelSendHeaderEvent,
                               globalLocalChannelReadyEvent);
        return FALSE;
    }

    channelDataStruct = (PCHANNEL_DATA)malloc(sizeof(CHANNEL_DATA));

    if (!channelDataStruct)
    {
        SsDestroyGlobalObjects(globalSharedMemory,
                               globalChannelReadyEvent,
                               globalChannelSendHeaderEvent,
                               globalLocalChannelReadyEvent);
        return FALSE;
    }

    *ChannelData = channelDataStruct;

    SsInitializeChannelData(channelDataStruct,
                            globalSharedMemory,
                            globalChannelReadyEvent,
                            globalChannelSendHeaderEvent,
                            globalLocalChannelReadyEvent,
                            numOfThreads,
                            threadHandles);

    SsResumeThreads(threadHandles, numOfThreads);

    return TRUE;
}

BOOL
IpcDestroyIpcChannel(PVOID ChannelData)
{
    BOOL result;
    PCHANNEL_DATA pChannelData;

    if (!ChannelData)
    {
        return FALSE;
    }

    pChannelData = (PCHANNEL_DATA)ChannelData;

    SsDestroyThreads(pChannelData->IpcThreads,
                     pChannelData->NumOfIpcThreads);

    result = SsDestroyGlobalObjects(pChannelData->GlobalSharedMemory,
                                    pChannelData->GlobalChannelReadyEvent,
                                    pChannelData->GlobalChannelSendHeaderEvent,
                                    pChannelData->GlobalLocalChannelReadyEvent);

    free(ChannelData);

    return result;
}

VOID
BuildHeader(PCHANNEL_HEADER ChannelHeader,
            LPCWSTR LocalChannelName,
            PVOID MessageBuf,
            DWORD MessageSize,
            PVOID AnswerBuf,
            DWORD AnswerSize,
            UINT64 UniqueId)
{
    wcscpy_s(ChannelHeader->LocalChannelName, 0x100, LocalChannelName);
    ChannelHeader->HasMessage = MessageBuf && MessageSize > 0;
    ChannelHeader->HasAnswer = AnswerBuf && AnswerSize > 0;
    ChannelHeader->MessageSize = ChannelHeader->HasMessage ? MessageSize : 0;
    ChannelHeader->AnswerSize = ChannelHeader->HasAnswer ? AnswerSize : 0;
    ChannelHeader->UniqueId = UniqueId;
}

VOID
GenerateUniqueLocalChannelName(LPCWSTR ChannelName,
                               LPWSTR LocalChannelName,
                               SIZE_T LocalChannelNameSize,
                               UINT64 UniqueId)
{
    WCHAR uniqueIdString[0x100];

    _ui64tow_s(UniqueId,
               uniqueIdString,
               0x100,
               10);

    wcscpy_s(LocalChannelName,
             LocalChannelNameSize,
             ChannelName);

    wcscat_s(LocalChannelName,
             LocalChannelNameSize,
             IPC_LOCAL_CHANNEL);

    wcscat_s(LocalChannelName,
             LocalChannelNameSize,
             uniqueIdString);
}

BOOL
IpcSendIpcMessage(LPCWSTR ChannelName,
                  PVOID MessageBuf,
                  DWORD MessageSize,
                  PVOID AnswerBuf,
                  DWORD AnswerSize,
                  DWORD Timeout,
                  BOOL MultiSession)
{
    BOOL result;
    SHARED_MEMORY globalSharedMemory;
    CHANNEL_HEADER globalChannelHeader;
    UINT64 uniqueLocalChannelId;
    WCHAR localChannelName[0x100];
    SHARED_MEMORY localSharedMemory;

    result = GlobalWaitChannelReadyEvent(ChannelName,
                                         MultiSession,
                                         Timeout);

    if (!result)
    {
        return FALSE;
    }

    result = GlobalOpenChannel(ChannelName,
                               MultiSession,
                               &globalSharedMemory);

    if (!result)
    {
        return FALSE;
    }

    result = GlobalReadChannelHeaderData(&globalSharedMemory,
                                         &globalChannelHeader);

    if (!result)
    {
        GlobalCloseChannel(&globalSharedMemory);
        return FALSE;
    }

    uniqueLocalChannelId = ++globalChannelHeader.UniqueId;

    GenerateUniqueLocalChannelName(ChannelName,
                                   localChannelName,
                                   0x100,
                                   uniqueLocalChannelId);

    BuildHeader(&globalChannelHeader,
                localChannelName,
                MessageBuf,
                MessageSize,
                AnswerBuf,
                AnswerSize,
                uniqueLocalChannelId);

    result = GlobalWriteChannelHeaderData(&globalSharedMemory,
                                          &globalChannelHeader);

    if (!result)
    {
        GlobalCloseChannel(&globalSharedMemory);
        return FALSE;
    }

    result = GlobalCloseChannel(&globalSharedMemory);

    if (!result)
    {
        return FALSE;
    }

    result = GlobalSetChannelSendHeaderEvent(ChannelName, MultiSession);

    if (!result)
    {
        return FALSE;
    }

    result = GlobalWaitLocalChannelReadyEvent(ChannelName,
                                              MultiSession,
                                              Timeout);

    if (!result)
    {
        return FALSE;
    }

    result = LocalOpenChannel(localChannelName,
                              MultiSession,
                              &localSharedMemory);

    if (!result)
    {
        return FALSE;
    }

    result = LocalWriteChannelMessage(&localSharedMemory,
                                      MessageBuf,
                                      MessageSize);

    if (!result)
    {
        LocalCloseChannel(&localSharedMemory);
        return FALSE;
    }

    result = LocalSetChannelMessageEvent(localChannelName,
                                         MultiSession);

    if (!result)
    {
        LocalCloseChannel(&localSharedMemory);
        return FALSE;
    }

    result = LocalWaitChannelAnswerEvent(localChannelName,
                                         MultiSession,
                                         Timeout);

    if (!result)
    {
        LocalCloseChannel(&localSharedMemory);
        return FALSE;
    }

    result = LocalReadChannelAnswer(&localSharedMemory,
                                    AnswerBuf,
                                    AnswerSize);

    if (!result)
    {
        LocalCloseChannel(&localSharedMemory);
        return FALSE;
    }

    result = LocalSetChannelReadyEvent(localChannelName,
                                       MultiSession);

    if (!result)
    {
        LocalCloseChannel(&localSharedMemory);
        return FALSE;
    }

    result = LocalCloseChannel(&localSharedMemory);

    if (!result)
    {
        return FALSE;
    }

    return TRUE;
}
