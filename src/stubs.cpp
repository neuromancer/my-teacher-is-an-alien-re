#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Globals
// Globals
class SoundManager* g_SoundManager = NULL;


extern "C" {

// Generic Stubs for Missing Functions
void FUN_0041ef47() {}
void FUN_0042ddd0(void*, int) {}
void FUN_00425fd0() {}
void FUN_0041eab0() {}
void FUN_004171b0() {}
void FUN_0041e666() {}
void FUN_0041b110() {}
void FUN_0041b0a0() {}
void FUN_00421700() {}
void FUN_00422e1a() {}
void FUN_00422e71() {}
void FUN_00422e8f() {}
void FUN_00422a01() {}
void FUN_004230d9() {}
void FUN_00423296() {}
void FUN_0042333a() {}
void FUN_004231ce() {}
void FUN_004233e8() {}
void FUN_0041afb1() {}
void FUN_0041af9f() {}
void FUN_0041b590() {}
void FUN_0041b091() {}
void FUN_0041b07f() {}
void FUN_0041b2ac() {}
void FUN_0041b29a() {}
void FUN_004234d5() {}
void FUN_004234f9() {}
void FUN_00423703() {}
void FUN_00423076() {}

void FUN_00422d98() {}
void FUN_00423a54() {}
void FUN_00423aac() {}
void FUN_00419170() {}
void FUN_00423cd9() {}
void FUN_00421e40() {}
void FUN_00421890(void*, int) {}

void FUN_0040c5d0() {}
void FUN_00421ea0() {}
void FUN_004227a0() {}
void FUN_00421010() {}
void FUN_00425f30(FILE* fp, long offset, int origin) {}
void FUN_00426550(const char* filename, int* stat_buf) {
    if (stat_buf) stat_buf[4] = 0; 
}
void FUN_0041a3b9() {} // Referenced in Message.cpp

// New stubs for Step 66 errors
void FUN_0041e670() {}
void FUN_0041ae0c() {}
void FUN_00424b00() {}
void FUN_00421671() {}
void FUN_00405770() {}
void FUN_0041fbd3() {}
void FUN_0041eb90() {}
void FUN_004224d0() {}
void FUN_0041ea80() {}
void FUN_0041eb70() {}
void FUN_00419390() {}
void FUN_0041eb50() {}
void FUN_004224e0() {}
void FUN_004224f0() {}
void FUN_00421af0() {}
void FUN_00421d10() {}
void FUN_00411550() {}
void FUN_00424fc0() {}
void FUN_00425000() {}
void FUN_004229ea() {}
void FUN_00422a2f() {}
void FUN_004110d0() {}
void FUN_00424940() {}
void FUN_0041e7d0() {}

// Named Stubs
HWND GetWindowHandle_() { 
    return GetActiveWindow(); 
}

void SetCursorVisible(int show) { 
    ShowCursor(show); 
}

void InitWorkBuffer() {}
void CreateGameObject_1() {}

void* AllocateMemory_Wrapper(int size) { 
    return malloc(size); 
}

char* FormatFilePath(char* path) { 
    return path; 
}

void CreatePaletteBuffer() {}
void MouseControl_Constructor() {}
void OpMenu__SetOptionState() {}
void OpMenu__GetOptionQ() {}
void OptionMenu__Init() {}
void HotspotManager_Init() {}

void SC_OnScreenMessage_AddMessage() {}
int OnScreenMessage_Update(void* param_1, int param_2) { return 0; }

void TimedEvent_Init() {}
void GameState_dtor_wrapper_4() {}
int GetCurrentVideoMode() { return 1; }
void InvalidateVideoMode() {}

void* Message_Constructor(void* mem, int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7, int param_8, int param_9, int param_10) {
    return mem;
}


// Variables required by string.cpp in extern "C"
int DAT_0043be34 = 0;
char DAT_0043be30 = 0;
void* DAT_0043f104 = 0;
void* DAT_0043f100 = 0;
int DAT_00435030 = 0;
int DAT_00435038 = 0;
int DAT_0043503c = 0;
int DAT_00435040 = 0;

int _rand() { return rand(); }

// GameWindow missing stubs from Step 28/46/66
// Mangled names with @16 imply stdcall. We might need logic or strict prototype.
// _FUN_00422590@16
void __stdcall FUN_00422590(int a, int b, int c, int d) {}
// _FUN_00422250 (likely cdecl or stdcall without args?)
void __cdecl FUN_00422250() {}

void FUN_0040cd15() {}
void FUN_0040cd1d() {}
void* FUN_00420140(void* a, const char* b, int c) { return a; }
void* FUN_004209e0(void* a, const char* b, int c) { return a; }

} // extern "C"

// C++ Stubs
void FUN_00421840() {}
int FUN_00422510() { return 0; }
void CleanupObjectArray(void* a, int b) {}
int FindCharIndex(char* c) { return 0; }
int mCNavNode_GetNextNode(void* a) { return 0; }
int mCNavNode_Update(void* a) { return 0; }
void* NavNode_Constructor(void* a) { return a; }

// Missing variables (C++ linkage to match mangled names)
// void* DAT_0043eff0 = NULL; // Moved to globals.cpp

int (*g_OutOfMemoryCallback)(unsigned int) = NULL;
int* g_GameStruct2 = NULL;
short _param_3 = 0; // Sound.obj ?_param_3@@3FA

// C++ Mangled Stubs
#include "SC_OnScreenMessage.h"
#include "TimedEvent.h"
#include "Message.h"

void SC_OnScreenMessage::Timer_DecrementCounter_wrapper_2() {}
void SC_OnScreenMessage::Timer_DecrementCounter_wrapper() {}
void TimedEvent::SetData(int a) {}
Message::Message(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10) {
    this->field_0x0 = p1;
}

// TimedEvent destructor wrapper
// @TimedEvent_dtor@4 is likely a fastcall or similar? 
// No, @Name@Bytes is fastcall.
// The error was: unresolved external symbol @TimedEvent_dtor@4
// We can define it as extern "C" __fastcall?
extern "C" void __fastcall TimedEvent_dtor(void* thisptr) {} 
