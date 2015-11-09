#ifndef __DLL_DECLS_H
#define __DLL_DECLS_H

#include <windows.h>

#define VOID void
#define EXPORT __declspec(dllexport)

LPVOID
__DllImport(
    LPCWSTR ModuleName,
    LPCSTR ProcName
);

#define DLL_IMPORT(cast, module, func) ((cast)__DllImport(module, func))

#endif
