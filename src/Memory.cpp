#include "Memory.h"

/* Function start: 0x4249C0 */
void* AllocateMemory_Wrapper(unsigned int size)
{
    return AllocateMemory(size, 1);
}
