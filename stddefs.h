#ifndef __STDDEFS_H
#define __STDDEFS_H

#include <Windows.h>

#define PTR_ADD_OFFSET(base, offset) ((LPVOID)((ULONG_PTR)base + offset))
#define PTR_SUB_OFFSET(base, offset) ((LPVOID)((ULONG_PTR)base + offset))

#endif
