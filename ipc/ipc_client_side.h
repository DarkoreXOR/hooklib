#ifndef __IPC_CLIENT_SIDE_H
#define __IPC_CLIENT_SIDE_H

#include "ipc_utils.h"
#include "ipc_shared_memory.h"
#include "ipc_event.h"
#include "ipc_defs.h"
#include "ipc_types.h"

BOOL
CsWaitReadyEvent(
    LPCWSTR ChannelName,
    DWORD Timeout
);

BOOL
CsReadUniqueId(
    LPCWSTR ChannelName,
    PUINT64 UniqueId
);

BOOL
CsGenerateUniqueName(
    LPWSTR Dest,
    LPCWSTR Postfix,
    UINT64 UniqueId,
    LPCWSTR ChannelName
);

VOID
CsMakeHeader(
    PCHANNEL_HEADER Header,
    UINT64 UniqueId,
    LPCWSTR LocalChannelName,
    PVOID MessageBuf,
    DWORD MessageSize,
    PVOID AnswerBuf,
    DWORD AnswerSize
);

BOOL
CsSetSendEvent(LPCWSTR ChannelName);

BOOL
CsHandleLocalChannel(
    LPCWSTR GlobalChannelName,
    LPCWSTR LocalChannelName,
    PCHANNEL_HEADER Header,
    PVOID MessageBuf,
    DWORD MessageSize,
    PVOID AnswerBuf,
    DWORD AnswerSize,
    DWORD WaitAnswerTimeout
);

#endif
