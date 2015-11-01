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
    BOOL Result;
    CHANNEL_HEADER ChannelHeader;
    PSHARED_MEMORY LocalSharedMemory = NULL;
    PEVENT LocalChannelMessageEvent = NULL;
    PEVENT LocalChannelAnswerEvent = NULL;
    PEVENT LocalChannelReadyEvent = NULL;
    LPVOID MessageBuf;
    SIZE_T MessageSize;
    LPVOID AnswerBuf;
    SIZE_T AnswerSize;
    SIZE_T LocalChannelSize;

    Result = GlobalWaitChannelSendHeaderEvent(ThreadData->GlobalChannelName,
                                              ThreadData->MultiSession,
                                              ThreadData->Timeout);

    if (!Result)
    {
        return FALSE;
    }

    Result = GlobalReadChannelHeaderData(ThreadData->GlobalSharedMemory,
                                         &ChannelHeader);

    if (!Result)
    {
        return FALSE;
    }

    Result = GlobalSetChannelReadyEvent(ThreadData->GlobalChannelReadyEvent);

    if (!Result)
    {
        return FALSE;
    }

    LocalChannelSize = MAX(ChannelHeader.MessageSize, ChannelHeader.AnswerSize);

    Result = LocalCreateChannel(ChannelHeader.LocalChannelName,
                                ThreadData->MultiSession,
                                LocalChannelSize,
                                &LocalSharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    Result = LocalCreateChannelMessageEvent(ChannelHeader.LocalChannelName,
                                            ThreadData->MultiSession,
                                            &LocalChannelMessageEvent);

    if (!Result)
    {
        LocalDestroyChannel(LocalSharedMemory);
        return FALSE;
    }

    Result = LocalCreateChannelAnswerEvent(ChannelHeader.LocalChannelName,
                                           ThreadData->MultiSession,
                                           &LocalChannelAnswerEvent);

    if (!Result)
    {
        LocalDestroyChannel(LocalSharedMemory);
        LocalDestroyChannelMessageEvent(LocalChannelMessageEvent);
        return FALSE;
    }

    Result = LocalCreateChannelReadyEvent(ChannelHeader.LocalChannelName,
                                          ThreadData->MultiSession,
                                          &LocalChannelReadyEvent);

    if (!Result)
    {
        LocalDestroyChannel(LocalSharedMemory);
        LocalDestroyChannelMessageEvent(LocalChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(LocalChannelAnswerEvent);
        return FALSE;
    }

    Result = GlobalSetLocalChannelReadyEvent(ThreadData->GlobalLocalChannelReadyEvent);

    if (!Result)
    {
        LocalDestroyChannel(LocalSharedMemory);
        LocalDestroyChannelMessageEvent(LocalChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(LocalChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(LocalChannelReadyEvent);
        return FALSE;
    }

    Result = LocalWaitChannelMessageEvent(LocalChannelMessageEvent,
                                          ThreadData->Timeout);

    if (!Result)
    {
        LocalDestroyChannel(LocalSharedMemory);
        LocalDestroyChannelMessageEvent(LocalChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(LocalChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(LocalChannelReadyEvent);
        return FALSE;
    }

    MessageSize = ChannelHeader.MessageSize;
    AnswerSize = ChannelHeader.AnswerSize;

    MessageBuf = malloc(MessageSize);
    AnswerBuf = LocalSharedMemory->MemoryPointer;

    if (!MessageBuf)
    {
        LocalDestroyChannel(LocalSharedMemory);
        LocalDestroyChannelMessageEvent(LocalChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(LocalChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(LocalChannelReadyEvent);
        return FALSE;
    }

    Result = LocalReadChannelMessage(LocalSharedMemory,
                                     MessageBuf,
                                     MessageSize);

    if (!Result)
    {
        LocalDestroyChannel(LocalSharedMemory);
        LocalDestroyChannelMessageEvent(LocalChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(LocalChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(LocalChannelReadyEvent);
        free(MessageBuf);
        return FALSE;
    }

    ThreadData->Routine(ThreadData->GlobalChannelName,
                        MessageBuf,
                        MessageSize,
                        AnswerBuf,
                        AnswerSize);

    free(MessageBuf);

    Result = LocalSetChannelAnswerEvent(LocalChannelAnswerEvent);

    if (!Result)
    {
        LocalDestroyChannel(LocalSharedMemory);
        LocalDestroyChannelMessageEvent(LocalChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(LocalChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(LocalChannelReadyEvent);
        return FALSE;
    }

    Result = LocalWaitChannelReadyEvent(LocalChannelReadyEvent,
                                        ThreadData->Timeout);

    if (!Result)
    {
        LocalDestroyChannel(LocalSharedMemory);
        LocalDestroyChannelMessageEvent(LocalChannelMessageEvent);
        LocalDestroyChannelAnswerEvent(LocalChannelAnswerEvent);
        LocalDestroyChannelReadyEvent(LocalChannelReadyEvent);
        return FALSE;
    }

    LocalDestroyChannel(LocalSharedMemory);
    LocalDestroyChannelMessageEvent(LocalChannelMessageEvent);
    LocalDestroyChannelAnswerEvent(LocalChannelAnswerEvent);
    LocalDestroyChannelReadyEvent(LocalChannelReadyEvent);

    return TRUE;
}

DWORD
WINAPI
IpcThreadProc(PTHREAD_DATA td)
{
    BOOL Result;
    THREAD_DATA ThreadData;

    if (!td)
    {
        return 1;
    }

    CopyThreadData(&ThreadData, td);

    do
    {
        Result = ThreadLoop(&ThreadData);
        if (!Result)
        {
            break;
        }
    }
    while (TRUE);

    if (!Result)
    {
        return 1;
    }

    return 0;
}

BOOL
StartIpcThread(LPCWSTR ChannelName,
               IPC_ROUTINE Routine,
               PSHARED_MEMORY GlobalSharedMemory,
               PEVENT GlobalChannelReadyEvent,
               PEVENT GlobalChannelSendHeaderEvent,
               PEVENT GlobalLocalChannelReadyEvent,
               DWORD Timeout,
               BOOL MultiSession)
{
    HANDLE ThreadHandle;
    DWORD ThreadId;
    PTHREAD_DATA ThreadData = (PTHREAD_DATA)malloc(sizeof(THREAD_DATA));

    if (!ThreadData)
    {
        return FALSE;
    }

    wcscpy_s(ThreadData->GlobalChannelName, 0x200, ChannelName);
    ThreadData->Routine = Routine;
    ThreadData->GlobalSharedMemory = GlobalSharedMemory;
    ThreadData->GlobalChannelReadyEvent = GlobalChannelReadyEvent;
    ThreadData->GlobalChannelSendHeaderEvent = GlobalChannelSendHeaderEvent;
    ThreadData->GlobalLocalChannelReadyEvent = GlobalLocalChannelReadyEvent;
    ThreadData->Timeout = Timeout;
    ThreadData->MultiSession = MultiSession;

    ThreadHandle = CreateThread(NULL,
                                0,
                                (LPTHREAD_START_ROUTINE)IpcThreadProc,
                                ThreadData,
                                CREATE_SUSPENDED,
                                &ThreadId);

    ThreadData->ThreadHandle = ThreadHandle;
    ThreadData->ThreadId = ThreadId;

    if (!ThreadHandle)
    {
        free(ThreadData);
        return FALSE;
    }

    ResumeThread(ThreadHandle);

    return TRUE;
}

BOOL
IpcCreateIpcChannel(LPCWSTR ChannelName,
                    IPC_ROUTINE Routine,
                    BOOL MultiSession,
                    PVOID *ChannelData)
{
    BOOL Result;
    PSHARED_MEMORY GlobalSharedMemory;
    PEVENT GlobalChannelReadyEvent;
    PEVENT GlobalChannelSendHeaderEvent;
    PEVENT GlobalLocalChannelReadyEvent;
    PCHANNEL_DATA ChannelDataStruct;

    if (!ChannelName || !Routine || !ChannelData)
    {
        return FALSE;
    }

    Result = GlobalCreateChannel(ChannelName,
                                 MultiSession,
                                 sizeof(CHANNEL_HEADER),
                                 &GlobalSharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    Result = GlobalCreateChannelReadyEvent(ChannelName,
                                           MultiSession,
                                           &GlobalChannelReadyEvent);

    if (!Result)
    {
        GlobalDestroyChannel(GlobalSharedMemory);
        return FALSE;
    }

    Result = GlobalCreateChannelSendHeaderEvent(ChannelName,
                                                MultiSession,
                                                &GlobalChannelSendHeaderEvent);

    if (!Result)
    {
        GlobalDestroyChannel(GlobalSharedMemory);
        GlobalDestroyChannelReadyEvent(GlobalChannelReadyEvent);
        return FALSE;
    }

    Result = GlobalCreateLocalChannelReadyEvent(ChannelName,
                                                MultiSession,
                                                &GlobalLocalChannelReadyEvent);

    if (!Result)
    {
        GlobalDestroyChannel(GlobalSharedMemory);
        GlobalDestroyChannelReadyEvent(GlobalChannelReadyEvent);
        GlobalDestroyChannelSendHeaderEvent(GlobalChannelSendHeaderEvent);
        return FALSE;
    }

    ChannelDataStruct = (PCHANNEL_DATA)malloc(sizeof(CHANNEL_DATA));

    if (!ChannelDataStruct)
    {
        GlobalDestroyChannel(GlobalSharedMemory);
        GlobalDestroyChannelReadyEvent(GlobalChannelReadyEvent);
        GlobalDestroyChannelSendHeaderEvent(GlobalChannelSendHeaderEvent);
        GlobalDestroyLocalChannelReadyEvent(GlobalLocalChannelReadyEvent);
        return FALSE;
    }

    *ChannelData = ChannelDataStruct;
    ChannelDataStruct->GlobalSharedMemory = GlobalSharedMemory;
    ChannelDataStruct->GlobalChannelReadyEvent = GlobalChannelReadyEvent;
    ChannelDataStruct->GlobalChannelSendHeaderEvent = GlobalChannelSendHeaderEvent;
    ChannelDataStruct->GlobalLocalChannelReadyEvent = GlobalLocalChannelReadyEvent;

    Result = StartIpcThread(ChannelName,
                            Routine,
                            GlobalSharedMemory,
                            GlobalChannelReadyEvent,
                            GlobalChannelSendHeaderEvent,
                            GlobalLocalChannelReadyEvent,
                            INFINITE,
                            MultiSession);

    if (!Result)
    {
        free(ChannelDataStruct);
        *ChannelData = NULL;
        GlobalDestroyChannel(GlobalSharedMemory);
        GlobalDestroyChannelReadyEvent(GlobalChannelReadyEvent);
        GlobalDestroyChannelSendHeaderEvent(GlobalChannelSendHeaderEvent);
        GlobalDestroyLocalChannelReadyEvent(GlobalLocalChannelReadyEvent);
        return FALSE;
    }

    return TRUE;
}

BOOL
IpcDestroyIpcChannel(PVOID ChannelData)
{
    BOOL Result;
    PCHANNEL_DATA ChannelStruct;

    if (!ChannelData)
    {
        return FALSE;
    }

    ChannelStruct = (PCHANNEL_DATA)ChannelData;

    Result = GlobalDestroyChannel(ChannelStruct->GlobalSharedMemory);
    Result = GlobalDestroyChannelReadyEvent(ChannelStruct->GlobalChannelReadyEvent) && Result;
    Result = GlobalDestroyChannelSendHeaderEvent(ChannelStruct->GlobalChannelSendHeaderEvent) && Result;
    Result = GlobalDestroyLocalChannelReadyEvent(ChannelStruct->GlobalLocalChannelReadyEvent) && Result;

    free(ChannelData);

    return Result;
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
    wcscpy_s(ChannelHeader->LocalChannelName, 0x200, LocalChannelName);
    ChannelHeader->MessageSize = MessageSize;
    ChannelHeader->AnswerSize = AnswerSize;
    ChannelHeader->HasMessage = MessageBuf && MessageSize > 0;
    ChannelHeader->HasAnswer = AnswerBuf && AnswerSize > 0;
    ChannelHeader->UniqueId = UniqueId;
}

VOID
GenerateUniqueLocalChannelName(LPCWSTR ChannelName,
                               LPWSTR LocalChannelName,
                               SIZE_T LocalChannelNameSize,
                               UINT64 UniqueId)
{
    WCHAR UniqueIdString[0x100];
    _ui64tow_s(UniqueId, UniqueIdString, 0x100, 10);
    
    wcscpy_s(LocalChannelName, LocalChannelNameSize, ChannelName);
    wcscat_s(LocalChannelName, LocalChannelNameSize, IPC_LOCAL_CHANNEL);
    wcscat_s(LocalChannelName, LocalChannelNameSize, UniqueIdString);
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
    BOOL Result;
    SHARED_MEMORY GlobalSharedMemory;
    CHANNEL_HEADER GlobalChannelHeader;
    UINT64 UniqueLocalChannelId;
    WCHAR LocalChannelName[0x200];
    SHARED_MEMORY LocalSharedMemory;

    Result = GlobalWaitChannelReadyEvent(ChannelName,
                                         MultiSession,
                                         Timeout);

    if (!Result)
    {
        return FALSE;
    }

    Result = GlobalOpenChannel(ChannelName,
                               MultiSession,
                               &GlobalSharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    Result = GlobalReadChannelHeaderData(&GlobalSharedMemory,
                                         &GlobalChannelHeader);

    if (!Result)
    {
        GlobalCloseChannel(&GlobalSharedMemory);
        return FALSE;
    }

    UniqueLocalChannelId = ++GlobalChannelHeader.UniqueId;

    GenerateUniqueLocalChannelName(ChannelName,
                                   LocalChannelName,
                                   0x200,
                                   UniqueLocalChannelId);

    BuildHeader(&GlobalChannelHeader,
                LocalChannelName,
                MessageBuf,
                MessageSize,
                AnswerBuf,
                AnswerSize,
                UniqueLocalChannelId);

    Result = GlobalWriteChannelHeaderData(&GlobalSharedMemory,
                                          &GlobalChannelHeader);

    if (!Result)
    {
        GlobalCloseChannel(&GlobalSharedMemory);
        return FALSE;
    }

    Result = GlobalCloseChannel(&GlobalSharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    Result = GlobalSetChannelSendHeaderEvent(ChannelName, MultiSession);

    if (!Result)
    {
        return FALSE;
    }

    Result = GlobalWaitLocalChannelReadyEvent(ChannelName,
                                              MultiSession,
                                              Timeout);

    if (!Result)
    {
        return FALSE;
    }

    Result = LocalOpenChannel(LocalChannelName,
                              MultiSession,
                              &LocalSharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    Result = LocalWriteChannelMessage(&LocalSharedMemory,
                                      MessageBuf,
                                      MessageSize);

    if (!Result)
    {
        LocalCloseChannel(&LocalSharedMemory);
        return FALSE;
    }

    Result = LocalSetChannelMessageEvent(LocalChannelName,
                                         MultiSession);

    if (!Result)
    {
        LocalCloseChannel(&LocalSharedMemory);
        return FALSE;
    }

    Result = LocalWaitChannelAnswerEvent(LocalChannelName,
                                         MultiSession,
                                         Timeout);

    if (!Result)
    {
        LocalCloseChannel(&LocalSharedMemory);
        return FALSE;
    }

    Result = LocalReadChannelAnswer(&LocalSharedMemory,
                                    AnswerBuf,
                                    AnswerSize);

    if (!Result)
    {
        LocalCloseChannel(&LocalSharedMemory);
        return FALSE;
    }

    Result = LocalSetChannelReadyEvent(LocalChannelName,
                                       MultiSession);

    if (!Result)
    {
        LocalCloseChannel(&LocalSharedMemory);
        return FALSE;
    }

    Result = LocalCloseChannel(&LocalSharedMemory);

    if (!Result)
    {
        return FALSE;
    }

    return TRUE;
}

