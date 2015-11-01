#ifndef __IL_UTILS_H
#define __IL_UTILS_H

#include <Windows.h>

#define IL_PTR_TO_UI32(p) ((UINT32)(p))
#define IL_PTR_TO_UI64(p) ((UINT64)(p))
#define IL_UINT_TO_PTR(ui) ((PVOID)(ui))

//typedef DWORD NTSTATUS;

#define STATUS_SUCCESS ((NTSTATUS)0x00000000)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001)

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

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

typedef
NTSTATUS
(NTAPI *LDR_LOAD_DLL)(
    PWCHAR PathToFile,
    ULONG Flags,
    PUNICODE_STRING ModuleFileName,
    HMODULE *ModuleHandle
);

BOOL
IlIsX64System();

BOOL
IlIsX64Process(
    DWORD ProcessId,
    PBOOL Result
);

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
IlDeallocateUnicodeString(
    HANDLE ProcessHandle,
    LPVOID UnicodeStringAddress
);

LPVOID
GetModuleProcAddress(
    LPCWSTR ModuleName,
    LPCSTR ProcName
);

#endif
