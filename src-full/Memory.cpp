#include "Memory.h"
#include "globals.h"
#include <windows.h>
#include <stdlib.h>

extern HANDLE g_ProcessHeap_0043eff0;
extern int (*g_OutOfMemoryCallback)(unsigned int);

// Global operator new/delete - full game uses CRT malloc/free (0x454500/0x454400)
void* operator new(unsigned int size) {
    return malloc(size);
}

void operator delete(void* ptr) {
    free(ptr);
}

/* Function start: 0x424940 */ /* DEMO ONLY - no full game match */
void FreeMemory(void* ptr)
{
    FreeFromGlobalHeap(ptr);
}

/* Function start: 0x4249C0 */ /* DEMO ONLY - no full game match */
void* AllocateMemory(unsigned int size)
{
    return AllocateMemoryInternal(size, 1);
}

/* Function start: 0x4277E0 */ /* DEMO ONLY - no full game match */
void FreeFromGlobalHeap(void* ptr)
{
    if (ptr != 0) {
        HeapFree(g_ProcessHeap_0043eff0, 0, ptr);
    }
}

/* Function start: 0x428440 */ /* DEMO ONLY - no full game match */
void* CrtMalloc(unsigned int size)
{
    return AllocateMemoryInternal(size, g_TimeSeed_0043cb64);
}

/* Function start: 0x428460 */ /* DEMO ONLY - no full game match */
void* AllocateMemoryInternal(unsigned int size, int flag)
{
    void* mem;
    if (size > 0xffffffe0) {
        return 0;
    }
    if (size == 0) {
        size = 1;
    }
    while (1) {
        mem = HeapAllocWrapper(size);
        if (mem != 0) break;
        if (flag == 0) break;
        if (OutOfMemoryHandler(size) == 0) {
            mem = 0;
            break;
        }
    }
    return mem;
}

/* Function start: 0x4284A0 */ /* DEMO ONLY - no full game match */
void* HeapAllocWrapper(unsigned int size)
{
    if (g_ProcessHeap_0043eff0 == 0) {
        g_ProcessHeap_0043eff0 = GetProcessHeap();
    }
    return HeapAlloc(g_ProcessHeap_0043eff0, 0, size);
}

/* Function start: 0x42C5C0 */ /* DEMO ONLY - no full game match */
int OutOfMemoryHandler(unsigned int size)
{
    if (g_OutOfMemoryCallback != 0) {
        if (g_OutOfMemoryCallback(size) != 0) {
            return 1;
        }
    }
    return 0;
}

// Wrapper used by menu/UI code
extern "C" void* AllocateMemory_Wrapper(int size)
{
    return malloc(size);
}
