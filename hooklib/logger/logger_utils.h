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

#ifndef __LOGGER_UTILS_H
#define __LOGGER_UTILS_H

#include <Windows.h>
#include <tchar.h>
#include "..\fastipc\ipc.h"

#define LOG_TYPE_DEFAULT 0x00
#define LOG_TYPE_WAIT 0x01

typedef struct _LGR_LOG_MESSAGE_PACKET
{
    BYTE Type;
    WCHAR File[MAX_PATH];
    WCHAR Function[0x100];
    UINT Line;
    WCHAR Message[0x100];
} LGR_LOG_MESSAGE_PACKET, *PLGR_LOG_MESSAGE_PACKET;

VOID
LgrLog(
    LPCSTR File,
    LPCSTR Function,
    UINT Line,
    LPCWSTR Format,
    ...
);

VOID
LgrLogWait(
    LPCSTR File,
    LPCSTR Function,
    UINT Line,
    LPCWSTR Format,
    ...
);

#define LOG(format, ...) \
LgrLog(__FILE__,\
       __FUNCSIG__,\
       __LINE__,\
       ##format,\
       ##__VA_ARGS__)

#define LOG_WAIT(format, ...) \
LgrLogWait(__FILE__,\
           __FUNCSIG__,\
           __LINE__,\
           ##format,\
           ##__VA_ARGS__)

#endif
