#include "ipc_server_side.h"

BOOL
SsAllocateThreadHandles(DWORD NumOfThreads,
                        HANDLE **ThreadHandles)
{
    *ThreadHandles = (HANDLE*)malloc(sizeof(HANDLE) * NumOfThreads);

    return *ThreadHandles != NULL;
}

VOID
SsDeallocateThreadHandles(HANDLE *ThreadHandles)
{
    free(ThreadHandles);
}

BOOL
SsCreateThread(LPCWSTR ChannelName,
               LPTHREAD_START_ROUTINE ThreadProc,
               IPC_ROUTINE Routine,
               BOOL CreateSuspended,
               PSHARED_MEMORY GlobalSharedMemory,
               PEVENT GlobalChannelReadyEvent,
               PEVENT GlobalChannelSendHeaderEvent,
               PEVENT GlobalLocalChannelReadyEvent,
               DWORD Timeout,
               BOOL MultiSession,
               PHANDLE ThreadHandle)
{
    PTHREAD_DATA threadData;

    if (!ThreadHandle)
    {
        return FALSE;
    }

    threadData = (PTHREAD_DATA)malloc(sizeof(THREAD_DATA));

    if (!threadData)
    {
        return FALSE;
    }

    wcscpy_s(threadData->GlobalChannelName,
             0x100,
             ChannelName);

    threadData->Routine = Routine;
    threadData->GlobalSharedMemory = GlobalSharedMemory;
    threadData->GlobalChannelReadyEvent = GlobalChannelReadyEvent;
    threadData->GlobalChannelSendHeaderEvent = GlobalChannelSendHeaderEvent;
    threadData->GlobalLocalChannelReadyEvent = GlobalLocalChannelReadyEvent;
    threadData->Timeout = Timeout;
    threadData->MultiSession = MultiSession;

    threadData->ThreadHandle = CreateThread(NULL,
                                            0,
                                            (LPTHREAD_START_ROUTINE)ThreadProc,
                                            threadData,
                                            CreateSuspended ? CREATE_SUSPENDED : 0,
                                            NULL);

    *ThreadHandle = threadData->ThreadHandle;

    if (!(threadData->ThreadHandle))
    {
        free(threadData);
        return FALSE;
    }

    return TRUE;
}

BOOL
SsCreateThreads(LPCWSTR ChannelName,
                LPTHREAD_START_ROUTINE ThreadProc,
                IPC_ROUTINE Routine,
                BOOL CreateSuspended,
                PSHARED_MEMORY GlobalSharedMemory,
                PEVENT GlobalChannelReadyEvent,
                PEVENT GlobalChannelSendHeaderEvent,
                PEVENT GlobalLocalChannelReadyEvent,
                DWORD Timeout,
                BOOL MultiSession,
                HANDLE *ThreadHandles[],
                DWORD NumOfThreads)
{
    BOOL result;
    DWORD numOfCreatedThreads = 0;

    if (!ThreadHandles)
    {
        return FALSE;
    }

    result = SsAllocateThreadHandles(NumOfThreads, ThreadHandles);

    if (!result)
    {
        return FALSE;
    }

    for (DWORD i = 0; i < NumOfThreads; ++i)
    {
        result = SsCreateThread(ChannelName,
                                ThreadProc,
                                Routine,
                                CreateSuspended,
                                GlobalSharedMemory,
                                GlobalChannelReadyEvent,
                                GlobalChannelSendHeaderEvent,
                                GlobalLocalChannelReadyEvent,
                                Timeout,
                                MultiSession,
                                &(*ThreadHandles)[i]);

        if (!result)
        {
            break;
        }

        numOfCreatedThreads++;
    }

    if (!result && numOfCreatedThreads > 0 && numOfCreatedThreads < NumOfThreads)
    {
        for (DWORD i = 0; i < numOfCreatedThreads; ++i)
        {
            TerminateThread(ThreadHandles[i], 0);
            CloseHandle(ThreadHandles[i]);
        }
    }

    return TRUE;
}

VOID
SsDestroyThreads(HANDLE *ThreadHandles,
                 DWORD NumOfThreads)
{
    for (DWORD i = 0; i < NumOfThreads; ++i)
    {
        TerminateThread(ThreadHandles[i], 0);
        CloseHandle(ThreadHandles[i]);
    }

    SsDeallocateThreadHandles(ThreadHandles);
}

VOID
SsSuspendThreads(HANDLE *ThreadHandles,
                 DWORD NumOfThreads)
{
    for (DWORD i = 0; i < NumOfThreads; ++i)
    {
        SuspendThread(ThreadHandles[i]);
    }
}

VOID
SsResumeThreads(HANDLE *ThreadHandles,
                DWORD NumOfThreads)
{
    for (DWORD i = 0; i < NumOfThreads; ++i)
    {
        ResumeThread(ThreadHandles[i]);
    }
}

BOOL
SsCreateGlobalObjects(LPCWSTR ChannelName,
                      BOOL MultiSession,
                      PSHARED_MEMORY *GlobalSharedMemory,
                      PEVENT *GlobalChannelReadyEvent,
                      PEVENT *GlobalChannelSendHeaderEvent,
                      PEVENT *GlobalLocalChannelReadyEvent)
{
    if(!GlobalCreateChannel(ChannelName,
                            MultiSession,
                            sizeof(CHANNEL_HEADER),
                            GlobalSharedMemory))
    {
        return FALSE;
    }

    if(!GlobalCreateChannelReadyEvent(ChannelName,
                                      MultiSession,
                                      GlobalChannelReadyEvent))
    {
        GlobalDestroyChannel(*GlobalSharedMemory);
        return FALSE;
    }

    if(!GlobalCreateChannelSendHeaderEvent(ChannelName,
                                           MultiSession,
                                           GlobalChannelSendHeaderEvent))
    {
        GlobalDestroyChannel(*GlobalSharedMemory);
        GlobalDestroyChannelReadyEvent(*GlobalChannelReadyEvent);
        return FALSE;
    }

    if(!GlobalCreateLocalChannelReadyEvent(ChannelName,
                                           MultiSession,
                                           GlobalLocalChannelReadyEvent))
    {
        GlobalDestroyChannel(*GlobalSharedMemory);
        GlobalDestroyChannelReadyEvent(*GlobalChannelReadyEvent);
        GlobalDestroyChannelSendHeaderEvent(*GlobalChannelSendHeaderEvent);
        return FALSE;
    }

    return TRUE;
}

BOOL
SsDestroyGlobalObjects(PSHARED_MEMORY GlobalSharedMemory,
                       PEVENT GlobalChannelReadyEvent,
                       PEVENT GlobalChannelSendHeaderEvent,
                       PEVENT GlobalLocalChannelReadyEvent)
{
    BOOL result;

    result = GlobalDestroyChannel(GlobalSharedMemory);
    result = GlobalDestroyChannelReadyEvent(GlobalChannelReadyEvent) && result;
    result = GlobalDestroyChannelSendHeaderEvent(GlobalChannelSendHeaderEvent) && result;
    result = GlobalDestroyLocalChannelReadyEvent(GlobalLocalChannelReadyEvent) && result;

    return result;
}

VOID
SsInitializeChannelData(PCHANNEL_DATA ChannelData,
                        PSHARED_MEMORY GlobalSharedMemory,
                        PEVENT GlobalChannelReadyEvent,
                        PEVENT GlobalChannelSendHeaderEvent,
                        PEVENT GlobalLocalChannelReadyEvent,
                        DWORD NumOfThreads,
                        HANDLE *ThreadHandles)
{
    ChannelData->GlobalSharedMemory = GlobalSharedMemory;
    ChannelData->GlobalChannelReadyEvent = GlobalChannelReadyEvent;
    ChannelData->GlobalChannelSendHeaderEvent = GlobalChannelSendHeaderEvent;
    ChannelData->GlobalLocalChannelReadyEvent = GlobalLocalChannelReadyEvent;
    ChannelData->NumOfIpcThreads = NumOfThreads;
    ChannelData->IpcThreads = ThreadHandles;
}
