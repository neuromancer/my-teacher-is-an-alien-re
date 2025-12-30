#include "GameLoop.h"
#include "Timer.h"
#include "GameState.h"
#include "JoystickManager.h"
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


// Forward declaration for the SC_Message destructor
extern "C" void __fastcall FUN_004199a0(void*);
// JoystickManager::CheckInput - takes this pointer and int param
extern "C" int FUN_00421d10(void*, int);
// Wait for key
int WaitForInput();
extern "C" void FUN_004179a0();              // UpdateGame (global)
extern "C" void __fastcall FUN_0041c5a0(void*);         // ZBufferManager method
extern "C" void __fastcall FUN_0041c960(void*);         // ZBufferManager method
extern "C" void __fastcall FUN_00417450(GameLoop*);     // CleanupLoop
extern "C" void ShowError(const char* format, ...);

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
};

/* Function start: 0x417200 */
GameLoop* GameLoop::Init() {
    Timer* pAlloc;
    Timer* pResult;
    EventList* pList;
    
    memset(this, 0, 7 * sizeof(int));
    
    try {
        pAlloc = (Timer*)AllocateMemory(0x14);
        pResult = 0;
        if (pAlloc != 0) {
            pResult = pAlloc->Init();
        }
        this->timer1 = pResult;
        
        pAlloc = (Timer*)AllocateMemory(0x14);
        pResult = 0;
        if (pAlloc != 0) {
            pResult = pAlloc->Init();
        }
        this->timer2 = pResult;
        
        pList = (EventList*)AllocateMemory(0x10);
        if (pList != 0) {
            pList->field_0x0C = 0;
            pList->head = 0;
            pList->tail = 0;
            pList->current = pList->head;
        }
    } catch (...) {
    }
    
    this->field_0x18 = 0;
    this->field_0x08 = 0x54;
    this->eventList = pList;
    return this;
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
    GameState* pGameState;
    int* pMouseCoords;
    int mouseX;
    int mouseY;

    this->ResetLoop();
    
    if (this->field_0x00 != 0) {
        goto exit_loop;
    }

    pSmackSoundCheck = g_SmackSoundCheck;
    
loop_start:
    this->ProcessInput();
    if (this->field_0x00 != 0) {
        goto exit_loop;
    }
    
    FUN_004179a0();
    if (this->field_0x00 != 0) {
        goto exit_loop;
    }
    
    this->DrawFrame();
    FUN_0041c5a0(DAT_0043698c);
    
    elapsedTime = ((Timer*)this->timer1)->Update();
    while (elapsedTime < (unsigned int)this->field_0x08) {
        pSmackSoundCheck();
        elapsedTime = ((Timer*)this->timer1)->Update();
    }
    
    pGameState = g_GameState_00436998;
    if (pGameState != 0) {
        if (pGameState->maxStates < 5) {
            ShowError("\"GameState Error  #%d\"", 1);
        }
        if (*(int*)((char*)pGameState->stateValues + 0x10) != 0) {
            mouseY = 0;
            pMouseCoords = *(int**)((char*)g_JoystickManager_00436968 + 0x1a0);
            if (pMouseCoords != 0) {
                mouseY = pMouseCoords[1];
            }
            if (pMouseCoords != 0) {
                mouseX = pMouseCoords[0];
            } else {
                mouseX = 0;
            }
            elapsedTime = ((Timer*)this->timer1)->Update();
            sprintf(g_Buffer_00436960, "FT %d, [%d,%d]", elapsedTime, mouseX, mouseY);
            ((SoundManager*)DAT_0043698c)->ShowSubtitle(g_Buffer_00436960, 0x14, 0x1e, 10000, 8);
        }
    }
    
    ((Timer*)this->timer1)->Reset();
    FUN_0041c960(DAT_0043698c);
    
    if (this->field_0x00 == 0) {
        goto loop_start;
    }

exit_loop:
    FUN_00417450(this);
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
    
    inputResult = FUN_00421d10(g_JoystickManager_00436968, 1);
    this->field_0x00 = this->field_0x00 | inputResult;
    if (this->field_0x00 != 0) {
        return;
    }
    
    pMouseCoords = *(int**)((char*)g_JoystickManager_00436968 + 0x1a0);
    if (pMouseCoords == 0) {
        return;
    }
    
    if (pMouseCoords[4] < 2 && pMouseCoords[5] < 2) {
        if (DAT_004373bc == 0) {
            return;
        }
    }
    
    try {
        pLocalMessage = (SC_Message*)buffer;
        pLocalMessage->SC_Message::SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    } catch (...) {
    }
    
    pLocalMessage->field_90 = 3; // type = 3
    
    if (DAT_004373bc != 0) {
        keyCode = WaitForInput();
    }
    
    pMouseCoords = *(int**)((char*)g_JoystickManager_00436968 + 0x1a0);
    if (pMouseCoords == 0) {
        mouseX = 0;
    } else {
        mouseX = pMouseCoords[4];
    }
    pLocalMessage->field_ac = mouseX;
    
    if (pMouseCoords == 0) {
        mouseY = 0;
    } else {
        mouseY = pMouseCoords[5];
    }
    pLocalMessage->field_b0 = mouseY;
    
    clickY = 0;
    if (pMouseCoords != 0) {
        clickY = pMouseCoords[1];
    }
    clickX = 0;
    if (pMouseCoords != 0) {
        clickX = pMouseCoords[0];
    }
    pLocalMessage->field_a4 = clickX;
    pLocalMessage->field_a8 = clickY;
    
    if (pMouseCoords != 0) {
        if (pMouseCoords[4] >= 2 || pMouseCoords[5] >= 2) {
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
    
    if (pLocalMessage->field_88 != 0 && pLocalMessage->field_98 != 0) {
        pPool = (TimedEventPool*)DAT_00436988;
        pEvent = pPool->Create((void*)pPool->list.tail, 0);
        ((TimedEvent*)((char*)pEvent + 8))->CopyFrom((TimedEvent*)pLocalMessage);
        if (pPool->list.tail == 0) {
            pPool->list.head = (TimedEvent*)pEvent;
        } else {
            *(TimedEvent**)pPool->list.tail = (TimedEvent*)pEvent;
        }
        pPool->list.tail = (TimedEvent*)pEvent;
    }
    
    FUN_004199a0(pLocalMessage);
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
