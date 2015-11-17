#include "process_utils.h"

BOOL
PsIsX64Process(HANDLE ProcessHandle,
               PBOOL Result)
{
    HMODULE moduleHandle;
    BOOL isTarget64Bit = FALSE;
    IS_WOW64_PROCESS isWow64ProcessProc;

    if (!Result)
    {
        return FALSE;
    }

#ifndef _M_X64
    GET_NATIVE_SYSTEM_INFO GetNativeSystemInfoProc;
    SYSTEM_INFO SysInfo;
#endif

    moduleHandle = GetModuleHandleW(L"kernel32");

    if (moduleHandle == NULL)
    {
        return FALSE;
    }

    if (ProcessHandle == NULL)
    {
        return FALSE;
    }

    isWow64ProcessProc = (IS_WOW64_PROCESS)GetProcAddress(moduleHandle, "IsWow64Process");

#ifdef _M_X64
    if (!isWow64ProcessProc(ProcessHandle, &isTarget64Bit))
    {
        return FALSE;
    }

    isTarget64Bit = !isTarget64Bit;
#else
    isTarget64Bit = FALSE;

    if (isWow64ProcessProc == NULL)
    {
        return FALSE;
    }

    GetNativeSystemInfoProc = (GET_NATIVE_SYSTEM_INFO)GetProcAddress(moduleHandle,
                                                                     "GetNativeSystemInfo");

    if (GetNativeSystemInfoProc == NULL)
    {
        return FALSE;
    }

    GetNativeSystemInfoProc(&SysInfo);

    if (SysInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL)
    {
        if (!isWow64ProcessProc(ProcessHandle, &isTarget64Bit))
        {
            return FALSE;
        }
        isTarget64Bit = !isTarget64Bit;
    }
#endif

    *Result = isTarget64Bit;

    return TRUE;
}

BOOL
PsGetProcesses(PROCESS_INFO *Entries,
               SIZE_T NumOfEntries,
               PSIZE_T RealNumOfEntries)
{
    BOOL result;
    PROCESSENTRY32 pe32;
    HANDLE processSnapHandle;
    SIZE_T processOffset = 0;
    SIZE_T processCounter = 0;

    processSnapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (processSnapHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    result = Process32Next(processSnapHandle, &pe32);

    if (!result)
    {
        CloseHandle(processSnapHandle);
        return FALSE;
    }

    do
    {
        result = FALSE;

        processCounter++;

        if (processOffset >= NumOfEntries)
        {
            result = Process32Next(processSnapHandle, &pe32);
            continue;
        }

        Entries[processOffset].ParentProcessId = pe32.th32ParentProcessID;
        Entries[processOffset].ProcessId = pe32.th32ProcessID;

        processOffset++;

        result = Process32Next(processSnapHandle, &pe32);
    }
    while (result);

    CloseHandle(processSnapHandle);

    if (RealNumOfEntries)
    {
        *RealNumOfEntries = processCounter;
    }

    if (processCounter > NumOfEntries)
    {
        return FALSE;
    }

    return TRUE;
}


BOOL
PeReadDosHeader(HANDLE ProcessHandle,
                LPVOID ModuleBase,
                PIMAGE_DOS_HEADER DosHeader)
{
    BOOL result;

    if (!DosHeader)
    {
        return FALSE;
    }

    result = ReadProcessMemory(ProcessHandle, ModuleBase, DosHeader, sizeof(IMAGE_DOS_HEADER), NULL);

    if (!result)
    {
        return FALSE;
    }

    if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return FALSE;
    }

    if (DosHeader->e_lfanew == 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL
PeReadNtHeader32(HANDLE ProcessHandle,
                 LPVOID ModuleBase,
                 PIMAGE_NT_HEADERS32 NtHeader)
{
    BOOL result;
    IMAGE_DOS_HEADER dosHeader = { 0 };
    LPVOID ntHeaderBase;

    result = PeReadDosHeader(ProcessHandle,
                             ModuleBase,
                             &dosHeader);

    if (!result)
    {
        return FALSE;
    }

    ntHeaderBase = (LPVOID)((uintptr_t)ModuleBase + dosHeader.e_lfanew);

    result = ReadProcessMemory(ProcessHandle,
                               ntHeaderBase,
                               NtHeader,
                               sizeof(IMAGE_NT_HEADERS32),
                               NULL);

    if (!result)
    {
        return FALSE;
    }

    if (NtHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        return FALSE;
    }

    if (NtHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
    {
        return FALSE;
    }

    if (NtHeader->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER32))
    {
        return FALSE;
    }

    if (NtHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        return FALSE;
    }

    if (NtHeader->OptionalHeader.ImageBase != (DWORD)ModuleBase)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL
PeReadNtHeader64(HANDLE ProcessHandle,
                 LPVOID ModuleBase,
                 PIMAGE_NT_HEADERS64 NtHeader)
{
    BOOL result;
    IMAGE_DOS_HEADER dosHeader = { 0 };
    LPVOID ntHeaderBase;

    result = PeReadDosHeader(ProcessHandle, ModuleBase, &dosHeader);

    if (!result)
    {
        return FALSE;
    }

    ntHeaderBase = (LPVOID)((uintptr_t)ModuleBase + dosHeader.e_lfanew);

    result = ReadProcessMemory(ProcessHandle,
                               ntHeaderBase,
                               NtHeader,
                               sizeof(IMAGE_NT_HEADERS64),
                               NULL);

    if (!result)
    {
        return FALSE;
    }

    if (NtHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        return FALSE;
    }

    if (NtHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 &&
        NtHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_IA64)
    {
        return FALSE;
    }

    if (NtHeader->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER64))
    {
        return FALSE;
    }

    if (NtHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
        return FALSE;
    }

    if (NtHeader->OptionalHeader.ImageBase != (ULONGLONG)ModuleBase)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL
PeReadExportTable(HANDLE ProcessHandle,
                  BOOL AsX86,
                  LPVOID ModuleBase,
                  PDWORD NumOfFunctions,
                  PDWORD NumOfNames,
                  PDWORD NumOfOrdinals,
                  DWORD **AddressTable,
                  DWORD **NameTable,
                  WORD **OrdinalTable)
{
    BOOL result;
    IMAGE_NT_HEADERS32 ntHeader32;
    IMAGE_NT_HEADERS64 ntHeader64;
    IMAGE_EXPORT_DIRECTORY exportDirectory;
    DWORD exportDirectoryRVA;
    LPVOID exportDirectoryBase;
    LPVOID addressBase;
    LPVOID nameBase;
    LPVOID ordinalBase;

    if (!NumOfFunctions || !NumOfNames || !NumOfOrdinals)
    {
        return FALSE;
    }

    if (!AddressTable || !NameTable || !OrdinalTable)
    {
        return FALSE;
    }

    if (AsX86)
    {
        result = PeReadNtHeader32(ProcessHandle,
                                  ModuleBase,
                                  &ntHeader32);
    }
    else
    {
        result = PeReadNtHeader64(ProcessHandle,
                                  ModuleBase,
                                  &ntHeader64);
    }

    if (!result)
    {
        return FALSE;
    }

    if (AsX86)
    {
        exportDirectoryRVA = ntHeader32.OptionalHeader.DataDirectory[0].VirtualAddress;
    }
    else
    {
        exportDirectoryRVA = ntHeader64.OptionalHeader.DataDirectory[0].VirtualAddress;
    }

    exportDirectoryBase = (LPVOID)((uintptr_t)ModuleBase + exportDirectoryRVA);

    result = ReadProcessMemory(ProcessHandle,
                               exportDirectoryBase,
                               &exportDirectory,
                               sizeof(exportDirectory),
                               NULL);

    if (!result)
    {
        return FALSE;
    }

    *NumOfFunctions = exportDirectory.NumberOfFunctions;
    *NumOfNames = exportDirectory.NumberOfNames;
    *NumOfOrdinals = exportDirectory.NumberOfFunctions;

    *AddressTable = (DWORD*)AllocMem(sizeof(DWORD) * exportDirectory.NumberOfFunctions);

    if (!(*AddressTable))
    {
        return FALSE;
    }

    *NameTable = (DWORD*)AllocMem(sizeof(DWORD) * exportDirectory.NumberOfNames);

    if (!(*NameTable))
    {
        DeallocMem(*AddressTable);
        return FALSE;
    }

    *OrdinalTable = (WORD*)AllocMem(sizeof(WORD) * exportDirectory.NumberOfFunctions);

    if (!(*OrdinalTable))
    {
        DeallocMem(*AddressTable);
        DeallocMem(*NameTable);
        return FALSE;
    }

    addressBase = (LPVOID)((uintptr_t)ModuleBase + exportDirectory.AddressOfFunctions);
    nameBase = (LPVOID)((uintptr_t)ModuleBase + exportDirectory.AddressOfNames);
    ordinalBase = (LPVOID)((uintptr_t)ModuleBase + exportDirectory.AddressOfNameOrdinals);

    result = ReadProcessMemory(ProcessHandle,
                               addressBase,
                               *AddressTable,
                               sizeof(DWORD) * exportDirectory.NumberOfFunctions,
                               NULL);

    if (!result)
    {
        DeallocMem(*AddressTable);
        DeallocMem(*NameTable);
        DeallocMem(*OrdinalTable);
        return FALSE;
    }

    result = ReadProcessMemory(ProcessHandle,
                               nameBase,
                               *NameTable,
                               sizeof(DWORD) * exportDirectory.NumberOfNames,
                               NULL);

    if (!result)
    {
        DeallocMem(*AddressTable);
        DeallocMem(*NameTable);
        DeallocMem(*OrdinalTable);
        return FALSE;
    }

    result = ReadProcessMemory(ProcessHandle,
                               ordinalBase,
                               *OrdinalTable,
                               sizeof(WORD) * exportDirectory.NumberOfFunctions,
                               NULL);

    if (!result)
    {
        DeallocMem(*AddressTable);
        DeallocMem(*NameTable);
        DeallocMem(*OrdinalTable);
        return FALSE;
    }

    return TRUE;
}

VOID
PeFreeExportTable(DWORD *AddressTable,
                  DWORD *NameTable,
                  WORD *OrdinalTable)
{
    DeallocMem(AddressTable);
    DeallocMem(NameTable);
    DeallocMem(OrdinalTable);
}

BOOL
PsGetModulesByProcessHandle(HANDLE ProcessHandle,
                            MODULE_INFO *Entries,
                            SIZE_T NumOfEntries,
                            PSIZE_T RealNumOfEntries)
{
    BOOL result;
    HMODULE *modules;
    DWORD needSize;
    SIZE_T moduleOffset = 0;
    SIZE_T moduleCounter = 0;

    if (!RealNumOfEntries)
    {
        return FALSE;
    }

    modules = (HMODULE*)AllocMem(sizeof(HMODULE) * NumOfEntries);

    if (!modules)
    {
        return FALSE;
    }

    result = EnumProcessModulesEx(ProcessHandle,
                                  modules,
                                  (DWORD)(sizeof(HMODULE) * NumOfEntries),
                                  &needSize,
                                  LIST_MODULES_ALL);

    if (!result)
    {
        DeallocMem(modules);
        return FALSE;
    }

    *RealNumOfEntries = needSize / sizeof(HMODULE);

    do
    {
        result = FALSE;

        moduleCounter++;

        if (moduleOffset >= NumOfEntries)
        {
            continue;
        }

        Entries[moduleOffset].ModuleHandle = modules[moduleOffset];

        GetModuleFileNameEx(ProcessHandle,
                            modules[moduleOffset],
                            Entries[moduleOffset].ModulePath,
                            MAX_PATH);

        GetShortFileName(Entries[moduleOffset].ModulePath,
                         Entries[moduleOffset].ModuleName,
                         MAX_PATH);

        result = moduleCounter < (*RealNumOfEntries);

        moduleOffset++;
    }
    while (result);

    DeallocMem(modules);

    *RealNumOfEntries = moduleCounter;

    if (moduleCounter > NumOfEntries)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL
PsGetImageFileNameEx(HANDLE ProcessHandle,
                     LPVOID ModuleBase,
                     LPWSTR FileName,
                     SIZE_T Length,
                     PSIZE_T ReturnLength)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    PNT_UNICODE_STRING unicodeString;
    SIZE_T bufferSize;
    LPVOID buffer;
    SIZE_T retLength;

    bufferSize = 0x100;

    do
    {
        buffer = AllocMem(bufferSize);

        if (!buffer)
        {
            break;
        }

        status = NtQueryVirtualMemory(ProcessHandle,
                                      ModuleBase,
                                      NT_MEMORY_INFORMATION_CLASS::MemoryMappedFilenameInformation,
                                      buffer,
                                      bufferSize,
                                      &retLength);

        if (status == STATUS_BUFFER_OVERFLOW)
        {
            DeallocMem(buffer);
            bufferSize = retLength;
        }
        else
        {
            break;
        }

    }
    while (TRUE);

    if (buffer)
    {
        unicodeString = (PNT_UNICODE_STRING)buffer;
        wcscpy_s(FileName, Length, unicodeString->Buffer);
        *ReturnLength = unicodeString->MaximumLength;
        DeallocMem(buffer);
        return STATUS_UNSUCCESSFUL;
    }

    return status;
}


BOOL
PsGetProcessMappedModules(HANDLE ProcessHandle,
                          MODULE_INFO *Entries,
                          SIZE_T NumOfEntries,
                          PSIZE_T RealNumOfEntries)
{
    SIZE_T moduleOffset = 0;
    SIZE_T moduleCounter = 0;

    MEMORY_BASIC_INFORMATION memBasicInfo;
    LPVOID baseAddress = NULL;
    SIZE_T queryResult;
    SIZE_T returnLength;

    do
    {
        queryResult = VirtualQueryEx(ProcessHandle, baseAddress, &memBasicInfo, sizeof(memBasicInfo));

        if (!queryResult)
        {
            break;
        }

        if (memBasicInfo.Type == MEM_IMAGE && baseAddress == memBasicInfo.AllocationBase)
        {
            moduleCounter++;

            if (moduleOffset >= NumOfEntries)
            {
                baseAddress = (LPVOID)((uintptr_t)baseAddress + memBasicInfo.RegionSize);
                continue;
            }

            DWORD Result = PsGetImageFileNameEx(ProcessHandle,
                                                baseAddress,
                                                Entries[moduleOffset].ModulePath,
                                                MAX_PATH,
                                                &returnLength);

            Entries[moduleOffset].ModuleHandle = (HMODULE)baseAddress;

            GetShortFileName(Entries[moduleOffset].ModulePath,
                             Entries[moduleOffset].ModuleName,
                             MAX_PATH);

            moduleOffset++;
        }

        baseAddress = (LPVOID)((uintptr_t)baseAddress + memBasicInfo.RegionSize);
    }
    while (TRUE);

    if (RealNumOfEntries)
    {
        *RealNumOfEntries = moduleCounter;
    }

    if (moduleCounter > NumOfEntries)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL
PsGetProcessModules(HANDLE ProcessHandle,
                    BOOL IsProcessInitialized,
                    MODULE_INFO *Modules,
                    SIZE_T NumOfModules,
                    PSIZE_T RealNumOfModules)
{
    BOOL result;
    HANDLE internalProcessHandle;

    if (IsProcessInitialized)
    {
        result = PsCopyHandle(GetCurrentProcess(),
                              ProcessHandle,
                              &internalProcessHandle,
                              PROCESS_ALL_ACCESS,
                              FALSE);

        if (!result)
        {
            return FALSE;
        }

        result = PsGetModulesByProcessHandle(ProcessHandle,
                                             Modules,
                                             NumOfModules,
                                             RealNumOfModules);

        CloseHandle(internalProcessHandle);
    }
    else
    {
        result = PsCopyHandle(GetCurrentProcess(),
                              ProcessHandle,
                              &internalProcessHandle,
                              PROCESS_ALL_ACCESS,
                              FALSE);

        if (!result)
        {
            return FALSE;
        }

        result = PsGetProcessMappedModules(internalProcessHandle,
                                           Modules,
                                           NumOfModules,
                                           RealNumOfModules);

        CloseHandle(internalProcessHandle);
    }

    return result;
}

BOOL
PsGetModuleInfo(HANDLE ProcessHandle,
                BOOL IsProcessInitialized,
                BOOL IsModule32,
                LPCWSTR ModuleName,
                PMODULE_INFO ModuleInfo)
{
    BOOL result;
    MODULE_INFO *modules;
    SIZE_T numOfModules;
    WCHAR compareModuleName[MAX_PATH];
    WCHAR retModuleName[MAX_PATH];

    result = wcscpy_s(compareModuleName, MAX_PATH, ModuleName) == 0;

    if (!result)
    {
        return FALSE;
    }

    result = _wcsupr_s(compareModuleName, MAX_PATH) == 0;

    if (!result)
    {
        return FALSE;
    }

    modules = (PMODULE_INFO)AllocMem(sizeof(MODULE_INFO) * 0x1000);

    if (!modules)
    {
        return FALSE;
    }

    result = PsGetProcessModules(ProcessHandle,
                                 IsProcessInitialized,
                                 modules,
                                 0x1000,
                                 &numOfModules);

    if (!result)
    {
        DeallocMem(modules);
        return FALSE;
    }

    if (numOfModules == 0)
    {
        DeallocMem(modules);
        return FALSE;
    }

    for (SIZE_T i = 0; i < numOfModules; ++i)
    {
        if (IsModule32 && ((UINT64)(modules[i].ModuleHandle)) > 0xFFFFFFFFULL)
        {
            continue;
        }

        result = wcscpy_s(retModuleName, MAX_PATH, modules[i].ModuleName) == 0;

        if (!result)
        {
            DeallocMem(modules);
            return FALSE;
        }

        result = _wcsupr_s(retModuleName, MAX_PATH) == 0;

        if (!result)
        {
            DeallocMem(modules);
            return FALSE;
        }

        result = wcscmp(retModuleName, compareModuleName) == 0;

        if (result)
        {
            ModuleInfo->ModuleHandle = modules[i].ModuleHandle;
            wcscpy_s(ModuleInfo->ModuleName, MAX_PATH, modules[i].ModuleName);
            wcscpy_s(ModuleInfo->ModulePath, MAX_PATH, modules[i].ModulePath);
            DeallocMem(modules);
            return TRUE;
        }
    }

    DeallocMem(modules);
    return FALSE;
}

DWORD
PsProcessHandleToId(HANDLE ProcessHandle)
{
    HANDLE targetHandle;
    DWORD processId;

    if (ProcessHandle == GetCurrentProcess())
    {
        return GetCurrentProcessId();
    }

    processId = GetProcessId(ProcessHandle);

    if (processId)
    {
        return processId;
    }

    if (!PsCopyHandle(GetCurrentProcess(),
                      ProcessHandle,
                      &targetHandle,
                      PROCESS_QUERY_INFORMATION,
                      FALSE))
    {
        return 0;
    }

    processId = GetProcessId(targetHandle);

    CloseHandle(targetHandle);

    return processId;
}

HMODULE
PsGetRemoteModuleHandle(HANDLE ProcessHandle,
                        BOOL IsProcessInitialized,
                        BOOL AsModule32,
                        LPCWSTR ModuleName)
{
    MODULE_INFO moduleInfo;

    if(!PsGetModuleInfo(ProcessHandle,
                        IsProcessInitialized,
                        AsModule32,
                        ModuleName,
                        &moduleInfo))
    {
        return NULL;
    }

    return moduleInfo.ModuleHandle;
}

BOOL
PeGetProcAddressByName(HANDLE ProcessHandle,
                       BOOL AsX86,
                       LPVOID *ProcAddress,
                       LPVOID ModuleBase,
                       LPCSTR ProcName)
{
    DWORD numOfFunctions;
    DWORD numOfNames;
    DWORD numOfOrdinals;
    DWORD *addressTable;
    DWORD *nameTable;
    WORD *ordinalTable;
    LPVOID nameBase;
    LPVOID addressBase = NULL;
    CHAR functionName[MAX_EXPORT_SYMBOL_NAME_LENGTH];
    WORD addressPosition;
    BOOL found;

    if (!ModuleBase || !ProcName)
    {
        return FALSE;
    }

    if(!PeReadExportTable(ProcessHandle,
                          AsX86,
                          ModuleBase,
                          &numOfFunctions,
                          &numOfNames,
                          &numOfOrdinals,
                          &addressTable,
                          &nameTable,
                          &ordinalTable))
    {
        return FALSE;
    }

    found = FALSE;

    for (DWORD i = 0; i < numOfFunctions; ++i)
    {
        nameBase = PTR_ADD_OFFSET(ModuleBase,
                                  nameTable[i]);

        if(!ReadProcessMemory(ProcessHandle,
                              nameBase, functionName,
                              sizeof(functionName),
                              NULL))
        {
            break;
        }

        addressPosition = ordinalTable[i];

        if (!addressPosition)
            continue;

        addressBase = PTR_ADD_OFFSET(ModuleBase,
                                     addressTable[addressPosition]);

        functionName[0xFF] = '\0';
        if (strcmp(functionName, ProcName) == 0)
        {
            found = TRUE;
            break;
        }
    }

    PeFreeExportTable(addressTable,
                      nameTable,
                      ordinalTable);

    if (found)
    {
        *ProcAddress = addressBase;
    }

    return found;
}

LPVOID
PsGetRemoteProcAddress(HANDLE ProcessHandle,
                       HMODULE ModuleHandle,
                       LPCSTR ProcName)
{
    LPVOID procAddress = NULL;
    BOOL isX64;

    if(!PsIsX64Process(ProcessHandle, &isX64))
    {
        return NULL;
    }

    if(!PeGetProcAddressByName(ProcessHandle,
                               !isX64,
                               &procAddress,
                               (LPVOID)ModuleHandle,
                               ProcName))
    {
        return NULL;
    }

    return procAddress;
}

BOOL
PsCopyHandle(HANDLE ProcessHandle,
             HANDLE SourceHandle,
             PHANDLE DestinationHandle,
             DWORD DesiredAccess,
             BOOL InheritHandle)
{
    return DuplicateHandle(ProcessHandle,
                           SourceHandle,
                           GetCurrentProcess(),
                           DestinationHandle,
                           DesiredAccess,
                           InheritHandle,
                           0);
}
