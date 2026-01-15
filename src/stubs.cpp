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

extern "C" {

// SEH cleanup funclets (auto-generated compiler code, not real functions)
// FUN_00421671, FUN_0041fbd3, FUN_00421c24, FUN_0041ae0c - All removed (SEH funclets)

// Game object constructors
void* __fastcall FUN_004165d0(void* mem) { return mem; }  // RockThrower constructor

} // extern "C"

// ============================================================================
// C++ Stubs (need C++ linkage for name mangling)
// ============================================================================

void FUN_00421840() {}

// CRT internal function
void CleanupObjectArray(void* a, int b) {}

// Functions moved to their class files as proper class members:
// - AllocateMemory_Wrapper -> Memory.cpp (extern "C" function)
// - FormatFilePath -> FileSystem.cpp (extern "C" function)
// - OptionMenu::SetOptionState, OptionMenu::GetOptionQ -> OptionMenu.cpp
// - HotspotManager::HotspotManager() constructor -> Hotspot.cpp
// - mCNavNode_Update, mCNavNode_GetNextNode, NavNode_Constructor -> mCNavigator.cpp
// - FindCharIndex -> mCNavigator.cpp

#include "InputManager.h"
// InputManager::PollEvents - InputManager.cpp

// ============================================================================
// Global variables needed for linking
// ============================================================================

int (*g_OutOfMemoryCallback)(unsigned int) = NULL;
short _param_3 = 0; // Sound.obj ?_param_3@@3FA

#include "SC_OnScreenMessage.h"
#include "SC_Question.h"
#include "Handler11.h"

// ============================================================================
// Handler8 stubs - message queue management
// ============================================================================

// Forward declaration for MessageQueue (defined in Handler8.cpp)
struct MessageQueue;

// Message queue globals and helpers
MessageQueue* g_MessageQueue = 0;
void* FUN_4065e0(void* pool, int capacity, int itemSize) { return 0; }
void FUN_406670(void* data, int param) {}
void FUN_406610(void* dest, SC_Message* src) {}

// ============================================================================
// Handler11 stubs
// ============================================================================

void FUN_418d60(void*, const char*, const char*) {}

// Placeholder destructors
SC_Dialog::~SC_Dialog() {}
UnknownClass_4092e0::~UnknownClass_4092e0() {}
