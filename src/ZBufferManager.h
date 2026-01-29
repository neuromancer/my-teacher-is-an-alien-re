#ifndef ZBUFFERMANAGER_H
#define ZBUFFERMANAGER_H

#include "Timer.h"

#include "Queue.h"

// Queue node structure - 12 bytes
struct ZBQueueNode {
    ZBQueueNode* next;  // 0x00
    ZBQueueNode* prev;  // 0x04
    void* data;         // 0x08

    ZBQueueNode() : next(0), prev(0), data(0) {}
    ~ZBQueueNode() { data = 0; next = 0; prev = 0; }
    ZBQueueNode* Init(void* d) { next = 0; prev = 0; data = d; return this; }  // 0x41CD10

    void* CleanupNode(int flag);  // 0x41CCE0
    void* Cleanup(int flag);      // 0x4189A0
};

// Queue structure - 16 bytes
struct ZBQueue {
    ZBQueueNode* head;     // 0x00
    ZBQueueNode* tail;     // 0x04
    ZBQueueNode* current;  // 0x08
    int type;              // 0x0C

    ZBQueue(int t) {
        type = t;
        head = 0;
        tail = 0;
        current = head;
    }

    void* GetCurrentData();       // 0x41BB10
    void* PopNode();              // 0x401810
    void* PopNode2();             // 0x401710
    void* PopNode2_2();           // 0x401790
    void ClearList();             // 0x401560
    void Insert(void* data);      // 0x409160
    void InsertBeforeCurrent(void* data);  // 0x41CB40
};

// Forward declaration for command type used in queuing
struct SoundCommand;

// ZBufferManager class
// Size: 0xAC (172 bytes)
// Global: g_ZBufferManager_0043698c (also known as g_ZBufferManager_0043698c)
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
    void DrawRect(int, int, int, int, int, int, int); // 0x41C130

    // Methods previously attributed to "SoundManager"
    void PlayAnimationSound(void* data, int priority, int x, int y, int mode, double scale); // 0x41BE20
    void QueueCommand(SoundCommand* cmd); // 0x41C2C0

    // Layout: 0xAC bytes total
    // Fields at 0x00 - 0x14 (20 bytes)
    unsigned int m_flags;        // 0x00 - flags (bit 2 = jitter mode)
    void* m_head;                // 0x04
    void* m_tail;                // 0x08
    void* m_current;             // 0x0C
    int m_field_0x10;            // 0x10
    unsigned int m_pad14[0x1C];  // 0x14 - 0x84 (28 dwords = 112 bytes)
    Timer timer;                 // 0x84 - Timer (20 bytes, 5 dwords) - also known as m_timer
    int m_state;                 // 0x98 - also known as m_mode
    ZBQueue* m_queue9c;          // 0x9C - also known as m_list1
    ZBQueue* m_queueA0;          // 0xA0 - also known as m_commandQueue
    ZBQueue* m_queueA4;          // 0xA4 - also known as m_list3
    void* m_fieldA8;             // 0xA8
};

// Global pointer - this is the single global instance
// Previously had two names (g_ZBufferManager_0043698c, g_ZBufferManager_0043698c) but they point to the same object
//extern ZBufferManager* g_ZBufferManager_0043698c;

#endif // ZBUFFERMANAGER_H
