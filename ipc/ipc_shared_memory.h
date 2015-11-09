#ifndef __IPC_SHARED_MEMORY_H
#define __IPC_SHARED_MEMORY_H

#include <Windows.h>
#include <tchar.h>
#include "ipc_utils.h"
#include "..\security\security_utils.h"

#define IPC_SHARED_MEMORY_NAME_LENGTH 0x100

typedef struct _SHARED_MEMORY
{
    WCHAR ObjectName[IPC_SHARED_MEMORY_NAME_LENGTH];
    HANDLE ObjectHandle;
    PVOID MemoryPointer;
    DWORD MemorySize;
} SHARED_MEMORY, *PSHARED_MEMORY;

BOOL
IpcCreateSharedMemoryW(
    LPCWSTR ObjectNamespace,
    LPCWSTR ObjectName,
    LPCWSTR ObjectPostfix,
    DWORD Size,
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
    LPVOID SourceBuffer,
    DWORD Size
);

BOOL
IpcReadFromSharedMemory(
    PSHARED_MEMORY SharedMemory,
    LPVOID DestinationBuffer,
    DWORD Size
);

#endif
