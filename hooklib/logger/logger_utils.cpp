#include "logger_utils.h"

VOID
AnsiToWide(LPCSTR Ansi,
           SIZE_T AnsiLength,
           LPWSTR Wide,
           SIZE_T WideLength)
{
    MultiByteToWideChar(CP_ACP,
                        MB_PRECOMPOSED,
                        Ansi,
                        (int)(AnsiLength + 1),
                        Wide,
                        (int)WideLength);
}

VOID
LgrLog(LPCSTR File,
       LPCSTR Function,
       UINT Line,
       LPCWSTR Format,
       ...)
{
    LGR_LOG_MESSAGE_PACKET messagePacket;
    va_list vl;
    va_start(vl, Format);

    messagePacket.Type = LOG_TYPE_DEFAULT;

    AnsiToWide(File,
               strlen(File),
               messagePacket.File,
               MAX_PATH);

    AnsiToWide(Function,
               strlen(Function),
               messagePacket.Function,
               0x100);

    messagePacket.Line = Line;

    vswprintf_s(messagePacket.Message,
                0x100,
                Format,
                vl);

    IpcSendIpcMessage(L"hooklib_debug_logger_server",
                      &messagePacket,
                      sizeof(LGR_LOG_MESSAGE_PACKET),
                      NULL,
                      0,
                      INFINITE,
                      TRUE);

    va_end(vl);
}

VOID
LgrLogWait(LPCSTR File,
           LPCSTR Function,
           UINT Line,
           LPCWSTR Format,
           ...)
{
    LGR_LOG_MESSAGE_PACKET messagePacket;
    BOOL answer;
    va_list vl;
    va_start(vl, Format);

    messagePacket.Type = LOG_TYPE_WAIT;

    AnsiToWide(File,
               strlen(File),
               messagePacket.File,
               MAX_PATH);

    AnsiToWide(Function,
               strlen(Function),
               messagePacket.Function,
               0x100);

    messagePacket.Line = Line;

    vswprintf_s(messagePacket.Message,
                0x100,
                Format,
                vl);

    IpcSendIpcMessage(L"hooklib_debug_logger_server",
                      &messagePacket,
                      sizeof(LGR_LOG_MESSAGE_PACKET),
                      &answer,
                      sizeof(answer),
                      INFINITE,
                      TRUE);

    va_end(vl);
}
