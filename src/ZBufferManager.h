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
class ZBufferManager {
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
    unsigned int data[0x21];     // 0x00 - 0x80 (33 dwords)
    Timer timer;                 // 0x84 - Timer (20 bytes, 5 dwords) - also known as m_timer
    int m_state;                 // 0x98 - also known as m_mode
    ZBQueue* m_queue9c;          // 0x9C - also known as m_list1
    ZBQueue* m_queueA0;          // 0xA0 - also known as m_commandQueue
    ZBQueue* m_queueA4;          // 0xA4 - also known as m_list3
    void* m_fieldA8;             // 0xA8
    
    // Compatibility aliases for code that uses old SoundManager member names
    // These are not actual members, they reference the same offsets above:
    // m_mode        -> m_state     (offset 0x98)
    // m_commandQueue -> m_queueA0  (offset 0xA0)
    // m_list1       -> m_queue9c   (offset 0x9C)
    // m_list3       -> m_queueA4   (offset 0xA4)
};

// Global pointer - this is the single global instance
// Previously had two names (g_SoundManager, DAT_0043698c) but they point to the same object
extern ZBufferManager* g_SoundManager;

#endif // ZBUFFERMANAGER_H
