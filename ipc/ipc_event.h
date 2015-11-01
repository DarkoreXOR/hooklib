#ifndef __IPC_EVENT_H
#define __IPC_EVENT_H

#include <Windows.h>
#include <tchar.h>

#define IPC_EVENT_NAME_SIZE 0x200

typedef struct _EVENT
{
    WCHAR Name[IPC_EVENT_NAME_SIZE];
    HANDLE Handle;
} EVENT, *PEVENT;

BOOL
IpcCreateEventW(
    LPCWSTR ObjectNamespace,
    LPCWSTR ObjectName,
    LPCWSTR ObjectPostfix,
    BOOL ManualReset,
    BOOL InitialState,
    PEVENT Event
);

BOOL
IpcOpenEventW(
    LPCWSTR ObjectNamespace,
    LPCWSTR ObjectName,
    LPCWSTR ObjectPostfix,
    PEVENT Event
);

BOOL
IpcCloseEvent(
    PEVENT Event
);

BOOL
IpcDestroyEvent(
    PEVENT Event
);

BOOL
IpcSetEvent(
    PEVENT Event
);

BOOL
IpcResetEvent(
    PEVENT Event
);

DWORD
IpcWaitEvent(
    PEVENT Event,
    DWORD Timeout
);

#endif
