#include "dll_utils.h"

BOOL
EnableDebugPrivilege(BOOL Enable)
{
    BOOL result;
    HANDLE tokenHandle;
    LUID luid;
    TOKEN_PRIVILEGES tokenPrivileges;

    result = OpenProcessToken(GetCurrentProcess(),
                              TOKEN_ADJUST_PRIVILEGES,
                              &tokenHandle);

    if (!result)
    {
        return FALSE;
    }

    result = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

    if (!result)
    {
        CloseHandle(tokenHandle);
        return FALSE;
    }

    tokenPrivileges.PrivilegeCount = 1;
    tokenPrivileges.Privileges[0].Luid = luid;
    tokenPrivileges.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;

    result = AdjustTokenPrivileges(tokenHandle,
                                   FALSE,
                                   &tokenPrivileges,
                                   0,
                                   NULL,
                                   NULL);

    result &= GetLastError() == ERROR_SUCCESS;

    CloseHandle(tokenHandle);
    return result;
}

BOOL
EnableCreateGlobalPrivilege(BOOL Enable)
{
    BOOL result;
    HANDLE tokenHandle;
    LUID luid;
    TOKEN_PRIVILEGES tokenPrivileges;

    result = OpenProcessToken(GetCurrentProcess(),
                              TOKEN_ADJUST_PRIVILEGES,
                              &tokenHandle);

    if (!result)
    {
        return FALSE;
    }

    result = LookupPrivilegeValue(NULL, SE_CREATE_GLOBAL_NAME, &luid);

    if (!result)
    {
        CloseHandle(tokenHandle);
        return FALSE;
    }

    tokenPrivileges.PrivilegeCount = 1;
    tokenPrivileges.Privileges[0].Luid = luid;
    tokenPrivileges.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;

    result = AdjustTokenPrivileges(tokenHandle,
                                   FALSE,
                                   &tokenPrivileges,
                                   0,
                                   NULL,
                                   NULL);

    result &= GetLastError() == ERROR_SUCCESS;

    CloseHandle(tokenHandle);
    return result;
}
