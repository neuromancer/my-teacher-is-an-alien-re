#ifndef MEMORY_H
#define MEMORY_H

void* AllocateMemory(unsigned int size);
void FreeMemory(void* ptr);
void FreeFromGlobalHeap(void* ptr);

void* AllocateMemory_Wrapper(int size);

#endif // MEMORY_H
