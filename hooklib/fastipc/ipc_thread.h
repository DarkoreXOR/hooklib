#ifndef __IPC_THREAD_H
#define __IPC_THREAD_H

#include <Windows.h>
#include "../defines.h"
#include "../memory/heap.h"

typedef struct _THREAD THREAD, *PTHREAD;

typedef
DWORD
(*IPC_THREAD_ROUTINE)(
    PTHREAD Thread,
    LPVOID UserData
);

typedef struct _THREAD
{
    DWORD ObjectId;
    HANDLE ObjectHandle;
    BOOL IsSuspended;
    LPVOID UserData;
    BOOL DoSafeStop;
    CRITICAL_SECTION CriticalSection;
    IPC_THREAD_ROUTINE ThreadRoutine;
} THREAD, *PTHREAD;

BOOL
IpcCreateThread(
    IN IPC_THREAD_ROUTINE ThreadRoutine,
    IN BOOL CreateSuspended,
    IN LPVOID UserData,
    OUT PTHREAD *Thread
);

BOOL
IpcDestroyThread(
    IN PTHREAD *Thread
);

VOID
IpcSafeStopThread(
    IN PTHREAD Thread,
    IN BOOL WaitThread,
    IN DWORD Timeout
);

BOOL
IpcIsSafeStopThread(
    IN PTHREAD Thread
);

BOOL
IpcTerminateThread(
    IN PTHREAD Thread,
    IN DWORD ExitCode
);

BOOL
IpcSuspendThread(
    IN PTHREAD Thread
);

BOOL
IpcResumeThread(
    IN PTHREAD Thread
);

LPVOID
IpcGetThreadUserData(
    IN PTHREAD Thread
);

#endif
