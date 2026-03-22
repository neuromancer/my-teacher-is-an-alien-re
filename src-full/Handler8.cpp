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

/* DEMO ONLY - ExpandPool does not exist as standalone function in full game */
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
    for (i = 0; i < sizeof(currentKey); i++) {
        Parser::currentKey[i] = src->currentKey[i];
    }
    Parser::lineNumber = src->lineNumber;
    dstPtr = &savedFilePos;
    srcPtr = &src->savedFilePos;
    tmp1 = srcPtr[0];
    tmp2 = srcPtr[1];
    dstPtr[0] = tmp1;
    dstPtr[1] = tmp2;
    for (i = 0; i < sizeof(filename); i++) {
        Parser::filename[i] = src->filename[i];
    }
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
