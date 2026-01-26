#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <share.h>
#include "Memory.h"
#include "OnScreenMessage.h"
#include "TimedEvent.h"
#include "globals.h"
#include "ZBufferManager.h"
#include "SC_Question.h"
#include "SC_OnScreenMessage.h"
#include "Handler11.h"
#include "InputManager.h"
#include "Character.h"
#include "SoundItem.h"
#include "Parser.h"
#include "Engine.h"
#include "SCI_Dialog.h"
#include "Queue.h"

extern "C" {

// SEH cleanup funclets (auto-generated compiler code, not real functions)
// FUN_00421671, FUN_0041fbd3, FUN_00421c24, FUN_0041ae0c - All removed (SEH funclets)

void __fastcall FUN_00404230_impl(void* thisptr);

} // extern "C"


// ============================================================================
// C++ Stubs (need C++ linkage for name mangling)
// ============================================================================

// FUN_00421840 -> InputManager::~InputManager in InputManager.cpp

// CRT internal function
void CleanupObjectArray(void* a, int b) {}

// Target helper stubs
int FUN_00421f90(void* cdData, char* param) { return 0; }

// ============================================================================
// Global variables needed for linking
// ============================================================================

int (*g_OutOfMemoryCallback)(unsigned int) = NULL;
short _param_3 = 0; // Sound.obj ?_param_3@@3FA



extern "C" {
void __fastcall FUN_00404230_impl(void* thisptr) {}
}

// ============================================================================
// Handler8 stubs - message queue management
// ============================================================================

// Forward declaration for MessageQueue (defined in Handler8.cpp)
struct MessageQueue;

MessageQueue* g_MessageQueue = 0;

// g_CombatEngine - Combat engine pointer (defined in globals.h as extern)
Engine* g_CombatEngine = 0;

// ============================================================================
// mCNavNode stubs
// ============================================================================

// FUN_004130f0 -> mCNavNode::Activate in mCNavNode.cpp
// FUN_0041d300 -> Sprite::Do in Sprite.cpp

// FUN_004155e0 - SpriteList play by ID
int FUN_004155e0(Parser* spriteList, int soundId) { return 0; }

// FUN_0041fa50 - SoundItem constructor
void* __fastcall FUN_0041fa50(void* soundItem) { return soundItem; }

// FUN_0041ff30 - Load/play sound
void FUN_0041ff30(void* soundItem, char* soundName, unsigned int flags) {}

// FUN_0041fb70 - SoundItem destructor
void __fastcall FUN_0041fb70(void* soundItem) {}
