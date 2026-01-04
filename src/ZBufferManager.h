#ifndef ZBUFFERMANAGER_H
#define ZBUFFERMANAGER_H

#include "Timer.h"

#include "Queue.h"

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

// Forward declaration for command type used in queuing
struct SoundCommand;

// ZBufferManager class
// Size: 0xAC (172 bytes)
// Global: DAT_0043698c (also known as g_SoundManager)
// Manages ZBuffer rendering, object lists, and animation/sound command queuing
// Contains Timer at offset 0x84
//
// Note: This was previously split into SoundManager and ZBufferManager,
// but assembly analysis shows they are the same class.
// The constructor logs "declaring ZBuffer", confirming this is the original name.
class ZBufferManager : public Queue {
public:
    ZBufferManager();             // 0x41B760
    ~ZBufferManager();
    void Cleanup();               // 0x41B8E0
    void ProcessRenderQueues();   // 0x41C5A0
    void UpdateScreen();          // 0x41C960
    void ShowSubtitle(char* text, int x, int y, int duration, int font); // 0x41C000

    // Methods previously attributed to "SoundManager"
    void PlayAnimationSound(void* data, int priority, int x, int y, int mode, int scale1, int scale2); // 0x41BE20
    void QueueCommand(SoundCommand* cmd); // 0x41C2C0
    
    // Layout: 0xAC bytes total
    // Queue fields at 0x00 - 0x10 (inherited)
    unsigned int m_pad10[0x1D];  // 0x10 - 0x84 (29 dwords = 116 bytes)
    Timer timer;                 // 0x84 - Timer (20 bytes, 5 dwords) - also known as m_timer
    int m_state;                 // 0x98 - also known as m_mode
    ZBQueue* m_queue9c;          // 0x9C - also known as m_list1
    ZBQueue* m_queueA0;          // 0xA0 - also known as m_commandQueue
    ZBQueue* m_queueA4;          // 0xA4 - also known as m_list3
    void* m_fieldA8;             // 0xA8
};

// Global pointer - this is the single global instance
// Previously had two names (g_SoundManager, DAT_0043698c) but they point to the same object
extern ZBufferManager* g_SoundManager;

#endif // ZBUFFERMANAGER_H
