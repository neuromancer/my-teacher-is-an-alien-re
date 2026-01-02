#include "GameLoop.h"
#include "ZBufferManager.h"
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

// JoystickManager::CheckInput - takes this pointer and int param
extern "C" int FUN_00421d10(void*, int);
// Wait for key
int WaitForInput();
// UpdateGame is now a member function of GameLoop
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

GameLoop::GameLoop()
{
    Init();
}

GameLoop::~GameLoop()
{
    Cleanup();
}

/* Function start: 0x417200 */
GameLoop* GameLoop::Init() {
    Timer* pResult;
    EventList* pList;
    
    memset(this, 0, 7 * sizeof(int));
    
    try {
        pResult = new Timer();
        this->timer1 = pResult;
        
        pResult = new Timer();
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
    
    this->UpdateGame();
    if (this->field_0x00 != 0) {
        goto exit_loop;
    }
    
    this->DrawFrame();
    if (DAT_0043698c != 0) {
        ((ZBufferManager*)DAT_0043698c)->ProcessRenderQueues();
    }
    
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
            InputState* pMouse = g_InputManager_00436968->pMouse;
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
            ((SoundManager*)DAT_0043698c)->ShowSubtitle(g_Buffer_00436960, 0x14, 0x1e, 10000, 8);
        }
    }
    
    ((Timer*)this->timer1)->Reset();
    if (DAT_0043698c != 0) {
        ((ZBufferManager*)DAT_0043698c)->FUN_0041c960();
    }
    
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

// External functions for UpdateGame
// Note: These are thiscall but we use shim stubs
extern "C" {
    void* FUN_00417c50(void* pool, void* buffer);
    void FUN_00417cb0(GameLoop* self, void* event);
}

/* Function start: 0x4179A0 */
int GameLoop::UpdateGame()
{
    int* puVar2;
    int* puVar3;
    int* puVar4;
    unsigned int uVar5;
    unsigned int uVar6;
    char local_258[192];  // [EBP - 0x254] buffer for inner iterator result
    char local_198[192];  // [EBP - 0x194] buffer for outer iterator result
    SC_Message local_d8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // [EBP - 0xD4]
    void* local_18;       // [EBP - 0x18] - pointer storage
    int local_14;         // [EBP - 0x10] - counter

    local_14 = 0;
    
    // First loop: pop items from DAT_00436988, copy to local_d8, create in DAT_00436984
    while (*(int*)((char*)DAT_00436988 + 8) != 0) {
        puVar2 = (int*)FUN_00417c50(DAT_00436988, local_198);
        
        // Copy fields from popped item to local_d8
        ((int*)&local_d8)[2] = puVar2[2];   // offset 0x8 -> field_8c
        ((int*)&local_d8)[3] = puVar2[3];   // offset 0xc -> field_90
        
        uVar6 = 0;
        do {
            uVar5 = uVar6 + 1;
            ((char*)&local_d8)[0x10 + uVar6] = *((char*)puVar2 + 0x10 + uVar6);
            uVar6 = uVar5;
        } while (uVar5 < 0x20);
        
        ((int*)&local_d8)[0xc] = puVar2[0xc];   // offset 0x30
        ((int*)&local_d8)[0xe] = puVar2[0xe];   // offset 0x38
        ((int*)&local_d8)[0xf] = puVar2[0xf];   // offset 0x3c
        
        uVar6 = 0;
        do {
            uVar5 = uVar6 + 1;
            ((char*)&local_d8)[0x40 + uVar6] = *((char*)puVar2 + 0x40 + uVar6);
            uVar6 = uVar5;
        } while (uVar5 < 0x40);
        
        ((int*)&local_d8)[0x20] = puVar2[0x20]; // offset 0x80
        ((int*)&local_d8)[0x22] = puVar2[0x22]; // offset 0x88
        ((int*)&local_d8)[0x23] = puVar2[0x23]; // offset 0x8c
        ((int*)&local_d8)[0x24] = puVar2[0x24]; // offset 0x90
        ((int*)&local_d8)[0x25] = puVar2[0x25]; // offset 0x94
        ((int*)&local_d8)[0x26] = puVar2[0x26]; // offset 0x98
        ((int*)&local_d8)[0x27] = puVar2[0x27]; // offset 0x9c
        ((int*)&local_d8)[0x28] = puVar2[0x28]; // offset 0xa0
        ((int*)&local_d8)[0x29] = puVar2[0x29]; // offset 0xa4
        ((int*)&local_d8)[0x2a] = puVar2[0x2a]; // offset 0xa8
        ((int*)&local_d8)[0x2b] = puVar2[0x2b]; // offset 0xac
        ((int*)&local_d8)[0x2c] = puVar2[0x2c]; // offset 0xb0
        ((int*)&local_d8)[0x2d] = puVar2[0x2d]; // offset 0xb4
        ((int*)&local_d8)[0x2e] = puVar2[0x2e]; // offset 0xb8
        local_18 = (void*)puVar2[0x2f]; // offset 0xbc
        
        // Create entry in DAT_00436984
        puVar3 = DAT_00436984;
        puVar4 = (int*)((TimedEventPool*)DAT_00436984)->Create(
            (void*)DAT_00436984[1], 0);
        ((TimedEvent*)(puVar4 + 2))->CopyFrom((TimedEvent*)&local_d8);
        
        if ((int*)puVar3[1] == 0) {
            *puVar3 = (int)puVar4;
        } else {
            *(int*)puVar3[1] = (int)puVar4;
        }
        puVar3[1] = (int)puVar4;
    }
    
    // Second loop: pop items from DAT_00436984
    while (*(int*)((char*)DAT_00436984 + 8) != 0) {
        puVar2 = (int*)FUN_00417c50(DAT_00436984, local_198);
        
        FUN_00417cb0(this, puVar2);
        
        // Inner loop: pop items from DAT_00436988 and add to DAT_00436984
        while (*(int*)((char*)DAT_00436988 + 8) != 0) {
            puVar3 = (int*)FUN_00417c50(DAT_00436988, local_258);
            puVar2 = DAT_00436984;
            puVar4 = (int*)((TimedEventPool*)DAT_00436984)->Create(
                (void*)DAT_00436984[1], 0);
            ((TimedEvent*)(puVar4 + 2))->CopyFrom((TimedEvent*)puVar3);
            
            if ((int*)puVar2[1] == 0) {
                *puVar2 = (int)puVar4;
            } else {
                *(int*)puVar2[1] = (int)puVar4;
            }
            puVar2[1] = (int)puVar4;
        }
        
        local_14++;
    }
    
    local_d8.SC_Message::~SC_Message();
    return local_14;
}
