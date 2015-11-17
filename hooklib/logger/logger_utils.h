#ifndef __LOGGER_UTILS_H
#define __LOGGER_UTILS_H

#include <Windows.h>
#include <tchar.h>
#include "..\fastipc\ipc.h"

#define LOG_TYPE_DEFAULT 0x00
#define LOG_TYPE_WAIT 0x01

typedef struct _LGR_LOG_MESSAGE_PACKET
{
    BYTE Type;
    WCHAR File[MAX_PATH];
    WCHAR Function[0x100];
    UINT Line;
    WCHAR Message[0x100];
} LGR_LOG_MESSAGE_PACKET, *PLGR_LOG_MESSAGE_PACKET;

VOID
LgrLog(
    LPCSTR File,
    LPCSTR Function,
    UINT Line,
    LPCWSTR Format,
    ...
);

VOID
LgrLogWait(
    LPCSTR File,
    LPCSTR Function,
    UINT Line,
    LPCWSTR Format,
    ...
);

#define LOG(format, ...) \
LgrLog(__FILE__,\
       __FUNCSIG__,\
       __LINE__,\
       ##format,\
       ##__VA_ARGS__)

#define LOG_WAIT(format, ...) \
LgrLogWait(__FILE__,\
           __FUNCSIG__,\
           __LINE__,\
           ##format,\
           ##__VA_ARGS__)

#endif
