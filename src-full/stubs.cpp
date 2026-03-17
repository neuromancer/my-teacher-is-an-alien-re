#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <share.h>
#include <sys/stat.h>
#include "string.h"
#include "Memory.h"
#include "OnScreenMessage.h"
#include "TimedEvent.h"
#include "globals.h"
#include "ZBufferManager.h"
#include "SC_Question.h"
#include "SC_OnScreenMessage.h"
#include "SCI_SearchScreen.h"
#include "InputManager.h"
#include "MouseControl.h"
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
extern "C" void ShowLoadingScreen(void);
extern "C" void SetVideoRes(int width, int height);
extern "C" char* MakeAudioName(char* baseName);

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
    void* DAT_0046ae54 = 0;
    void* DAT_0046ae58 = 0;
    void* DAT_0046ae5c = 0;
    void* DAT_0046ae60 = 0;
    void* DAT_0046ae64 = 0;
    void* DAT_0046ae68 = 0;
    void* DAT_0046ae6c = 0;
    void* DAT_0046ae70 = 0;
    void* DAT_0046ae74 = 0;
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

// SCI_Inventory globals — typed as their real classes
ZBufferManager* DAT_0046aa24 = 0;    // was void*
MouseControl* DAT_0046aa18 = 0;      // was void*
void* DAT_0046a6e4 = 0;
int DAT_00473334 = 0;
int DAT_004733e8 = 0;
InputManager* DAT_0046aa08 = 0;      // was void*
void* DAT_0046a6dc = 0;
char* DAT_0046aa00 = 0;
// g_InventoryList removed - same global as g_MsgList (DAT_0046a6dc) in main.cpp

// Full game globals — DAT_0046aa30 typed as GameState* (extern "C" definition below)
extern "C" extern GameState* DAT_0046aa30;
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
    GameState* DAT_0046aa30 = 0;
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

// SC_Detention globals
int DAT_00468a18 = 0;
extern "C" void* DAT_0046bf30 = 0;   // Pods palette pointer
void __fastcall FUN_00447790(void*, int, int) {} // SC_SpaceShipNav::ShutDown
int __cdecl FUN_00426ac0() { return 0; }
void __fastcall FUN_00408ee0(void*, int, int) {}

// SC_FireAlarm / SC_Pods globals
int DAT_004685ac = 0;
class SlimeTable;
SlimeTable* DAT_0046bf28 = 0;
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
/* Function start: 0x41A6D0 */
int __cdecl FUN_0041a6d0(void* a, void* b) {
    int diff = *(int*)(*(int*)b + 0x40) - *(int*)(*(int*)a + 0x40);
    if (diff < 1) return 0;
    return 1;
}

// ============================================================================
// SoundTracker stubs and globals
// ============================================================================
#include "SoundTracker.h"
struct SoundPool;
SoundPool* DAT_00469134 = 0;
int DAT_00469128 = 0;
void* DAT_00469138 = 0;    // FilePosCache LRU node
int DAT_0046912c = 0;      // FilePosCache hit counter
int DAT_00469130 = 0;      // FilePosCache miss counter

void SoundTracker::Init() {}
void SoundTracker::Cleanup() {}

// ============================================================================
// Implemented functions (with real code)
// ============================================================================

extern "C" void WriteToLog(const char* format, ...) {}

extern "C" {
// FUN_0044e530 = GetSoundFilename in string.cpp (callers updated)
void FUN_004265a0() { ShowLoadingScreen(); }
} // extern "C"

// ============================================================================
// Remaining __fastcall stubs (unidentified functions)
// ============================================================================

void __fastcall FUN_004148f0(void* param) {}
void* __fastcall FUN_00418540(void* param) { return 0; }
void* __fastcall FUN_00403620(void*) { return 0; }
// FUN_00401c80 = ZBuffer::CleanUpVBuffer / DrawEntry::~DrawEntry (in ZBuffer.cpp/Engine.cpp)
// Still called from: ZBufferManager, SC_Pods, SC_Cinematic, SCI_SchoolMenu, SC_SelectHotSpot
void __fastcall FUN_00401c80(void*) {}
void __fastcall FUN_004061e0(void*) {}
// FUN_00406cc0 = Queue::Add in Queue.cpp
void __fastcall FUN_00406fd0(void*, int, int) {}
void __fastcall FUN_004070a0(void*, int, int) {}
// FUN_004308c0 = ClearActionList in SC_Cinematic.cpp
void* __fastcall FUN_00407180(void*) { return 0; }
// FUN_0041dbe0 = InitPalette in Palette.cpp
// FUN_0042be00 = InitCombatScreen in SC_Pods.cpp
// FUN_00404d70 = ListNode sdtor (callers: SC_ExtBridge, SC_Rats, ZBufferManager)
void __fastcall FUN_00404d70(void*, int, int) {}
// FUN_00404b80 = LinkedList::GetCurrentData non-inline (callers: SC_ExtBridge)
void* __fastcall FUN_00404b80(void*) { return 0; }
void __fastcall FUN_0040b760(void*, int, int) {}
// FUN_0044bac0 = mCNavigator::SetNavParams in mCNavigator.cpp
void* __fastcall FUN_00450b10(void*) { return 0; }
void __fastcall FUN_0044c880(void*) {}
// FUN_00420ac0 = FlagArray::ClearFlag in FlagArray.cpp
// FUN_004128f0 = TimedEventPool dtor in Parser.cpp (callers updated)
// FUN_00432da0 = MouseControl::DrawCursor in MouseControl.cpp
// FUN_00421bc0 = T_MenuHotspot::Update in SC_SaveLoad.cpp
void* __fastcall FUN_004407c0(void* self) { return 0; }
// FUN_0040c6e0 = T_Object::~T_Object in T_Object.cpp (callers updated)
// FUN_004309a0 = Handler::CopyCommandData in Handler.cpp (no remaining callers)
void __fastcall FUN_0042b100(void*) {}
void __fastcall FUN_0042b270(void*) {}
void __fastcall FUN_00429c10(void*) {}
void* __fastcall FUN_0042bc50(void*) { return 0; }
void __fastcall FUN_00420f00(void*) {}
void __fastcall FUN_00429df0(void*) {}
int __fastcall FUN_0042a010(void*, int, void*) { return 0; }
void* __fastcall FUN_00429b60(void*, int, int, void*) { return 0; }
void* __fastcall FUN_00420ce0(void*, int, int) { return 0; }
// FUN_00410fd0 = VBuffer::~VBuffer in VBuffer.cpp
void __fastcall FUN_004274c0(void*, int, int) {}
// FUN_00410fb0 = VBuffer::VBuffer(char*, int) in VBuffer.cpp (callers still use __fastcall extern)
void* __fastcall FUN_00410fb0(void*, int, char*, int) { return 0; }
// FUN_00411180 = VBuffer::ClearScreen in VBuffer.cpp
void __fastcall FUN_00420d90(void*) {}
// FUN_00421020 = ProcessSpriteActions in SCI_SchoolMenu.cpp
// FUN_00421880 = Timer constructor in Timer.cpp
// FUN_00425490 = SlimeTable::~SlimeTable in SC_Slime.cpp
// Still needed as extern by SCI_PracticeRoom, SC_FireAlarm, SC_Pods
void __fastcall FUN_00425490(void*) {}
// FUN_00425480 = SlimeTable::SlimeTable in SC_Slime.cpp
// Still needed as extern by SC_FireAlarm, SC_Pods
void* __fastcall FUN_00425480(void*) { return 0; }

// FUN_004254a0 = SlimeTable::Allocate in SC_Slime.cpp
void* __fastcall FUN_00440860(void*) { return 0; }
// FUN_00421920 = InitTimeOut in TimeOut.cpp
void __fastcall FUN_00427880(void*) {}
void __fastcall FUN_00403fd0(void* self, int, void* data, int priority, int x, int y, int mode, int scale_lo, int scale_hi, int left, int top, int right, int bottom) {
    double scale;

    ((int*)&scale)[0] = scale_lo;
    ((int*)&scale)[1] = scale_hi;
    ((ZBufferManager*)self)->DrawVBufferRegion(data, priority, x, y, mode, scale, left, top, right, bottom);
}
void __fastcall FUN_0044cb40(void*, int, int, int) {}
// FUN_004127c0 = Parser::Parser in Parser.cpp
void __fastcall FUN_00407b60(void*) {}
// FUN_004047c0 = ZBufferManager::ProcessMessage in ZBufferManager.cpp
// Legacy alias for the remove-current wrapper used by older callers.
void* __fastcall FUN_004036a0(void* self) { return ((LinkedList*)self)->RemoveCurrent(); }
// FUN_00445970 = T_Hotspot::StopAll in Hotspot.cpp (removed stub, callers updated)
// FUN_004459a0 = T_Hotspot::DoItem in Hotspot.cpp (stub kept: __fastcall caller loads EDX=0, __thiscall would not)
void __fastcall FUN_004459a0(void*, int, int) {}
// FUN_004279a0 = ZBuffer::ResetItems in ZBuffer.cpp
void __fastcall FUN_00404350(void*, int, int, int, int, int, int, int, int) {}
// FUN_0044c350 = StringTable::GetString in StringTable.cpp
// FUN_00420a00 = FlagArray::GetFlag in FlagArray.cpp
// FUN_00420a50 = FlagArray::SetFlag in FlagArray.cpp
void __fastcall FUN_00404b90(void*) {}
void __fastcall FUN_00404230(void*, int, char*, int, int, int, int) {}

// ============================================================================
// Remaining __cdecl stubs
// ============================================================================

// FUN_00412a50 = Parser::LBLParse/ReportUnknownLabel in Parser.cpp (callers updated)
// FUN_00413e70 = Parser::ProcessFile in Parser.cpp (callers updated)
// FUN_00425a90 = SetVideoRes in VBuffer.cpp (callers updated)
// Legacy alias for the renamed enqueue wrapper in SpriteAction.cpp.
void __cdecl FUN_00444e40(void* action) { EnqueueSpriteAction(action); }
// ParseSpriteAction (was FUN_00445450) moved to SC_Message.cpp
// FUN_00444a40 = SpriteAction::SpriteAction constructor in SpriteAction.cpp (stub kept: callers use __cdecl, real is __thiscall constructor)
void* __cdecl FUN_00444a40(void*, int, int, int, int, int, int, int, int, int, int) { return 0; }
// FUN_00426190 = ResolveAssetPath in CDData.cpp

// Path resolution helpers (stubbed until CDData is fully implemented)
void* DAT_0046aa1c = 0;
int __cdecl FUN_004341f0(char*) { return 0; }
void __cdecl FUN_004342d0(char*, int) {}
void __fastcall FUN_00433230(void*, int, char*) {}

// FUN_00425cb0 = ShowMessage in string.cpp
// FUN_0044ccf0 = Sprite::RenderAt in Sprite.cpp (stub kept: callers use __cdecl, real is __thiscall method)
int __cdecl FUN_0044ccf0(int, int, int, int) { return 0; }
void __cdecl FUN_004344b0() {}
void __cdecl FUN_00434030(void*, int) {}
char* __cdecl FUN_0044e470(char* baseName) { return MakeAudioName(baseName); }
void __cdecl FUN_00425bc0(char* param_1, char* param_2, int param_3) { ExtractQuotedString(param_1, param_2, param_3); }

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
    // FUN_004307b0 = SmackSummary display in SC_Cinematic.cpp
    void __stdcall SmackSummary(void*, void*) {}
    // FUN_0044e3e0 = GetCinematicFilename in SC_Cinematic.cpp
    void SetFontPosition(int, int) {}
    void SetFontColor(int) {}
    void DrawFontText(char*, int) {}
    // FUN_00413e10 = ParseFile in Parser.cpp (callers updated)
    // FUN_004309c0 = Handler::WriteMessageAddress in Handler.cpp (callers updated)
}

// ============================================================================
// Remaining IconBar.obj stubs
// ============================================================================

// FUN_00412a50 = Parser::LBLParse in Parser.cpp (callers updated)

#include "GameLoop.h"
// FUN_0044b7e0 = mCNavNode::CallActivateNeighbor in mCNavNode.cpp (callers updated)

// CombatViewport = Viewport class (SetClip=SetDimensions, SetOffset=SetDimensions2, Refresh=SetCenter, SetSize=SetAnchor)
// Methods implemented in Viewport.cpp
// CombatWeapon = Palette class (SetPriority = SetPalette) — implemented in Palette.cpp
// CombatSprite2 = Sample class (SetVolume = Play) — implemented in Sample.cpp

// ============================================================================
// Unresolved externals from new implementations
// ============================================================================

// Hotspot/T_Hotspot stubs moved to Hotspot.cpp

// EventList stub
#include "EventList.h"
void EventList::InsertNode(void* data) { LinkedList::InsertNode(data); }

// SC_DodgeOrville / SC_PRHotSpot external stubs
void __fastcall FUN_0044c740(void*) {}
void __fastcall FUN_00444af0(void*) {}
// FUN_0041b3a0 = HotspotAction::~HotspotAction in HotspotAction.cpp (removed, callers updated)
// FUN_0041b790 = HotspotAction::CheckConditions in HotspotAction.cpp (removed, callers updated)
// FUN_0041b6e0 = HotspotAction::Update in HotspotAction.cpp (removed, callers updated)
// FUN_0041b5a0 = HotspotAction::Reset in HotspotAction.cpp (removed, callers updated)
void __fastcall FUN_00425550(void*, int, int) {}
void __fastcall FUN_00449520(void*) {}
void __fastcall FUN_00425100(void*, int, int, int) {}
void __fastcall FUN_0042b0f0(void*) {}

// SC_CombatBase extern stubs
int __fastcall FUN_0044be70(void*, int) { return 0; }
int FUN_0042c9d0(void*) { return 0; }
void __fastcall FUN_00442940(void*, int, int) {}
void* __fastcall FUN_00443660(void*, int) { return 0; }
void __fastcall FUN_00409730(void*, int, int) {}
// SC_CombatBase::Initialize extern stubs (constructors)
void* __fastcall FUN_00425170(void*, int, int) { return 0; }
void* __fastcall FUN_00434660(void*, int) { return 0; }
void* __fastcall FUN_0044b8d0(void*, int) { return 0; }
void* __fastcall FUN_0041dbe0(void*, int) { return 0; }
void* __fastcall FUN_00408fb0(void*, int) { return 0; }
void* __fastcall FUN_004432f0(void*, int) { return 0; }
void* __fastcall FUN_0040c5e0(void*, int) { return 0; }
void* __fastcall FUN_004454f0(void*, int) { return 0; }
void __fastcall FUN_004127c0(void*, int) {}
// SC_CombatBase::CleanupAll extern stubs (destructors)
void __fastcall FUN_0044b950(void*, int) {}
void __fastcall FUN_0041dc10(void*, int) {}
void __fastcall FUN_00443360(void*, int) {}
void __fastcall FUN_00434740(void*, int) {}
void __fastcall FUN_00409020(void*, int) {}
void __fastcall FUN_00425200(void*, int) {}
// mCNavigator extern
// FUN_0044aec0 = mCNavNode::~mCNavNode (moved to mCNavNode.cpp)

// FUN_00455040 = fclose (CRT)
// FUN_00455110 = fsopen (string.cpp) = _fsopen(filename, mode, _SH_DENYNO)

// FUN_00425FC0 = GetFileSize (moved to string.cpp)

// SC_DodgeOrville globals
struct HitBounds { int minVal; int maxVal; };
HitBounds g_HitBounds_00473260[3];
struct BombData { int data[6]; };
BombData g_BombData_00473278[6];
int g_AnimStates_0046ac30[5];
int g_LastBombDir_0046ac44;
POINT g_CursorPos_00473308;
int rand_00454920() { return rand(); }
