#ifndef __HOOK_WRAPPER_H
#define __HOOK_WRAPPER_H

#include "MinHook.h"
#include <Windows.h>
#include "..\logger\logger_utils.h"

BOOL
HkInitialize();

BOOL
HkUninitialize();

BOOL
HkHookCode(
    LPVOID Target,
    LPVOID Detour,
    LPVOID *Original,
    BOOL SetEnabled
);

BOOL
HkUnhookCode(LPVOID Target);

BOOL
HkHookApi(
    LPCWSTR ModuleName,
    LPCSTR ProcName,
    LPVOID Detour,
    LPVOID *Original,
    BOOL SetEnabled
);

BOOL
HkUnhookApi(
    LPCWSTR ModuleName,
    LPCSTR ProcName
);

BOOL
HkEnableHook(LPVOID Target);

BOOL
HkDisableHook(LPVOID Target);

BOOL
HkEnableAllHooks();

BOOL
HkDisableAllHooks();

#endif
