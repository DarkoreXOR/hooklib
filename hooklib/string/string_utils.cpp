#include "string_utils.h"

BOOL
GetShortFileName(LPCWSTR SourceFilePath,
                 LPWSTR DestinationFileName,
                 SIZE_T DestinationFileNameSize)
{
    BOOL result;
    LPCWSTR fileNameStartWithBackSlash;
    LPCWSTR fileNameStartWithSlash;
    LPCWSTR fileNameStart;

    fileNameStartWithBackSlash = wcsrchr(SourceFilePath, '\\');
    fileNameStartWithSlash = wcsrchr(SourceFilePath, '/');

    if (!fileNameStartWithBackSlash && !fileNameStartWithSlash)
    {
        wcscpy_s(DestinationFileName,
                 DestinationFileNameSize,
                 SourceFilePath);

        return TRUE;
    }

    fileNameStart = fileNameStartWithBackSlash;

    if (fileNameStartWithSlash && fileNameStartWithSlash > fileNameStartWithBackSlash)
    {
        fileNameStart = fileNameStartWithSlash;
    }

    if (!fileNameStart)
    {
        return FALSE;
    }

    result = wcscpy_s(DestinationFileName,
                      DestinationFileNameSize,
                      fileNameStart + 1) == 0;

    return result;
}
