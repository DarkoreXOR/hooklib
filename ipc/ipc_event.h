#ifndef __IPC_EVENT_H
#define __IPC_EVENT_H

#include <Windows.h>
#include <tchar.h>

typedef struct _EVENT
{
    WCHAR Name[0x100];
    HANDLE Handle;
} EVENT, *PEVENT;

BOOL
IpcCreateEventW(
    LPCWSTR EventName,
    LPCWSTR Postfix,
    BOOL ManualReset,
    BOOL InitialState,
    PEVENT Event
);

BOOL
IpcOpenEventW(
    LPCWSTR EventName,
    LPCWSTR Postfix,
    PEVENT Event
);

BOOL
IpcCloseEvent(PEVENT Event);

BOOL
IpcDestroyEvent(PEVENT Event);

BOOL
IpcSetEvent(PEVENT Event);

BOOL
IpcResetEvent(PEVENT Event);

DWORD
IpcWaitEvent(
    PEVENT Event,
    DWORD Timeout
);

#endif

