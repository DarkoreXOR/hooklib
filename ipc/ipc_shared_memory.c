#include "ipc_shared_memory.h"

BOOL
IpcCreateSharedMemoryW(LPCWSTR SharedMemoryName,
                       LPCWSTR Postfix,
                       DWORD Size,
                       PSHARED_MEMORY SharedMemory)
{
    WCHAR Name[0x100];
    HANDLE FileMappingHandle;
    PVOID Map;

    if (!SharedMemory)
    {
        return FALSE;
    }

    wcscpy_s(Name, 0x100, SharedMemoryName);
    wcscat_s(Name, 0x100, Postfix);

    SetLastError(ERROR_SUCCESS);

    FileMappingHandle = CreateFileMappingW(INVALID_HANDLE_VALUE,
                                           NULL,
                                           PAGE_READWRITE,
                                           0,
                                           Size,
                                           Name);

    if (!FileMappingHandle)
    {
        return FALSE;
    }

    if (FileMappingHandle && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(FileMappingHandle);
        return FALSE;
    }

    Map = MapViewOfFile(FileMappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (!Map)
    {
        CloseHandle(FileMappingHandle);
        return FALSE;
    }

    wcscpy_s(SharedMemory->Name, 0x100, Name);
    SharedMemory->MemoryPointer = Map;
    SharedMemory->MemorySize = Size;
    SharedMemory->FileMappingHandle = FileMappingHandle;

    return TRUE;
}

BOOL
IpcOpenSharedMemoryW(LPCWSTR SharedMemoryName,
                     LPCWSTR Postfix,
                     PSHARED_MEMORY SharedMemory)
{
    WCHAR Name[0x100];
    HANDLE FileMappingHandle;
    PVOID Map;

    if (!SharedMemory)
    {
        return FALSE;
    }

    wcscpy_s(Name, 0x100, SharedMemoryName);
    wcscat_s(Name, 0x100, Postfix);

    FileMappingHandle = OpenFileMappingW(FILE_MAP_ALL_ACCESS,
                                         FALSE,
                                         Name);

    if (!FileMappingHandle)
    {
        return FALSE;
    }

    Map = MapViewOfFile(FileMappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (!Map)
    {
        CloseHandle(FileMappingHandle);
        return FALSE;
    }

    wcscpy_s(SharedMemory->Name, 0x100, Name);
    SharedMemory->MemoryPointer = Map;
    SharedMemory->MemorySize = 0;
    SharedMemory->FileMappingHandle = FileMappingHandle;

    return TRUE;
}

BOOL
IpcCloseSharedMemory(PSHARED_MEMORY SharedMemory)
{
    if (!SharedMemory)
    {
        return FALSE;
    }

    BOOL result = CloseHandle(SharedMemory->FileMappingHandle);

    return UnmapViewOfFile(SharedMemory->MemoryPointer) && result;
}

BOOL
IpcDestroySharedMemory(PSHARED_MEMORY SharedMemory)
{
    return IpcCloseSharedMemory(SharedMemory);
}

VOID
IpcWriteToSharedMemory(PSHARED_MEMORY SharedMemory,
                       LPVOID SrcBuffer,
                       DWORD Size)
{
    memcpy(SharedMemory->MemoryPointer, SrcBuffer, Size);
}

VOID
IpcReadFromSharedMemory(PSHARED_MEMORY SharedMemory,
                        LPVOID DestBuffer,
                        DWORD Size)
{
    memcpy(DestBuffer, SharedMemory->MemoryPointer, Size);
}
