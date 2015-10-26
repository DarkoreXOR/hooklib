#include "ipc_event.h"

BOOL
IpcCreateEventW(LPCWSTR EventName,
                LPCWSTR Postfix,
                BOOL ManualReset,
                BOOL InitialState,
                PEVENT Event)
{
    WCHAR Name[0x100];

    if (!Event)
    {
        return FALSE;
    }

    wcscpy_s(Name, 0x100, EventName);
    wcscat_s(Name, 0x100, Postfix);

    Event->Handle = CreateEventW(NULL, ManualReset, InitialState, Name);

    if (!Event->Handle)
    {
        return FALSE;
    }

    if (Event->Handle && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(Event->Handle);
        return FALSE;
    }

    wcscpy_s(Event->Name, 0x100, Name);

    return TRUE;
}

BOOL
IpcOpenEventW(LPCWSTR EventName,
              LPCWSTR Postfix,
              PEVENT Event)
{
    WCHAR Name[0x100];

    if (!Event)
    {
        return FALSE;
    }

    wcscpy_s(Name, 0x100, EventName);
    wcscat_s(Name, 0x100, Postfix);

    Event->Handle = OpenEventW(EVENT_ALL_ACCESS, FALSE, Name);

    if (!Event->Handle)
    {
        return FALSE;
    }

    wcscpy_s(Event->Name, 0x100, Name);

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
        return FALSE;
    }

    return WaitForSingleObject(Event->Handle, Timeout);
}
