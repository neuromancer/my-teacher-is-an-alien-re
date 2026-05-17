#include "Memory.h"
#include <stdlib.h>

/* Function start: 0x424940 */ /* DEMO ONLY - no full game match */ /* No assembly extracted */
void FreeMemory(void* ptr)
{
    operator delete(ptr);
}

/* Function start: 0x4249C0 */ /* DEMO ONLY - no full game match */ /* No assembly extracted */
void* AllocateMemory(unsigned int size)
{
    return operator new(size);
}

/* Function start: 0x4277E0 */ /* DEMO ONLY - no full game match */ /* No assembly extracted */
void FreeFromGlobalHeap(void* ptr)
{
    operator delete(ptr);
}

// Wrapper used by menu/UI code
extern "C" void* AllocateMemory_Wrapper(int size)
{
    return malloc(size);
}
