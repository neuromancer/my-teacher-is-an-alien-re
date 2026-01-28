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

} // extern "C"


// ============================================================================
// C++ Stubs (need C++ linkage for name mangling)
// ============================================================================

// FUN_00421840 -> InputManager::~InputManager in InputManager.cpp

// CRT internal function
void CleanupObjectArray(void* a, int b) {}

// FUN_00421f90 -> CDData::ResolvePath in CDData.cpp
// FUN_00404230 -> AssetList::Cleanup in AssetList.cpp

// ============================================================================
// Global variables needed for linking
// ============================================================================

int (*g_OutOfMemoryCallback)(unsigned int) = NULL;
short _param_3 = 0; // Sound.obj ?_param_3@@3FA

// ============================================================================
// Handler8 stubs - message queue management
// ============================================================================

// Forward declaration for MessageQueue (defined in Handler8.cpp)
struct MessageQueue;

MessageQueue* g_MessageQueue = 0;

// g_CombatEngine - Combat engine pointer (defined in globals.h as extern)
Engine* g_CombatEngine = 0;

// ============================================================================
// CombatSprite stubs
// ============================================================================
#include "CombatSprite.h"

// CombatSprite::ParseSpriteData -> CombatSprite.cpp

// ============================================================================
// EngineB stubs
// ============================================================================

void* DAT_00435f1c = 0;

// ============================================================================
// CombatSprite / misc stubs
// ============================================================================

// Target::Spawn (0x414060) -> Target.cpp

// ============================================================================
// Projectile stubs
// ============================================================================

int DAT_0043d150 = 0;

// FUN_00416ba0 -> EngineSubsystems.cpp
// FUN_00422ac3, FUN_00422aaf, FUN_00424176 -> Graphics.cpp

// ============================================================================
// CDData helper stubs
// ============================================================================

extern "C" {
void __cdecl FUN_004195c0(char* param) {}
void __cdecl FUN_00419620(char* param) {}
int __cdecl FUN_004195a0(const char* path) { return 0; }
void __cdecl FUN_004261c0(char* param1, void* param2, char* outBuffer, void* param4, void* param5) {}
int __cdecl FUN_004304a0(const char* path) { return 0; }
void __cdecl FUN_00419660(const char* dest, const char* src) {}
void __cdecl FUN_0042ad80(void) {}
}

// ============================================================================
// AssetList helper stubs
// ============================================================================

extern "C" {
void __fastcall FUN_004088f0(void* thisPtr) {}
void __fastcall FUN_00404490(void* thisPtr, int param) {}
}
