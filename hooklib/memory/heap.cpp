#include "heap.h"

HANDLE DefaultHeapHandle;

LPVOID
AllocMem(SIZE_T Size)
{
    /*
    return HeapAlloc(DefaultHeapHandle,
                     HEAP_ZERO_MEMORY,
                     Size);
    */

    return malloc(Size);
}

BOOL
DeallocMem(LPVOID MemoryPointer)
{
    /*
    return HeapFree(DefaultHeapHandle,
                    HEAP_ZERO_MEMORY,
                    MemoryPointer);
    */

    if (!MemoryPointer)
    {
        return FALSE;
    }

    free(MemoryPointer);

    return TRUE;
}

VOID
HeapInitialize()
{
    DefaultHeapHandle = GetProcessHeap();
}
