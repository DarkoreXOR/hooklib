#ifndef __IL_UTILS_H
#define __IL_UTILS_H

#include <Windows.h>
#include "..\native\native_utils.h"
#include "..\process\process_utils.h"
#include "..\logger\logger_utils.h"

BOOL
IlAllocate(
    HANDLE ProcessHandle,
    SIZE_T Size,
    DWORD Protect,
    LPVOID *Address
);

BOOL
IlWrite(
    HANDLE ProcessHandle,
    LPVOID Address,
    LPVOID Buffer,
    SIZE_T Size
);

BOOL
IlRead(
    HANDLE ProcessHandle,
    LPVOID Address,
    LPVOID Buffer,
    SIZE_T Size
);

BOOL
IlAllocateAndWrite(
    HANDLE ProcessHandle,
    LPVOID Buffer,
    SIZE_T Size,
    DWORD Protect,
    LPVOID *Address
);

BOOL
IlDeallocate(
    HANDLE ProcessHandle,
    LPVOID Address
);

BOOL
IlAllocateWideString(
    HANDLE ProcessHandle,
    LPCWSTR String,
    LPVOID *WideStringAddress
);

BOOL
IlAllocateUnicodeString(
    HANDLE ProcessHandle,
    LPCWSTR WideString,
    LPVOID *UnicodeStringAddress
);

BOOL
IlAllocateUnicodeString32(
    HANDLE ProcessHandle,
    LPCWSTR WideString,
    LPVOID *UnicodeStringAddress
);

BOOL
IlDeallocateUnicodeString(
    HANDLE ProcessHandle,
    LPVOID UnicodeStringAddress
);

BOOL
IlDeallocateUnicodeString32(
    HANDLE ProcessHandle,
    LPVOID UnicodeStringAddress
);

LPVOID
IlGetRemoteModuleProcAddress(
    HANDLE ProcessHandle,
    BOOL IsProcessInitialized,
    BOOL IsModule32,
    LPCWSTR ModuleName,
    LPCSTR ProcName
);

#endif
