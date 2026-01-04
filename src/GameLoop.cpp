#include "GameLoop.h"
#include "ZBufferManager.h"
#include "ZBuffer.h"
#include "Timer.h"
#include "GameState.h"
#include "InputManager.h"
#include "Memory.h"
#include "SC_Question.h"
#include "Sample.h"
#include "TimedEvent.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    void __cdecl SmackSoundCheck();
}

typedef void (__cdecl *SmackSoundCheckFn)();
extern SmackSoundCheckFn g_SmackSoundCheck;

// Wait for key
int WaitForInput();
extern "C" void ShowError(const char* format, ...);

// Helper functions for CleanupLoop
extern "C" {
    void* __fastcall FUN_00417680(void* queue);  // Queue::GetCurrentData
    void __fastcall FUN_00417660(void* obj, int freeFlag);  // Object destructor/cleanup
    void __fastcall FUN_004189a0(void* node, int freeFlag); // Node cleanup
    //void __fastcall FUN_00417652();               // SEH unwind thunk (empty stub)
}

// Doubly linked list node structure for eventList
// field_0x00: next pointer
// field_0x04: prev pointer
// field_0x08: data pointer (to object with vtable)
struct EventNode {
    EventNode* next;    // 0x00
    EventNode* prev;    // 0x04
    void* data;         // 0x08 - pointer to object with vtable
};

// List header structure for eventList
// field_0x00: head pointer
// field_0x04: tail pointer  
// field_0x08: current pointer (iteration)
// field_0x0C: count or flags
struct EventList {
    EventNode* head;    // 0x00
    EventNode* tail;    // 0x04
    EventNode* current; // 0x08
    int field_0x0C;     // 0x0C
    
    EventList() {
        field_0x0C = 0;
        head = 0;
        tail = 0;
        current = head;
    }
};

/* Function start: 0x417200 */
GameLoop::GameLoop() {
    Timer* pResult;
    EventList* pList;
    
    memset(this, 0, 7 * sizeof(int));
    
    pResult = new Timer();
    this->timer1 = pResult;
    
    pResult = new Timer();
    this->timer2 = pResult;
    
    pList = new EventList();
    
    this->field_0x18 = 0;
    this->field_0x08 = 0x54;
    this->eventList = pList;
}

GameLoop::~GameLoop()
{
    Cleanup();
}

/* Function start: 0x417420 */
void GameLoop::ResetLoop() {
    Timer* pTimer;
    
    this->field_0x00 = 0;
    
    pTimer = (Timer*)this->timer2;
    if (pTimer != 0) {
        pTimer->Reset();
    }
    
    pTimer = (Timer*)this->timer1;
    if (pTimer != 0) {
        pTimer->Reset();
    }
}

/* Function start: 0x417690 */
void GameLoop::Run() {
    SmackSoundCheckFn pSmackSoundCheck;
    unsigned int elapsedTime;
    int mouseX;
    int mouseY;
    InputState* pMouse;
    GameState* pGameState;
    int zero;

    this->ResetLoop();
    
    if (this->field_0x00 != 0) {
        goto exit_loop;
    }

    pSmackSoundCheck = g_SmackSoundCheck;
    zero = 0;
    
loop_start:
    this->ProcessInput();
    if (this->field_0x00 != zero) {
        goto exit_loop;
    }
    
    this->UpdateGame();
    if (this->field_0x00 != zero) {
        goto exit_loop;
    }
    
    this->DrawFrame();
    DAT_0043698c->ProcessRenderQueues();
    
    elapsedTime = ((Timer*)this->timer1)->Update();
    if (elapsedTime < (unsigned int)this->field_0x08) {
        do {
            pSmackSoundCheck();
            elapsedTime = ((Timer*)this->timer1)->Update();
        } while (elapsedTime < (unsigned int)this->field_0x08);
    }
    
    if (g_GameState_00436998 == (GameState*)zero) {
        goto skip_debug;
    }
    pGameState = g_GameState_00436998;
    if (pGameState->maxStates < 5) {
        ShowError("\"GameState Error  #%d\"", 1);
    }
    if (*(int*)((char*)pGameState->stateValues + 0x10) == zero) {
        goto skip_debug;
    }
    mouseY = 0;
    pMouse = g_InputManager_00436968->pMouse;
    if (pMouse != 0) {
        mouseY = pMouse->y;
    }
    if (pMouse != 0) {
        mouseX = pMouse->x;
    } else {
        mouseX = 0;
    }
    elapsedTime = ((Timer*)this->timer1)->Update();
    sprintf(g_Buffer_00436960, "FT %d, [%d,%d]", elapsedTime, mouseX, mouseY);
    DAT_0043698c->ShowSubtitle(g_Buffer_00436960, 0x14, 0x1e, 10000, 8);
    
skip_debug:
    ((Timer*)this->timer1)->Reset();
    DAT_0043698c->UpdateScreen();
    
    if (this->field_0x00 == zero) {
        goto loop_start;
    }

exit_loop:
    this->CleanupLoop();
}

/* Function start: 0x4177B0 */
void GameLoop::ProcessInput() {
    int inputResult;
    int* pMouseCoords;
    int keyCode;
    int mouseX;
    int mouseY;
    int clickX;
    int clickY;
    int mouseButton;
    void* pHandler;
    TimedEvent* pEvent;
    TimedEventPool* pPool;
    char buffer[0xC0]; // Raw buffer for SC_Message
    SC_Message* pLocalMessage;
    
    inputResult = g_InputManager_00436968->PollEvents(1);
    this->field_0x00 = this->field_0x00 | inputResult;
    if (this->field_0x00 != 0) {
        return;
    }
    
    InputState* pMouse = g_InputManager_00436968->pMouse;
    if (pMouse == 0) {
        return;
    }
    
    if (pMouse->ext1 < 2 && pMouse->ext2 < 2) { // field_10 and field_14
        if (DAT_004373bc == 0) {
            return;
        }
    }
    
    try {
        pLocalMessage = (SC_Message*)buffer;
        pLocalMessage->SC_Message::SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    } catch (...) {
    }
    
    pLocalMessage->command = 3; // command = 3
    
    if (DAT_004373bc != 0) {
        keyCode = WaitForInput();
    }
    
    // pMouse was already read
    if (pMouse == 0) {
        mouseX = 0;
    } else {
        mouseX = pMouse->ext1; // field_10
    }
    pLocalMessage->mouseX = mouseX;
    
    if (pMouse == 0) {
        mouseY = 0;
    } else {
        mouseY = pMouse->ext2; // field_14
    }
    pLocalMessage->mouseY = mouseY;
    
    clickY = 0;
    if (pMouse != 0) {
        clickY = pMouse->y; // field_4
    }
    clickX = 0;
    if (pMouse != 0) {
        clickX = pMouse->x; // field_0
    }
    pLocalMessage->clickX = clickX;
    pLocalMessage->clickY = clickY;
    
    if (pMouse != 0) {
        if (pMouse->ext1 >= 2 || pMouse->ext2 >= 2) {
            mouseButton = 1;
        } else {
            mouseButton = 0;
        }
    } else {
        mouseButton = 0;
    }
    
    if (mouseButton != 0) {
        Sample* pSample = *(Sample**)((char*)g_Mouse_00436978 + 0x1bc);
        if (pSample != 0) {
            pSample->Play(100, 1);
        }
    }
    
    if (keyCode == 0x5a) {
        // Z key - increase frame time
        this->field_0x08 = this->field_0x08 + 10;
    }
    else if (keyCode == 0x41) {
        // A key - decrease frame time (min 10)
        int newVal = this->field_0x08 - 10;
        if ((unsigned int)newVal <= 10) {
            newVal = 10;
        }
        this->field_0x08 = newVal;
    }
    else {
        // Pass to handler
        pHandler = (void*)this->field_0x18;
        if (pHandler != 0) {
            (*(void (**)(void*, SC_Message*))(*(int*)pHandler + 0x14))(pHandler, pLocalMessage);
        }
    }
    
    if (pLocalMessage->targetAddress != 0 && pLocalMessage->priority != 0) {
        pPool = DAT_00436988;
        pEvent = pPool->Create((void*)pPool->list.tail, 0);
        ((TimedEvent*)((char*)pEvent + 8))->CopyFrom((TimedEvent*)pLocalMessage);
        if (pPool->list.tail == 0) {
            pPool->list.head = (TimedEvent*)pEvent;
        } else {
            *(TimedEvent**)pPool->list.tail = (TimedEvent*)pEvent;
        }
        pPool->list.tail = (TimedEvent*)pEvent;
    }
    
    pLocalMessage->SC_Message::~SC_Message();
}

/* Function start: 0x417320 */
void GameLoop::Cleanup() {
    void* pTimer;
    EventList* pEventList;
    EventNode* pNode;
    EventNode* pNext;
    EventNode* pPrev;
    void* pData;
    
    // Cleanup timer1
    pTimer = this->timer1;
    if (pTimer != 0) {
        Timer_DecrementCounter();
        FreeMemory(pTimer);
        this->timer1 = 0;
    }
    
    // Cleanup timer2
    pTimer = this->timer2;
    if (pTimer != 0) {
        Timer_DecrementCounter();
        FreeMemory(pTimer);
        this->timer2 = 0;
    }
    
    // Cleanup eventList
    pEventList = (EventList*)this->eventList;
    if (pEventList != 0) {
        if (pEventList->head != 0) {
            pEventList->current = pEventList->head;
            if (pEventList->head != 0) {
                do {
                    pNode = pEventList->current;
                    if (pNode == 0) {
                        pData = 0;
                    } else {
                        // Unlink node from list - head check
                        if (pEventList->head == pNode) {
                            pEventList->head = pNode->next;
                        }
                        // Tail check
                        pNode = pEventList->current;
                        if (pEventList->tail == pNode) {
                            pEventList->tail = pNode->next;
                        }
                        // Update next node's prev pointer
                        pNode = pEventList->current;
                        pNext = pNode->next;
                        if (pNext != 0) {
                            pNext->prev = pNode->prev;
                        }
                        // Update prev node's next pointer
                        pNode = pEventList->current;
                        pPrev = pNode->prev;
                        if (pPrev != 0) {
                            pPrev->next = pNode->next;
                        }
                        // Save data and free node
                        pNode = pEventList->current;
                        pData = 0;
                        if (pNode != 0) {
                            pData = pNode->data;
                            pNode->data = 0;
                            pNode->next = 0;
                            pNode->prev = 0;
                            FreeMemory(pNode);
                            pEventList->current = 0;
                        }
                        pEventList->current = pEventList->head;
                    }
                    // Call destructor through vtable if data exists
                    if (pData != 0) {
                        (*(void (__cdecl **)(int))(*((int*)pData) + 0xc))(1);
                    }
                } while (pEventList->head != 0);
            }
        }
        FreeMemory(pEventList);
        this->eventList = 0;
    }
    
    this->field_0x18 = 0;
}

/* Function start: 0x417ED0 */
void GameLoop::DrawFrame() {
    EventList* pList;
    EventNode* pNode;
    void* pData;
    
    pList = (EventList*)this->eventList;
    pList->current = pList->head;
    pList = (EventList*)this->eventList;
    if (pList->current == 0) {
        return;
    }
    
    do {
        pList = (EventList*)this->eventList;
        pNode = pList->current;
        pData = pNode->data;
        if (pData == 0) {
            return;
        }
        pData = (void*)((((int)pNode < 1) - 1) & (int)pData);
        (*(void (__stdcall **)(int, int))(*(int*)pData + 0x1c))(0, *(int*)((char*)this->field_0x18 + 0x88));
        pList = (EventList*)this->eventList;
        pNode = pList->current;
        if (pList->tail == pNode) {
            return;
        }
        if (pNode != 0) {
            pList->current = pNode->prev;
        }
        pList = (EventList*)this->eventList;
    } while (pList->current != 0);
}

/* Function start: 0x417450 */
void GameLoop::CleanupLoop() {
    int iVar2;
    int iVar3;
    unsigned int uVar4;
    int* puVar5;
    void* pvVar6;
    int* local_1c;
    ZBufferManager* pZBuf;
    Queue* pQueue;
    
    pZBuf = DAT_0043698c;
    
    // Process queue at offset 0xa0
    pQueue = (Queue*)pZBuf->m_queueA0;
    if (pQueue->m_head != 0) {
        pQueue->m_current = pQueue->m_head;
        iVar2 = (int)pQueue->m_head;
        while (iVar2 != 0) {
            puVar5 = (int*)ZBuffer::PopNode((int*)pQueue);
            delete puVar5;
            iVar2 = (int)pQueue->m_head;
        }
    }
    
    // Process queue at offset 0xa4
    pQueue = (Queue*)pZBuf->m_queueA4;
    if (pQueue->m_head != 0) {
        pQueue->m_current = pQueue->m_head;
        iVar2 = (int)pQueue->m_head;
        while (iVar2 != 0) {
            pvVar6 = ZBuffer::PopNode_2((int*)pQueue);
            if (pvVar6 != 0) {
                if (*(void**)((char*)pvVar6 + 4) != 0) {
                    FUN_00417660(*(void**)((char*)pvVar6 + 4), 1);
                    *(int*)((char*)pvVar6 + 4) = 0;
                }
                if (*(int**)((char*)pvVar6 + 8) != 0) {
                    // Call virtual destructor through vtable
                    (**(void (**)(int))(**(int**)((char*)pvVar6 + 8)))(1);
                    *(int*)((char*)pvVar6 + 8) = 0;
                }
                delete pvVar6;
            }
            iVar2 = (int)pQueue->m_head;
        }
    }
    
    // Process queue at offset 0x9c
    pQueue = (Queue*)pZBuf->m_queue9c;
    if (pQueue->m_head != 0) {
        pQueue->m_current = pQueue->m_head;
        iVar3 = (int)pQueue->m_head;
        while (iVar3 != 0) {
            iVar3 = (int)pQueue->m_current;
            if (iVar3 == 0) {
                local_1c = 0;
            } else {
                if (pQueue->m_head == pQueue->m_current) {
                    pQueue->m_head = (void*)*(int*)(iVar3 + 4);
                }
                if (pQueue->m_tail == pQueue->m_current) {
                    pQueue->m_tail = (void*)*(int*)pQueue->m_current;
                }
                iVar3 = *(int*)pQueue->m_current;
                if (iVar3 != 0) {
                    *(int*)(iVar3 + 4) = ((int*)pQueue->m_current)[1];
                }
                puVar5 = (int*)((int*)pQueue->m_current)[1];
                if (puVar5 != 0) {
                    *puVar5 = *(int*)pQueue->m_current;
                }
                local_1c = (int*)FUN_00417680(pQueue);
                if (pQueue->m_current != 0) {
                    FUN_004189a0(pQueue->m_current, 1);
                    pQueue->m_current = 0;
                }
                pQueue->m_current = pQueue->m_head;
            }
            //FUN_00417652();
            delete local_1c;
            iVar3 = (int)pQueue->m_head;
        }
    }
    
    // Process GameLoop's eventList at offset 0x14
    EventList* pEventList = (EventList*)this->eventList;
    pEventList->current = pEventList->head;
    iVar3 = (int)pEventList->current;
    while (iVar3 != 0) {
        iVar3 = (int)pEventList->current;
        uVar4 = (unsigned int)((EventNode*)iVar3)->data;
        if (uVar4 == 0) break;
        // Call method at vtable offset 0x18 with param 0
        int masked = ((iVar3 == 0) - 1) & uVar4;
        (*(void (**)(int))(*(int*)masked + 0x18))(0);
        iVar3 = (int)pEventList;
        iVar2 = (int)pEventList->current;
        if ((int)pEventList->tail == iVar2) break;
        if (iVar2 != 0) {
            pEventList->current = ((EventNode*)iVar2)->prev;
        }
        iVar3 = (int)pEventList->current;
    }
}

// External functions for UpdateGame
extern "C" {
    void* FUN_00417c50(void* pool, void* buffer);
    void FUN_00419fd0(SC_Message* msg, int unused);
    void FUN_004191d0(const char* msg);
    void FUN_0041a150(int, int, int, int, int, int, int, int, int, int);
}

// Forward declarations for message handling
extern "C" void GameLoop_HandleSystemMessage(GameLoop* self, SC_Message* msg);
extern "C" int GameLoop_ProcessControlMessage(GameLoop* self, SC_Message* msg);
extern "C" void* GameLoop_GetHandlerForCommand(GameLoop* self, int command);

// extern void* DAT_004369a4;  // GameState for string lookup (replaced with g_GameState2_004369a4 from globals.h)
extern char* g_Unknown_00436994; // String buffer for game state strings

/* Function start: 0x417CB0 */
void GameLoop::ProcessMessage(SC_Message* msg)
{
    int result;
    
    if (msg->priority == 5) {
        // System message with priority 5
        result = 1;
        if (msg->command == 3) {
            // Copy string from GameState to global buffer
            char* srcStr = (char*)g_GameState2_004369a4->stateValues[msg->data];
            strcpy(g_Unknown_00436994, srcStr);
        } else {
            GameLoop_HandleSystemMessage(this, msg);
        }
    }
    else if (msg->command == 0) {
        // Null command - mark as handled
        result = 1;
    }
    else if (msg->command == 3) {
        // Control message
        result = GameLoop_ProcessControlMessage(this, msg);
    }
    else {
        // Route to handler at field_0x18
        void* handler = (void*)this->field_0x18;
        result = (*(int (**)(SC_Message*))(*(int*)handler + 0x20))(msg);
        
        if (result == 0) {
            // Handler didn't process it, try event list
            EventList* pList = (EventList*)this->eventList;
            pList->current = pList->head;
            
            while (pList->current != 0) {
                EventNode* pNode = pList->current;
                void* pData = pNode->data;
                
                if (pData == 0) {
                    break;
                }
                
                pData = (void*)((((int)pNode < 1) - 1) & (int)pData);
                result = (*(int (**)(SC_Message*))(*(int*)pData + 0x20))(msg);
                
                if (result != 0) {
                    break;
                }
                
                pList = (EventList*)this->eventList;
                pNode = pList->current;
                if (pList->tail == pNode) {
                    break;
                }
                if (pNode != 0) {
                    pList->current = pNode->prev;
                }
            }
        }
    }
    
    if (result == 0) {
        // Message not handled - look up by command and try default handler
        void* defaultHandler = GameLoop_GetHandlerForCommand(this, msg->command);
        int handled = (*(int (**)(SC_Message*))(*(int*)defaultHandler + 0x20))(msg);
        
        if (handled == 0) {
            // Still not handled - dump message and show error
            FUN_00419fd0(msg, 0);
            FUN_004191d0("\"lost message\"");
            FUN_0041a150(0xf, 2, 3, 0, 0x13, 0, 0, 0, 0, 0);
        }
    }
}

/* Function start: 0x4179A0 */
int GameLoop::UpdateGame()
{
    SC_Message* pSourceMsg;
    TimedEventPool* pPool;
    TimedEvent* pNewEvent;
    unsigned int uVar5;
    unsigned int uVar6;
    char local_258[192];  // [EBP - 0x254] buffer for inner iterator result
    char local_198[192];  // [EBP - 0x194] buffer for outer iterator result
    SC_Message local_d8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // [EBP - 0xD4]
    void* local_18;       // [EBP - 0x18] - pointer storage
    int local_14;         // [EBP - 0x10] - counter

    local_14 = 0;
    
    // First loop: pop items from DAT_00436988, copy to local_d8, create in DAT_00436984
    while (DAT_00436988->m_count != 0) {
        pSourceMsg = (SC_Message*)FUN_00417c50(DAT_00436988, local_198);
        
        // Copy Parser base class fields
        local_d8.m_subObject = pSourceMsg->m_subObject;
        local_d8.isProcessingKey = pSourceMsg->isProcessingKey;
        
        uVar6 = 0;
        do {
            uVar5 = uVar6 + 1;
            local_d8.currentKey[uVar6] = pSourceMsg->currentKey[uVar6];
            uVar6 = uVar5;
        } while (uVar5 < 0x20);  // currentKey[32]
        
        local_d8.lineNumber = pSourceMsg->lineNumber;
        local_d8.savedFilePos = pSourceMsg->savedFilePos;
        local_d8.field_0x3c = pSourceMsg->field_0x3c;
        
        uVar6 = 0;
        do {
            uVar5 = uVar6 + 1;
            local_d8.filename[uVar6] = pSourceMsg->filename[uVar6];
            uVar6 = uVar5;
        } while (uVar5 < 0x40);  // filename[64]
        
        local_d8.pFile = pSourceMsg->pFile;
        
        // Copy SC_Message specific fields using named access
        local_d8.targetAddress = pSourceMsg->targetAddress;
        local_d8.sourceAddress = pSourceMsg->sourceAddress;
        local_d8.command = pSourceMsg->command;
        local_d8.data = pSourceMsg->data;
        local_d8.priority = pSourceMsg->priority;
        local_d8.param1 = pSourceMsg->param1;
        local_d8.param2 = pSourceMsg->param2;
        local_d8.clickX = pSourceMsg->clickX;
        local_d8.clickY = pSourceMsg->clickY;
        local_d8.mouseX = pSourceMsg->mouseX;
        local_d8.mouseY = pSourceMsg->mouseY;
        local_d8.field_b4 = pSourceMsg->field_b4;
        local_d8.field_b8 = pSourceMsg->field_b8;
        local_18 = (void*)pSourceMsg->userPtr;
        
        // Create entry in DAT_00436984
        pPool = DAT_00436984;
        pNewEvent = pPool->Create((void*)pPool->list.tail, 0);
        ((TimedEvent*)((int*)pNewEvent + 2))->CopyFrom((TimedEvent*)&local_d8);
        
        if (pPool->list.tail == 0) {
            pPool->list.head = pNewEvent;
        } else {
            *(TimedEvent**)pPool->list.tail = pNewEvent;
        }
        pPool->list.tail = pNewEvent;
    }
    
    // Second loop: pop items from DAT_00436984
    while (DAT_00436984->m_count != 0) {
        pSourceMsg = (SC_Message*)FUN_00417c50(DAT_00436984, local_198);
        
        this->ProcessMessage(pSourceMsg);
        
        // Inner loop: pop items from DAT_00436988 and add to DAT_00436984
        while (DAT_00436988->m_count != 0) {
            SC_Message* pInnerMsg = (SC_Message*)FUN_00417c50(DAT_00436988, local_258);
            pPool = DAT_00436984;
            pNewEvent = pPool->Create((void*)pPool->list.tail, 0);
            ((TimedEvent*)((int*)pNewEvent + 2))->CopyFrom((TimedEvent*)pInnerMsg);
            
            if (pPool->list.tail == 0) {
                pPool->list.head = pNewEvent;
            } else {
                *(TimedEvent**)pPool->list.tail = pNewEvent;
            }
            pPool->list.tail = pNewEvent;
        }
        
        local_14++;
    }
    
    local_d8.SC_Message::~SC_Message();
    return local_14;
}

// -------------------------------------------------------------------------
// Stubs for missing functions
// -------------------------------------------------------------------------

static int StubHandleMessage(SC_Message* msg) {
    ShowError("STUB: StubHandleMessage called (cmd=%d)", msg->command);
    return 1; // Mark handled
}

static void* g_StubVTable[] = { 
    0, 0, 0, 0, 0, 0, 0, 0, 
    (void*)StubHandleMessage // Offset 0x20
};
static void* g_StubObject = g_StubVTable;

extern "C" {
    void FUN_0041a150(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
        ShowError("STUB: FUN_0041a150 called");
    }

    void FUN_00419fd0(SC_Message* msg, int unused) {
         // This appears to be a message dump function
         // We can ignore it or print something
         // char buffer[256];
         // sprintf(buffer, "Msg Dump: cmd=%d", msg->command);
         // FUN_004191d0(buffer); // Log it
    }

    void GameLoop_HandleSystemMessage(GameLoop* self, SC_Message* msg) {
        ShowError("STUB: GameLoop_HandleSystemMessage called command=%d", msg->command);
    }

    int GameLoop_ProcessControlMessage(GameLoop* self, SC_Message* msg) {
        ShowError("STUB: GameLoop_ProcessControlMessage called");
        return 0; // Not handled? Or handled?
    }

    void* GameLoop_GetHandlerForCommand(GameLoop* self, int command) {
        ShowError("STUB: GameLoop_GetHandlerForCommand called cmd=%d", command);
        return &g_StubObject; // Return pointer to object (which is pointer to vtable)
    }
}

