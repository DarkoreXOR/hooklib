#include "dll_decls.h"

LPVOID
__DllImport(LPCWSTR ModuleName,
            LPCSTR ProcName)
{
    HMODULE ModuleHandle;

    ModuleHandle = GetModuleHandleW(ModuleName);

    if (!ModuleHandle)
    {
        return NULL;
    }

    return GetProcAddress(ModuleHandle, ProcName);
}
