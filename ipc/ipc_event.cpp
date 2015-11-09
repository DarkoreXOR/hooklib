#include "ipc_event.h"

BOOL
IpcCreateEventW(LPCWSTR ObjectNamespace,
                LPCWSTR ObjectName,
                LPCWSTR ObjectPostfix,
                BOOL ManualReset,
                BOOL InitialState,
                PEVENT Event)
{
    PSID sid;
    SECURITY_DESCRIPTOR sd = { 0 };
    SECURITY_ATTRIBUTES sa = { 0 };

    IpcMakeObjectName(Event->ObjectName,
                      IPC_EVENT_NAME_LENGTH,
                      ObjectNamespace,
                      ObjectName,
                      ObjectPostfix);

    if(!ScCreateSecurityAttributes(&sa, &sd, &sid))
    {
        return FALSE;
    }

    Event->ObjectHandle = CreateEventW(&sa,
                                       ManualReset,
                                       InitialState,
                                       Event->ObjectName);

    ScDestroySecurityAttributes(sid);

    if (!Event->ObjectHandle)
    {
        return FALSE;
    }

    if (Event->ObjectHandle && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(Event->ObjectHandle);
        return FALSE;
    }

    return TRUE;
}

BOOL
IpcOpenEventW(LPCWSTR ObjectNamespace,
              LPCWSTR ObjectName,
              LPCWSTR ObjectPostfix,
              PEVENT Event)
{
    IpcMakeObjectName(Event->ObjectName,
                      IPC_EVENT_NAME_LENGTH,
                      ObjectNamespace,
                      ObjectName,
                      ObjectPostfix);

    Event->ObjectHandle = OpenEventW(EVENT_ALL_ACCESS,
                                     FALSE,
                                     Event->ObjectName);

    if (!Event->ObjectHandle)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL
IpcCloseEvent(PEVENT Event)
{
    if (!Event)
    {
        return FALSE;
    }

    return CloseHandle(Event->ObjectHandle);
}

BOOL
IpcDestroyEvent(PEVENT Event)
{
    return IpcCloseEvent(Event);
}

BOOL
IpcSetEvent(PEVENT Event)
{
    return SetEvent(Event->ObjectHandle);
}

BOOL
IpcResetEvent(PEVENT Event)
{
    return ResetEvent(Event->ObjectHandle);
}

DWORD
IpcWaitEvent(PEVENT Event,
             DWORD Timeout)
{
    return WaitForSingleObject(Event->ObjectHandle,
                               Timeout);
}
