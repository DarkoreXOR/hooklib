#include "dll_utils.h"

BOOL
EnableDebugPrivilege(BOOL Enable)
{
    BOOL Result;
    HANDLE TokenHandle;
    LUID Luid;
    TOKEN_PRIVILEGES TokenPriv;

    Result = OpenProcessToken(GetCurrentProcess(),
                              TOKEN_ADJUST_PRIVILEGES,
                              &TokenHandle);

    if (!Result)
    {
        return FALSE;
    }

    Result = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid);

    if (!Result)
    {
        CloseHandle(TokenHandle);
        return FALSE;
    }

    TokenPriv.PrivilegeCount = 1;
    TokenPriv.Privileges[0].Luid = Luid;
    TokenPriv.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;

    Result = AdjustTokenPrivileges(TokenHandle,
                                   FALSE,
                                   &TokenPriv,
                                   sizeof(TOKEN_PRIVILEGES),
                                   NULL,
                                   NULL);

    CloseHandle(TokenHandle);
    return Result;
}
