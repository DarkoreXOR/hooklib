#include "hook_wrapper.h"

BOOL
HkInitialize()
{
    MH_STATUS status;

    status = MH_Initialize();

    return status == MH_OK;
}

BOOL
HkUninitialize()
{
    MH_STATUS status;

    status = MH_Uninitialize();

    return status == MH_OK;
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
