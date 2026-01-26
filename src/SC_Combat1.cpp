#include "SC_Combat1.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"
#include "SC_Question.h"
#include "Parser.h"
#include "ZBuffer.h"
#include "SoundCommand.h"
#include "ZBufferManager.h"
#include "InputManager.h"
#include "VBuffer.h"
#include "DrawEntry.h"
#include "Engine.h"
#include "EngineA.h"
#include "EngineB.h"

// Forward declarations for queue management (from Handler8.cpp)
struct MessageQueueItem;
struct MessageQueue;
extern MessageQueue* g_MessageQueue;
extern void* __stdcall ExpandPool(void** pool, int capacity, int itemSize);
extern void InitMessageArray(int* param_1, int param_2);

// Already implemented elsewhere:
// - DrawEntry::Cleanup(int) at 0x411080 in Engine.cpp
extern "C" void InitWorkBuffer(int width, int height);  // 0x41A8C0 in VBuffer.cpp


/* Function start: 0x410610 */
int SC_Combat1::Exit(SC_Message* msg) {
    memset(&DAT_0043d130, 0, 16);
    return 0;
}

/* Function start: 0x410650 */
SC_Combat1::SC_Combat1() {
    int* pA0;
    int* pA8;
    int i;

    // Get pointers to 0xa0 and 0xa8 fields
    pA0 = &savedScreenX;
    pA8 = &screenWidth;

    // Manually set fields at 0xa0, 0xa4, 0xa8, 0xac to 0
    *pA0 = 0;
    pA0[1] = 0;
    *pA8 = 0;
    pA8[1] = 0;

    // Zero 22 dwords from 0xA0 onwards
    for (i = 0x16; i != 0; i--) {
        *pA0 = 0;
        pA0++;
    }

    // Set field_98 to 0 (isInitialized flag)
    field_98 = 0;

    // Set default screen dimensions
    *pA8 = 0x140;   // 320
    // handlerId is set to 16 at offset 0x88
    handlerId = 16;
    screenHeight = 0xc8;   // 200
}

/* Function start: 0x410760 */
SC_Combat1::~SC_Combat1() {
    Parser* pParser;

    pParser = scriptParser;
    if (pParser != 0) {
        delete (SC_Message*)pParser;
        scriptParser = 0;
    }
}

/* Function start: 0x410810 */
void SC_Combat1::Init(SC_Message* msg) {
    ZBQueue* pQueue;
    void* pNode;
    int zState;
    ZBufferManager* pZBuf;
    InputManager* pInput;

    if (field_98 != 0) {
        return;
    }

    WriteToMessageLog("ENTER COMBAT1");
    CopyCommandData(msg);

    pZBuf = g_ZBufferManager_0043698c;
    zState = pZBuf->m_state;
    pZBuf->m_state = 1;

    // Clear queue at 0xa0
    pQueue = pZBuf->m_queueA0;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            pNode = ZBuffer::PopNode(pQueue);
            if (pNode != 0) {
                delete (SoundCommand*)pNode;
            }
        }
    }

    // Clear queue at 0xa4
    pQueue = pZBuf->m_queueA4;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            pNode = ZBuffer::PopNode_2(pQueue);
            if (pNode != 0) {
                ((DrawEntry*)pNode)->Cleanup(1);
            }
        }
    }

    // Clear queue at 0x9c
    ZBuffer::ClearList(pZBuf->m_queue9c);

    savedRendererState = zState;
    if (msg != 0) {
        field_8C = msg->data;
    }

    // Save screen position and initialize combat buffer
    {
        VBuffer* pVB = g_WorkBuffer_00436974;
        int h = pVB->height;
        int w = pVB->width;
        int sw = screenWidth;
        savedScreenX = w;
        savedScreenY = h;
        InitWorkBuffer(sw, screenHeight);
    }

    // Save and set clipping rect
    {
        GlyphRect* pBounds;
        int b, r, t, l;

        pInput = g_InputManager_00436968;
        b = pInput->bounds.bottom;
        r = pInput->bounds.right;
        t = pInput->bounds.top;
        l = pInput->bounds.left;
        pBounds = &pInput->bounds;

        DAT_0043d130 = l;
        DAT_0043d134 = t;
        DAT_0043d138 = r;
        DAT_0043d13c = b;

        pBounds->left = 0;
        pBounds->top = 0;
        pBounds->right = screenWidth - 1;
        pBounds->bottom = screenHeight - 1;
    }

    // Copy filename and parse
    strcpy(filename, "mis\\COMBAT1.mis");
    ParseFile(this, filename, 0);

    field_98 = 1;
}

/* Function start: 0x4109B0 */
int SC_Combat1::ShutDown(SC_Message* msg) {
    ZBQueue* pQueue;
    void* puVar7;
    void* pVVar5;
    ZBufferManager* pZBuf;
    InputManager* pInput;

    if (field_98 == 0) {
        return 0;
    }

    field_98 = 0;

    // Handle combat object cleanup
    if (g_CombatEngine != 0) {
        g_CombatEngine->StopAndCleanup();
        if (g_CombatEngine != 0) {
            delete g_CombatEngine;
            g_CombatEngine = 0;
        }
    }

    // Restore work buffer to original size
    InitWorkBuffer(savedScreenX, savedScreenY);

    // Restore clipping rect
    {
        GlyphRect* pBounds;
        int b, r, t, l;

        pInput = g_InputManager_00436968;
        pBounds = &pInput->bounds;
        b = DAT_0043d13c;
        r = DAT_0043d138;
        t = DAT_0043d134;
        l = DAT_0043d130;

        pBounds->left = l;
        pBounds->top = t;
        pBounds->right = r;
        pBounds->bottom = b;
    }

    // Restore ZBuffer state
    pZBuf = g_ZBufferManager_0043698c;
    pZBuf->m_state = savedRendererState;

    // Clear queue at 0xa0
    pQueue = pZBuf->m_queueA0;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            puVar7 = ZBuffer::PopNode(pQueue);
            if (puVar7 != 0) {
                delete (SoundCommand*)puVar7;
            }
        }
    }

    // Clear queue at 0xa4
    pQueue = pZBuf->m_queueA4;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            pVVar5 = ZBuffer::PopNode_2(pQueue);
            if (pVVar5 != 0) {
                delete (ZBuffer*)pVVar5;
            }
        }
    }

    // Clear queue at 0x9c
    pQueue = pZBuf->m_queue9c;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            pVVar5 = pQueue->PopNode();
            if (pVVar5 != 0) {
                delete (ZBuffer*)pVVar5;
            }
        }
    }

    WriteToMessageLog("EXIT COMBAT1");

    return 0;
}

int SC_Combat1::AddMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    WriteToMessageLog("SC_Combat1");
    return 1;
}

/* Function start: 0x410B00 */
void SC_Combat1::Update(int param1, int param2) {
    int result;

    if (field_98 == 0) {
        return;
    }
    if (handlerId != param2) {
        return;
    }

    // Check if combat engine is active
    result = g_CombatEngine->UpdateAndCheck();
    if (result != 0) {
        ProcessMessage();
    }
}

// MessageQueueItem structure (size 0xC8 = 200 bytes)
struct MessageQueueItem {
    MessageQueueItem* next;  // 0x00
    int tailRef;             // 0x04
    char data[0x88];         // 0x08 - Parser fields copied here
    int field_90;            // 0x90 - command
    int field_94;            // 0x94 - sourceAddress
    int field_98;            // 0x98 - targetAddress
    int field_9c;            // 0x9c - data
    int field_a0;            // 0xa0 - priority
    int field_a4;            // 0xa4 - param1
    int field_a8;            // 0xa8 - param2
    int field_ac;            // 0xac - clickX
    int field_b0;            // 0xb0 - clickY
    int field_b4;            // 0xb4 - mouseX
    int field_b8;            // 0xb8 - mouseY
    int field_bc;            // 0xbc - field_b4
    int field_c0;            // 0xc0 - field_b8
    int field_c4;            // 0xc4 - userPtr
};

// MessageQueue structure
struct MessageQueue {
    MessageQueueItem* head;     // 0x00
    MessageQueueItem* tail;     // 0x04
    int count;                  // 0x08
    MessageQueueItem* freeList; // 0x0c
    void* poolBase;             // 0x10
    int poolCapacity;           // 0x14
};

/* Function start: 0x410CA0 */
void SC_Combat1::ProcessMessage()
{
    SC_Message* script;
    MessageQueue* queue;
    MessageQueueItem** tailPtr;
    int tail;
    MessageQueueItem* item;
    int i;
    char* poolResult;
    MessageQueueItem** freeListPtr;

    script = (SC_Message*)SC_Combat1::scriptParser;
    if (script != 0) {
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
        ((Parser*)&item->data)->CopyParserFields(script);

        // Copy SC_Message fields in memory order
        item->field_90 = script->targetAddress;
        item->field_94 = script->sourceAddress;
        item->field_98 = script->command;
        item->field_9c = script->data;
        item->field_a0 = script->priority;
        item->field_a4 = script->param1;
        item->field_a8 = script->param2;
        item->field_ac = script->clickX;
        item->field_b0 = script->clickY;
        item->field_b4 = script->mouseX;
        item->field_b8 = script->mouseY;
        item->field_bc = script->field_b4;
        item->field_c0 = script->field_b8;
        item->field_c4 = script->userPtr;

        if (*tailPtr != 0) {
            (*tailPtr)->next = item;
        } else {
            queue->head = item;
        }
        *tailPtr = item;

        script = (SC_Message*)SC_Combat1::scriptParser;
        if (script != 0) {
            delete script;
            SC_Combat1::scriptParser = 0;
        }
        return;
    }
    SC_Message_Send(SC_Combat1::field_90, SC_Combat1::field_94, SC_Combat1::handlerId, SC_Combat1::field_8C, 5, 0, 0, 0, 0, 0);
}

/* Function start: 0x410E80 */
int SC_Combat1::LBLParse(char* line) {
    char label[32];
    char moduleType[32];

    if (sscanf(line, "%s", label) != 1) {
        return 0;
    }

    if (strcmp(label, "MODULE") == 0) {
        if (sscanf(line, "%s %s", label, moduleType) == 2) {
            if (strcmp(moduleType, "A") == 0) {
                g_CombatEngine = new EngineB();
            } else if (strcmp(moduleType, "B") == 0) {
                g_CombatEngine = new EngineA();
            } else {
                Parser::LBLParse("SC_Combat1");
            }
        }
        Parser::ProcessFile(g_CombatEngine, this, 0);
        g_CombatEngine->SetupViewport();
        return 0;
    }

    if (strcmp(label, "END") == 0) {
        return 1;
    }

    Parser::LBLParse("SC_Combat1");
    return 0;
}
