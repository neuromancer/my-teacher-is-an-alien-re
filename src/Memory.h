#ifndef MEMORY_H
#define MEMORY_H

void* AllocateMemory_Wrapper(unsigned int size);
extern "C" void* AllocateMemory(unsigned int size, int flag);

#endif // MEMORY_H
