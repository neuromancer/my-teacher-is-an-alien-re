#include "Handler8.h"
#include "SC_Question.h"
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

/* Function start: 0x4415E0 */ /* ~92% match */
Handler8::Handler8() {
    int* ptr = &handlerId;
    int* pA0 = (int*)&message;

    // Set handler-specific fields at 0xa0 and 0xa4
    pA0[0] = 0;
    pA0[1] = 0;

    // Set handlerId at 0x88
    *ptr = 8;
}

/* Function start: 0x44A900 */
Handler8::~Handler8() {
    if (message != 0) {
        delete message;
        message = 0;
    }
}

