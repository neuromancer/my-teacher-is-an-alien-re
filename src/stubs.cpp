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
// FUN_4065e0 (ExpandPool) implemented in Handler8.cpp
void FUN_406670(void* data, int param) {}
// FUN_406610 (CopyParserData) implemented in Handler8.cpp

// ============================================================================
// Handler11 stubs
// ============================================================================

// FUN_00418d60 now implemented in Parser.cpp as ParseFile()

// Placeholder destructors
SC_Dialog::~SC_Dialog() {}
UnknownClass_4092e0::~UnknownClass_4092e0() {}

// ============================================================================
// Handler10 stubs - Character/OptionMenu/Sprite functions
// ============================================================================

// Character object constructor now in Character.cpp

// Functions used by Character constructor
#include "Character.h"
extern "C" char* FUN_00424c00(char* haystack, char* needle) { return haystack; }  // String search stub
CharSprite* __fastcall FUN_00408880(CharSprite* mem) { return mem; }  // CharSprite constructor
void __fastcall FUN_004043a0(PriorityQueue* queue, CharSprite* sprite) {}  // Queue add

// CharSprite/CharButton functions now implemented in Hotspot.cpp as Hotspot methods:
// - FUN_00409400 -> Hotspot::Do()
// - FUN_00409440 -> Hotspot::SetState(int)
// - FUN_00409470 -> Hotspot::GetState()
// - FUN_004094a0 -> Hotspot::Exit()

// FUN_004092e0 now implemented in Hotspot.cpp

// FUN_0041ce30 - Hotspot-related cleanup function
void __fastcall FUN_0041ce30(void* data) {}

// OptionMenu functions now implemented in OptionMenu.cpp:
// - FUN_00409bf0 -> OptionMenu::~OptionMenu()
// - FUN_00409f00 -> OptionMenu::Render()
// - FUN_00409fb0 -> OptionMenu::SetOptionState()
// - FUN_0040a150 -> OptionMenu::SelectCharacter()
// - FUN_0040a1a0 -> OptionMenu::Deactivate()

// MouseControl functions now implemented in MouseControl.cpp:
// - FUN_0041f360 -> MouseControl::~MouseControl()
// - FUN_0041f480 -> MouseControl::StopAll()
// - FUN_0041f800 -> MouseControl::DoAll()

// IconBar::PlayButtonSound is now in IconBar.cpp:
// - FUN_00403300 -> IconBar::PlayButtonSound(int)

// ============================================================================
// Handler13 stubs - MessageList and TimerNode functions
// ============================================================================

// FUN_00401890 - TimerNode constructor (returns new TimerNode)
void* __fastcall FUN_00401890(void* mem) { return mem; }

// FUN_00401910 - cleanup function for node data in Handler13's MessageList
void __fastcall FUN_00401910(void* data) {}

// FUN_00401990 - Set timer duration on TimerNode
void __fastcall FUN_00401990(void* timerNode, int duration) {}

// FUN_004019a0 - Timer check/process function
int __fastcall FUN_004019a0(void* timerNode) { return 0; }

// FUN_004024d0 - Insert node at current position in list
void __fastcall FUN_004024d0(int* list, int node) {}

// FUN_004025a0 - Append node at end of list
void __fastcall FUN_004025a0(int* list, int node) {}

// FUN_00402680 - Pop and return current node from list
void* __fastcall FUN_00402680(int* list) { return 0; }

// FUN_00402700 - Node destructor (calls dtor and frees)
void __fastcall FUN_00402700(void* node, int shouldDelete) {}
