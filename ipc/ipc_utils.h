#ifndef __IPC_UTILS_H
#define __IPC_UTILS_H

#include <Windows.h>
#include <time.h>

BOOL
IpcMakeWideString(
    LPWSTR Dest,
    LPCWSTR ChannelName
);

BOOL
IpcAppendPostfix(
    LPWSTR Dest,
    LPCWSTR Postfix
);

BOOL
IpcAppendNumValue(
    LPWSTR Dest,
    UINT64 Num
);

#endif
