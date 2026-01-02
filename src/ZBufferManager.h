#ifndef ZBUFFERMANAGER_H
#define ZBUFFERMANAGER_H

#include "Timer.h"

// Queue node structure - 12 bytes
struct ZBQueueNode {
    void* next;     // 0x00
    void* prev;     // 0x04
    void* data;     // 0x08
    
    void* CleanupNode(int flag);  // 0x41CCE0
};

// Queue structure - 16 bytes
struct ZBQueue {
    void* head;     // 0x00
    void* tail;     // 0x04
    void* current;  // 0x08
    int type;       // 0x0C
    
    void* GetCurrentData();  // 0x41BB10
    void* PopNode();         // 0x401810
};

// ZBufferManager class 
// Size: 0xAC (172 bytes)
// Global: DAT_0043698c
// Manages ZBuffer rendering and object lists
// Contains Timer at offset 0x84
class ZBufferManager {
public:
    ZBufferManager();             // 0x41B760
    ~ZBufferManager();
    void Cleanup();               // 0x41B8E0
    void ProcessRenderQueues();   // 0x41C5A0
    void FUN_0041c960();          // 0x41C960
    
    unsigned int data[0x21];     // 0x00 - 0x80 (33 dwords)
    Timer m_timer;               // 0x84 - Timer (20 bytes, 5 dwords)
    int m_state;                 // 0x98
    ZBQueue* m_queue9c;          // 0x9C
    ZBQueue* m_queueA0;          // 0xA0
    ZBQueue* m_queueA4;          // 0xA4
    void* m_fieldA8;             // 0xA8
};

#endif // ZBUFFERMANAGER_H
