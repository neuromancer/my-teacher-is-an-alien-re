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

// CRT wrappers referenced by FlagArray and FileSystem
int CrtFileRead(void* buf, int size, int count, FILE* stream) {
    return fread(buf, size, count, stream);
}

void CrtFileClose(FILE* stream) {
    fclose(stream);
}

char* GetFullPath(char* dst, const char* src, unsigned int maxlen) {
    return _fullpath(dst, src, maxlen);
}

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

int g_ProjectileHits_0043d150 = 0;

// FUN_00416ba0 -> EngineSubsystems.cpp
// DrawLine, DrawCircle, FUN_00424176 -> Graphics.cpp

// ============================================================================
// AssetList helper stubs
// ============================================================================

extern "C" {
void __fastcall AssetCleanup(void* thisPtr) {}
void __fastcall AssetNodeDelete(void* thisPtr, int param) {}
}

// Handler31 helper functions
void __fastcall FUN_004148f0(void* param) {}
void* __fastcall FUN_00418540(void* param) { return 0; }

// ============================================================================
// SC_Detention method stubs (called from AddMessage/Exit)
// ============================================================================
#include "SC_Detention.h"

// SC_Detention::InitDetention -> SC_Detention.cpp
// SC_Detention::GoToClass -> SC_Detention.cpp
// SC_Detention::HandleInput3C -> SC_Detention.cpp
// SC_Detention::ProcessDetention -> SC_Detention.cpp
// SC_Detention::HandleInput3F -> SC_Detention.cpp
// SC_Detention::SetupDetentionState -> SC_Detention.cpp
// SC_Detention::DrawDetention -> SC_Detention.cpp
void SC_Detention::HandleCombat() {}
// SC_Detention::HandleInput40 -> SC_Detention.cpp
void SC_Detention::HandlePractice() {}
void SC_Detention::ResetAnimations() {}
void SC_Detention::ProcessPeriodAction(int param) {}

// ============================================================================
// SCI_Inventory stubs
// ============================================================================
#include "SCI_Inventory.h"
#include "LinkedList.h"

InvSlotItem::~InvSlotItem() {}
InvPanel::InvPanel() {}
InvPanel::~InvPanel() {}

LinkedList* g_InventoryList = 0;

class InvListItem {
public:
    void Cleanup();
};
void InvListItem::Cleanup() {}

void SCI_Inventory::DisplayPanels(int param) {}
void SCI_Inventory::ProcessInventory() {}
void* SCI_Inventory::FindItem(int itemID) { return 0; }
int* SCI_Inventory::FindItemInList(int itemID) { return 0; }

// Global variables for SCI_Inventory
void* DAT_0046aa24 = 0;
void* DAT_0046aa18 = 0;
void* DAT_0046a6e4 = 0;
int DAT_00473334 = 0;
int DAT_004733e8 = 0;
void* DAT_0046aa08 = 0;
void* DAT_0046a6dc = 0;
char* DAT_0046aa00 = 0;

// Full game globals for HandleSystemMessage debug display
#include "GameState.h"
GameState* g_GameState_0046aa30 = 0;   // DAT_0046aa30
GameState* g_StringTable_0046aa34 = 0; // DAT_0046aa34

extern "C" void WriteToLog(const char* format, ...) {}  // FUN_00425d70 stub
void __fastcall FUN_00426a90(void* self) {}             // FUN_00426a90 - reset input state

// Functions referenced by SCI_Inventory
void __fastcall FUN_00432da0(void* self) {}
void __fastcall FUN_00421bc0(void* self) {}
void* __fastcall FUN_004407c0(void* self) { return 0; }
void __fastcall FUN_0040c6e0(void* self) {}

extern "C" {
void FUN_00444d90(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {}
char* FUN_0044e530(int handle) { return 0; }
}

// Class method stubs
class InvItemObj {
public:
    void Display(int x, int y, int visible);
    void Reset();
    void HandleMsg(int* msg);
};
void InvItemObj::Display(int x, int y, int visible) {}
void InvItemObj::Reset() {}
void InvItemObj::HandleMsg(int* msg) {}

class SelectionDraw {
public:
    void DrawSelection(int x1, int y1, int x2, int y2, int p5, int p6, int p7);
};
void SelectionDraw::DrawSelection(int x1, int y1, int x2, int y2, int p5, int p6, int p7) {}

struct HitPoint {
    int x;
    int y;
    HitPoint(int* ptr) { x = ptr[0]; y = ptr[1]; }
    ~HitPoint();
};
HitPoint::~HitPoint() {}

#include "Sprite.h"
void Sprite::ResetAnimation(int param1, int param2) {}

// ============================================================================
// RunGame helper class stubs (declared in main.cpp)
// ============================================================================

// QuestionInit - Parser-derived class, constructor + destructor stubs
class QuestionInit : public Parser {
public:
    QuestionInit(const char* filename);
    ~QuestionInit();
};
QuestionInit::QuestionInit(const char* filename) {}
QuestionInit::~QuestionInit() {}

// SoundTracker
class SoundTracker {
public:
    SoundTracker(int param);
    ~SoundTracker();
};
SoundTracker::SoundTracker(int param) {}
SoundTracker::~SoundTracker() {}

// GameLoopHelper (animation cache at 0x46a6f0)
class GameLoopHelper {
public:
    void PostProcess();
    ~GameLoopHelper();
    void AddAnimation(char* name, int handle);    // 0x41A7A0
    void RemoveAnimation(int handle);             // 0x41A8D0
};
GameLoopHelper::~GameLoopHelper() {}
void GameLoopHelper::PostProcess() {}
void GameLoopHelper::AddAnimation(char* name, int handle) {}
void GameLoopHelper::RemoveAnimation(int handle) {}

// ============================================================================
// StringTable::TestStrings stub
// ============================================================================
#include "StringTable.h"
void StringTable::TestStrings(void* textMgr, int count) {}

// ============================================================================
// CDData stubs
// ============================================================================
#include "CDData.h"
CDData::~CDData() {}
// CDData::Setup -> CDData.cpp

// ============================================================================
// SC_Cinematic + SC_SelectHotSpot link stubs
// ============================================================================

// --- extern "C" globals ---
extern "C" {
    int DAT_0046a6ec = 0;
    void* DAT_0046aa10 = 0;
    void* DAT_0046aa14 = 0;
    void* DAT_0046aa30 = 0;
    int DAT_0046ac04 = 0;
    char DAT_00473400 = 0;
    int (__stdcall *DAT_0047655c)(int) = 0;
    // Smack function pointer table (populated at runtime from SMACKW32.DLL)
    void (__stdcall *DAT_00476538)(int, void*, unsigned int, unsigned int) = 0; // SmackColorRemap
    unsigned int (__stdcall *DAT_00476540)(int) = 0;                            // SmackDoFrame
    void (__stdcall *DAT_00476544)(int) = 0;                                    // SmackNextFrame
    unsigned int (__stdcall *DAT_00476548)(int, unsigned int) = 0;              // SmackGoto
    void (__stdcall *DAT_0047654c)(int) = 0;                                    // SmackClose
    void (__stdcall *DAT_00476550)(void*, void*, unsigned int) = 0;             // SmackBufferNewPalette
    int (__stdcall *DAT_00476554)(const char*, unsigned int, unsigned int) = 0; // SmackOpen
    unsigned int (__stdcall *DAT_00476570)(int, unsigned int) = 0;              // SmackSoundOnOff
    // Animation global buffers
    char DAT_00472c70[256] = {0};   // g_AnimFilename
    char DAT_00472cb0[256] = {0};   // g_AnimFilename2
    void FUN_004265a0() {}
}

// --- extern "C" functions ---
extern "C" {
    int* FUN_004205e0() { static int v = 0; return &v; }
    int* FUN_004205f0() { static int v = 0; return &v; }
    void FUN_00425f10() {}
    int FUN_00425fa0(void*) { return 0; }
    void* FUN_004260f0(char* name) { return name; }
    void FUN_004307b0(int) {}
    char* FUN_0044e3e0(int) { return 0; }
    void SetFontPosition(int, int) {}       // 0x4524C2
    void SetFontColor(int) {}               // 0x4525EC
    void DrawFontText(char*, int) {}        // 0x45329B
}

// --- C++ __fastcall stubs ---
void* __fastcall FUN_00403520(void*) { return 0; }
void* __fastcall FUN_004035a0(void*) { return 0; }
void* __fastcall FUN_00403620(void*) { return 0; }
void __fastcall FUN_00401130(void*) {}
void __fastcall FUN_00401c80(void*) {}
void __fastcall FUN_004061e0(void*) {}
void __fastcall FUN_00406cc0(void*, int, void*) {}
void __fastcall FUN_00406fd0(void*, int, int) {}
void __fastcall FUN_004070a0(void*, int, int) {}
void __fastcall FUN_004145f0(void*) {}
void __fastcall FUN_0041dc10(void*) {}
void __fastcall FUN_00424ee0(void*) {}
void __fastcall FUN_004308c0(void*) {}
void __fastcall FUN_00443990(void*) {}
void __fastcall FUN_00443ab0(void*) {}
int __fastcall FUN_00443e30(void*) { return 0; }
void __fastcall FUN_0044c740(void*) {}
void __fastcall FUN_0044c9d0(void*) {}
void* __fastcall FUN_00407180(void*) { return 0; }
void* __fastcall FUN_0041dbe0(void*) { return 0; }
void* __fastcall FUN_004438a0(void*) { return 0; }
int __fastcall FUN_00433ae0(void*, int, char*) { return 0; }
void __fastcall FUN_0042be00(void*) {}
void __fastcall FUN_0042bf00(void*) {}
void __fastcall FUN_004252a0(void*) {}
void __fastcall FUN_00404d70(void*, int, int) {}
void __fastcall FUN_0040b760(void*, int, int) {}
void __fastcall FUN_0044bac0(void*, int, int, int) {}
void* __fastcall FUN_00450b10(void*) { return 0; }
void* __fastcall FUN_0044c660(void*, int, char*) { return 0; }
void __fastcall FUN_00443b90(void*, int, int) {}
void __fastcall FUN_0044d2a0(void*, int, int) {}
void __fastcall FUN_0044c880(void*) {}
void __fastcall FUN_0044d210(void*, int, int, int, int, int) {}
void __fastcall FUN_00420ac0(void*, int, int) {}

// --- C++ __cdecl stubs ---
void __cdecl FUN_00412a50() {}
void Parser::ReportUnknownLabel(char*) {}
void __cdecl FUN_00413e70(void*, int, char*) {}
void __cdecl FUN_00425a90(int, int) {}
void __cdecl FUN_00425c50(char*, ...) {}
void __cdecl FUN_00425d70(char*, ...) {}
void __cdecl FUN_00444e40(void*) {}
void __cdecl FUN_00445450(void*, void*) {}
void* __cdecl FUN_00444a40(void*, int, int, int, int, int, int, int, int, int, int) { return 0; }
char* __cdecl FUN_00426190(char* name) { return name; }

// T_MenuHotspot stub
#include "T_MenuHotspot.h"
int T_MenuHotspot::LBLParse(char*) { return 0; }

// --- C++ global ---
int DAT_00472d58 = 0;
int (__stdcall *DAT_0047652c)(int) = 0;

// --- Class method stubs for SC_Cinematic ---
#include "GameLoop.h"
int GameLoop::ProcessEvents(int) { return 0; }

// VBuffer::Blit stub - declared locally in SC_Cinematic.cpp
// Must add to VBuffer.h to avoid class redefinition

struct CinematicAction {
    void Execute(int);
};
void CinematicAction::Execute(int) {}

// Handler::InitFromMessage stub
#include "Handler.h"
void Handler::InitFromMessage(SC_Message*) {}

// VBuffer stubs (CallBlitter, CallBlitter5 are implemented in VBuffer.cpp)
#include "VBuffer.h"

// PaletteObj::Load stub
class PaletteObj {
public:
    void Load(char*);
};
void PaletteObj::Load(char*) {}

// Non-variadic overloads for SC_SelectHotSpot (different mangling from variadic versions)
void __cdecl FUN_00425c50(char*) {}
void __cdecl FUN_00425d70(char*) {}

// ============================================================================
// SCI_SchoolMenu stubs
// ============================================================================

// extern "C" globals
extern "C" {
    int DAT_0046a190 = 0;
    void* DAT_0046af08 = 0;
    int DAT_00472d20 = 0;
}

// extern "C" functions
extern "C" {
    void FUN_00413e10(void*, char*, char*, ...) {}
    void FUN_00454400(void*) {}
    void* FUN_00454500(int) { return 0; }
    int FUN_00454510(char*, char*, ...) { return 0; }
    int FUN_00454850(char*, char*, ...) { return 0; }
    char* FUN_00454960(char*, char*) { return 0; }
    void FUN_00444e20(void*) {}
}

// C++ globals
int DAT_0046cb90 = 0;
char* DAT_0046cb94 = 0;
int DAT_00473358 = 0;
int DAT_0047337c = 0;

// C++ fastcall stubs
void __fastcall FUN_00420d90(void*) {}
void __fastcall FUN_00424ed0(void*) {}
void __fastcall FUN_00421020(void*) {}
void __fastcall FUN_00421880(void*) {}
void __fastcall FUN_004250e0(void*) {}

// Class method stubs for SCI_SchoolMenu
class GSVal { public: void FUN_00409f20(int); int GetStateValue(int); };
void GSVal::FUN_00409f20(int) {}

class QObj { public: void FUN_00444920(void*); };
void QObj::FUN_00444920(void*) {}

class SprInit { public: void FUN_00420ce0(int); };
void SprInit::FUN_00420ce0(int) {}

// ============================================================================
// SC_Slime stubs
// ============================================================================
#include "SC_Slime.h"

void SC_Slime::Update(int, int) {}
SlimeDim::~SlimeDim() {}
SlotPair::SlotPair() { field_0 = 0; field_4 = 0; }

int DAT_00468bbc = 0;

// SlimeTable stubs (class defined locally in SC_Slime.cpp)
class SlimeTable {
    int fields[3];
public:
    SlimeTable();
    ~SlimeTable();
    void Init(int);
    void FUN_00425620(int, char*, int);
    void FUN_004254a0(int);
};
SlimeTable::SlimeTable() {}
SlimeTable::~SlimeTable() {}
void SlimeTable::Init(int) {}
void SlimeTable::FUN_00425620(int, char*, int) {}
void SlimeTable::FUN_004254a0(int) {}

// TimerWrapper/TimerObj stubs removed — now using TimeOut class

void __fastcall FUN_00425200(void*) {}
void __fastcall FUN_00425490(void*) {}
// FUN_00421930 stub removed — now TimeOut::~TimeOut
void __fastcall FUN_00426ce0(void*, int, int) {}

extern "C" {
    void FUN_004309c0(void*) {}
}

// ============================================================================
// SC_Message method stubs (engine message class, used by many systems)
// ============================================================================

SC_Message::SC_Message(int, int, int, int, int, int, int, int, int, int) {}
SC_Message::~SC_Message() {}
int SC_Message::LBLParse(char*) { return 0; }
void SC_Message::Dump(int) {}

// ============================================================================
// SC_Question extern stubs (full game)
// ============================================================================

int __fastcall FUN_0044c350(void*, int, char*) { return 0; }
void __cdecl FUN_00425cb0(char*, ...) {}
int __fastcall FUN_00420a00(void*, int, int) { return 0; }
void __fastcall FUN_00420a50(void*, int, int) {}
void __fastcall FUN_00404230(void*, int, char*, int, int, int, int) {}
void __fastcall FUN_00443ed0(void*, int, int) {}

// SC_Question globals
void* DAT_0046a6e0 = 0;
void* DAT_0046a6e8 = 0;
char DAT_00468108[32] = {0};
char DAT_00468168[32] = {0};
char DAT_004690e4[32] = {0};
void* DAT_0046aa38 = 0;

// ============================================================================
// SCI_PracticeRoom / SC_Wahoo / SC_WordSearch link stubs
// ============================================================================

// Missing __fastcall stubs
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
void __fastcall FUN_00409f20(void*, int) {}
void __fastcall FUN_004274c0(void*, int, int) {}
void* __fastcall FUN_00410fb0(void*, int, char*, int) { return 0; }
void __fastcall FUN_004218c0(void*) {}
unsigned int __fastcall FUN_004218e0(void*) { return 0; }
void __fastcall FUN_00411180(void*, int, int) {}
void __fastcall FUN_00404350(void*, int, int, int, int, int, int, int, int) {}

// Missing __cdecl stubs
char* __cdecl FUN_0044e470(char*) { return 0; }

// Missing C++ globals
int DAT_0046ad6c = 0;
void* DAT_0046bbfc = 0;
char* DAT_0046bacc = 0;

// Class method stubs for SC_Wahoo local classes
class InputObj { public: void Refresh(int); };
void InputObj::Refresh(int) {}

class DetectionObj { public: void Render(); };
void DetectionObj::Render() {}

class DetMask { public: unsigned int CheckHit(int, int); };
unsigned int DetMask::CheckHit(int, int) { return 0; }

// SC_WordSearch virtual method stubs
#include "SC_WordSearch.h"
int SC_WordSearch::AddMessage(SC_Message*) { return 0; }
int SC_WordSearch::LBLParse(char*) { return 0; }
