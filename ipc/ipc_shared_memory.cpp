#include "ipc_shared_memory.h"

BOOL
IpcCreateSharedMemoryW(LPCWSTR ObjectNamespace,
                       LPCWSTR ObjectName,
                       LPCWSTR ObjectPostfix,
                       DWORD Size,
                       PSHARED_MEMORY SharedMemory)
{
    PSID sid;
    SECURITY_DESCRIPTOR sd = { 0 };
    SECURITY_ATTRIBUTES sa = { 0 };

    IpcMakeObjectName(SharedMemory->ObjectName,
                      IPC_SHARED_MEMORY_NAME_LENGTH,
                      ObjectNamespace,
                      ObjectName,
                      ObjectPostfix);

    if (!ScCreateSecurityAttributes(&sa, &sd, &sid))
    {
        return FALSE;
    }

    SharedMemory->ObjectHandle = CreateFileMappingW(INVALID_HANDLE_VALUE,
                                                    &sa,
                                                    PAGE_READWRITE,
                                                    0,
                                                    (DWORD)Size,
                                                    SharedMemory->ObjectName);

    ScDestroySecurityAttributes(sid);

    if (!(SharedMemory->ObjectHandle))
    {
        return FALSE;
    }

    if (SharedMemory->ObjectHandle && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(SharedMemory->ObjectHandle);
        return FALSE;
    }

    SharedMemory->MemoryPointer = MapViewOfFile(SharedMemory->ObjectHandle,
                                                FILE_MAP_ALL_ACCESS,
                                                0, 0, 0);

    if (!(SharedMemory->MemoryPointer))
    {
        CloseHandle(SharedMemory->ObjectHandle);
        return FALSE;
    }

    SharedMemory->MemorySize = Size;

    return TRUE;
}

BOOL
IpcOpenSharedMemoryW(LPCWSTR ObjectNamespace,
                     LPCWSTR ObjectName,
                     LPCWSTR ObjectPostfix,
                     PSHARED_MEMORY SharedMemory)
{
    IpcMakeObjectName(SharedMemory->ObjectName,
                      IPC_SHARED_MEMORY_NAME_LENGTH,
                      ObjectNamespace,
                      ObjectName,
                      ObjectPostfix);

    SharedMemory->ObjectHandle = OpenFileMappingW(FILE_MAP_ALL_ACCESS,
                                                  FALSE,
                                                  SharedMemory->ObjectName);

    if (!(SharedMemory->ObjectHandle))
    {
        return FALSE;
    }

    SharedMemory->MemoryPointer = MapViewOfFile(SharedMemory->ObjectHandle,
                                                FILE_MAP_ALL_ACCESS,
                                                0, 0, 0);

    if (!(SharedMemory->MemoryPointer))
    {
        CloseHandle(SharedMemory->ObjectHandle);
        return FALSE;
    }

    wcscpy_s(SharedMemory->ObjectName,
             IPC_SHARED_MEMORY_NAME_LENGTH,
             SharedMemory->ObjectName);

    SharedMemory->MemorySize = 0;

    return TRUE;
}

BOOL
IpcCloseSharedMemory(PSHARED_MEMORY SharedMemory)
{
    if (!SharedMemory)
    {
        return FALSE;
    }

    CloseHandle(SharedMemory->ObjectHandle);

    return UnmapViewOfFile(SharedMemory->MemoryPointer);
}

BOOL
IpcDestroySharedMemory(PSHARED_MEMORY SharedMemory)
{
    return IpcCloseSharedMemory(SharedMemory);
}

BOOL
IpcWriteToSharedMemory(PSHARED_MEMORY SharedMemory,
                       LPVOID SourceBuffer,
                       DWORD Size)
{
    BOOL result;
    SIZE_T numOfBytes;

    result = WriteProcessMemory(GetCurrentProcess(),
                                SharedMemory->MemoryPointer,
                                SourceBuffer,
                                Size,
                                &numOfBytes);

    return result && Size == numOfBytes;
}

BOOL
IpcReadFromSharedMemory(PSHARED_MEMORY SharedMemory,
                        LPVOID DestinationBuffer,
                        DWORD Size)
{
    BOOL result;
    SIZE_T numOfBytes;

    result = ReadProcessMemory(GetCurrentProcess(),
                               SharedMemory->MemoryPointer,
                               DestinationBuffer,
                               Size,
                               &numOfBytes);

    return result && Size == numOfBytes;
}
