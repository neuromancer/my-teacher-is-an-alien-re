#include "GameLoop.h"
#include "Timer.h"
#include "GameState.h"
#include "JoystickManager.h"
#include "Memory.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    void __fastcall FUN_00417320_impl(void*);
    void __cdecl SmackSoundCheck();
}

typedef void (__cdecl *SmackSoundCheckFn)();
extern SmackSoundCheckFn g_SmackSoundCheck;

extern "C" void __fastcall FUN_00417420(GameLoop*);     // ResetLoop
extern "C" void __fastcall FUN_004177b0(GameLoop*);     // ProcessInput (this method)
extern "C" void FUN_004179a0();              // UpdateGame (global)
extern "C" void __fastcall FUN_00417ed0(GameLoop*);     // DrawFrame (this method)
extern "C" void __fastcall FUN_0041c5a0(void*);         // ZBufferManager method
extern "C" void __fastcall FUN_0041c960(void*);         // ZBufferManager method
extern "C" void __fastcall FUN_00417450(GameLoop*);     // CleanupLoop
extern "C" void ShowError(const char* format, ...);

// Simple linked list node structure for eventList
struct ListNode {
    int field_0x00;
    int field_0x04;
    int field_0x08;
    int field_0x0C;
};

/* Function start: 0x417200 */
GameLoop* GameLoop::Init() {
    Timer* pTimer1;
    Timer* pTimer2;
    ListNode* pList;
    
    memset(this, 0, 7 * sizeof(int));
    
    pTimer1 = (Timer*)AllocateMemory(0x14);
    if (pTimer1 != 0) {
        pTimer1 = pTimer1->Init();
    } else {
        pTimer1 = 0;
    }
    this->timer1 = pTimer1;
    
    pTimer2 = (Timer*)AllocateMemory(0x14);
    if (pTimer2 != 0) {
        pTimer2 = pTimer2->Init();
    } else {
        pTimer2 = 0;
    }
    this->timer2 = pTimer2;
    
    pList = (ListNode*)AllocateMemory(0x10);
    if (pList != 0) {
        pList->field_0x0C = 0;
        pList->field_0x00 = 0;
        pList->field_0x04 = 0;
        pList->field_0x08 = pList->field_0x00;
    } else {
        pList = 0;
    }
    
    this->field_0x18 = 0;
    this->field_0x08 = 0x54;
    this->eventList = pList;
    return this;
}

/* Function start: 0x417690 */
void GameLoop::Run() {
    SmackSoundCheckFn pSmackSoundCheck;
    unsigned int elapsedTime;
    GameState* pGameState;
    int* pMouseCoords;
    int mouseX;
    int mouseY;

    FUN_00417420(this);
    
    if (this->field_0x00 != 0) {
        goto exit_loop;
    }

    pSmackSoundCheck = g_SmackSoundCheck;
    
loop_start:
    FUN_004177b0(this);
    if (this->field_0x00 != 0) {
        goto exit_loop;
    }
    
    FUN_004179a0();
    if (this->field_0x00 != 0) {
        goto exit_loop;
    }
    
    FUN_00417ed0(this);
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

/* Function start: 0x417320 */
void GameLoop::Cleanup() {
    FUN_00417320_impl(this);
}
