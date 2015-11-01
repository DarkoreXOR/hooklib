#ifndef __STR_UTILS_H
#define __STR_UTILS_H

#include <Windows.h>
#include <tchar.h>

BOOL
GetShortFileName(
    LPCWSTR SrcFilePath,
    LPWSTR DestFileName,
    SIZE_T DestFileNameSize
);

#endif
