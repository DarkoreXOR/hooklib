#include "ipc_event.h"

BOOL
IpcCreateEventW(LPCWSTR ObjectNamespace,
                LPCWSTR ObjectName,
                LPCWSTR ObjectPostfix,
                BOOL ManualReset,
                BOOL InitialState,
                PEVENT Event)
{
    WCHAR Name[IPC_EVENT_NAME_SIZE];
    SECURITY_DESCRIPTOR sd = { 0 };
    SECURITY_ATTRIBUTES sa = { 0 };

    if (!Event)
    {
        return FALSE;
    }

    if (ObjectNamespace)
    {
        wcscpy_s(Name, IPC_EVENT_NAME_SIZE, ObjectNamespace);
        wcscat_s(Name, IPC_EVENT_NAME_SIZE, L"\\");
        wcscat_s(Name, IPC_EVENT_NAME_SIZE, ObjectName);
        wcscat_s(Name, IPC_EVENT_NAME_SIZE, ObjectPostfix);
    }
    else
    {
        wcscpy_s(Name, IPC_EVENT_NAME_SIZE, ObjectName);
        wcscat_s(Name, IPC_EVENT_NAME_SIZE, ObjectPostfix);
    }

    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorOwner(&sd, NULL, FALSE);
    SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = &sd;
    sa.bInheritHandle = FALSE;

    Event->Handle = CreateEventW(&sa, ManualReset, InitialState, Name);

    if (!Event->Handle)
    {
        return FALSE;
    }

    if (Event->Handle && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(Event->Handle);
        return FALSE;
    }

    wcscpy_s(Event->Name, IPC_EVENT_NAME_SIZE, Name);

    return TRUE;
}

BOOL
IpcOpenEventW(LPCWSTR ObjectNamespace,
              LPCWSTR ObjectName,
              LPCWSTR ObjectPostfix,
              PEVENT Event)
{
    WCHAR Name[IPC_EVENT_NAME_SIZE];

    if (!Event)
    {
        return FALSE;
    }

    if (ObjectNamespace)
    {
        wcscpy_s(Name, IPC_EVENT_NAME_SIZE, ObjectNamespace);
        wcscat_s(Name, IPC_EVENT_NAME_SIZE, L"\\");
        wcscat_s(Name, IPC_EVENT_NAME_SIZE, ObjectName);
        wcscat_s(Name, IPC_EVENT_NAME_SIZE, ObjectPostfix);
    }
    else
    {
        wcscpy_s(Name, IPC_EVENT_NAME_SIZE, ObjectName);
        wcscat_s(Name, IPC_EVENT_NAME_SIZE, ObjectPostfix);
    }

    Event->Handle = OpenEventW(EVENT_ALL_ACCESS, FALSE, Name);

    if (!Event->Handle)
    {
        return FALSE;
    }

    wcscpy_s(Event->Name, IPC_EVENT_NAME_SIZE, Name);

    return TRUE;
}

BOOL
IpcCloseEvent(PEVENT Event)
{
    if (!Event)
    {
        return FALSE;
    }

    return CloseHandle(Event->Handle);
}

BOOL
IpcDestroyEvent(PEVENT Event)
{
    return IpcCloseEvent(Event);
}

BOOL
IpcSetEvent(PEVENT Event)
{
    if (!Event)
    {
        return FALSE;
    }

    return SetEvent(Event->Handle);
}

BOOL
IpcResetEvent(PEVENT Event)
{
    if (!Event)
    {
        return FALSE;
    }

    return ResetEvent(Event->Handle);
}

DWORD
IpcWaitEvent(PEVENT Event,
             DWORD Timeout)
{
    if (!Event)
    {
        return WAIT_FAILED;
    }

    return WaitForSingleObject(Event->Handle, Timeout);
}
