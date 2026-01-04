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


// Thiscall functions - declared outside extern "C"
void FUN_00411080(void* obj, int flag);  // Object destructor with cleanup
int FUN_00418540(GameLoop* self, int command);  // FindHandlerInEventList
int* FUN_00418510(GameLoop* self, int command); // GetOrCreateHandler
void* FUN_004188a0(void* node, int flag);  // Node destructor
int* FUN_004189d0(void* node, void* data);  // Node::Init

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
    
    void InsertNode(void* data);  // 0x4188d0
};

/* Function start: 0x417200 */
GameLoop::GameLoop() {
    Timer* pResult;
    EventList* pList;
    
    memset(this, 0, 7 * sizeof(int));
    
    pResult = new Timer();
    timer1 = pResult;
    
    pResult = new Timer();
    timer2 = pResult;
    
    pList = new EventList();
    
    field_0x18 = 0;
    field_0x08 = 0x54;
    eventList = pList;
}

GameLoop::~GameLoop()
{
    Cleanup();
}

/* Function start: 0x417420 */
void GameLoop::ResetLoop() {
    Timer* pTimer;
    
    field_0x00 = 0;
    
    pTimer = (Timer*)timer2;
    if (pTimer != 0) {
        pTimer->Reset();
    }
    
    pTimer = (Timer*)timer1;
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

    ResetLoop();
    
    if (field_0x00 != 0) {
        goto exit_loop;
    }

    pSmackSoundCheck = g_SmackSoundCheck;
    zero = 0;
    
loop_start:
    ProcessInput();
    if (field_0x00 != zero) {
        goto exit_loop;
    }
    
    UpdateGame();
    if (field_0x00 != zero) {
        goto exit_loop;
    }
    
    DrawFrame();
    DAT_0043698c->ProcessRenderQueues();
    
    elapsedTime = ((Timer*)timer1)->Update();
    if (elapsedTime < (unsigned int)field_0x08) {
        do {
            pSmackSoundCheck();
            elapsedTime = ((Timer*)timer1)->Update();
        } while (elapsedTime < (unsigned int)field_0x08);
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
    elapsedTime = ((Timer*)timer1)->Update();
    sprintf(g_Buffer_00436960, "FT %d, [%d,%d]", elapsedTime, mouseX, mouseY);
    DAT_0043698c->ShowSubtitle(g_Buffer_00436960, 0x14, 0x1e, 10000, 8);
    
skip_debug:
    ((Timer*)timer1)->Reset();
    DAT_0043698c->UpdateScreen();
    
    if (field_0x00 == zero) {
        goto loop_start;
    }

exit_loop:
    CleanupLoop();
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
    field_0x00 = field_0x00 | inputResult;
    if (field_0x00 != 0) {
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
        field_0x08 = field_0x08 + 10;
    }
    else if (keyCode == 0x41) {
        // A key - decrease frame time (min 10)
        int newVal = field_0x08 - 10;
        if ((unsigned int)newVal <= 10) {
            newVal = 10;
        }
        field_0x08 = newVal;
    }
    else {
        // Pass to handler
        pHandler = (void*)field_0x18;
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
    pTimer = timer1;
    if (pTimer != 0) {
        Timer_DecrementCounter();
        FreeMemory(pTimer);
        timer1 = 0;
    }
    
    // Cleanup timer2
    pTimer = timer2;
    if (pTimer != 0) {
        Timer_DecrementCounter();
        FreeMemory(pTimer);
        timer2 = 0;
    }
    
    // Cleanup eventList
    pEventList = (EventList*)eventList;
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
        eventList = 0;
    }
    
    field_0x18 = 0;
}

/* Function start: 0x417ED0 */
void GameLoop::DrawFrame() {
    EventList* pList;
    EventNode* pNode;
    void* pData;
    
    pList = (EventList*)eventList;
    pList->current = pList->head;
    pList = (EventList*)eventList;
    if (pList->current == 0) {
        return;
    }
    
    do {
        pList = (EventList*)eventList;
        pNode = pList->current;
        pData = pNode->data;
        if (pData == 0) {
            return;
        }
        pData = (void*)((((int)pNode < 1) - 1) & (int)pData);
        (*(void (__stdcall **)(int, int))(*(int*)pData + 0x1c))(0, *(int*)((char*)field_0x18 + 0x88));
        pList = (EventList*)eventList;
        pNode = pList->current;
        if (pList->tail == pNode) {
            return;
        }
        if (pNode != 0) {
            pList->current = pNode->prev;
        }
        pList = (EventList*)eventList;
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
    EventList* pEventList = (EventList*)eventList;
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
    void FUN_004191d0(const char* msg);
    void FUN_0041a150(int, int, int, int, int, int, int, int, int, int);
}

// Forward declarations for message handling
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
            HandleSystemMessage(msg);
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
        void* handler = (void*)field_0x18;
        result = (*(int (**)(SC_Message*))(*(int*)handler + 0x20))(msg);
        
        if (result == 0) {
            // Handler didn't process it, try event list
            EventList* pList = (EventList*)eventList;
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
                
                pList = (EventList*)eventList;
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
            msg->Dump(0);
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
    SC_Message local_258(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);  // [EBP - 0x254] buffer for inner iterator result
    SC_Message local_198(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);  // [EBP - 0x194] buffer for outer iterator result
    SC_Message local_d8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // [EBP - 0xD4]
    int local_14;         // [EBP - 0x10] - counter
    void* local_18;       // [EBP - 0x18] - pointer storage

    local_14 = 0;
    
    // First loop: pop items from DAT_00436988, copy to local_d8, create in DAT_00436984
    while (DAT_00436988->m_count != 0) {
        pSourceMsg = DAT_00436988->Pop(&local_198);
        
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
        
        // Destruct buffer contents after copy (matches original SEH thunk)
        local_198.SC_Message::~SC_Message();
        
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
        pSourceMsg = DAT_00436984->Pop(&local_198);
        
        ProcessMessage(pSourceMsg);
        
        // Destruct buffer contents after ProcessMessage (matches original SEH thunk 0x417c22)
        local_198.SC_Message::~SC_Message();
        
        // Inner loop: pop items from DAT_00436988 and add to DAT_00436984
        while (DAT_00436988->m_count != 0) {
            SC_Message* pInnerMsg = DAT_00436988->Pop(&local_258);
            pPool = DAT_00436984;
            pNewEvent = pPool->Create((void*)pPool->list.tail, 0);
            ((TimedEvent*)((int*)pNewEvent + 2))->CopyFrom((TimedEvent*)pInnerMsg);
            
            if (pPool->list.tail == 0) {
                pPool->list.head = pNewEvent;
            } else {
                *(TimedEvent**)pPool->list.tail = pNewEvent;
            }
            pPool->list.tail = pNewEvent;
            
            // Destruct inner buffer contents (matches original SEH thunk 0x417c17)
            local_258.SC_Message::~SC_Message();
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
}

/* Function start: 0x417F40 */
void GameLoop::HandleSystemMessage(SC_Message* msg) {
    int* piVar5;
    void* puVar2;
    void* pvVar3;
    void* handler;
    ZBQueue* pQueue;
    
    if (msg == 0) {
        return;
    }
    
    // Call current handler's vtable method at +0x18 (Update method) with the message
    handler = (void*)field_0x18;
    if (handler != 0) {
        (*(void (**)(SC_Message*))(*(int*)handler + 0x18))(msg);
    }
    
    // Clear ZBufferManager queues if DAT_0043698c exists
    if (DAT_0043698c != 0) {
        ZBufferManager* pZBuf = DAT_0043698c;
        
        // Process queue at offset 0xa0
        pQueue = pZBuf->m_queueA0;
        if (*(int*)pQueue != 0) {
            ((int*)pQueue)[2] = *(int*)pQueue;
            while (*(int*)pQueue != 0) {
                puVar2 = (void*)ZBuffer::PopNode((int*)pQueue);
                if (puVar2 != 0) {
                    *(int*)puVar2 = 0x431050;  // PTR_LAB_00431050 vtable
                    FreeMemory(puVar2);
                }
            }
        }
        
        // Process queue at offset 0xa4
        pQueue = pZBuf->m_queueA4;
        if (*(int*)pQueue != 0) {
            ((int*)pQueue)[2] = *(int*)pQueue;
            while (*(int*)pQueue != 0) {
                pvVar3 = (void*)ZBuffer::PopNode_2((int*)pQueue);
                if (pvVar3 != 0) {
                    FUN_00411080(pvVar3, 1);
                }
            }
        }
        
        // Process queue at offset 0x9c
        ZBuffer::ClearList((int*)pZBuf->m_queue9c);
    }
    
    // Try to find existing handler for this command
    int found = FUN_00418540(this, *(int*)((char*)msg + 0x88));
    if (found == 0) {
        // Not found - create/insert new handler
        piVar5 = FUN_00418510(this, *(int*)((char*)msg + 0x88));
        field_0x18 = (int)piVar5;
    } else {
        // Found - pop handler from eventList at field_0x14
        EventList* pList = (EventList*)eventList;
        int iVar4 = (int)pList->current;
        if (iVar4 == 0) {
            field_0x18 = 0;
        } else {
            // Unlink node from list
            if ((int)pList->head == iVar4) {
                pList->head = (EventNode*)*(int*)(iVar4 + 4);
            }
            if ((int)pList->tail == iVar4) {
                pList->tail = (EventNode*)*(int*)iVar4;
            }
            int iNext = *(int*)iVar4;
            if (iNext != 0) {
                *(int*)(iNext + 4) = *(int*)(iVar4 + 4);
            }
            void* pPrev = (void*)*(int*)(iVar4 + 4);
            if (pPrev != 0) {
                *(int*)pPrev = *(int*)iVar4;
            }
            void* pData = 0;
            if ((void*)iVar4 != 0) {
                pData = (void*)*(int*)(iVar4 + 8);
                FUN_004188a0((void*)iVar4, 1);
                pList->current = 0;
            }
            pList->current = pList->head;
            field_0x18 = (int)pData;
        }
        
        // Reinsert handler with priority-based insertion
        handler = (void*)field_0x18;
        EventList* pList2 = (EventList*)eventList;
        if (handler == 0) {
            ShowError("\"queue fault 0101\"");
        }
        pList2->current = pList2->head;
        if (pList2->field_0x0C == 1 || pList2->field_0x0C == 2) {
            if ((int)pList2->head == 0) {
                pList2->InsertNode(handler);
            } else {
                // Priority-based insertion loop
                while ((int)pList2->current != 0) {
                    int iCur = (int)pList2->current;
                    int curData = *(int*)(iCur + 8);
                    if (curData != 0 && *(int*)(curData + 0x88) < *(int*)((int)handler + 0x88)) {
                        pList2->InsertNode(handler);
                        break;
                    }
                    if ((int)pList2->tail == iCur) {
                        // Append at end
                        if (handler == 0) {
                            ShowError("\"queue fault 0112\"");
                        }
                        void* pvNode = (void*)AllocateMemory(0xc);
                        int* piNode = 0;
                        if (pvNode != 0) {
                            piNode = FUN_004189d0(pvNode, handler);
                        }
                        if ((int)pList2->current == 0) {
                            pList2->current = pList2->tail;
                        }
                        if ((int)pList2->head == 0) {
                            pList2->head = (EventNode*)piNode;
                            pList2->tail = (EventNode*)piNode;
                            pList2->current = (EventNode*)piNode;
                        } else {
                            if ((int)pList2->tail == 0 || *(int*)((int)pList2->tail + 4) != 0) {
                                ShowError("\"queue fault 0113\"");
                            }
                            piNode[1] = 0;
                            *piNode = (int)pList2->tail;
                            *(int*)((int)pList2->tail + 4) = (int)piNode;
                            pList2->tail = (EventNode*)piNode;
                        }
                        break;
                    }
                    if (iCur != 0) {
                        pList2->current = (EventNode*)*(int*)(iCur + 4);
                    }
                }
            }
        } else {
            pList2->InsertNode(handler);
        }
    }
    
    // Call handler's vtable method at +0x10 (Init method) with the message
    handler = (void*)field_0x18;
    if (handler == 0) {
        ShowError("\"missing modual %d\"", *(int*)((char*)msg + 0x88));
    } else {
        (*(void (**)(SC_Message*))(*(int*)handler + 0x10))(msg);
    }
}

extern "C" {


    int GameLoop_ProcessControlMessage(GameLoop* self, SC_Message* msg) {
        ShowError("STUB: GameLoop_ProcessControlMessage called");
        return 0; // Not handled? Or handled?
    }

    void* GameLoop_GetHandlerForCommand(GameLoop* self, int command) {
        // ShowError("STUB: GameLoop_GetHandlerForCommand called cmd=%d", command);
        return FUN_00418510(self, command);
    }
}

// -------------------------------------------------------------------------
// Implementations of missing functions
// -------------------------------------------------------------------------

void FUN_0041aa10(int param_1) {
    // Stub for recursive cleanup
}

int* FUN_0040cdd0(int command) {
    // Factory stub
    // This should create a handler based on the command.
    return 0; 
}

void* FUN_004188a0(void* node, int flag) {
    *(int*)((char*)node + 8) = 0;
    *(int*)node = 0;
    *(int*)((char*)node + 4) = 0;
    if ((flag & 1) != 0) {
        FreeMemory(node);
    }
    return node;
}

int* FUN_004189d0(void* node, void* data) {
    *(int*)node = 0;
    *(int*)((char*)node + 4) = 0;
    *(int*)((char*)node + 8) = (int)data;
    return (int*)node;
}

/* Function start: 0x4188D0 */
void EventList::InsertNode(void* data) {
    if (data == 0) {
        ShowError("\"queue fault 0102\"");
    }
    EventNode* node = (EventNode*)AllocateMemory(0xc);
    EventNode* newNode = 0;
    if (node != 0) {
        newNode = node;
        node->data = data;
        node->next = 0;
        node->prev = 0;
    }
    
    if (current == 0) {
        current = head;
    }
    if (head == 0) {
        head = newNode;
        tail = newNode;
        current = newNode;
    } else {
        newNode->prev = current;
        newNode->next = current->next;
        if (current->next == 0) {
            head = newNode;
            current->next = newNode;
        } else {
            current->next->prev = newNode;
            current->next = newNode;
        }
    }
}

int FUN_00418540(GameLoop* self, int command) {
    if (self->eventList == 0) {
        return 0;
    }
    EventList* list = (EventList*)self->eventList;
    list->current = list->head;
    EventNode* node = list->head;
    
    while (1) {
        if (node == 0) return 0;
        
        // iVar2 = *(int *)((int)this + 0x14); -> list
        // iVar3 = *(int *)(iVar2 + 8); -> current (which is node now)
        // iVar4 = *(int *)(*(int *)(iVar3 + 8) + 0x88); -> current->data->priority (at 0x88)
        
        void* data = node->data;
        if (data != 0) {
             // Check command at offset 0x88 (implied by usage in HandleSystemMessage msg->command is not 0x88, but msg structure has command. 
             // Here we are comparing against 'param_1' which is 'command'.
             // In HandleSystemMessage: FUN_00418540(this, *(int*)((char*)msg + 0x88)) 
             // msg+0x88 is likely command? No, msg has 0x88 size?
             // Let's assume passed param is the command ID.
             
             int handlerCmd = *(int*)((char*)data + 0x88); // 0x88 is distinct field
             if (command == handlerCmd) break;
        }
        
        // if (*(int *)(iVar2 + 4) == iVar3) return 0; -> if list->tail == node return 0
        if (list->tail == node) return 0;
        
        // if (iVar3 != 0) { *(undefined4 *)(iVar2 + 8) = *(undefined4 *)(iVar3 + 4); } -> current = current->prev
        // Original loop iterates backwards?
        // Decomp: *(undefined4 *)(iVar2 + 8) = *(undefined4 *)(iVar3 + 4);
        // iVar3 is current (node). +4 is prev.
        // So list->current = node->prev.
        
        if (node != 0) {
             list->current = node->prev;
             // Update local node variable
             node = node->prev;
        }
    }
    
    // Found it
    // iVar2 = *(int *)(*(int *)((int)this + 0x14) + 8); -> list->current
    // return *(undefined4 *)(iVar2 + 8); -> current->data
    if (list->current != 0) {
        return (int)list->current->data;
    }
    return 0;
}

/* Function start: 0x418200 */
int GameLoop::AddHandler(void* handler) {
    EventList* list;
    EventNode* current;
    void* data;
    void* newNode;
    void* nodePtr;
    
    // Check handler not null
    if (handler == 0) {
        ShowError("\"illegal modual insertion\"");
    }
    
    // Get eventList
    list = (EventList*)eventList;
    
    // Check list not null (uses handler as check, matches disasm)
    if (handler == 0) {
        ShowError("\"queue fault 0103\"");
    }
    
    // Set current to head for duplicate check
    list->current = list->head;
    
    // First loop: check for duplicates
    while ((int)list->current != 0) {
        current = list->current;
        data = 0;
        if (current != 0) {
            data = current->data;
        }
        // Compare handler IDs at offset 0x88
        if (*(int*)((char*)data + 0x88) == *(int*)((char*)handler + 0x88)) {
            ShowError("\"illegal modual insertion double\"");
            return 0;
        }
        // Check if at tail
        if (list->tail == current) {
            break;
        }
        // Move to next (via offset 4)
        if (current != 0) {
            list->current = current->prev;
        }
    }
    
    // Reset for insertion
    list = (EventList*)eventList;
    if (handler == 0) {
        ShowError("\"queue fault 0101\"");
    }
    list->current = list->head;
    
    // Check list type
    if (list->field_0x0C == 1 || list->field_0x0C == 2) {
        // Priority-based insertion for type 1 or 2
        if ((int)list->head == 0) {
            list->InsertNode(handler);
        } else {
            // Priority insertion loop
            while ((int)list->current != 0) {
                current = list->current;
                data = current->data;
                // Compare priority at offset 0x88
                if (*(int*)((char*)data + 0x88) < *(int*)((char*)handler + 0x88)) {
                    // Insert before current
                    if (handler == 0) {
                        ShowError("\"queue fault 0102\"");
                    }
                    newNode = (void*)AllocateMemory(0xc);
                    nodePtr = 0;
                    if (newNode != 0) {
                        nodePtr = newNode;
                        *(void**)((char*)newNode + 8) = handler;
                        *(int*)newNode = 0;
                        *(int*)((char*)newNode + 4) = 0;
                    }
                    // Insert node
                    if ((int)list->current == 0) {
                        list->current = list->head;
                    }
                    if ((int)list->head == 0) {
                        list->head = (EventNode*)nodePtr;
                        list->tail = (EventNode*)nodePtr;
                        list->current = (EventNode*)nodePtr;
                    } else {
                        *(EventNode**)((char*)nodePtr + 4) = list->current;
                        *(int*)nodePtr = *(int*)list->current;
                        if (*(int*)list->current == 0) {
                            list->head = (EventNode*)nodePtr;
                            *(void**)list->current = nodePtr;
                        } else {
                            *(void**)((char*)*(int*)list->current + 4) = nodePtr;
                            *(void**)list->current = nodePtr;
                        }
                    }
                    break;
                }
                // Check if at tail
                if (list->tail == current) {
                    // Append at end
                    if (handler == 0) {
                        ShowError("\"queue fault 0112\"");
                    }
                    newNode = (void*)AllocateMemory(0xc);
                    if (newNode != 0) {
                        *(void**)((char*)newNode + 8) = handler;
                        handler = newNode;
                        *(int*)newNode = 0;
                        *(int*)((char*)newNode + 4) = 0;
                    } else {
                        handler = 0;
                    }
                    // Insert at tail
                    if ((int)list->current == 0) {
                        list->current = list->tail;
                    }
                    if ((int)list->head == 0) {
                        list->head = (EventNode*)handler;
                        list->tail = (EventNode*)handler;
                        list->current = (EventNode*)handler;
                    } else {
                        if ((int)list->tail == 0 || *(int*)((char*)list->tail + 4) != 0) {
                            ShowError("\"queue fault 0113\"");
                        }
                        *(int*)((char*)handler + 4) = 0;
                        *(int*)handler = (int)list->tail;
                        *(int*)((char*)list->tail + 4) = (int)handler;
                        list->tail = (EventNode*)handler;
                    }
                    break;
                }
                // Move to next
                if (current != 0) {
                    list->current = current->prev;
                }
            }
        }
    } else {
        list->InsertNode(handler);
    }
    
    return 1;
}


int* FUN_00418510(GameLoop* self, int command) {
    int* handler = FUN_0040cdd0(command);
    if (handler != 0) {
        self->AddHandler(handler);
    }
    return handler;
}

void FUN_00411080(void* obj, int flag) {
    void* v1 = *(void**)((char*)obj + 4);
    if (v1 != 0) {
        FUN_0041aa10((int)v1);
        FreeMemory(v1);
        *(int*)((char*)obj + 4) = 0;
    }
    void* vtable = *(void**)((char*)obj + 8);
    if (vtable != 0) {
         (*(void (__cdecl **)(int))((int*)vtable)[0])(1); // Get destructor? 
         // Code: (**(code **)**(undefined4 **)((int)this + 8))(1);
         // `*(this+8)` is a pointer to object? Or vtable?
         // `**(this+8)` is vtable?
         // `(** ...)(1)` calls index 0 with 1.
         // Yes, virtual destructor.
         *(int*)((char*)obj + 8) = 0;
    }
    if ((flag & 1) != 0) {
        FreeMemory(obj);
    }
}


