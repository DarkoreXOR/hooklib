/*
 *  [ BSD License: http://opensource.org/licenses/bsd-license.php ]
 *  ===========================================================================
 *  Copyright (c) 2015, Lakutin Ivan
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
