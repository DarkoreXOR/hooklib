#ifndef __IPC_TYPES_H
#define __IPC_TYPES_H

#include <Windows.h>

typedef struct _CHANNEL_HEADER
{
    UINT64 UniqueId;
    BOOL HasMessage;
    BOOL HasAnswer;
    DWORD MessageSize;
    DWORD AnswerSize;
    WCHAR LocalChannelName[0x200];
} CHANNEL_HEADER, *PCHANNEL_HEADER;

#endif
