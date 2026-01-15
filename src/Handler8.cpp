#include "Handler8.h"
#include "SC_Question.h"
#include "Animation.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

extern "C" int FileExists(const char* filename);
extern void SC_Message_Send(int, int, int, int, int, int, int, int, int, int);

// Message queue structure at 0x436988
// Queue item is 0xc8 (200) bytes
struct MessageQueueItem {
    MessageQueueItem* next;  // 0x00
    int tailRef;             // 0x04
    char data[0x88];         // 0x08 - copied data
    int field_90;            // 0x90
    int field_94;            // 0x94
    int field_98;            // 0x98
    int field_9c;            // 0x9c
    int field_a0;            // 0xa0
    int field_a4;            // 0xa4
    int field_a8;            // 0xa8
    int field_ac;            // 0xac
    int field_b0;            // 0xb0
    int field_b4;            // 0xb4
    int field_b8;            // 0xb8
    int field_bc;            // 0xbc
    int field_c0;            // 0xc0
    int field_c4;            // 0xc4
};

struct MessageQueue {
    MessageQueueItem* head;  // 0x00
    MessageQueueItem* tail;  // 0x04
    int count;               // 0x08
    MessageQueueItem* freeList; // 0x0c
    void* poolBase;          // 0x10
    int poolCapacity;        // 0x14
};

extern MessageQueue* g_MessageQueue;  // 0x436988
extern void* FUN_4065e0(void* pool, int capacity, int itemSize);
extern void FUN_406670(void* data, int param);
extern void FUN_406610(void* dest, SC_Message* src);

/* Function start: 0x406120 */
Handler8::Handler8() {
    ShowMessage("Handler8::Init");
    int* ptr = &handlerId;

    // Zero 6 dwords at offset 0x88-0x9C using memset for rep stosd pattern
    memset(ptr, 0, 6 * sizeof(int));

    // Set handler-specific fields at 0xa0 and 0xa4
    message = 0;
    field_A4 = 0;

    // Set handlerId at 0x88
    *ptr = 8;
}

/* Function start: 0x4061E0 */
Handler8::~Handler8() {
    if (message != 0) {
        delete message;
        message = 0;
    }
}

/* Function start: 0x406260 */
void Handler8::Init(SC_Message* msg) {
    char filename[32];
    ShowMessage("Handler8::Init");

    CopyCommandData(msg);
    if (msg != 0) {
        field_8C = msg->data;
        sprintf(filename, "cine\\cine%4.4d.smk", field_8C);
        ShowMessage(filename);
        if (FileExists(filename)) {
            Animation anim;
            anim.Play(filename, 0);
        } else {
            WriteToMessageLog("missing cinematic %s", filename);
        }
        if (msg->userPtr != 0) {
            Handler8::message = (SC_Message*)msg->userPtr;
            msg->userPtr = 0;
        }
        ProcessMessage();
    }
}

/* Function start: 0x406370 */
int Handler8::Exit(SC_Message* msg) {
    WriteMessageAddress(msg);
    if (msg->field_b4 == 0x1b || msg->mouseY > 1) {
        msg->command = Handler8::field_90;
        msg->priority = 5;
        msg->data = Handler8::field_94;
    }
    return 1;
}

/* Function start: 0x4063C0 */
void Handler8::Update(SC_Message* msg) {
    int cmd = msg->command;
    if (handlerId != cmd) {
        return;
    }
    int prio = msg->priority;
    if (prio != 0) {
        if (prio != 6) {
            return;
        }
        ProcessMessage();
    }
}

/* Function start: 0x406400 */
void Handler8::ProcessMessage() {
    SC_Message* msg;
    MessageQueue* queue;
    MessageQueueItem** tailPtr;
    int tail;
    MessageQueueItem* item;
    int i;
    char* poolResult;
    MessageQueueItem** freeListPtr;

    msg = Handler8::message;
    if (msg != 0) {
        queue = g_MessageQueue;
        freeListPtr = &queue->freeList;
        tailPtr = &queue->tail;
        tail = (int)queue->tail;

        if (*freeListPtr == 0) {
            poolResult = (char*)FUN_4065e0(&queue->poolBase, queue->poolCapacity, 0xc8);
            i = queue->poolCapacity;
            i--;
            item = (MessageQueueItem*)(poolResult + i * 0xc8 - 0xc4);
            while (i >= 0) {
                item->next = *freeListPtr;
                *freeListPtr = item;
                item = (MessageQueueItem*)((char*)item - 0xc8);
                i--;
            }
        }

        item = *freeListPtr;
        *freeListPtr = item->next;
        item->tailRef = tail;
        item->next = 0;
        queue->count++;

        FUN_406670(&item->data, 1);
        FUN_406610(&item->data, msg);

        item->field_90 = msg->command;
        item->field_94 = msg->data;
        item->field_98 = msg->targetAddress;
        item->field_9c = msg->sourceAddress;
        item->field_a0 = msg->priority;
        item->field_a4 = msg->param1;
        item->field_a8 = msg->param2;
        item->field_ac = msg->clickX;
        item->field_b0 = msg->clickY;
        item->field_b4 = msg->mouseX;
        item->field_b8 = msg->mouseY;
        item->field_bc = msg->field_b4;
        item->field_c0 = msg->field_b8;
        item->field_c4 = msg->userPtr;

        if (*tailPtr != 0) {
            (*tailPtr)->next = item;
        } else {
            queue->head = item;
        }
        *tailPtr = item;

        msg = Handler8::message;
        if (msg != 0) {
            delete msg;
            Handler8::message = 0;
        }
        return;
    }
    SC_Message_Send(handlerId, field_8C, field_90, field_94, 5, 0, 0, 0, 0, 0);
}
