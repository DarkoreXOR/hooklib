#include "ipc_shared_memory.h"

BOOL
IpcCreateSharedMemoryW(LPCWSTR ObjectNamespace,
                       LPCWSTR ObjectName,
                       LPCWSTR ObjectPostfix,
                       SIZE_T Size,
                       PSHARED_MEMORY SharedMemory)
{
    WCHAR Name[IPC_SHARED_MEMORY_NAME_SIZE];
    HANDLE FileMappingHandle;
    PVOID Map;
    SECURITY_DESCRIPTOR sd = { 0 };
    SECURITY_ATTRIBUTES sa = { 0 };

    if (!SharedMemory)
    {
        return FALSE;
    }

    if (ObjectNamespace)
    {
        wcscpy_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectNamespace);
        wcscat_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, L"\\");
        wcscat_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectName);
        wcscat_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectPostfix);
    }
    else
    {
        wcscpy_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectName);
        wcscat_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectPostfix);
    }

    ZeroMemory(&sd, sizeof(sd));

    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorOwner(&sd, NULL, FALSE);
    SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = &sd;
    sa.bInheritHandle = FALSE;

    FileMappingHandle = CreateFileMappingW(INVALID_HANDLE_VALUE,
                                           &sa,
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

    wcscpy_s(SharedMemory->Name, IPC_SHARED_MEMORY_NAME_SIZE, Name);
    SharedMemory->MemoryPointer = Map;
    SharedMemory->MemorySize = Size;
    SharedMemory->FileMappingHandle = FileMappingHandle;

    return TRUE;
}

BOOL
IpcOpenSharedMemoryW(LPCWSTR ObjectNamespace,
                     LPCWSTR ObjectName,
                     LPCWSTR ObjectPostfix,
                     PSHARED_MEMORY SharedMemory)
{
    WCHAR Name[IPC_SHARED_MEMORY_NAME_SIZE];
    HANDLE FileMappingHandle;
    PVOID Map;

    if (!SharedMemory)
    {
        return FALSE;
    }

    if (ObjectNamespace)
    {
        wcscpy_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectNamespace);
        wcscat_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, L"\\");
        wcscat_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectName);
        wcscat_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectPostfix);
    }
    else
    {
        wcscpy_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectName);
        wcscat_s(Name, IPC_SHARED_MEMORY_NAME_SIZE, ObjectPostfix);
    }

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

    wcscpy_s(SharedMemory->Name, IPC_SHARED_MEMORY_NAME_SIZE, Name);
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

BOOL
IpcWriteToSharedMemory(PSHARED_MEMORY SharedMemory,
                       LPVOID SrcBuffer,
                       SIZE_T Size)
{
    BOOL Result;
    HANDLE ProcessHandle;
    SIZE_T NumOfBytes;

    ProcessHandle = GetCurrentProcess();

    Result = WriteProcessMemory(ProcessHandle,
                                SharedMemory->MemoryPointer,
                                SrcBuffer,
                                Size,
                                &NumOfBytes);

    return Result && Size == NumOfBytes;
}

BOOL
IpcReadFromSharedMemory(PSHARED_MEMORY SharedMemory,
                        LPVOID DestBuffer,
                        SIZE_T Size)
{
    BOOL Result;
    HANDLE ProcessHandle;
    SIZE_T NumOfBytes;

    ProcessHandle = GetCurrentProcess();

    Result = ReadProcessMemory(ProcessHandle,
                               SharedMemory->MemoryPointer,
                               DestBuffer,
                               Size,
                               &NumOfBytes);

    return Result && Size == NumOfBytes;
}
