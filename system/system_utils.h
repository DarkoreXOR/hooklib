#ifndef __SYSTEM_UTILS_H
#define __SYSTEM_UTILS_H

#include <Windows.h>
#include "..\logger\logger_utils.h"

typedef
VOID
(WINAPI *GET_NATIVE_SYSTEM_INFO)(
    LPSYSTEM_INFO SysInfo
);

BOOL
SysIsX64System();

#endif
