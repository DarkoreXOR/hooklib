#include "str_utils.h"

BOOL
GetShortFileName(LPCWSTR SrcFilePath,
                 LPWSTR DestFileName,
                 SIZE_T DestFileNameSize)
{
    BOOL Result;
    LPCWSTR FileNameStartWithBackSlash;
    LPCWSTR FileNameStartWithSlash;
    LPCWSTR FileNameStart;

    FileNameStartWithBackSlash = wcsrchr(SrcFilePath, '\\');
    FileNameStartWithSlash = wcsrchr(SrcFilePath, '/');

    if (!FileNameStartWithBackSlash && !FileNameStartWithSlash)
    {
        wcscpy_s(DestFileName, DestFileNameSize, SrcFilePath);
        return TRUE;
    }

    FileNameStart = FileNameStartWithBackSlash;

    if (FileNameStartWithSlash && FileNameStartWithSlash > FileNameStartWithBackSlash)
    {
        FileNameStart = FileNameStartWithSlash;
    }

    if (!FileNameStart)
    {
        return FALSE;
    }

    Result = wcscpy_s(DestFileName, DestFileNameSize, FileNameStart + 1) == 0;

    return Result;
}
