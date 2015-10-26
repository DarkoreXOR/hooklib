#include "il_utils_x64.h"

BYTE InjectCodeX64[INJECT_CODE_X64_SIZE] =
{
    // [0..7] DLL_FILE_NAME_ADDR
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // [8..15] LOADLIBRARYW_ADDR
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // [16..19] sub rsp, 0x28
    0x48, 0x83, 0xEC, 0x28,
    // [20..29] mov rax, [0x0000000000000000] ; DLL_FILE_NAME_ADDR address
    0x48, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // [30..32] mov rcx, rax
    0x48, 0x8B, 0xC8,
    // [33..42] mov rax, [0x0000000000000000] ; LOADLIBRARYW_ADDR address
    0x48, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // [43..45] call rax
    0x48, 0xFF, 0xD0,
    // [46..49] add rsp, 0x28
    0x48, 0x83, 0xC4, 0x28,
    // [50..50]  ret
    0xC3
};

BOOL
FixStringPointersX64(HANDLE ProcessHandle,
                     LPVOID StubAddress,
                     LPVOID StringAddress)
{
    BOOL Result;
    UINT64 StringPointerVal;
    UINT64 StringAddressVal;

    StringPointerVal = IL_PTR_TO_UI64(StubAddress);
    StringAddressVal = IL_PTR_TO_UI64(StringAddress);

    Result = WriteProcessMemory(ProcessHandle,
                                StubAddress,
                                &StringAddressVal,
                                8,
                                NULL);

    if (!Result)
    {
        return FALSE;
    }

    Result = WriteProcessMemory(ProcessHandle,
                                IL_UINT_TO_PTR(IL_PTR_TO_UI64(StubAddress) + 22),
                                &StringPointerVal,
                                8,
                                NULL);

    return Result;
}

BOOL
FixCallPointersX64(HANDLE ProcessHandle,
                   LPVOID StubAddress,
                   LPVOID CallAddress)
{
    BOOL Result;

    UINT64 CallPointerVal;
    UINT64 CallAddressVal;

    CallPointerVal = IL_PTR_TO_UI64(StubAddress) + 8;
    CallAddressVal = IL_PTR_TO_UI64(CallAddress);

    Result = WriteProcessMemory(ProcessHandle,
                                IL_UINT_TO_PTR(IL_PTR_TO_UI64(StubAddress) + 8),
                                &CallAddressVal,
                                8,
                                NULL);

    if (!Result)
    {
        return FALSE;
    }

    Result = WriteProcessMemory(ProcessHandle,
                                IL_UINT_TO_PTR(IL_PTR_TO_UI64(StubAddress) + 35),
                                &CallPointerVal,
                                8,
                                NULL);

    return Result;
}

BOOL
IlInjectLibrary64(LPCWSTR DllFileName,
                  DWORD ProcessId,
                  DWORD Timeout)
{
    BOOL Result;
    PVOID LoadLibraryWPtr;
    HANDLE ProcessHandle;
    LPVOID CodeMemPtr;
    LPVOID StrMemPtr;
    HANDLE ThreadHandle;

    // get LoadLibraryW address
    Result = IlGetLoadLibraryWProcAddress(&LoadLibraryWPtr);

    if (!Result)
    {
        return FALSE;
    }

    // open target process
    ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS,
                                FALSE,
                                ProcessId);

    if (!ProcessHandle)
    {
        return FALSE;
    }

    // allocate code
    Result = IlAllocateCode(ProcessHandle,
                            InjectCodeX64,
                            INJECT_CODE_X64_SIZE,
                            &CodeMemPtr);

    if (!Result)
    {
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    Result = IlAllocateWideString(ProcessHandle,
                                  DllFileName,
                                  &StrMemPtr);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, CodeMemPtr);
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    Result = FixStringPointersX64(ProcessHandle,
                                  CodeMemPtr,
                                  StrMemPtr);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, CodeMemPtr);
        IlDeallocate(ProcessHandle, StrMemPtr);
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    Result = FixCallPointersX64(ProcessHandle,
                                CodeMemPtr,
                                LoadLibraryWPtr);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, CodeMemPtr);
        IlDeallocate(ProcessHandle, StrMemPtr);
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    // create thread
    ThreadHandle = CreateRemoteThread(ProcessHandle,
                                      NULL,
                                      0,
                                      IL_UINT_TO_PTR(IL_PTR_TO_UI64(CodeMemPtr) + 16),
                                      NULL,
                                      0,
                                      NULL);

    Result = ThreadHandle != 0;

    if (!Result)
    {
        IlDeallocate(ProcessHandle, CodeMemPtr);
        IlDeallocate(ProcessHandle, StrMemPtr);
        CloseHandle(ProcessHandle);
    }

    WaitForSingleObject(ThreadHandle, Timeout);

    CloseHandle(ThreadHandle);
    IlDeallocate(ProcessHandle, CodeMemPtr);
    IlDeallocate(ProcessHandle, StrMemPtr);
    CloseHandle(ProcessHandle);

    return TRUE;
}
