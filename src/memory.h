#ifndef MEMORY_H
#define MEMORY_H

#include <windows.h>

void* __cdecl AllocateMemory_Wrapper(unsigned int size);
void* __cdecl AllocateMemory(size_t size, int flag);
void* __cdecl AllocateMemoryFromHeap(size_t size);
int __cdecl CallOutOfMemoryHandler(size_t size);

#endif // MEMORY_H
