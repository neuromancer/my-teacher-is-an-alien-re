#include "Handler8.h"
#include "SC_Question.h"
#include "Animation.h"
#include "Parser.h"
#include "string.h"
#include "Memory.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>

extern "C" int FileExists(const char* filename);
extern void SC_Message_Send(int, int, int, int, int, int, int, int, int, int);

// Forward declarations
void* __stdcall ExpandPool(void** pool, int capacity, int itemSize);
void InitMessageArray(int* param_1, int param_2);

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

/* Function start: 0x406120 */
Handler8::Handler8() {
    int* ptr = &handlerId;
    int* pA0 = (int*)&message;

    // Set handler-specific fields at 0xa0 and 0xa4
    pA0[0] = 0;
    pA0[1] = 0;

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

    CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = msg->data;
        sprintf(filename, "cine\\cine%4.4d.smk", moduleParam);
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

/* Function start: 0x406350 */
int Handler8::ShutDown(SC_Message* msg) {
    return 0;
}

/* Function start: 0x406360 */
void Handler8::Update(int param1, int param2) {
}

/* Function start: 0x406370 */
int Handler8::AddMessage(SC_Message* msg) {
    Handler8::WriteMessageAddress(msg);
    if (msg->lastKey == 0x1b || msg->mouseY > 1) {
        msg->targetAddress = savedCommand;
        msg->priority = 5;
        msg->sourceAddress = savedMsgData;
    }
    return 1;
}

/* Function start: 0x4063C0 */
int Handler8::Exit(SC_Message* msg) {
    int prio;

    if (handlerId != msg->targetAddress) {
        return 0;
    }
    prio = msg->priority;
    if (prio != 0) {
        if (prio != 6) {
            return 0;
        }
        ProcessMessage();
    }
    return 1;
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
            poolResult = (char*)ExpandPool(&queue->poolBase, queue->poolCapacity, 0xc8);
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

        InitMessageArray((int*)&item->data, 1);
        ((Parser*)&item->data)->CopyParserFields(msg);

        item->targetAddress = msg->targetAddress;
        item->sourceAddress = msg->sourceAddress;
        item->command = msg->command;
        item->msgData = msg->data;
        item->priority = msg->priority;
        item->param1 = msg->param1;
        item->param2 = msg->param2;
        item->clickX = msg->clickPos.x;
        item->clickY = msg->clickPos.y;
        item->mouseX = msg->mouseX;
        item->mouseY = msg->mouseY;
        item->field_bc = msg->lastKey;
        item->field_c0 = msg->time;
        item->userPtr = msg->userPtr;

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
    SC_Message_Send(savedCommand, savedMsgData, handlerId, moduleParam, 5, 0, 0, 0, 0, 0);
}

/* Function start: 0x4065E0 */
void* __stdcall ExpandPool(void** pool, int capacity, int itemSize)
{
    void** block;

    block = (void**)new char[capacity * itemSize + 4];
    *block = *pool;
    *pool = block;
    return block;
}

/* Function start: 0x406610 */
Parser* Parser::CopyParserFields(Parser* src)
{
    unsigned int i;
    int* dstPtr;
    int* srcPtr;
    int tmp1, tmp2;

    Parser::m_subObject = src->m_subObject;
    Parser::isProcessingKey = src->isProcessingKey;
    i = 0;
    do {
        Parser::currentKey[i] = src->currentKey[i];
        i++;
    } while (i < sizeof(currentKey));
    Parser::lineNumber = src->lineNumber;
    dstPtr = &savedFilePos;
    srcPtr = &src->savedFilePos;
    tmp1 = srcPtr[0];
    tmp2 = srcPtr[1];
    dstPtr[0] = tmp1;
    dstPtr[1] = tmp2;
    i = 0;
    do {
        Parser::filename[i] = src->filename[i];
        i++;
    } while (i < sizeof(filename));
    Parser::pFile = src->pFile;
    return this;
}

/* Function start: 0x406670 */
void InitMessageArray(int* param_1, int param_2)
{
    int* ptr;
    int count;

    ptr = param_1;
    memset(ptr, 0, param_2 * sizeof(SC_Message));

    count = param_2;
    param_2 = count - 1;
    if (count != 0) {
        do {
            if (param_1 != 0) {
                ((SC_Message*)param_1)->SC_Message::SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            }
            param_1 = param_1 + sizeof(SC_Message) / sizeof(int);
            count = param_2;
            param_2--;
        } while (count != 0);
    }
}
