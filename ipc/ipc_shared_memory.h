#ifndef __IPC_SHARED_MEMORY_H
#define __IPC_SHARED_MEMORY_H

#include <Windows.h>
#include <tchar.h>
#include "ipc_defs.h"

#define IPC_SHARED_MEMORY_NAME_SIZE 0x200

typedef struct _SHARED_MEMORY
{
    WCHAR Name[IPC_SHARED_MEMORY_NAME_SIZE];
    HANDLE FileMappingHandle;
    PVOID MemoryPointer;
    SIZE_T MemorySize;
} SHARED_MEMORY, *PSHARED_MEMORY;

BOOL
IpcCreateSharedMemoryW(
    LPCWSTR ObjectNamespace,
    LPCWSTR ObjectName,
    LPCWSTR ObjectPostfix,
    SIZE_T Size,
    PSHARED_MEMORY SharedMemory
);

BOOL
IpcOpenSharedMemoryW(
    LPCWSTR ObjectNamespace,
    LPCWSTR ObjectName,
    LPCWSTR ObjectPostfix,
    PSHARED_MEMORY SharedMemory
);

BOOL
IpcCloseSharedMemory(
    PSHARED_MEMORY SharedMemory
);

BOOL
IpcDestroySharedMemory(
    PSHARED_MEMORY SharedMemory
);

BOOL
IpcWriteToSharedMemory(
    PSHARED_MEMORY SharedMemory,
    LPVOID SrcBuffer,
    SIZE_T Size
);

BOOL
IpcReadFromSharedMemory(
    PSHARED_MEMORY SharedMemory,
    LPVOID DestBuffer,
    SIZE_T Size
);

#endif
