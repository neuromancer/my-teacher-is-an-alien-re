#include "GameLoop.h"
#include "ZBufferManager.h"
#include "ZBuffer.h"
#include "VBuffer.h"
#include "Timer.h"
#include "GameState.h"
#include "InputManager.h"
#include "Memory.h"
#include "SC_Question.h"
#include "ScriptHandler.h"
#include "Sample.h"
#include "TimedEvent.h"
#include "globals.h"
#include "Message.h"
#include "string.h"
#include "DrawEntry.h"
#include "Handler12.h"
#include "Handler1.h"
#include "Handler2.h"
#include "Handler4.h"
#include "Handler5.h"
#include "Handler6.h"
#include "Handler8.h"
#include "Handler10.h"
#include "Handler11.h"
#include "Handler.h"
#include <smack.h>
#include <stdio.h>
#include <string.h>

// Helper functions for CleanupLoop
// FUN_00417660 is now VBuffer::Destroy (C++ member function)
// FUN_004189a0 is now ZBQueueNode::Cleanup (C++ member function)


// Thiscall functions - declared outside extern "C"
// FUN_00411080 is DrawEntry::Cleanup, defined in DrawEntry.cpp

// Base handler class for vtable calls
class BaseHandler {
public:
    virtual void Destructor(int flag);      // +0x00
    virtual void Method04();                // +0x04
    virtual void Method08();                // +0x08
    virtual void Delete(int flag);          // +0x0c
    virtual void Method10();                // +0x10
    virtual void Method14();                // +0x14
    virtual void Cleanup(int flag);         // +0x18
};

int* CreateHandler(int command); // 0x40CDD0 - Handler factory

// Doubly linked list node structure for eventList
// field_0x00: next pointer
// field_0x04: prev pointer
// field_0x08: data pointer (to object with vtable)
struct EventNode {
    EventNode* next;    // 0x00
    EventNode* prev;    // 0x04
    void* data;         // 0x08 - pointer to object with vtable
    
    EventNode(void* d) {
        next = 0;
        prev = 0;
        data = d;
    }
    
    ~EventNode() {
        data = 0;
        next = 0;
        prev = 0;
    }
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
    
    currentHandler = 0;
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
    g_ZBufferManager_0043698c->ProcessRenderQueues();
    
    elapsedTime = ((Timer*)timer1)->Update();
    if (elapsedTime < (unsigned int)field_0x08) {
        do {
            SmackSoundCheck();
            elapsedTime = ((Timer*)timer1)->Update();
        } while (elapsedTime < (unsigned int)field_0x08);
    }
    
    if (g_GameState_00436998 == (GameState*)zero) {
        goto skip_debug;
    }
    pGameState = g_GameState_00436998;
    if (pGameState->maxStates < 5) {
        ShowError("GameState Error  #%d", 1);
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
    g_ZBufferManager_0043698c->ShowSubtitle(g_Buffer_00436960, 0x14, 0x1e, 10000, 8);
    
skip_debug:
    ((Timer*)timer1)->Reset();
    g_ZBufferManager_0043698c->UpdateScreen();
    
    if (field_0x00 == zero) {
        goto loop_start;
    }

exit_loop:
    CleanupLoop();
}

/* Function start: 0x4177B0 */
void GameLoop::ProcessInput() {
    int inputResult;
    InputState** ppMouse;
    InputState* pMouse;
    int keyCode;
    int mouseX;
    int mouseY;
    int clickX;
    int clickY;
    int mouseButton;
    TimedEvent* pEvent;
    TimedEventPool* pPool;
    
    inputResult = g_InputManager_00436968->PollEvents(1);
    field_0x00 = field_0x00 | inputResult;
    if (field_0x00 != 0) {
        return;
    }
    
    pMouse = g_InputManager_00436968->pMouse;
    if (pMouse == 0) {
        return;
    }
    
    if (pMouse->ext1 >= 2 || pMouse->ext2 >= 2 || DAT_004373bc != 0) {
        // SC_Message on stack - triggers SEH
        SC_Message localMessage(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        
        localMessage.command = 3;
        
        if (DAT_004373bc != 0) {
            keyCode = WaitForInput();
        }
        
        // Get pointer to pMouse slot - matches ADD ECX, 0x1a0 pattern
        ppMouse = &g_InputManager_00436968->pMouse;
        
        pMouse = *ppMouse;
        if (pMouse == 0) {
            mouseX = 0;
        } else {
            mouseX = pMouse->ext1;
        }
        localMessage.mouseX = mouseX;
        
        pMouse = *ppMouse;
        if (pMouse == 0) {
            mouseY = 0;
        } else {
            mouseY = pMouse->ext2;
        }
        localMessage.mouseY = mouseY;
        
        pMouse = *ppMouse;
        clickY = 0;
        if (pMouse != 0) {
            clickY = pMouse->y;
        }
        if (pMouse != 0) {
            clickX = pMouse->x;
        } else {
            clickX = 0;
        }
        localMessage.clickX = clickX;
        localMessage.clickY = clickY;
        
        pMouse = *ppMouse;
        if (pMouse == 0) {
            mouseButton = 0;
        } else {
            if (pMouse->ext1 >= 2 || pMouse->ext2 >= 2) {
                mouseButton = 1;
            } else {
                mouseButton = 0;
            }
        }
        
        if (mouseButton != 0) {
            Sample* pSample = *(Sample**)((char*)g_Mouse_00436978 + 0x1bc);
            if (pSample != 0) {
                pSample->Play(100, 1);
            }
        }
        
        if (keyCode == 0x5a) {
            field_0x08 = field_0x08 + 10;
        }
        else if (keyCode == 0x41) {
            unsigned int newVal = field_0x08 - 10;
            if (newVal <= 10) {
                newVal = 10;
            }
            field_0x08 = newVal;
        }
        else {
            Handler* pHandler = currentHandler;
            if (pHandler != 0) {
                pHandler->HandleMessage(&localMessage);
            }
        }
        
        if (localMessage.targetAddress != 0 && localMessage.priority != 0) {
            pPool = g_TimedEventPool2_00436988;
            pEvent = pPool->Create((void*)pPool->list.tail, 0);
            ((TimedEvent*)((char*)pEvent + 8))->CopyFrom((TimedEvent*)&localMessage);
            if (pPool->list.tail == 0) {
                pPool->list.head = (TimedEvent*)pEvent;
            } else {
                *(TimedEvent**)pPool->list.tail = (TimedEvent*)pEvent;
            }
            pPool->list.tail = (TimedEvent*)pEvent;
        }
    }
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
                    // Call virtual Delete(1) at vtable offset 0xC
                    if (pData != 0) {
                        ((BaseHandler*)pData)->Delete(1);
                    }
                } while (pEventList->head != 0);
            }
        }
        FreeMemory(pEventList);
        eventList = 0;
    }
    
    currentHandler = 0;
}

/* Function start: 0x417ED0 */
void GameLoop::DrawFrame() {
    EventList* pList;
    EventNode* pNode;
    Handler* pHandler;
    
    pList = eventList;
    pList->current = pList->head;
    pList = eventList;
    if (pList->current == 0) {
        return;
    }
    
    do {
        pList = eventList;
        pNode = pList->current;
        pHandler = (Handler*)pNode->data;
        if (pHandler == 0) {
            return;
        }
        pHandler->Draw(0, currentHandler->handlerId);
        pList = eventList;
        pNode = pList->current;
        if (pList->tail == pNode) {
            return;
        }
        if (pNode != 0) {
            pList->current = pNode->prev;
        }
        pList = eventList;
    } while (pList->current != 0);
}

/* Function start: 0x417450 */
void GameLoop::CleanupLoop() {
    return;
    ZBufferManager* pZBuf;
    ZBQueue* pQueue;
    ZBQueueNode* pNode;
    ZBQueueNode* pNextNode;
    void* pResult;
    void* pData;
    EventList* pEventList;
    EventNode* pCurrent;
    Handler* pHandler;

    pZBuf = g_ZBufferManager_0043698c;

    // Process queue at offset 0xa0 - simple delete
    pQueue = pZBuf->m_queueA0;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            pResult = ZBuffer::PopNode((int*)pQueue);
            delete pResult;
        }
    }

    // Process queue at offset 0xa4 - cleanup with destructors
    pQueue = pZBuf->m_queueA4;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            pResult = ZBuffer::PopNode_2((int*)pQueue);
            if (pResult != 0) {
                pNode = (ZBQueueNode*)pResult;
                if (pNode->prev != 0) {
                    ((VBuffer*)pNode->prev)->Destroy(1);
                    pNode->prev = 0;
                }
                if (pNode->data != 0) {
                    // TODO: Fix vtable call - needs __thiscall with ECX=pNode->data
                    // (*(void (**)(int))(*(int*)pNode->data))(1);
                    pNode->data = 0;
                }
                delete pResult;
            }
        }
    }

    // Process queue at offset 0x9c - unlink and delete nodes
    pQueue = pZBuf->m_queue9c;
    if (pQueue->head != 0) {
        pQueue->current = pQueue->head;
        while (pQueue->head != 0) {
            pNode = pQueue->current;
            if (pNode == 0) {
                pData = 0;
            } else {
                // Unlink from head
                if (pQueue->head == pNode) {
                    pQueue->head = pNode->prev;
                }
                // Unlink from tail
                if (pQueue->tail == pNode) {
                    pQueue->tail = pNode->next;
                }
                // Update next->prev
                pNextNode = pNode->next;
                if (pNextNode != 0) {
                    pNextNode->prev = pNode->prev;
                }
                // Update prev->next
                if (pNode->prev != 0) {
                    pNode->prev->next = pNode->next;
                }
                // Get data and cleanup node
                pData = pQueue->GetCurrentData();
                if (pQueue->current != 0) {
                    pQueue->current->Cleanup(1);
                    pQueue->current = 0;
                }
                pQueue->current = pQueue->head;
            }
            delete pData;
        }
    }

    // Process eventList - call Update(0) on each handler
    pEventList = (EventList*)eventList;
    pEventList->current = pEventList->head;
    if (pEventList->current == 0) {
        return;
    }

    do {
        pCurrent = pEventList->current;
        pHandler = (Handler*)pCurrent->data;
        if (pHandler == 0) {
            break;
        }
        pHandler->Update(0);
        pCurrent = pEventList->current;
        if (pEventList->tail == pCurrent) {
            break;
        }
        if (pCurrent != 0) {
            pEventList->current = pCurrent->prev;
        }
    } while (pEventList->current != 0);
}

// External functions for UpdateGame
// FUN_004191d0 is WriteToMessageLog from string.h
// FUN_0041a150 is SC_Message_Send from Message.h

// extern void* DAT_004369a4;  // GameState for string lookup (replaced with g_GameState2_004369a4 from globals.h)
// g_Unknown_00436994 is declared in globals.h

/* Function start: 0x417CB0 */
void GameLoop::ProcessMessage(SC_Message* msg)
{
    int result;
    EventList* pList;
    EventNode* pNode;
    unsigned int pData;
    int zero;
    
    if (msg->priority == 5) {
        result = 1;
        if (msg->command == 3) {
            char* srcStr = (char*)g_GameState2_004369a4->stateValues[msg->data];
            strcpy(g_Unknown_00436994, srcStr);
        }
        else {
            HandleSystemMessage(msg);
        }
    }
    else if (msg->command == 0) {
        result = 1;
    }
    else if (msg->command == 3) {
        result = ProcessControlMessage(msg);
    }
    else {
        result = currentHandler->Exit(msg);
        if (result == 0) {
            pList = (EventList*)eventList;
            pList->current = pList->head;
            pNode = ((EventList*)eventList)->current;
            if (pNode != 0) {
                zero = 0;
                do {
                    pNode = ((EventList*)eventList)->current;
                    pData = (unsigned int)pNode->data;
                    if (pData == 0) break;
                    result = (*(int (**)(SC_Message*))(*(int*)((((int)pNode < 1) - 1) & pData) + 0x20))(msg);
                    if (result != zero) break;
                    pList = (EventList*)eventList;
                    pNode = pList->current;
                    if (pList->tail == pNode) break;
                    if (pNode != 0) {
                        pList->current = pNode->prev;
                    }
                    pNode = ((EventList*)eventList)->current;
                } while (pNode != (EventNode*)zero);
            }
        }
    }

    if (result == 0) {
        Handler* pDefaultHandler = (Handler*)GetOrCreateHandler(msg->command);
        if (pDefaultHandler->Exit(msg) == 0) {
            msg->Dump(0);
            WriteToMessageLog("lost message");
            SC_Message_Send(0xf, 2, 3, 0, 0x13, 0, 0, 0, 0, 0);
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
    
    // First loop: pop items from g_TimedEventPool2_00436988, copy to local_d8, create in g_TimedEventPool1_00436984
    while (g_TimedEventPool2_00436988->m_count != 0) {
        pSourceMsg = g_TimedEventPool2_00436988->Pop(&local_198);
        
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
        
        // Create entry in g_TimedEventPool1_00436984
        pPool = g_TimedEventPool1_00436984;
        pNewEvent = pPool->Create((void*)pPool->list.tail, 0);
        ((TimedEvent*)((int*)pNewEvent + 2))->CopyFrom((TimedEvent*)&local_d8);
        
        if (pPool->list.tail == 0) {
            pPool->list.head = pNewEvent;
        } else {
            *(TimedEvent**)pPool->list.tail = pNewEvent;
        }
        pPool->list.tail = pNewEvent;
    }
    
    // Second loop: pop items from g_TimedEventPool1_00436984
    while (g_TimedEventPool1_00436984->m_count != 0) {
        pSourceMsg = g_TimedEventPool1_00436984->Pop(&local_198);
        
        ProcessMessage(pSourceMsg);
        
        // Destruct buffer contents after ProcessMessage (matches original SEH thunk 0x417c22)
        local_198.SC_Message::~SC_Message();
        
        // Inner loop: pop items from g_TimedEventPool2_00436988 and add to g_TimedEventPool1_00436984
        while (g_TimedEventPool2_00436988->m_count != 0) {
            SC_Message* pInnerMsg = g_TimedEventPool2_00436988->Pop(&local_258);
            pPool = g_TimedEventPool1_00436984;
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

static void __cdecl StubDestructor(int) {}
static void __cdecl StubMethods() {}
static void __stdcall StubDraw(int, int) {}
static void __fastcall StubHandlerMethod(void* _this, SC_Message* msg) {}

static int StubHandleMessage(SC_Message* msg) {
    return 1; // Mark handled
}

static void* g_HandlerVTable[] = { 
    (void*)StubDestructor,    // 0x00
    (void*)StubMethods,       // 0x04
    (void*)StubMethods,       // 0x08
    (void*)StubDestructor,    // 0x0c
    (void*)StubMethods,       // 0x10
    (void*)StubHandlerMethod, // 0x14
    (void*)StubDestructor,    // 0x18
    (void*)StubDraw,          // 0x1c
    (void*)StubHandleMessage  // 0x20
};
static void* g_StubObject = g_HandlerVTable;

// FUN_0041a150 is SC_Message_Send, implemented in Message.cpp

/* Function start: 0x417F40 */
void GameLoop::HandleSystemMessage(SC_Message* msg) {
    Handler* handler;
    Handler* pData;
    EventList* pList;
    EventNode* pNode;
    ZBQueue* pQueue;
    void* pPopResult;

    if (msg == 0) {
        return;
    }

    // Call current handler's Update method
    handler = currentHandler;
    if (handler != 0) {
        handler->Update(msg);
    }
    
    // Clear ZBufferManager queues if g_ZBufferManager_0043698c exists
    if (g_ZBufferManager_0043698c != 0) {
        ZBufferManager* pZBuf = g_ZBufferManager_0043698c;
        
        // Process queue at offset 0xa0
        pQueue = pZBuf->m_queueA0;
        if (*(int*)pQueue != 0) {
            ((int*)pQueue)[2] = *(int*)pQueue;
            while (*(int*)pQueue != 0) {
                pPopResult = (void*)ZBuffer::PopNode((int*)pQueue);
                if (pPopResult != 0) {
                    FreeMemory(pPopResult);
                }
            }
        }
        
        // Process queue at offset 0xa4
        pQueue = pZBuf->m_queueA4;
        if (*(int*)pQueue != 0) {
            ((int*)pQueue)[2] = *(int*)pQueue;
            while (*(int*)pQueue != 0) {
                pPopResult = (void*)ZBuffer::PopNode_2((int*)pQueue);
                if (pPopResult != 0) {
                    ((DrawEntry*)pPopResult)->Cleanup(1);
                }
            }
        }
        
        // Process queue at offset 0x9c
        ZBuffer::ClearList((int*)pZBuf->m_queue9c);
    }
    
    // Try to find existing handler for this command using msg->command
    int found = FindHandlerInEventList(msg->command);
    if (found == 0) {
        // Not found - create/insert new handler
        handler = (Handler*)GetOrCreateHandler(msg->command);
        currentHandler = handler;
    } else {
        // Found - pop handler from eventList at field_0x14
        pList = (EventList*)eventList;
        pNode = pList->current;
        if (pNode == 0) {
            currentHandler = 0;
        } else {
            // Unlink node from list
            if (pList->head == pNode) {
                pList->head = pNode->prev;
            }
            if (pList->tail == pNode) {
                pList->tail = pNode->next;
            }
            if (pNode->next != 0) {
                pNode->next->prev = pNode->prev;
            }
            if (pNode->prev != 0) {
                pNode->prev->next = pNode->next;
            }
            pData = 0;
            if (pNode != 0) {
                pData = (Handler*)pNode->data;
                delete pNode;
                pList->current = 0;
            }
            pList->current = pList->head;
            currentHandler = pData;
        }
        
        // Reinsert handler with priority-based insertion
        handler = currentHandler;
        pList = (EventList*)eventList;
        if (handler == 0) {
            ShowError("queue fault 0101");
        }
        pList->current = pList->head;
        if (pList->field_0x0C == 1 || pList->field_0x0C == 2) {
            if (pList->head == 0) {
                pList->InsertNode(handler);
            } else {
                // Priority-based insertion loop
                while (pList->current != 0) {
                    pNode = pList->current;
                    pData = (Handler*)pNode->data;
                    if (pData->handlerId < handler->handlerId) {
                        pList->InsertNode(handler);
                        break;
                    }
                    if (pList->tail == pNode) {
                        // Append at end
                        if (handler == 0) {
                            ShowError("queue fault 0112");
                        }
                        EventNode* newNode = new EventNode(handler);
                        if (pList->current == 0) {
                            pList->current = pList->tail;
                        }
                        if (pList->head == 0) {
                            pList->head = newNode;
                            pList->tail = newNode;
                            pList->current = newNode;
                        } else {
                            if (pList->tail == 0 || pList->tail->prev != 0) {
                                ShowError("queue fault 0113");
                            }
                            newNode->prev = 0;
                            newNode->next = pList->tail;
                            pList->tail->prev = newNode;
                            pList->tail = newNode;
                        }
                        break;
                    }
                    if (pNode != 0) {
                        pList->current = pNode->prev;
                    }
                }
            }
        } else {
            pList->InsertNode(handler);
        }
    }
    
    // Call handler's Init method
    handler = currentHandler;
    if (handler == 0) {
        ShowError("missing modual %d", msg->command);
    } else {
        handler->Init(msg);
    }
}

/* Function start: 0x417E20 */
int GameLoop::ProcessControlMessage(SC_Message* msg) {
    if (msg->command != 3) {
        return 0;
    }
    switch(msg->priority) {
    case 6:
        field_0x00 = 1;
        return 1;
    case 0x12:
        field_0x08 = msg->sourceAddress;
        return 1;
    case 0x13:
        GetOrCreateHandler(msg->sourceAddress);
        return 1;
    case 0x14:
        RemoveHandler(msg->sourceAddress);
        return 1;
    default:
        return 0;
    }
}

// -------------------------------------------------------------------------
// Implementations of missing functions
// -------------------------------------------------------------------------

// Handler classes with correct sizes for new operator
// Handler classes with inline constructors using stub vtable
// Handler1 is defined in Handler1.h
// Handler2 is defined in Handler2.h
// Handler4 is defined in Handler4.h
// Handler5 is defined in Handler5.h
// Handler6 is defined in Handler6.h
// Handler8 is defined in Handler8.h
class Handler9 { public: Handler9() { *(void**)data = g_HandlerVTable; } char data[0x650]; };  // 0x406fc0
// Handler10 is defined in Handler10.h
// Handler11 is defined in Handler11.h
// Handler12 is defined in Handler12.h
class Handler13 { public: Handler13() { *(void**)data = g_HandlerVTable; } char data[0xd0]; };  // 0x401b60
class Handler14 { public: Handler14() { *(void**)data = g_HandlerVTable; } char data[0xb8]; };  // 0x40b7e0
class Handler15 { public: Handler15() { *(void**)data = g_HandlerVTable; } char data[0xb8]; };  // 0x40a2e0
class Handler16 { public: Handler16() { *(void**)data = g_HandlerVTable; } char data[0xf8]; };  // 0x410650

/* Function start: 0x40CDD0 */
int* CreateHandler(int command) {
    char buffer[128];
    int* handler = 0;
    
    switch(command) {
    case 1:
        handler = (int*)new Handler1();
        break;
    case 2:
        handler = (int*)new Handler2();
        break;
    case 4:
        handler = (int*)new Handler4();
        break;
    case 5:
        handler = (int*)new Handler5();
        break;
    case 6:
        handler = (int*)new Handler6();
        break;
    case 8:
        handler = (int*)new Handler8();
        break;
    case 9:
        handler = (int*)new Handler9();
        break;
    case 10:
        handler = (int*)new Handler10();
        break;
    case 11:
        handler = (int*)new Handler11();
        break;
    case 12:
        handler = (int*)new Handler12();
        break;
    case 13:
        handler = (int*)new Handler13();
        break;
    case 14:
        handler = (int*)new Handler14();
        break;
    case 15:
        handler = (int*)new Handler15();
        break;
    case 16:
        handler = (int*)new Handler16();
        break;
    default:
        ShowError("Unknown modual %d", command);
        handler = 0;
        break;
    }
    return handler;
}

// Base handler class for vtable calls - moved to top


/* Function start: 0x418460 */
int GameLoop::RemoveHandler(int command) {
    EventList* list;
    EventNode* current;
    BaseHandler* pHandler;
    EventNode* pNext;
    EventNode* pPrev;
    
    // Find the handler in eventList
    if (FindHandlerInEventList(command) == 0) {
        return 0;
    }
    
    // Get eventList and current node
    list = (EventList*)eventList;
    current = list->current;
    
    if (current == 0) {
        pHandler = 0;
    } else {
        // Unlink node from doubly-linked list
        if (list->head == current) {
            list->head = current->prev;
        }
        current = list->current;
        if (list->tail == current) {
            list->tail = current->next;
        }
        current = list->current;
        pNext = current->next;
        if (pNext != 0) {
            pNext->prev = current->prev;
        }
        current = list->current;
        pPrev = current->prev;
        if (pPrev != 0) {
            pPrev->next = current->next;
        }
        
        // Get data from node - matches LAB_004184b7
        current = list->current;
        pHandler = 0;
        if (current != 0) {
            pHandler = (BaseHandler*)current->data;
        }
        // Free node - matches LAB_004184c6
        if (current != 0) {
            current->data = 0;
            current->next = 0;
            current->prev = 0;
            FreeMemory(current);
            list->current = 0;
        }
        list->current = list->head;
    }
    
    // Call handler's Cleanup method with param 0 (vtable offset 0x18)
    pHandler->Cleanup(0);
    
    // Call handler's Delete method with param 1 (vtable offset 0x0c)
    if (pHandler != 0) {
        pHandler->Delete(1);
    }
    
    return 1;
}

/* Function start: 0x4188D0 */
void EventList::InsertNode(void* data) {
    if (data == 0) {
        ShowError("queue fault 0102");
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

/* Function start: 0x418540 */
int GameLoop::FindHandlerInEventList(int command) {
    EventList* list;
    EventNode* node;
    void* data;
    
    list = (EventList*)GameLoop::eventList;
    if (list == 0) {
        return 0;
    }
    list->current = list->head;
    if (list->head == 0) {
        return 0;
    }
    do {
        list = (EventList*)GameLoop::eventList;
        node = list->current;
        if (node != 0) {
            data = node->data;
            if (command == *(int*)((char*)data + 0x88)) {
                break;
            }
        } else {
            if (command == *(int*)0x88) {
                break;
            }
        }
        if (list->tail == node) {
            return 0;
        }
        if (node != 0) {
            list->current = node->prev;
        }
        list = (EventList*)GameLoop::eventList;
    } while (list->head != 0);
    
    node = ((EventList*)GameLoop::eventList)->current;
    if (node == 0) {
        return 0;
    }
    return (int)node->data;
}

/* Function start: 0x418200 */
int GameLoop::AddHandler(void* handler) {
    EventList* list;
    EventNode* current;
    void* data;
    EventNode* newNode;
    EventNode* nodePtr;
    
    // Check handler not null
    if (handler == 0) {
        ShowError("illegal modual insertion");
    }
    
    // Get eventList
    list = (EventList*)eventList;
    
    // Check list not null (uses handler as check, matches disasm)
    if (handler == 0) {
        ShowError("queue fault 0103");
    }
    
    // Set current to head for duplicate check
    list->current = list->head;
    
    // First loop: check for duplicates
    while (list->current != 0) {
        current = list->current;
        data = 0;
        if (current != 0) {
            data = current->data;
        }
        // Compare handler IDs at offset 0x88
        if (*(int*)((char*)data + 0x88) == *(int*)((char*)handler + 0x88)) {
            ShowError("illegal modual insertion double");
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
        ShowError("queue fault 0101");
    }
    list->current = list->head;
    
    // Check list type
    if (list->field_0x0C == 1 || list->field_0x0C == 2) {
        // Priority-based insertion for type 1 or 2
        if (list->head == 0) {
            list->InsertNode(handler);
        } else {
            // Priority insertion loop
            while (list->current != 0) {
                current = list->current;
                data = current->data;
                // Compare priority at offset 0x88
                if (*(int*)((char*)data + 0x88) < *(int*)((char*)handler + 0x88)) {
                    // Insert before current
                    if (handler == 0) {
                        ShowError("queue fault 0102");
                    }
                    newNode = new EventNode(handler);
                    nodePtr = newNode;
                    // Insert node
                    if (list->current == 0) {
                        list->current = list->head;
                    }
                    if (list->head == 0) {
                        list->head = nodePtr;
                        list->tail = nodePtr;
                        list->current = nodePtr;
                    } else {
                        nodePtr->prev = list->current;
                        nodePtr->next = list->current->next;
                        if (list->current->next == 0) {
                            list->head = nodePtr;
                            list->current->next = nodePtr;
                        } else {
                            list->current->next->prev = nodePtr;
                            list->current->next = nodePtr;
                        }
                    }
                    break;
                }
                // Check if at tail
                if (list->tail == current) {
                    // Append at end
                    if (handler == 0) {
                        ShowError("queue fault 0112");
                    }
                    newNode = new EventNode(handler);
                    handler = newNode;
                    // Insert at tail
                    if (list->current == 0) {
                        list->current = list->tail;
                    }
                    if (list->head == 0) {
                        list->head = (EventNode*)handler;
                        list->tail = (EventNode*)handler;
                        list->current = (EventNode*)handler;
                    } else {
                        if (list->tail == 0 || list->tail->prev != 0) {
                            ShowError("queue fault 0113");
                        }
                        ((EventNode*)handler)->prev = 0;
                        ((EventNode*)handler)->next = list->tail;
                        list->tail->prev = (EventNode*)handler;
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


/* Function start: 0x418510 */
int* GameLoop::GetOrCreateHandler(int command) {
    int* handler = CreateHandler(command);
    if (handler != 0) {
        AddHandler(handler);
    }
    return handler;
}

// FUN_00411080 is DrawEntry::Cleanup, implemented in DrawEntry.cpp



