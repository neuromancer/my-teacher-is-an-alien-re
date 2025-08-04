#ifndef MEMORY_H
#define MEMORY_H

void* AllocateMemory(unsigned int size);
void FreeMemory(void* ptr);
void FreeFromGlobalHeap(void* ptr);
extern "C" void* AllocateMemoryInternal(unsigned int size, int flag);
void* HeapAllocWrapper(unsigned int size);
int OutOfMemoryHandler(unsigned int size);

#endif // MEMORY_H
