/*
 *  [ BSD License: http://opensource.org/licenses/bsd-license.php ]
 *  ===========================================================================
 *  Copyright (c) 2015, Lakutin Ivan
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
