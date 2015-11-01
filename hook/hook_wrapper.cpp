#include "hook_wrapper.h"

BOOL
HkInitialize()
{
    MH_STATUS Status;

    Status = MH_Initialize();

    return Status == MH_OK;
}

BOOL
HkUninitialize()
{
    MH_STATUS Status;

    Status = MH_Uninitialize();

    return Status == MH_OK;
}

BOOL
HkHookCode(LPVOID Target,
           LPVOID Detour,
           LPVOID *Original,
           BOOL SetEnabled)
{
    MH_STATUS Status;

    Status = MH_CreateHook(Target, Detour, Original);

    if (Status != MH_OK)
    {
        return FALSE;
    }

    if (SetEnabled)
    {
        Status = MH_EnableHook(Target);
    }

    return Status == MH_OK;
}

BOOL
HkUnhookCode(LPVOID Target)
{
    MH_STATUS Status;

    Status = MH_RemoveHook(Target);

    return Status == MH_OK;
}

BOOL
HkHookApi(LPCWSTR ModuleName,
          LPCSTR ProcName,
          LPVOID Detour,
          LPVOID *Original,
          BOOL SetEnabled)
{
    MH_STATUS Status;
    HMODULE ModuleHandle;
    LPVOID ProcAddress;

    Status = MH_CreateHookApi(ModuleName,
                              ProcName,
                              Detour,
                              Original);

    if (Status != MH_OK)
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

        Status = MH_EnableHook(ProcAddress);
    }

    return Status == MH_OK;
}

BOOL
HkUnhookApi(LPCWSTR ModuleName,
            LPCSTR ProcName)
{
    MH_STATUS Status;

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

    Status = MH_RemoveHook(ProcAddress);

    return Status == MH_OK;
}

BOOL
HkEnableHook(LPVOID Target)
{
    MH_STATUS Status;

    if (Target == NULL)
    {
        return FALSE;
    }

    Status = MH_EnableHook(Target);

    return Status == MH_OK;
}

BOOL
HkDisableHook(LPVOID Target)
{
    MH_STATUS Status;

    if (Target == NULL)
    {
        return FALSE;
    }

    Status = MH_EnableHook(Target);

    return Status == MH_OK;
}

BOOL
HkEnableAllHooks()
{
    MH_STATUS Status;

    Status = MH_QueueEnableHook(MH_ALL_HOOKS);

    if (Status != MH_OK)
    {
        return FALSE;
    }

    Status = MH_ApplyQueued();

    return Status == MH_OK;
}

BOOL
HkDisableAllHooks()
{
    MH_STATUS Status;

    Status = MH_QueueDisableHook(MH_ALL_HOOKS);

    if (Status != MH_OK)
    {
        return FALSE;
    }

    Status = MH_ApplyQueued();

    return Status == MH_OK;
}
