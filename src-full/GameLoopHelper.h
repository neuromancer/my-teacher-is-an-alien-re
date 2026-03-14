#ifndef GAMELOOPHELPER_H
#define GAMELOOPHELPER_H

struct AnimPool {
    int* head;      // 0x00
    int* tail;      // 0x04
    int count;      // 0x08
    int* freeList;  // 0x0C
    int* blockList; // 0x10
    int blockSize;  // 0x14
};

class GameLoopHelper {
public:
    AnimPool* pool;  // 0x00

    void PostProcess();                          // 0x41a960
    ~GameLoopHelper();                           // 0x41a730
    void AddAnimation(char* name, int handle);   // 0x41a7a0
    void RemoveAnimation(int handle);            // 0x41a8d0
};

extern GameLoopHelper* g_GameLoopHelper;  // DAT_0046a6f0

#endif // GAMELOOPHELPER_H
