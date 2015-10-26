#ifndef __IL_UTILS_H
#define __IL_UTILS_H

#include <Windows.h>

#define IL_PTR_TO_UI32(p) ((UINT32)(p))
#define IL_PTR_TO_UI64(p) ((UINT64)(p))
#define IL_UINT_TO_PTR(ui) ((PVOID)(ui))

typedef
BOOL
(WINAPI *IS_WOW64_PROCESS)(
    HANDLE ProcessHandle,
    PBOOL Result
);

typedef
VOID
(WINAPI *GET_NATIVE_SYSTEM_INFO)(
    LPSYSTEM_INFO SysInfo
);

BOOL
IlGetLoadLibraryWProcAddress(PVOID *LoadLibraryWPtr);

BOOL
IlIsX64System();

BOOL
IlIsX64Process(
    DWORD ProcessId,
    PBOOL Result
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
IlAllocateCode(
    HANDLE ProcessHandle,
    PBYTE CodeBuf,
    SIZE_T CodeSize,
    LPVOID *Address
);

BOOL
IlAllocateWideString(
    HANDLE ProcessHandle,
    LPCWSTR String,
    LPVOID *Address
);

#endif
