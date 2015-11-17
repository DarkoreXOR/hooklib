#include "ipc_event.h"

VOID
IpcMakeObjectName(LPWSTR DestinationObjectName,
                  SIZE_T DestinationObjectNameLength,
                  LPCWSTR ObjectNamespace,
                  LPCWSTR ObjectName,
                  LPCWSTR ObjectPostfix)
{
    if (ObjectNamespace)
    {
        wcscpy_s(DestinationObjectName,
                 DestinationObjectNameLength,
                 ObjectNamespace);

        wcscat_s(DestinationObjectName,
                 DestinationObjectNameLength,
                 L"\\");

        wcscat_s(DestinationObjectName,
                 DestinationObjectNameLength,
                 ObjectName);

        wcscat_s(DestinationObjectName,
                 DestinationObjectNameLength,
                 ObjectPostfix);
    }
    else
    {
        wcscpy_s(DestinationObjectName,
                 DestinationObjectNameLength,
                 ObjectName);

        wcscat_s(DestinationObjectName,
                 DestinationObjectNameLength,
                 ObjectPostfix);
    }
}

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
        Event->ObjectHandle = 0;
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
        Event->ObjectHandle = 0;
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
    BOOL Result;

    if (!Event)
    {
        return FALSE;
    }

    if (!Event->ObjectHandle)
    {
        return FALSE;
    }

    Result = CloseHandle(Event->ObjectHandle);
    Event->ObjectHandle = 0;

    return Result;
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
