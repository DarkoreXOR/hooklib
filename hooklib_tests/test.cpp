#include <Windows.h>
#include <stdio.h>

typedef
BOOL
(WINAPI *IPC_ROUTINE)(
    LPCWSTR ChannelName,
    LPVOID MessageBuffer,
    DWORD MessageSize,
    LPVOID AnswerBuffer,
    DWORD AnswerSize
);

typedef
VOID
(WINAPI *DEFAULT_PROC)();

typedef
BOOL
(WINAPI *CREATE_IPC_CHANNEL_PROC)(
    LPCWSTR ChannelName,
    IPC_ROUTINE Routine,
    BOOL MultiSession,
    PVOID *ChannelData
);

typedef
BOOL
(WINAPI *DESTROY_IPC_CHANNEL_PROC)(
    PVOID ChannelData
);

typedef
BOOL
(WINAPI *SEND_IPC_MESSAGE_PROC)(
    LPCWSTR ChannelName,
    PVOID MessageBuf,
    DWORD MessageSize,
    PVOID AnswerBuf,
    DWORD AnswerSize,
    DWORD Timeout,
    BOOL MultiSession
);

template<class R>
R HL_IMPORT(LPCSTR ProcName)
{
#ifdef _M_X64
    HMODULE Module = GetModuleHandleW(L"hooklib64");
#else
    HMODULE Module = GetModuleHandleW(L"hooklib32");
#endif
    return (R)GetProcAddress(Module, ProcName);
}

VOID
InitializeLibrary()
{
    DEFAULT_PROC Proc;

    Proc = HL_IMPORT<DEFAULT_PROC>(__FUNCTION__);

    Proc();
}

VOID
FinalizeLibrary()
{
    DEFAULT_PROC Proc;

    Proc = HL_IMPORT<DEFAULT_PROC>(__FUNCTION__);

    Proc();
}

BOOL
CreateIpcChannel(LPCWSTR ChannelName,
                 IPC_ROUTINE Routine,
                 BOOL MultiSession,
                 PVOID *ChannelData)
{
    CREATE_IPC_CHANNEL_PROC Proc;

    Proc = HL_IMPORT<CREATE_IPC_CHANNEL_PROC>(__FUNCTION__);

    return Proc(ChannelName,
                Routine,
                MultiSession,
                ChannelData);
}

BOOL
DestroyIpcChannel(PVOID ChannelData)
{
    DESTROY_IPC_CHANNEL_PROC Proc;

    Proc = HL_IMPORT<DESTROY_IPC_CHANNEL_PROC>(__FUNCTION__);

    return Proc(ChannelData);
}

BOOL
SendIpcMessage(LPCWSTR ChannelName,
               PVOID MessageBuf,
               DWORD MessageSize,
               PVOID AnswerBuf,
               DWORD AnswerSize,
               DWORD Timeout,
               BOOL MultiSession)
{
    SEND_IPC_MESSAGE_PROC Proc;

    Proc = HL_IMPORT<SEND_IPC_MESSAGE_PROC>(__FUNCTION__);

    return Proc(ChannelName,
                MessageBuf,
                MessageSize,
                AnswerBuf,
                AnswerSize,
                Timeout,
                MultiSession);
}

CRITICAL_SECTION cs;
static UINT64 Counter = 0;

BOOL
WINAPI
TestIpcRoutine(LPCWSTR ChannelName,
               PVOID MessageBuf,
               DWORD MessageSize,
               PVOID AnswerBuf,
               DWORD AnswerSize)
{
    EnterCriticalSection(&cs);
    wprintf_s(L"[%llu] %s\n", ++Counter, MessageBuf);
    *((BOOL*)AnswerBuf) = rand() % 2 == 1;
    LeaveCriticalSection(&cs);
    return TRUE;
}



DWORD
WINAPI
LoopSend(LPVOID Parameter)
{
    LPCWSTR Message = L"Wow. There is random text...";
    DWORD MessageSize = (DWORD)((wcslen(Message) + 1) * sizeof(WCHAR));
    BOOL AnswerBool;

    while (TRUE)
    {
        if (!SendIpcMessage(L"MyTestIpcChannel",
                            (LPVOID)Message,
                            MessageSize,
                            &AnswerBool,
                            sizeof(AnswerBool),
                            INFINITE,
                            FALSE))
        {
            printf("thread %d exit\n", GetCurrentThreadId());
            return 1;
        }

        /*
        printf("server return boolean = %s\n",
               AnswerBool ? "TRUE" : "FALSE");
        */
    }

    return 0;
}

int main()
{
    LPVOID ChannelData;

#ifdef _M_X64
    LoadLibraryW(L"hooklib64");
#else
    LoadLibraryW(L"hooklib32");
#endif

    InitializeCriticalSection(&cs);

    if (!CreateIpcChannel(L"MyTestIpcChannel",
                          TestIpcRoutine,
                          FALSE,
                          &ChannelData))
    {
        printf("CreateIpcChannel() failed\n");
        getchar();
        return 1;
    }

    Sleep(1000);

    const DWORD NumOfThreads = 8;
    HANDLE ThreadHandles[NumOfThreads];

    for (int i = 0; i < NumOfThreads; ++i)
    {
        ThreadHandles[i] = CreateThread(NULL, 0, LoopSend, NULL, 0, NULL);
    }

    getchar();

    for (int i = 0; i < NumOfThreads; ++i)
    {
        CloseHandle(ThreadHandles[i]);
    }

    // all threads will be stopped, SendIpcMessage() returns FALSE

    if (!DestroyIpcChannel(ChannelData))
    {
        printf("DestroyIpcChannel() failed\n");
    }

    printf("all tests done\n");
    getchar();

    return 0;
}
