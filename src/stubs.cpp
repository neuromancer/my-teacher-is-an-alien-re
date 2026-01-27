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
#include "SCI_SearchScreen.h"
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
// FUN_004155e0 -> CombatSprite::PlayById in CombatSprite.cpp
// FUN_0041fa50 -> Animation::Animation() in Animation.cpp
// FUN_0041ff30 -> Animation::Play() in Animation.cpp
// FUN_0041fb70 -> Animation::~Animation() in Animation.cpp

// ============================================================================
// CombatSprite stubs
// ============================================================================
#include "CombatSprite.h"

// SpriteHashTable method stubs
void* SpriteHashTable::Lookup(int index, int* outSlot) { return 0; }
void SpriteHashTable::Resize(int size, int flag) {}
void* SpriteHashTable::AllocEntry() { return 0; }

// CombatSprite method stubs
void CombatSprite::ParseSpriteData(char* line) {}

// ============================================================================
// EngineB stubs
// ============================================================================

void FUN_004150f0(Parser* parser) {}
void FUN_00416880(int* param) {}
char* FUN_0040d200(char* filename) { return filename; }
void FUN_0041d190(void* param, int value) {}
void* DAT_00435f1c = 0;

// ============================================================================
// CombatSprite / misc stubs
// ============================================================================

void FUN_0041556a() {}

// ============================================================================
// EngineSubsystems / TargetList stubs
// ============================================================================

void FUN_00416ba0(int* scoreManager, int value) {}

// ============================================================================
// Projectile stubs
// ============================================================================

int DAT_0043d150 = 0;

// ============================================================================
// Weapon / WeaponDraw stubs (C linkage)
// ============================================================================

extern "C" {
int __cdecl FUN_00422ac3(int param_1, int param_2) { return 0; }
int __cdecl FUN_00422aaf(int param_1) { return 0; }
}
