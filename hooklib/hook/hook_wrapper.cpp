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

#include "hook_wrapper.h"

BOOL
HkInitialize()
{
    return MH_Initialize() == MH_OK;
}

BOOL
HkUninitialize()
{
    return MH_Uninitialize() == MH_OK;
}

BOOL
HkHookCode(LPVOID Target,
           LPVOID Detour,
           LPVOID *Original,
           BOOL SetEnabled)
{
    MH_STATUS status;

    status = MH_CreateHook(Target, Detour, Original);

    if (status != MH_OK)
    {
        return FALSE;
    }

    if (SetEnabled)
    {
        status = MH_EnableHook(Target);
    }

    return status == MH_OK;
}

BOOL
HkUnhookCode(LPVOID Target)
{
    MH_STATUS status;

    status = MH_RemoveHook(Target);

    return status == MH_OK;
}

BOOL
HkHookApi(LPCWSTR ModuleName,
          LPCSTR ProcName,
          LPVOID Detour,
          LPVOID *Original,
          BOOL SetEnabled)
{
    MH_STATUS status;
    HMODULE ModuleHandle;
    LPVOID ProcAddress;

    status = MH_CreateHookApi(ModuleName,
                              ProcName,
                              Detour,
                              Original);

    if (status != MH_OK)
    {
        return FALSE;
    }

    if (SetEnabled)
    {
        ModuleHandle = GetModuleHandleW(ModuleName);

        if (!ModuleHandle)
        {
            return FALSE;
        }

        ProcAddress = GetProcAddress(ModuleHandle, ProcName);

        if (!ProcAddress)
        {
            return FALSE;
        }

        status = MH_EnableHook(ProcAddress);
    }

    return status == MH_OK;
}

BOOL
HkUnhookApi(LPCWSTR ModuleName,
            LPCSTR ProcName)
{
    MH_STATUS status;

    HMODULE ModuleHandle;
    LPVOID ProcAddress;

    ModuleHandle = GetModuleHandleW(ModuleName);

    if (!ModuleHandle)
    {
        return FALSE;
    }

    ProcAddress = GetProcAddress(ModuleHandle, ProcName);

    if (ProcAddress == NULL)
    {
        return FALSE;
    }

    status = MH_RemoveHook(ProcAddress);

    return status == MH_OK;
}

BOOL
HkEnableHook(LPVOID Target)
{
    MH_STATUS status;

    if (Target == NULL)
    {
        return FALSE;
    }

    status = MH_EnableHook(Target);

    return status == MH_OK;
}

BOOL
HkDisableHook(LPVOID Target)
{
    MH_STATUS status;

    if (Target == NULL)
    {
        return FALSE;
    }

    status = MH_EnableHook(Target);

    return status == MH_OK;
}

BOOL
HkEnableAllHooks()
{
    MH_STATUS status;

    status = MH_QueueEnableHook(MH_ALL_HOOKS);

    if (status != MH_OK)
    {
        return FALSE;
    }

    status = MH_ApplyQueued();

    return status == MH_OK;
}

BOOL
HkDisableAllHooks()
{
    MH_STATUS status;

    status = MH_QueueDisableHook(MH_ALL_HOOKS);

    if (status != MH_OK)
    {
        return FALSE;
    }

    status = MH_ApplyQueued();

    return status == MH_OK;
}
