#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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
#include "smack.h"
#include "mss.h"
#include "SoundItem.h"
#include "Parser.h"
#include "Engine.h"
#include "Timer.h"
#include "SCI_Dialog.h"
#include "Queue.h"
#include "Sound.h"
#include "SpriteAction.h"
#include "GameState.h"
#include "LinkedList.h"

extern "C" int DAT_0046a6ec;

// ============================================================================
// Global variables needed for linking
// ============================================================================

int (*g_OutOfMemoryCallback)(unsigned int) = NULL;
short _param_3 = 0; // Sound.obj ?_param_3@@3FA

// Handler8 - message queue management
struct MessageQueue;
MessageQueue* g_MessageQueue = 0;

// EngineB globals
void* DAT_00435f1c = 0;

extern "C" {
    void* DAT_0046ae4c = 0;
    void* DAT_0046ae50 = 0;
    void* DAT_0046ae58 = 0;
    void* DAT_0046ae60 = 0;
    void* DAT_0046ae64 = 0;
    void* DAT_0046ae68 = 0;
    void* DAT_0046ae6c = 0;
    int DAT_00473e18 = 0;
}

// Projectile globals
int g_ProjectileHits_0043d150 = 0;

// Parser globals
int g_ParserCount = 0;              // DAT_00469288
void* g_FilePosCache = 0;           // DAT_00469144
int DAT_00469160 = 0;               // preprocessor variable substitution flag
int DAT_00469148 = 0;
int DAT_0046914c = 0;
int DAT_00469150 = 0;
int DAT_00469154 = 0;
int DAT_00469158 = 0;
int DAT_0046915c = 0;

// SCI_Inventory globals
void* DAT_0046aa24 = 0;
void* DAT_0046aa18 = 0;
void* DAT_0046a6e4 = 0;
int DAT_00473334 = 0;
int DAT_004733e8 = 0;
void* DAT_0046aa08 = 0;
void* DAT_0046a6dc = 0;
char* DAT_0046aa00 = 0;
// g_InventoryList removed - same global as g_MsgList (DAT_0046a6dc) in main.cpp

// Full game globals
extern "C" extern void* DAT_0046aa30;
#define g_GameState_0046aa30 ((GameState*)DAT_0046aa30)
GameState* g_StringTable_0046aa34 = 0;
char* DAT_0046aa2c = 0;
GameState* DAT_0046aa3c = 0;

// SpriteAction globals
SpriteAction DAT_00472d58;
SpriteAction DAT_00472d90;
SpriteAction DAT_00472d20;

// SCI_SchoolMenu globals
extern "C" {
    int DAT_0046a190 = 0;
    void* DAT_0046af08 = 0;
}
int DAT_0046cb90 = 0;
char* DAT_0046cb94 = 0;
int DAT_00473358 = 0;
int DAT_0047337c = 0;

// SC_Cinematic + SC_SelectHotSpot globals
extern "C" {
    int DAT_0046a6ec = 0;
    void* DAT_0046aa10 = 0;
    void* DAT_0046aa14 = 0;
    void* DAT_0046aa30 = 0;
    // DAT_0046ac04 = g_WaitForInputValue_004373bc in globals.cpp (last key pressed)
    char DAT_00473400 = 0;
    char DAT_00472c70[256] = {0};
    char DAT_00472cb0[256] = {0};
}

// SC_Question globals
void* DAT_0046a6e0 = 0;
void* DAT_0046a6e8 = 0;
char DAT_00468108[32] = {0};
char DAT_00468168[32] = {0};
char DAT_004690e4[32] = {0};
void* DAT_0046aa38 = 0;

// SC_Wahoo / SC_WordSearch / SCI_PracticeRoom globals
int DAT_0046ad6c = 0;
void* DAT_0046bbfc = 0;
void* DAT_00468ef0 = 0;
char* DAT_0046bacc = 0;

// SC_FireAlarm / SC_Pods globals
int DAT_004685ac = 0;
void* DAT_0046bf28 = 0;
int DAT_00472bd8 = 0;
int DAT_004685a0 = 0;
int DAT_00472bdc = 0;

// Engine globals
Sound* DAT_0046AA0C = 0;
int DAT_0046AA24 = 0;

// HotspotAction globals
void* DAT_0046aa34 = 0;

// SCI_IconBarModule / SC_Detention globals
int DAT_00468764 = 0;

// ============================================================================
// EngineB proxy class stubs
// ============================================================================

class TargetListFull { public: void ProcessTargets(); };
void TargetListFull::ProcessTargets() {}

class WeaponParser { public: void UpdateProjectiles(); };
void WeaponParser::UpdateProjectiles() {}

class InputManagerFull { public: void PollEvents(int); };
void InputManagerFull::PollEvents(int) {}

class PaletteLoader { public: void Load(char*); };
void PaletteLoader::Load(char*) {}

// InputManager::Refresh → InputManager.cpp

// Engine COMDAT thunks (needed by SC_DodgeOrville)
void __fastcall FUN_00449320(void*, int, int) {}
int __fastcall FUN_00449400(void*, int, void*) { return 0; }
void __fastcall FUN_004494e0(void*) {}

// ============================================================================
// AssetList helper stubs
// ============================================================================

extern "C" {
void __fastcall AssetCleanup(void* thisPtr) {}
void __fastcall AssetNodeDelete(void* thisPtr, int param) {}
}

// ============================================================================
// Remaining proxy classes (real class unknown)
// ============================================================================

// SCI_SchoolMenu - constructor proxy (0x420CE0)
class SprInit { public: void FUN_00420ce0(int); };
void SprInit::FUN_00420ce0(int) {}


// ============================================================================
// GameLoopHelper helper stubs
// ============================================================================
void __cdecl FUN_0041a6f0(void* data, int count) {}
int __cdecl FUN_0041a6d0(void* a, void* b) { return 0; }

// ============================================================================
// SoundTracker stubs and globals
// ============================================================================
#include "SoundTracker.h"
struct SoundPool;
SoundPool* DAT_00469134 = 0;
int DAT_00469128 = 0;

void SoundTracker::Init() {}
void SoundTracker::Cleanup() {}

// ============================================================================
// Implemented functions (with real code)
// ============================================================================

extern "C" void WriteToLog(const char* format, ...) {}

extern "C" {
char* FUN_0044e530(int handle) { return 0; }
void FUN_004265a0() {}
} // extern "C"

// ============================================================================
// Remaining __fastcall stubs (unidentified functions)
// ============================================================================

void __fastcall FUN_004148f0(void* param) {}
void* __fastcall FUN_00418540(void* param) { return 0; }
void* __fastcall FUN_00403620(void*) { return 0; }
void __fastcall FUN_00401c80(void*) {}
void __fastcall FUN_004061e0(void*) {}
void __fastcall FUN_00406cc0(void*, int, void*) {}
void __fastcall FUN_00406fd0(void*, int, int) {}
void __fastcall FUN_004070a0(void*, int, int) {}
void __fastcall FUN_004308c0(void*) {}
void* __fastcall FUN_00407180(void*) { return 0; }
void* __fastcall FUN_0041dbe0(void*) { return 0; }
void __fastcall FUN_0042be00(void*) {}
void __fastcall FUN_00404d70(void*, int, int) {}
void* __fastcall FUN_00404b80(void*) { return 0; }
void __fastcall FUN_0040b760(void*, int, int) {}
void __fastcall FUN_0044bac0(void*, int, int, int) {}
void* __fastcall FUN_00450b10(void*) { return 0; }
void __fastcall FUN_0044c880(void*) {}
void __fastcall FUN_00420ac0(void*, int, int) {}
void __fastcall FUN_004128f0(void*) {}
void __fastcall FUN_00432da0(void* self) {}
void __fastcall FUN_00421bc0(void* self) {}
void* __fastcall FUN_004407c0(void* self) { return 0; }
void __fastcall FUN_0040c6e0(void* self) {}
void __fastcall FUN_004309a0(void*, int, int) {}
void __fastcall FUN_0042b100(void*) {}
void __fastcall FUN_0042b270(void*) {}
void __fastcall FUN_00429c10(void*) {}
void* __fastcall FUN_0042bc50(void*) { return 0; }
void __fastcall FUN_00420f00(void*) {}
void __fastcall FUN_00429df0(void*) {}
int __fastcall FUN_0042a010(void*, int, void*) { return 0; }
void* __fastcall FUN_00429b60(void*, int, int, void*) { return 0; }
void* __fastcall FUN_00420ce0(void*, int, int) { return 0; }
void __fastcall FUN_00410fd0(void*) {}
void __fastcall FUN_004274c0(void*, int, int) {}
void* __fastcall FUN_00410fb0(void*, int, char*, int) { return 0; }
void __fastcall FUN_00411180(void*, int, int) {}
void __fastcall FUN_00420d90(void*) {}
void __fastcall FUN_00421020(void*) {}
void __fastcall FUN_00421880(void*) {}
void __fastcall FUN_00425490(void*) {}
void* __fastcall FUN_00425480(void*) { return 0; }

void __fastcall FUN_004254a0(void*, int, int) {}
void* __fastcall FUN_00440860(void*) { return 0; }
void __fastcall FUN_00421920(void*) {}
void __fastcall FUN_00427880(void*) {}
void __fastcall FUN_00403fd0(void*, int, void*, int, int, int, int, int, int, int, int, int, int) {}
void __fastcall FUN_0044cb40(void*, int, int, int) {}
void __fastcall FUN_004127c0(void*) {}
void __fastcall FUN_00407b60(void*) {}
void __fastcall FUN_004047c0(void*) {}
void* __fastcall FUN_004036a0(void*) { return 0; }
void __fastcall FUN_00445970(void*) {}
void __fastcall FUN_004459a0(void*, int, int) {}
void __fastcall FUN_004279a0(void*) {}
void __fastcall FUN_00404350(void*, int, int, int, int, int, int, int, int) {}
int __fastcall FUN_0044c350(void*, int, char*) { return 0; }
int __fastcall FUN_00420a00(void*, int, int) { return 0; }
void __fastcall FUN_00420a50(void*, int, int) {}
void __fastcall FUN_00404b90(void*) {}
void __fastcall FUN_00404230(void*, int, char*, int, int, int, int) {}

// ============================================================================
// Remaining __cdecl stubs
// ============================================================================

void __cdecl FUN_00412a50() {}
int __cdecl FUN_00412a50(void*) { return 0; }
void __cdecl FUN_00413e70(void*, int, char*) {}
void __cdecl FUN_00425a90(int, int) {}
void __cdecl FUN_00444e40(void*) {}
// ParseSpriteAction (was FUN_00445450) moved to SC_Message.cpp
void* __cdecl FUN_00444a40(void*, int, int, int, int, int, int, int, int, int, int) { return 0; }
char* __cdecl FUN_00426190(char* name) { return name; }
void __cdecl FUN_00425cb0(char*, ...) {}
int __cdecl FUN_0044ccf0(int, int, int, int) { return 0; }
void __cdecl FUN_004344b0() {}
void __cdecl FUN_00434030(void*, int) {}
char* __cdecl FUN_0044e470(char*) { return 0; }
void __cdecl FUN_00425bc0(char*, char*, int) {}

// ============================================================================
// Remaining extern "C" stubs
// ============================================================================

extern "C" {
    // FUN_004205e0 = GetScreenWidth in GameWindow.cpp
    // FUN_004205f0 = GetScreenHeight in GameWindow.cpp
    // FUN_00425f10 = BlankScreen in Graphics.cpp
    static int s_WindowHeight = 480;
    int* GetWindowHeight() { return &s_WindowHeight; }
    static int s_WindowedMode = 0;
    int* GetWindowedModeFlag() { return &s_WindowedMode; }
    // FUN_00425fa0 = FileExists in main.cpp
    void* FUN_004260f0(char* name) { return name; }
    void FUN_004307b0(int) {}
    // FUN_0044e3e0 = GetCinematicFilename in SC_Cinematic.cpp
    void SetFontPosition(int, int) {}
    void SetFontColor(int) {}
    void DrawFontText(char*, int) {}
    void FUN_00413e10(void*, char*, char*, ...) {}
    void FUN_004309c0(void*) {}
}

// ============================================================================
// Remaining __stdcall stubs
// ============================================================================

void __stdcall FUN_004309C0(int*) {}
void __stdcall FUN_004309a0(int) {}

// ============================================================================
// Remaining IconBar.obj stubs
// ============================================================================

void __fastcall FUN_00412a50(void*, int, char*) {}

#include "GameLoop.h"

// VBuffer stubs
#include "VBuffer.h"

// ============================================================================
// Unresolved externals from new implementations
// ============================================================================

// Hotspot/T_Hotspot stubs
#include "Hotspot.h"
Hotspot::Hotspot() { memset(&hotspot, 0, 104); field_D0 = 1; state = 1; }
Hotspot::~Hotspot() {}
int Hotspot::Do() { return 0; }
int Hotspot::LBLParse(char*) { return 0; }
int T_Hotspot::LBLParse(char*) { return 0; }
void T_Hotspot::Exit() {}
int T_Hotspot::Update(int, int, int) { return 0; }
int T_Hotspot::Do() { return 0; }
int T_Hotspot::SetState(int) { return 0; }
int T_Hotspot::GetState() { return 0; }
void Hotspot::QueueEvents(Queue*) {}

// EventList stub
#include "EventList.h"
void EventList::InsertNode(void* data) { LinkedList::InsertNode(data); }

// SC_DodgeOrville / SC_PRHotSpot external stubs
void __fastcall FUN_0044c740(void*) {}
void __fastcall FUN_00444af0(void*) {}
void __fastcall FUN_0041b3a0(void*) {}
int __fastcall FUN_0041b790(void*) { return 0; }
void __fastcall FUN_0041b6e0(void*, int, int) {}
void __fastcall FUN_0041b5a0(void*) {}
void __fastcall FUN_00425550(void*, int, int) {}
void __fastcall FUN_00449520(void*) {}
void __fastcall FUN_00425100(void*, int, int, int) {}
void __fastcall FUN_0042b0f0(void*) {}

extern "C" {
    void FUN_00455040(FILE*) {}
    FILE* FUN_00455110(char*, char*) { return 0; }
}

int FUN_00425fc0(char*) { return 0; }

// SC_DodgeOrville globals
struct HitBounds { int minVal; int maxVal; };
HitBounds g_HitBounds_00473260[3];
struct BombData { int data[6]; };
BombData g_BombData_00473278[6];
int g_AnimStates_0046ac30[5];
int g_LastBombDir_0046ac44;
POINT g_CursorPos_00473308;
int rand_00454920() { return rand(); }
