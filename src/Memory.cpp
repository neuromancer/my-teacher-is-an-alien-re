#include "Memory.h"
#include <windows.h>

extern "C" {
}

extern HANDLE DAT_0043eff0;
extern int (*g_OutOfMemoryCallback)(unsigned int);

/* Function start: 0x4249C0 */
void* AllocateMemory(unsigned int size)
{
    return AllocateMemoryInternal(size, 1);
}

/* Function start: 0x428460 */
void* AllocateMemoryInternal(unsigned int size, int flag)
{
    if (size > 0xffffffe0) {
        return 0;
    }
    if (size == 0) {
        size = 1;
    }
    while (1) {
        void* mem = HeapAllocWrapper(size);
        if (mem != 0) {
            return mem;
        }
        if (flag == 0) {
            break;
        }
        if (OutOfMemoryHandler(size) == 0) {
            return 0;
        }
    }
    return 0;
}

/* Function start: 0x424940 */
void FreeMemory(void* ptr)
{
    FreeFromGlobalHeap(ptr);
}

/* Function start: 0x4284A0 */
void* HeapAllocWrapper(unsigned int size)
{
    return HeapAlloc(DAT_0043eff0, 0, size);
}

/* Function start: 0x42C5C0 */
int OutOfMemoryHandler(unsigned int size)
{
    if (g_OutOfMemoryCallback != 0) {
        if (g_OutOfMemoryCallback(size) != 0) {
            return 1;
        }
    }
    return 0;
}

/* Function start: 0x4277E0 */
void FreeFromGlobalHeap(void* ptr)
{
    if (ptr != 0) {
        HeapFree(DAT_0043eff0, 0, ptr);
    }
}
