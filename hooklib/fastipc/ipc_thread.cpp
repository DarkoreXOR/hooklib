#include "ipc_thread.h"

DWORD
WINAPI
IpcSystemThreadRoutine(IN LPVOID Parameter)
{
    PTHREAD CurrentThread;

    if (!Parameter)
    {
        return 1;
    }

    CurrentThread = (PTHREAD)Parameter;

    return CurrentThread->ThreadRoutine(CurrentThread,
                                        CurrentThread->UserData);
}

BOOL
IpcCreateThread(IN IPC_THREAD_ROUTINE ThreadRoutine,
                IN BOOL CreateSuspended,
                IN LPVOID UserData,
                OUT PTHREAD *Thread)
{
    PTHREAD NewThread = NULL;

    TRY(BOOL, TRUE)
    {
        if (!(NewThread = (PTHREAD)AllocMem(sizeof(THREAD))))
        {
            LEAVE(FALSE);
        }

        InitializeCriticalSection(&NewThread->CriticalSection);

        NewThread->DoSafeStop = FALSE;
        NewThread->IsSuspended = CreateSuspended;
        NewThread->ThreadRoutine = ThreadRoutine;
        NewThread->UserData = UserData;
        NewThread->ObjectHandle = CreateThread(NULL,
                                               0,
                                               IpcSystemThreadRoutine,
                                               NewThread,
                                               CREATE_SUSPENDED,
                                               &NewThread->ObjectId);

        if (!NewThread->ObjectHandle)
        {
            LEAVE(FALSE);
        }

        if (!CreateSuspended)
        {
            ResumeThread(NewThread->ObjectHandle);
        }
    }
    FINALLY
    {
        if (!TRY_VALUE && NewThread)
        {
            DeleteCriticalSection(&NewThread->CriticalSection);
            TerminateThread(NewThread->ObjectHandle, 0);
            CloseHandle(NewThread->ObjectHandle);
            DeallocMem(NewThread);
            NewThread = NULL;
        }
    }

    *Thread = NewThread;

    return TRY_VALUE;
}

BOOL
IpcDestroyThread(IN PTHREAD *Thread)
{
    BOOL Result;

    DeleteCriticalSection(&(*Thread)->CriticalSection);
    Result = CloseHandle((*Thread)->ObjectHandle);
    Result = DeallocMem(*Thread) && Result;

    *Thread = NULL;

    return Result;
}

VOID
IpcSafeStopThread(IN PTHREAD Thread,
                  IN BOOL WaitThread,
                  IN DWORD Timeout)
{
    EnterCriticalSection(&Thread->CriticalSection);
    Thread->DoSafeStop = TRUE;
    LeaveCriticalSection(&Thread->CriticalSection);

    if (WaitThread)
    {
        WaitForSingleObject(Thread->ObjectHandle, Timeout);
    }
}

BOOL
IpcIsSafeStopThread(IN PTHREAD Thread)
{
    BOOL Result;
    EnterCriticalSection(&Thread->CriticalSection);
    Result = Thread->DoSafeStop;
    LeaveCriticalSection(&Thread->CriticalSection);
    return Result;
}

BOOL
IpcTerminateThread(IN PTHREAD Thread,
                   IN DWORD ExitCode)
{
    return TerminateThread(Thread->ObjectHandle, ExitCode);
}

BOOL
IpcSuspendThread(IN PTHREAD Thread)
{
    if (Thread->IsSuspended)
    {
        return FALSE;
    }

    SuspendThread(Thread->ObjectHandle);
    Thread->IsSuspended = TRUE;

    return TRUE;
}

BOOL
IpcResumeThread(IN PTHREAD Thread)
{
    if (!Thread->IsSuspended)
    {
        return FALSE;
    }

    ResumeThread(Thread->ObjectHandle);
    Thread->IsSuspended = FALSE;

    return TRUE;
}

LPVOID
IpcGetThreadUserData(IN PTHREAD Thread)
{
    return Thread->UserData;
}
