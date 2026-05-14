#include "Handler8.h"
#include "SC_Question.h"
#include "SpriteAction.h"
#include "Animation.h"
#include "Parser.h"
#include "string.h"
#include "Memory.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>

// Message queue structures
// Queue item is 0xc8 (200) bytes
struct MessageQueueItem {
    MessageQueueItem* next;  // 0x00
    int tailRef;             // 0x04
    char data[0x88];         // 0x08 - copied parser data
    int targetAddress;       // 0x90
    int sourceAddress;       // 0x94
    int command;             // 0x98
    int msgData;             // 0x9c
    int priority;            // 0xa0
    int param1;              // 0xa4
    int param2;              // 0xa8
    int clickX;              // 0xac
    int clickY;              // 0xb0
    int mouseX;              // 0xb4
    int mouseY;              // 0xb8
    int field_bc;            // 0xbc
    int field_c0;            // 0xc0
    int userPtr;             // 0xc4
};

struct MessageQueue {
    MessageQueueItem* head;  // 0x00
    MessageQueueItem* tail;  // 0x04
    int count;               // 0x08
    MessageQueueItem* freeList; // 0x0c
    void* poolBase;          // 0x10
    int poolCapacity;        // 0x14
};

/* Constructor not matched to a unique full-game function yet. */
Handler8::Handler8() {
    int* ptr = &handlerId;
    int* pA0 = &field_A0;

    // Set handler-specific fields at 0xA0 and 0xA4
    pA0[0] = 0;
    pA0[1] = 0;

    // Set handlerId
    *ptr = 8;
}

// Full-game address is not identified here; 0x44A900 is mCNavNode_TypeLogic::~mCNavNode_TypeLogic.
Handler8::~Handler8() {
    SpriteAction* action = (SpriteAction*)field_A0;
    if (action != 0) {
        action->~SpriteAction();
        FreeMemory(action);
        field_A0 = 0;
    }
}
