#include "ipc_utils.h"

BOOL
IpcMakeWideString(LPWSTR Dest,
                  LPCWSTR ChannelName)
{
    return wcscpy_s(Dest, 0x100, ChannelName) == 0;
}

BOOL
IpcAppendPostfix(LPWSTR Dest,
                 LPCWSTR Postfix)
{
    return wcscat_s(Dest, 0x100, Postfix) == 0;
}

BOOL
IpcAppendNumValue(LPWSTR Dest,
                  UINT64 Num)
{
    BOOL Result;
    WCHAR NumStr[0x100];
    Result = _i64tow_s(Num, NumStr, 0x100, 10) == 0;
    return wcscat_s(Dest, 0x100, NumStr) == 0 && Result;
}
