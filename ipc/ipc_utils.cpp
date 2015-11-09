#include "ipc_utils.h"

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

DWORD
IpcWaitEventEx(LPCWSTR ChannelName,
               LPCWSTR Postfix,
               BOOL GlobalObject,
               DWORD Timeout)
{
    EVENT eventObject;
    LPCWSTR objectNamespace = NULL;
    DWORD waitStatus;
    DWORD timeoutEx;

    if (Timeout == 0)
    {
        return FALSE;
    }

    if (GlobalObject)
    {
        objectNamespace = L"Global";
    }

    timeoutEx = Timeout;

    while (TRUE)
    {
        if(!IpcOpenEventW(objectNamespace,
                          ChannelName,
                          Postfix,
                          &eventObject))
        {
            return WAIT_FAILED;
        }

        waitStatus = IpcWaitEvent(&eventObject, 1);

        timeoutEx--;

        IpcCloseEvent(&eventObject);

        if (waitStatus != WAIT_TIMEOUT)
        {
            break;
        }

        if (!timeoutEx && Timeout != INFINITE)
        {
            break;
        }
    }

    return waitStatus;
}
