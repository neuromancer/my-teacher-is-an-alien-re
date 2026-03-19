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
#include "FlagArray.h"
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

// Graphics globals
int DAT_0043748c = 0;     // HDC for text rendering
int DAT_004374ae = 640;   // max X position for text
int DAT_00437608[256];    // palette color table

// EngineB globals
void* DAT_00435f1c = 0;

class EngineInfoParser;
class Sprite;
class Viewport;
class TargetList;
class CombatSprite;
class Parser;
class Weapon;
class Palette;
class SoundList;
class ScoreDisplay;
class mCNavigator;
struct HotspotListData;

extern "C" {
    EngineInfoParser* DAT_0046ae4c = 0;
    Sprite*           DAT_0046ae50 = 0;
    Viewport*         DAT_0046ae54 = 0;
    TargetList*       DAT_0046ae58 = 0;
    CombatSprite*     DAT_0046ae5c = 0;
    Weapon*           DAT_0046ae60 = 0;
    Palette*          DAT_0046ae64 = 0;
    SoundList*        DAT_0046ae68 = 0;
    ScoreDisplay*     DAT_0046ae6c = 0;
    mCNavigator*      DAT_0046ae70 = 0;
    HotspotListData*  DAT_0046ae74 = 0;
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
// DAT_0046a6dc removed — same as g_MsgList in main.cpp
char* DAT_0046aa00 = 0;

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
#include "VBuffer.h"
extern "C" {
    int DAT_0046a6ec = 0;
    void* DAT_0046aa10 = 0;
    VBuffer* DAT_0046aa14 = 0;
    GameState* DAT_0046aa30 = 0;
    // DAT_0046ac04 = g_WaitForInputValue_004373bc in globals.cpp (last key pressed)
    char DAT_00473400 = 0;
    char DAT_00472c70[256] = {0};
    char DAT_00472cb0[256] = {0};
}

// SC_Question globals
class StringTable;
StringTable* DAT_0046a6e0 = 0;
FlagArray* DAT_0046a6e8 = 0;
char DAT_00468108[32] = {0};
char DAT_004690e4[32] = {0};
GameState* DAT_0046aa38 = 0;

// SC_Wahoo / SC_WordSearch / SCI_PracticeRoom globals
int DAT_0046ad6c = 0;
void* DAT_0046bbfc = 0;
class Weapon;
Weapon* DAT_00468ef0 = 0;
char* DAT_0046bacc = 0;

// SC_Detention globals
int DAT_00468a18 = 0;
extern "C" void* DAT_0046bf30 = 0;   // Pods palette pointer
void __fastcall FUN_00447790(void*, int, int) {} // SC_SpaceShipNav::ShutDown
void* __fastcall FUN_0044c660(void*, int, char*) { return 0; } // Sprite ctor wrapper
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
// DAT_0046AA24 removed — duplicate of DAT_0046aa24 (MSVC linker is case-insensitive)

// HotspotAction globals
void* DAT_0046aa34 = 0;

// SCI_IconBarModule / SC_Detention globals
int DAT_00468764 = 0;

// ============================================================================
// EngineB proxy class stubs
// ============================================================================

// TargetListFull proxy — replaced with TargetList::ProcessTargets
// InputManagerFull proxy — replaced with InputManager::Refresh
// PaletteLoader proxy — replaced with Palette::LoadFile

// WeaponParser proxy removed — Weapon::UpdateProjectiles implemented in Weapon.cpp

// InputManager::Refresh → InputManager.cpp

// Engine COMDAT thunks (needed by SC_DodgeOrville)
// FUN_00449320 = SC_Combat::ShutDown — callers updated
// FUN_00449400 = SC_Combat::AddMessage — callers updated
// FUN_004494e0 = SC_Combat::ProcessLose (callers updated)

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

// FUN_00420CE0 = T_MenuHotspot(int) ctor — implemented in T_MenuHotspot.cpp


// ============================================================================
// GameLoopHelper helper stubs
// ============================================================================
// FUN_0041A6D0 = CompareNodePriority — moved to GameLoopHelper.cpp
// FUN_0041A6F0 = FreeNodeData — moved to GameLoopHelper.cpp

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
int DAT_00469140 = 0;
int DAT_0046913c = 0;
// SoundTracker string globals — replaced with inline strings

// FUN_00455130 = remove() — callers updated
// FUN_00425fa0 = FileExists — callers updated
// FUN_00455110 = _fsopen — callers updated
// FUN_00426400 = internal_ReadLine — callers updated
// FUN_00426490 = EncryptAndWrite — callers updated
// FUN_00454850 = sscanf — callers updated
// FUN_00454D20 = _findfirst — callers updated
// FUN_00454E70 = _findnext — callers updated
// FUN_00455040 = fclose — callers updated
// FUN_004550B0 = fgetpos — callers updated
// FUN_00454510 = sprintf — callers updated
// FUN_00421880 = Timer::Timer — callers updated
// FUN_004218c0 = Timer::Reset — callers updated
// FUN_004218e0 = Timer::Update — callers updated
// FUN_00412210 = FilePosCache::Store — callers updated

// Parser globals
char DAT_00469168[160] = {0};

// ============================================================================
// Implemented functions (with real code)
// ============================================================================

extern "C" void WriteToLog(const char* format, ...) {}

extern "C" {
// FUN_0044e530 = GetSoundFilename in string.cpp (callers updated)
// FUN_004265a0 = ShowLoadingScreen — all callers already use the proper name
} // extern "C"

// ============================================================================
// Remaining __fastcall stubs (unidentified functions)
// ============================================================================

// FUN_004148f0 = SC_Question::~SC_Question — callers updated to use dtor directly
// FUN_00418540 = LinkedList::RemoveCurrent COMDAT — callers updated
// FUN_00403620 = LinkedList::RemoveCurrent COMDAT — callers updated
// FUN_00401c80 = ZBuffer::CleanUpVBuffer — callers updated
void __fastcall FUN_004061e0(void*) {}  // SEH wrapper dtor (auto-complete, still needed for linking)
// FUN_004218b0 = Timer dtor — callers updated to use delete
// FUN_00406cc0 = Queue::Add in Queue.cpp
// FUN_00406fd0 = Queue::InsertAtCurrent in Queue.cpp — callers updated
// FUN_004070a0 = Queue::Push in Queue.cpp — callers updated
// FUN_004308c0 = ClearActionList in SC_Cinematic.cpp
// FUN_00407180 = LinkedList::RemoveCurrent COMDAT — callers updated
// FUN_0041dbe0 = InitPalette in Palette.cpp
// FUN_0042be00 = InitCombatScreen in SC_Pods.cpp
// FUN_00404d70 = ListNode sdtor (callers: SC_ExtBridge, SC_Rats, ZBufferManager)
// FUN_00404d70 = ListNode sdtor — callers updated to use delete
// FUN_00404b80 = LinkedList::GetCurrentData non-inline (callers: SC_ExtBridge)
// FUN_00404b80 = LinkedList::GetCurrentData — callers updated
// FUN_0040b760 = VBuffer sdtor — callers updated to use delete
// FUN_0044bac0 = mCNavigator::SetNavParams in mCNavigator.cpp
void* __fastcall FUN_00450b10(void*) { return 0; }  // unknown ctor (SC_Rats)
// FUN_0044c880 = Sprite::InitAnimation in Sprite.cpp — no remaining callers
// FUN_004407c0 = RemoveCurrent COMDAT — callers updated
// FUN_00429c10 = SC_PRHotSpot::~SC_PRHotSpot in SC_PRHotSpot.cpp — callers updated
// FUN_00429df0 = SC_PRHotSpot::Update in SC_PRHotSpot.cpp — callers updated
// FUN_00420f00 = T_MenuHotspot::Update in T_MenuHotspot.cpp — callers updated
void __fastcall FUN_0042b100(void*) {}  // SCI_PracticeRoom helper (unknown)
void __fastcall FUN_0042b270(void*) {}  // SCI_PracticeRoom helper (unknown)
void* __fastcall FUN_0042bc50(void*) { return 0; }  // SCI_PracticeRoom helper (unknown)
// FUN_0042a010 = SC_PRHotSpot::AddMessage — callers updated to use method call
// FUN_00429b60 = SC_PRHotSpot ctor — callers updated to use new
// FUN_00420ce0 = T_MenuHotspot(int) ctor — callers updated to use new
// FUN_004274c0 = RockThrower ctor — callers updated to use new
// FUN_00410fb0 = VBuffer ctor — callers updated to use new
// FUN_00420d90 = T_MenuHotspot::~T_MenuHotspot — callers updated to use delete
// FUN_00425490 = SlimeTable::~SlimeTable — callers updated to use delete
// FUN_00425480 = SlimeTable::SlimeTable — no remaining callers
// FUN_00440860 = PodsEngine ctor — still called via __fastcall
// FUN_00421920 = InitTimeOut in TimeOut.cpp
void __fastcall FUN_00427880(void*) {}
// FUN_00403fd0 = ZBufferManager::DrawVBufferRegion (callers updated to use method call)
// FUN_0044cb40 = Sprite::ResetAnimation (callers updated)
// FUN_004127c0 = Parser::Parser in Parser.cpp
void __fastcall FUN_00407b60(void*) {}
// FUN_004047c0 = ZBufferManager::ProcessMessage in ZBufferManager.cpp
// Legacy alias for the remove-current wrapper used by older callers.
// FUN_004036a0 = LinkedList::RemoveCurrent — last caller updated
// FUN_00445970 = T_Hotspot::StopAll in Hotspot.cpp (removed stub, callers updated)
// FUN_004459a0 = T_Hotspot::DoItem in Hotspot.cpp (stub kept: __fastcall caller loads EDX=0, __thiscall would not)
void __fastcall FUN_004459a0(void*, int, int) {}
// FUN_004279a0 = ZBuffer::ResetItems in ZBuffer.cpp
// FUN_00404350 = ZBufferManager::DrawRect — callers updated
// FUN_00404230 = ZBufferManager::DrawText — callers updated
// FUN_00404b90 = ZBufferManager::UpdateScreen — no callers

// ============================================================================
// Remaining __cdecl stubs
// ============================================================================

// FUN_00412a50 = Parser::LBLParse/ReportUnknownLabel in Parser.cpp (callers updated)
// FUN_00413e70 = Parser::ProcessFile in Parser.cpp (callers updated)
// FUN_00425a90 = SetVideoRes in VBuffer.cpp (callers updated)
// Legacy alias for the renamed enqueue wrapper in SpriteAction.cpp.
// FUN_00444e40 = EnqueueSpriteAction — all callers already use the proper name
// ParseSpriteAction (was FUN_00445450) moved to SC_Message.cpp
// FUN_00444a40 = SpriteAction::SpriteAction constructor in SpriteAction.cpp (callers updated to use placement new)
// FUN_00426190 = ResolveAssetPath in CDData.cpp

// Path resolution
void* DAT_0046aa1c = 0;
// DAT_0046b78c removed — duplicate of definition in main.cpp
int DAT_0046b784 = 0;      // cache hit counter
int DAT_0046b788 = 0;      // cache miss counter
// DAT_0046b790, DAT_00473440, DAT_00473444 defined in main.cpp
// FUN_004341f0 = FileCacheLookup — moved to CDData.cpp
// FUN_004342d0 = FileCacheRegister — moved to CDData.cpp
// FUN_00433230 = CDData::ResolvePath — moved to CDData.cpp
// FUN_00434030 = FileCacheEntryCleanup — moved to CDData.cpp

// FUN_004560f0 = _splitpath — callers updated
// FUN_004603d0 = _chdir — callers updated
// FUN_004602d0 = _mkdir — callers updated
// FUN_00426220 = CopyFileContent — callers updated


// SC_Combat base method stubs
#include "SC_Combat.h"
// FUN_00432da0 = MouseControl::DrawCursor (removed, callers updated)

// ProcessEvents helper stubs
// FUN_0042d1a0 = TimedEventPool::Pop — callers updated
// FUN_00444a40/FUN_00444920/FUN_00444af0 = SpriteAction ctor/CopyFrom/dtor (callers updated)

// FUN_0044e3e0 = MakeAnimName — moved to string.cpp
// FUN_0044e320 = MakeSoundName — moved to string.cpp
// FUN_0044d420 = Sprite::SetSpriteLogic — moved to Sprite.cpp as class member

// FUN_0044ccf0 = Sprite::Do — callers updated to use method calls
// FUN_004344b0 = FileCacheCleanup — moved to CDData.cpp
// FUN_00434030 = cache entry cleanup — moved to CDData globals section above

// ============================================================================
// Remaining extern "C" stubs
// ============================================================================

extern "C" {
    // FUN_004205e0 = GetScreenWidth in GameWindow.cpp
    // FUN_004205f0 = GetScreenHeight in GameWindow.cpp
    // FUN_00425f10 = BlankScreen in Graphics.cpp
    // GetWindowHeight, GetWindowedModeFlag moved to GameWindow.cpp
    // FUN_00425fa0 = FileExists in main.cpp
    // FUN_004260f0 = FormatAssetPath in CDData.cpp
    // FUN_004307b0 = ShowSmackSummary in SC_Cinematic.cpp
    void __stdcall SmackSummary(void*, void*) {}
    // FUN_0044e3e0 = GetCinematicFilename in SC_Cinematic.cpp
    // SetFontPosition, SetFontColor, DrawFontText moved to Graphics.cpp
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

// External function stubs (still referenced by other .cpp files)
// Extern stubs still referenced by callers using raw __fastcall convention
// FUN_0044c740 = Sprite::~Sprite — callers updated to use delete/dtor
// FUN_00444af0 = SpriteAction::~SpriteAction — callers updated to use delete/dtor
void __fastcall FUN_0042b0f0(void*) {}                           // SC_PRHotSpot helper
// FUN_00409730 = CombatSprite::ProcessFrame — callers updated
// FUN_0044be70 = mCNavigator::Update — callers updated
// FUN_00443360 = TargetList dtor — callers updated to use delete
// FUN_00434740 = EngineInfoParser dtor — callers updated to use delete
// FUN_00409020 = CombatSprite dtor — callers updated to use delete

// Not-yet-typed ctors (classes not fully defined)
// FUN_00421880 = Timer ctor — callers updated to use new Timer()
// FUN_00418690 = NavCrystal ctor — callers updated to use new NavCrystal()
// FUN_0041dbf0 = Palette(char*) ctor — callers updated to use new Palette(name)
// FUN_00421a50 = T_MenuHotspot(char*, int*) ctor — callers updated to use new
// FUN_0044d210 = Sprite::ConfigRange — callers updated to use method call

// SC_ExtBridge extern stubs
// FUN_0044c9d0 = Sprite::StopAnimationSound — callers updated
// FUN_00431030 = GameEngine::ProcessMessage — callers updated
// FUN_00444e40 already defined above as __cdecl (SpriteAction::Dispatch)
// FUN_0041dcc0 = Palette::LoadFile — no remaining callers

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
// rand_00454920 = rand() — callers updated to use rand() directly
// FUN_00433cb0 = GameState::SetFromAction — implemented in GameState.cpp, callers updated
// FUN_00425620 = SlimeTable::LoadEntry — callers updated
// FUN_004254a0 = SlimeTable::Allocate — callers updated
// FUN_00445450 = ParseSpriteAction — callers updated
// FUN_00449480 = SC_Combat::Update — callers updated to use SC_Combat::Update()
int DAT_00472be0 = 0;
int DAT_00472be4 = 0;
int DAT_00472be8 = 0;
int DAT_00472bec = 0;
// FUN_004250e0 = Sample::~Sample — callers updated
int DAT_0046bcd0[3] = {0};
extern "C" {
int __cdecl FUN_004260a0(char*) { return 0; }
void FUN_00434520() {}
void FUN_004345b0() {}
}
int DAT_004719c0 = 0;
void __fastcall FUN_00427390(void*) {}
