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

extern "C" {

// SEH cleanup funclets (auto-generated compiler code, not real functions)
// FUN_00421671 - SEH funclet
// FUN_0041fbd3 - SEH funclet
// FUN_00421c24 - SEH funclet
void FUN_0041ae0c() {}  // SEH funclet in VBuffer.cpp - still called

// Coordinate scaling stubs (used by Graphics.cpp GetMousePosition)
int FUN_0042449b(int x) { return x; }  // Scale X coordinate
int FUN_004244c2(int y) { return y; }  // Scale Y coordinate

// Game object constructors
void* __fastcall FUN_004165d0(void* mem) { return mem; }  // RockThrower constructor


// Memory wrappers (used by other stubs)
void* AllocateMemory_Wrapper(int size) {
    return malloc(size);
}

char* FormatFilePath(char* path) {
    return path;
}

// Menu system stubs
void OpMenu__SetOptionState() {}
void OpMenu__GetOptionQ() {}
void OptionMenu__Init() {}
void HotspotManager_Init() {}

} // extern "C"

// ============================================================================
// C++ Stubs (need C++ linkage for name mangling)
// ============================================================================

// VBuffer::Destroy - VBuffer.cpp
// ZBQueueNode::Cleanup - ZBufferManager.cpp

void FUN_00421840() {}

void CleanupObjectArray(void* a, int b) {}
int FindCharIndex(char* c) { return 0; }
int mCNavNode_GetNextNode(void* a) { return 0; }
int mCNavNode_Update(void* a) { return 0; }
void* NavNode_Constructor(void* a) { return a; }

#include "InputManager.h"
// InputManager::PollEvents - InputManager.cpp

// ============================================================================
// Global variables needed for linking
// ============================================================================

int (*g_OutOfMemoryCallback)(unsigned int) = NULL;
short _param_3 = 0; // Sound.obj ?_param_3@@3FA

#include "SC_OnScreenMessage.h"
