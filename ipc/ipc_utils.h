#ifndef __IPC_UTILS_H
#define __IPC_UTILS_H

#include <Windows.h>
#include "ipc_event.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

VOID
IpcMakeObjectName(
    LPWSTR DestinationObjectName,
    SIZE_T DestinationObjectNameLength,
    LPCWSTR ObjectNamespace,
    LPCWSTR ObjectName,
    LPCWSTR ObjectPostfix
);

DWORD
IpcWaitEventEx(
    LPCWSTR ChannelName,
    LPCWSTR Postfix,
    BOOL GlobalObject,
    DWORD Timeout
);

#endif
