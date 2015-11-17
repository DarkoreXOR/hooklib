#include "ipc_named_pipe.h"

VOID
IpcMakePipeName(OUT LPWSTR DestinationName,
                IN SIZE_T DestinationNameSize,
                IN LPCWSTR SourceName)
{
    wcscpy_s(DestinationName,
             DestinationNameSize,
             L"\\\\.\\pipe\\");

    wcscat_s(DestinationName,
             DestinationNameSize,
             SourceName);
}

BOOL
IpcCreateNamedPipe(IN LPCWSTR ObjectName,
                   IN BOOL FirstInstance,
                   IN DWORD OutBufferSize,
                   IN DWORD InBufferSize,
                   IN DWORD DefaultTimeout,
                   OUT PNAMED_PIPE NamedPipe)
{
    DWORD OpenMode;
    DWORD PipeMode;
    PSID sid;
    SECURITY_DESCRIPTOR sd = { 0 };
    SECURITY_ATTRIBUTES sa = { 0 };

    IpcMakePipeName(NamedPipe->ObjectName,
                    0x100,
                    ObjectName);

    NamedPipe->EventHandle = CreateEventW(NULL,
                                          TRUE,
                                          TRUE,
                                          NULL);

    if (!NamedPipe->EventHandle)
    {
        return FALSE;
    }

    OpenMode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;
    OpenMode |= FirstInstance ? FILE_FLAG_FIRST_PIPE_INSTANCE : 0;

    PipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE;
    PipeMode |= PIPE_WAIT;
    PipeMode |= PIPE_REJECT_REMOTE_CLIENTS;

    if (!ScCreateSecurityAttributes(&sa, &sd, &sid))
    {
        NamedPipe->ObjectHandle = 0;
        CloseHandle(NamedPipe->EventHandle);
        NamedPipe->EventHandle = 0;
        return FALSE;
    }

    NamedPipe->ObjectHandle = CreateNamedPipeW(NamedPipe->ObjectName,
                                               OpenMode,
                                               PipeMode,
                                               PIPE_UNLIMITED_INSTANCES,
                                               OutBufferSize,
                                               InBufferSize,
                                               DefaultTimeout,
                                               &sa);

    ScDestroySecurityAttributes(sid);

    if (NamedPipe->ObjectHandle == INVALID_HANDLE_VALUE ||
        GetLastError() == ERROR_ACCESS_DENIED)
    {
        NamedPipe->ObjectHandle = 0;
        CloseHandle(NamedPipe->EventHandle);
        NamedPipe->EventHandle = 0;
        return FALSE;
    }

    return TRUE;
}

#include <stdio.h>

BOOL
IpcDestroyNamedPipe(IN PNAMED_PIPE NamedPipe)
{
    BOOL Result = FALSE;

    if (!NamedPipe->ObjectHandle || !NamedPipe->EventHandle)
    {
        return FALSE;
    }

    TRY(BOOL, TRUE)
    {
        Result = CloseHandle(NamedPipe->EventHandle);
        NamedPipe->EventHandle = 0;
        Result = CloseHandle(NamedPipe->ObjectHandle) && Result;
        NamedPipe->ObjectHandle = 0;
        LEAVE(Result);
    }
    EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        return TRY_VALUE;
    }

    return Result;
}

UINT32
IpcGetCurrentTimeMilliseconds()
{
    UINT32 Milliseconds = 0;
    SYSTEMTIME Time;
    GetSystemTime(&Time);

    Milliseconds += Time.wMinute * 60 * 1000;
    Milliseconds += Time.wSecond * 1000;
    Milliseconds += Time.wMilliseconds;

    return Milliseconds;
}

BOOL
IpcOpenNamedPipe(IN LPCWSTR ObjectName,
                 IN BOOL WaitIfBusy,
                 OUT PNAMED_PIPE NamedPipe)
{
    DWORD LastError;

    IpcMakePipeName(NamedPipe->ObjectName,
                    0x100,
                    ObjectName);

    NamedPipe->EventHandle = CreateEventW(NULL,
                                          TRUE,
                                          TRUE,
                                          NULL);

    if (!NamedPipe->EventHandle)
    {
        return FALSE;
    }

    while (TRUE)
    {
        NamedPipe->ObjectHandle = CreateFileW(NamedPipe->ObjectName,
                                              GENERIC_READ | GENERIC_WRITE,
                                              0,
                                              NULL,
                                              OPEN_EXISTING,
                                              0,
                                              NULL);

        LastError = GetLastError();

        if (NamedPipe->ObjectHandle == INVALID_HANDLE_VALUE &&
            LastError == ERROR_FILE_NOT_FOUND)
        {
            NamedPipe->ObjectHandle = 0;
            CloseHandle(NamedPipe->EventHandle);
            NamedPipe->EventHandle = 0;
            return FALSE;
        }

        if (WaitIfBusy &&
            NamedPipe->ObjectHandle &&
            LastError == ERROR_PIPE_BUSY)
        {
            WaitNamedPipeW(NamedPipe->ObjectName,
                           NMPWAIT_WAIT_FOREVER);
            continue;
        }

        break;
    }

    if (!SetNamedPipeHandleState(NamedPipe->ObjectHandle,
                                 PIPE_READMODE_BYTE | PIPE_WAIT,
                                 NULL,
                                 NULL))
    {
        CloseHandle(NamedPipe->EventHandle);
        NamedPipe->EventHandle = 0;
        CloseHandle(NamedPipe->ObjectHandle);
        NamedPipe->ObjectHandle = 0;
        return FALSE;
    }

    return TRUE;
}

BOOL
IpcCloseNamedPipe(IN PNAMED_PIPE NamedPipe)
{
    BOOL Result = FALSE;

    if (!NamedPipe->ObjectHandle || !NamedPipe->EventHandle)
    {
        return FALSE;
    }

    TRY(BOOL, TRUE)
    {
        Result = CloseHandle(NamedPipe->EventHandle);
        NamedPipe->EventHandle = 0;
        Result = CloseHandle(NamedPipe->ObjectHandle) && Result;
        NamedPipe->ObjectHandle = 0;
        LEAVE(Result);
    }
    EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        return TRY_VALUE;
    }

    return Result;
}

BOOL
IpcWaitConnectClient(IN PNAMED_PIPE NamedPipe,
                     IN DWORD Timeout,
                     IN PBOOL LeaveFlag)
{
    BOOL Result;
    OVERLAPPED Overlapped = { 0 };
    DWORD UnknownValue;

    Overlapped.hEvent = NamedPipe->EventHandle;

    Result = ConnectNamedPipe(NamedPipe->ObjectHandle,
                              &Overlapped);

    if (Result && GetLastError() != ERROR_IO_PENDING)
    {
        return FALSE;
    }

    while (!GetOverlappedResult(NamedPipe->ObjectHandle,
                                &Overlapped,
                                &UnknownValue,
                                FALSE) && GetLastError() == ERROR_IO_INCOMPLETE)
    {
        if (LeaveFlag && *LeaveFlag)
        {
            break;
        }
    }

    if (HasOverlappedIoCompleted(&Overlapped))
    {
        Result = TRUE;
    }

    Result = GetOverlappedResult(NamedPipe->ObjectHandle,
                                 &Overlapped,
                                 &UnknownValue,
                                 FALSE);

    return Result;
}

BOOL
IpcDisconnectClient(IN PNAMED_PIPE NamedPipe)
{
    return DisconnectNamedPipe(NamedPipe->ObjectHandle);
}

BOOL
IpcReadFromNamedPipe(IN PNAMED_PIPE NamedPipe,
                     IN LPVOID Buffer,
                     IN DWORD BufferSize,
                     IN BOOL IsServerSide,
                     IN PBOOL LeaveFlag)
{
    BOOL Result;
    DWORD Read;
    OVERLAPPED Overlapped = { 0 };

    Overlapped.hEvent = NamedPipe->EventHandle;

    Result = ReadFile(NamedPipe->ObjectHandle,
                      Buffer,
                      BufferSize,
                      &Read,
                      IsServerSide ? &Overlapped : NULL);

    if (!IsServerSide)
    {
        return Result && Read == BufferSize;
    }

    // return if ReadFile is already completed

    if (Result &&
        Read == BufferSize &&
        GetLastError() == ERROR_SUCCESS &&
        HasOverlappedIoCompleted(&Overlapped))
    {
        return TRUE;
    }

    if (Result &&
        GetLastError() != ERROR_IO_PENDING &&
        !HasOverlappedIoCompleted(&Overlapped))
    {
        return FALSE;
    }

    // ReadFile is pending

    while (!GetOverlappedResult(NamedPipe->ObjectHandle,
                                &Overlapped,
                                &Read,
                                FALSE) && GetLastError() == ERROR_IO_INCOMPLETE)
    {
        if (LeaveFlag && *LeaveFlag)
        {
            break;
        }
    }

    if (HasOverlappedIoCompleted(&Overlapped))
    {
        Result = TRUE;
    }

    return Result && Read == BufferSize;
}

BOOL
IpcWriteToNamedPipe(IN PNAMED_PIPE NamedPipe,
                    IN LPVOID Buffer,
                    IN DWORD BufferSize,
                    IN BOOL IsServerSide,
                    IN PBOOL LeaveFlag)
{
    BOOL Result;
    DWORD Written;
    OVERLAPPED Overlapped = { 0 };

    Overlapped.hEvent = NamedPipe->EventHandle;

    Result = WriteFile(NamedPipe->ObjectHandle,
                       Buffer,
                       BufferSize,
                       &Written,
                       IsServerSide ? &Overlapped : NULL);

    if (!IsServerSide)
    {
        return Result && Written == BufferSize;
    }

    if (Result &&
        Written == BufferSize &&
        GetLastError() == ERROR_SUCCESS &&
        HasOverlappedIoCompleted(&Overlapped))
    {
        return TRUE;
    }

    if (Result &&
        GetLastError() != ERROR_IO_PENDING &&
        !HasOverlappedIoCompleted(&Overlapped))
    {
        return FALSE;
    }

    while (!GetOverlappedResult(NamedPipe->ObjectHandle,
                                &Overlapped,
                                &Written,
                                FALSE) && GetLastError() == ERROR_IO_INCOMPLETE)
    {
        if (LeaveFlag && *LeaveFlag)
        {
            break;
        }
    }

    if (HasOverlappedIoCompleted(&Overlapped))
    {
        Result = TRUE;
    }

    Result = GetOverlappedResult(NamedPipe->ObjectHandle,
                                 &Overlapped,
                                 &Written,
                                 FALSE);

    return Result && Written == BufferSize;
}

BOOL
IpcFlushNamedPipe(IN PNAMED_PIPE NamedPipe)
{
    return FlushFileBuffers(NamedPipe->ObjectHandle);
}
