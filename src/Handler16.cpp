#include "Handler16.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"
#include "SC_Question.h"
#include "Parser.h"
#include "ZBuffer.h"
#include "ZBufferManager.h"
#include "InputManager.h"
#include "VBuffer.h"
#include "DrawEntry.h"
#include "Engine.h"

// External functions
extern void __fastcall FUN_004199a0(void* parser);  // Parser destructor wrapper (0x4199A0)
extern void __fastcall FUN_00410ca0(void* handler);  // Handler16 internal draw (0x410CA0)

// Already implemented elsewhere:
// - DrawEntry::Cleanup(int) at 0x411080 in Engine.cpp
extern void InitWorkBuffer(int width, int height);  // 0x41A8C0 in VBuffer.cpp

// Global combat engine object (EngineB or Engine subclass)
extern Engine* g_CombatEngine;

// Vtable pointer at 0x431050
extern void* PTR_LAB_00431050;

/* Function start: 0x410650 */
Handler16::Handler16() {
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
Handler16::~Handler16() {
    Parser* pParser;

    pParser = scriptParser;
    if (pParser != 0) {
        FUN_004199a0(pParser);
        FreeMemory(pParser);
        scriptParser = 0;
    }
}

/* Function start: 0x410810 */
void Handler16::Init(SC_Message* msg) {
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
            pNode = ZBuffer::PopNode((int*)pQueue);
            if (pNode != 0) {
                *(void**)pNode = &PTR_LAB_00431050;
                FreeMemory(pNode);
            }
        }
    }

    // Clear queue at 0xa4
    pQueue = pZBuf->m_queueA4;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            pNode = ZBuffer::PopNode_2((int*)pQueue);
            if (pNode != 0) {
                ((DrawEntry*)pNode)->Cleanup(1);
            }
        }
    }

    // Clear queue at 0x9c
    ZBuffer::ClearList((int*)pZBuf->m_queue9c);

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
int Handler16::Update(SC_Message* msg) {
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
            puVar7 = ZBuffer::PopNode((int*)pQueue);
            if (puVar7 != 0) {
                *(void**)puVar7 = &PTR_LAB_00431050;
                FreeMemory(puVar7);
            }
        }
    }

    // Clear queue at 0xa4
    pQueue = pZBuf->m_queueA4;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            pVVar5 = ZBuffer::PopNode_2((int*)pQueue);
            if (pVVar5 != 0) {
                ((ZBuffer*)pVVar5)->CleanUpVBuffer();
                FreeMemory(pVVar5);
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

/* Function start: 0x40A7D0 */
int Handler16::HandleMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    WriteToMessageLog("SC_Combat1");
    return 1;
}

/* Function start: 0x410B00 */
void Handler16::Draw(int param1, int param2) {
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
        FUN_00410ca0(this);
    }
}

/* Function start: 0x410610 */
int Handler16::Exit(SC_Message* msg) {
    return 0;
}
