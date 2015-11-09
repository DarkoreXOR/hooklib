#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <Windows.h>
#include <tchar.h>
#include "..\logger\logger_utils.h"

BOOL
GetShortFileName(
    LPCWSTR SourceFilePath,
    LPWSTR DestinationFileName,
    SIZE_T DestinationFileNameSize
);

#endif
