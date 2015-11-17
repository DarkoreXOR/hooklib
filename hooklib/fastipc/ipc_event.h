#ifndef __IPC_EVENT_H
#define __IPC_EVENT_H

#include <Windows.h>
#include <tchar.h>
#include "../security/security_utils.h"

#define IPC_EVENT_NAME_LENGTH 0x100

typedef struct _EVENT
{
    WCHAR ObjectName[IPC_EVENT_NAME_LENGTH];
    HANDLE ObjectHandle;
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
