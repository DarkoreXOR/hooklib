#include "il_utils_x86.h"

BYTE InjectCodeX86[INJECT_CODE_X86_SIZE] =
{
    0x68, 0x00, 0x00, 0x00, 0x00, // push 0x00000000
    0xE8, 0x00, 0x00, 0x00, 0x00, // call 0x00000000
    0xC3 // ret
};

LPVOID
CalcJumpRelX86(LPVOID InstructionAddress,
               LPVOID JumpAddress)
{
    UINT32 InstructionAddressVal;
    UINT32 JumpAddressVal;

    InstructionAddressVal = IL_PTR_TO_UI32(InstructionAddress);
    JumpAddressVal = IL_PTR_TO_UI32(JumpAddress);

    return IL_UINT_TO_PTR(0 - (InstructionAddressVal - JumpAddressVal) - 5);
}

BOOL
FixStringPointerX86(HANDLE ProcessHandle,
                    LPVOID StubAddress,
                    LPVOID StringAddress)
{
    BOOL Result;
    UINT StubAddressVal;
    UINT StringAddressVal;
    LPVOID CodeAddressOffset;

    StubAddressVal = IL_PTR_TO_UI32(StubAddress);
    StringAddressVal = IL_PTR_TO_UI32(StringAddress);

    CodeAddressOffset = IL_UINT_TO_PTR(StubAddressVal + 1);

    Result = WriteProcessMemory(ProcessHandle,
                                CodeAddressOffset,
                                &StringAddressVal,
                                4,
                                NULL);

    return Result;
}

BOOL
FixCallPointerX86(HANDLE ProcessHandle,
                  LPVOID CodeAddress,
                  LPVOID CallAddress)
{
    BOOL Result;
    UINT CodeAddressVal;
    LPVOID CodeAddressOffset;
    LPVOID Bytes;

    CodeAddressVal = IL_PTR_TO_UI32(CodeAddress);

    Bytes = CalcJumpRelX86(IL_UINT_TO_PTR(CodeAddressVal + 5),
                           CallAddress);

    CodeAddressOffset = IL_UINT_TO_PTR(CodeAddressVal + 6);

    Result = WriteProcessMemory(ProcessHandle,
                                CodeAddressOffset,
                                &Bytes,
                                4,
                                NULL);

    return Result;
}

BOOL
IlInjectLibrary32(LPCWSTR DllFileName,
                  DWORD ProcessId,
                  DWORD Timeout)
{
    BOOL Result;
    PVOID LoadLibraryWPtr;
    HANDLE ProcessHandle;
    LPVOID CodeMemPtr;
    LPVOID StrMemPtr;
    HANDLE ThreadHandle;
    //DWORD ThreadExitCode;

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
                            InjectCodeX86,
                            INJECT_CODE_X86_SIZE,
                            &CodeMemPtr);

    if (!Result)
    {
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    // allocate dll file name
    Result = IlAllocateWideString(ProcessHandle,
                                  DllFileName,
                                  &StrMemPtr);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, CodeMemPtr);
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    // fix push address
    Result = FixStringPointerX86(ProcessHandle,
                                 CodeMemPtr,
                                 StrMemPtr);

    if (!Result)
    {
        IlDeallocate(ProcessHandle, CodeMemPtr);
        IlDeallocate(ProcessHandle, StrMemPtr);
        CloseHandle(ProcessHandle);
        return FALSE;
    }

    // fix call address
    Result = FixCallPointerX86(ProcessHandle,
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
                                      CodeMemPtr,
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

    //GetExitCodeThread(ThreadHandle, &ThreadExitCode);

    CloseHandle(ThreadHandle);
    IlDeallocate(ProcessHandle, CodeMemPtr);
    IlDeallocate(ProcessHandle, StrMemPtr);
    CloseHandle(ProcessHandle);

    return Result;
}
