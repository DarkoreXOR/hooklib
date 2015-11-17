#ifndef __HEAP_H
#define __HEAP_H

#include <Windows.h>

LPVOID
AllocMem(
    SIZE_T Size
);

BOOL
DeallocMem(
    LPVOID MemoryPointer
);

VOID
HeapInitialize();

#endif
