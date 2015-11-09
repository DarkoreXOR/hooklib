#include "system_utils.h"

BOOL
SysIsX64System()
{
#ifndef _M_X64
    HMODULE moduleHandle;
    GET_NATIVE_SYSTEM_INFO proc;
    SYSTEM_INFO systemInfo;

    moduleHandle = GetModuleHandleW(L"kernel32");

    if (moduleHandle == NULL)
    {
        return FALSE;
    }

    proc = (GET_NATIVE_SYSTEM_INFO)GetProcAddress(moduleHandle,
                                                  "GetNativeSystemInfo");

    if (proc == NULL)
    {
        return FALSE;
    }

    proc(&systemInfo);

    if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
    {
        return FALSE;
    }

#endif
    return TRUE;
}
