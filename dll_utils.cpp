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
                                   0,
                                   NULL,
                                   NULL);

    Result &= GetLastError() == ERROR_SUCCESS;

    CloseHandle(TokenHandle);
    return Result;
}

BOOL
EnableCreateGlobalPrivilege(BOOL Enable)
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

    Result = LookupPrivilegeValue(NULL, SE_CREATE_GLOBAL_NAME, &Luid);

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
                                   0,
                                   NULL,
                                   NULL);

    Result &= GetLastError() == ERROR_SUCCESS;

    CloseHandle(TokenHandle);
    return Result;
}

DWORD
PsProcessHandleToId(HANDLE ProcessHandle)
{
    BOOL Result;
    HANDLE Process;
    HANDLE TargetHandle;
    DWORD ProcessId;

    if (ProcessHandle == GetCurrentProcess())
    {
        return GetCurrentProcessId();
    }

    Process = GetCurrentProcess();

    Result = DuplicateHandle(Process,
                             ProcessHandle,
                             Process,
                             &TargetHandle,
                             PROCESS_QUERY_INFORMATION,
                             FALSE,
                             0);

    if (!Result)
    {
        return 0;
    }

    ProcessId = GetProcessId(TargetHandle);

    CloseHandle(TargetHandle);

    return ProcessId;
}
