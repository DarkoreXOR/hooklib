#include "process_utils.h"

BOOL
PsGetProcesses(PROCESS_INFO *Entries,
               SIZE_T NumOfEntries,
               PSIZE_T RealNumOfEntries)
{
    BOOL Result;
    PROCESSENTRY32 pe32;
    HANDLE hProcessSnap;
    SIZE_T ProcessOffset = 0;
    SIZE_T ProcessCounter = 0;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    Result = Process32Next(hProcessSnap, &pe32);

    if (!Result)
    {
        CloseHandle(hProcessSnap);
        return FALSE;
    }

    do
    {
        Result = FALSE;

        ProcessCounter++;

        if (ProcessOffset >= NumOfEntries)
        {
            Result = Process32Next(hProcessSnap, &pe32);
            continue;
        }

        Entries[ProcessOffset].ParentProcessId = pe32.th32ParentProcessID;
        Entries[ProcessOffset].ProcessId = pe32.th32ProcessID;

        ProcessOffset++;

        Result = Process32Next(hProcessSnap, &pe32);
    }
    while (Result);

    CloseHandle(hProcessSnap);

    if (RealNumOfEntries)
    {
        *RealNumOfEntries = ProcessCounter;
    }

    if (ProcessCounter > NumOfEntries)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL
PsGetModules(DWORD ProcessId,
             MODULE_INFO *Entries,
             SIZE_T NumOfEntries,
             PSIZE_T RealNumOfEntries)
{
    BOOL Result;
    MODULEENTRY32 me32;
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    SIZE_T ModuleOffset = 0;
    SIZE_T ModuleCounter = 0;

    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessId);

    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    me32.dwSize = sizeof(MODULEENTRY32);

    Result = Module32First(hModuleSnap, &me32);

    if (!Result)
    {
        CloseHandle(hModuleSnap);
        return FALSE;
    }

    do
    {
        Result = FALSE;

        ModuleCounter++;

        if (ModuleOffset >= NumOfEntries)
        {
            Result = Module32Next(hModuleSnap, &me32);
            continue;
        }

        Entries[ModuleOffset].ModuleHandle = (HMODULE)me32.modBaseAddr;
        wcscpy_s(Entries[ModuleOffset].ModulePath, MAX_PATH, me32.szExePath);
        wcscpy_s(Entries[ModuleOffset].ModuleName, MAX_PATH, me32.szModule);

        ModuleOffset++;

        Result = Module32Next(hModuleSnap, &me32);
    }
    while (Result);

    CloseHandle(hModuleSnap);

    if (RealNumOfEntries)
    {
        *RealNumOfEntries = ModuleCounter;
    }

    if (ModuleCounter > NumOfEntries)
    {
        return FALSE;
    }

    return TRUE;
}


BOOL
GetModuleInfoByProcessId(DWORD ProcessId,
                         LPCWSTR ModuleName,
                         PMODULE_INFO ModuleHandle)
{
    BOOL Result;
    MODULE_INFO *Modules;
    SIZE_T NumOfModules;
    WCHAR CompareModuleName[MAX_PATH];
    WCHAR RetModuleName[MAX_PATH];

    Result = wcscpy_s(CompareModuleName, MAX_PATH, ModuleName) == 0;

    if (!Result)
    {
        return FALSE;
    }

    Result = _wcsupr_s(CompareModuleName, MAX_PATH) == 0;

    if (!Result)
    {
        return FALSE;
    }

    Modules = (PMODULE_INFO)malloc(sizeof(MODULE_INFO) * 0x1000);

    if (!Modules)
    {
        return FALSE;
    }

    Result = PsGetModules(ProcessId,
                          Modules,
                          0x1000,
                          &NumOfModules);

    if (!Result)
    {
        free(Modules);
        return FALSE;
    }

    if (NumOfModules == 0)
    {
        free(Modules);
        return FALSE;
    }

    for (SIZE_T i = 0; i < NumOfModules; ++i)
    {
        Result = wcscpy_s(RetModuleName, MAX_PATH, Modules[i].ModuleName) == 0;

        if (!Result)
        {
            free(Modules);
            return FALSE;
        }

        Result = _wcsupr_s(RetModuleName, MAX_PATH) == 0;

        if (!Result)
        {
            free(Modules);
            return FALSE;
        }

        Result = wcscmp(RetModuleName, CompareModuleName) == 0;

        if (Result)
        {
            ModuleHandle->ModuleHandle = Modules[i].ModuleHandle;
            wcscpy_s(ModuleHandle->ModuleName, MAX_PATH, Modules[i].ModuleName);
            wcscpy_s(ModuleHandle->ModulePath, MAX_PATH, Modules[i].ModulePath);
            free(Modules);
            return TRUE;
        }
    }

    free(Modules);
    return FALSE;
}

