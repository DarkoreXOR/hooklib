#ifndef __IPC_SHARED_MEMORY_H
#define __IPC_SHARED_MEMORY_H

#include <Windows.h>
#include <tchar.h>
#include "ipc_defs.h"

typedef struct _SHARED_MEMORY
{
    WCHAR Name[0x100];
    HANDLE FileMappingHandle;
    PVOID MemoryPointer;
    DWORD MemorySize;
} SHARED_MEMORY, *PSHARED_MEMORY;

BOOL
IpcCreateSharedMemoryW(
    LPCWSTR SharedMemoryName,
    LPCWSTR Postfix,
    DWORD Size,
    PSHARED_MEMORY SharedMemory
);

BOOL
IpcOpenSharedMemoryW(
    LPCWSTR SharedMemoryName,
    LPCWSTR Postfix,
    PSHARED_MEMORY SharedMemory
);

BOOL
IpcCloseSharedMemory(PSHARED_MEMORY SharedMemory);

BOOL
IpcDestroySharedMemory(PSHARED_MEMORY SharedMemory);

VOID
IpcWriteToSharedMemory(
    PSHARED_MEMORY SharedMemory,
    LPVOID SrcBuffer,
    DWORD Size
);

VOID
IpcReadFromSharedMemory(
    PSHARED_MEMORY SharedMemory,
    LPVOID DestBuffer,
    DWORD Size
);

#endif

