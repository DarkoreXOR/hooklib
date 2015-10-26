#ifndef __IPC_TYPES_H
#define __IPC_TYPES_H

typedef struct _CHANNEL_HEADER
{
    UINT64 UniqueId;
    BOOL HasMessage;
    BOOL HasAnswer;
    DWORD MessageSize;
    DWORD AnswerSize;
    WCHAR LocalChannelName[0x100];
} CHANNEL_HEADER, *PCHANNEL_HEADER;

typedef
BOOL
(WINAPI *IPC_ROUTINE)(
    LPCWSTR ChannelName,
    PVOID MessageBuf,
    DWORD MessageSize,
    PVOID AnswerBuf,
    DWORD AnswerSize
);

#endif

