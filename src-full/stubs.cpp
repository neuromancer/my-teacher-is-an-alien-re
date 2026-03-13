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
#include "GameEngine.h"
#include "Timer.h"
#include "SCI_Dialog.h"
#include "Queue.h"
#include "Sound.h"
#include "SpriteAction.h"
#include "GameState.h"
#include "LinkedList.h"

extern "C" int DAT_0046a6ec;
extern "C" int ProcessMessages();
extern Timer g_leftClickTimer;
extern Timer g_rightClickTimer;

// ============================================================================
// CRT wrappers referenced by FlagArray and FileSystem
// ============================================================================

extern "C" {

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

// CRT internal function
void CleanupObjectArray(void* a, int b) {}

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
LinkedList* g_InventoryList = 0;

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
    int DAT_0046ac04 = 0;
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

// ============================================================================
// AssetList helper stubs
// ============================================================================

extern "C" {
void __fastcall AssetCleanup(void* thisPtr) {}
void __fastcall AssetNodeDelete(void* thisPtr, int param) {}
}

// ============================================================================
// Real class method stubs (methods not yet implemented)
// ============================================================================

#include "Sprite.h"
#include "Palette.h"

// GameState methods
void GameState::FUN_00409f20(int) {}
int GameState::GetStateValue(int) { return 0; }
int GameState::FindLabel(char*) { return 0; }
int GameState::FUN_00433bb0(int*) { return 0; }
int GameState::FUN_00432e20(char*) { return 0; }

// InputManager methods
int InputManager::Refresh(int) { return 0; }

// Sprite methods
int Sprite::RenderAt(int, int, int, int) { return 0; }
void Sprite::ConfigRange(int, int, int, int) {}
void Sprite::ConfigStates(int) {}

// Palette methods
void Palette::PlaySound(int) {}
int Palette::CheckSound(int) { return 0; }

// ============================================================================
// Remaining proxy classes (real class unknown)
// ============================================================================

// SCI_SchoolMenu - constructor proxy (0x420CE0)
class SprInit { public: void FUN_00420ce0(int); };
void SprInit::FUN_00420ce0(int) {}

// SCI_Inventory proxy classes
class InvListItem { public: void Cleanup(); };
void InvListItem::Cleanup() {}

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

// SC_Wahoo proxy classes (real class unconfirmed)
class DetectionObj { public: void Render(); };
void DetectionObj::Render() {}

class DetMask { public: int CheckHit(int, int); };
int DetMask::CheckHit(int, int) { return 0; }

// SCI_IconBarModule proxy class (real class unconfirmed)
class CursorControl { public: void SetCursor(int, int); };
void CursorControl::SetCursor(int, int) {}

// SC_Cinematic proxy class
struct CinematicAction { void Execute(int); };
void CinematicAction::Execute(int) {}

// ============================================================================
// RunGame helper class stubs (declared in main.cpp)
// ============================================================================

class QuestionInit : public Parser {
public:
    QuestionInit(const char* filename);
    ~QuestionInit();
};
QuestionInit::QuestionInit(const char* filename) {}
QuestionInit::~QuestionInit() {}

class SoundTracker {
public:
    SoundTracker(int param);
    ~SoundTracker();
};
SoundTracker::SoundTracker(int param) {}
SoundTracker::~SoundTracker() {}

class GameLoopHelper {
public:
    void PostProcess();
    ~GameLoopHelper();
    void AddAnimation(char* name, int handle);
    void RemoveAnimation(int handle);
};
GameLoopHelper::~GameLoopHelper() {}
void GameLoopHelper::PostProcess() {}
void GameLoopHelper::AddAnimation(char* name, int handle) {}
void GameLoopHelper::RemoveAnimation(int handle) {}

// ============================================================================
// T_MenuHotspot stub
// ============================================================================
#include "T_MenuHotspot.h"
int T_MenuHotspot::LBLParse(char*) { return 0; }

// ============================================================================
// Implemented functions (with real code)
// ============================================================================

extern "C" void WriteToLog(const char* format, ...) {}

/* Function start: 0x426A90 */
void __fastcall FUN_00426a90(void* self) {
    InputState* mouse;
    mouse = ((InputManager*)self)->pMouse;
    mouse->ext2 = 0;
    mouse->ext1 = mouse->ext2;
}

extern "C" {
void FUN_00444d90(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    SpriteAction action(a, b, c, d, e, f, g, h, i, j);
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction(&action);
        action.field_34 = 0;
    }
}

/* Function start: 0x426570 */
char* FUN_00426570(char* s1, char* s2) {
    char* p = strstr(s1, s2);
    if (p != 0) {
        p += strlen(s2);
    }
    return p;
}

void FUN_00444e20(void* action) {
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction((SpriteAction*)action);
        ((SpriteAction*)action)->field_34 = 0;
    }
}

char* FUN_0044e530(int handle) { return 0; }
void FUN_004265a0() {}
} // extern "C"

void FUN_00444E40(SpriteAction* action) {
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction(action);
        action->field_34 = 0;
    }
}

// IconBar.obj: FUN_00444e40 with SpriteAction* param (different mangling from void* version)
void FUN_00444e40(SpriteAction* action) {
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction(action);
        action->field_34 = 0;
    }
}

/* Function start: 0x403520 */
void* __fastcall FUN_00403520(void* self) { return ((ZBQueue*)self)->Pop(); }
/* Function start: 0x4035A0 */
void* __fastcall FUN_004035a0(void* self) { return ((ZBQueue*)self)->Pop(); }
/* Function start: 0x4145F0 */
void __fastcall FUN_004145f0(void* self) { ((ZBQueue*)self)->ClearList(); }

/* Function start: 0x4218C0 */
void __fastcall FUN_004218c0(void* self) {
    ((Timer*)self)->Reset();
}
/* Function start: 0x4218E0 */
unsigned int __fastcall FUN_004218e0(void* self) {
    return ((Timer*)self)->Update();
}

/* Function start: 0x426CE0 */
int __fastcall FUN_00426ce0(void* self) {
    InputManager* input;
    InputState* mouse;
    unsigned int buttons;

    if (ProcessMessages() != 0) {
        return 1;
    }

    input = (InputManager*)self;
    input->PollMouse(input->pMouseLocal);
    input->PollJoystick(input->pJoystick);
    FUN_00426a90(self);

    mouse = input->pMouse;
    buttons = 0;
    if (mouse != 0) {
        buttons = mouse->buttons & 1;
    }

    if (buttons != 0) {
        mouse->ext1 = 1;
    } else {
        buttons = 0;
        if (mouse != 0) {
            buttons = mouse->buttons & 1;
        }
        if (buttons == 0 && (mouse->prevButtons & 1) != 0) {
            buttons = g_leftClickTimer.Update();
            if (buttons < (unsigned int)g_DoubleClickTime_004373b8) {
                input->pMouse->ext1 = 3;
            } else {
                input->pMouse->ext1 = 2;
            }
            g_leftClickTimer.Reset();
        }
    }

    mouse = input->pMouse;
    buttons = 0;
    if (mouse != 0) {
        buttons = mouse->buttons & 2;
    }

    if (buttons != 0) {
        mouse->ext2 = 1;
        return 0;
    }

    buttons = 0;
    if (mouse != 0) {
        buttons = mouse->buttons & 2;
    }
    if (buttons == 0 && (mouse->prevButtons & 2) != 0) {
        buttons = g_rightClickTimer.Update();
        if (buttons < (unsigned int)g_DoubleClickTime_004373b8) {
            input->pMouse->ext2 = 3;
        } else {
            input->pMouse->ext2 = 2;
        }
        g_rightClickTimer.Reset();
    }

    return 0;
}

// ============================================================================
// Remaining __fastcall stubs (unidentified functions)
// ============================================================================

void __fastcall FUN_004148f0(void* param) {}
void* __fastcall FUN_00418540(void* param) { return 0; }
void __fastcall FUN_00449320(void* self, int edx, int flag) {}
int __fastcall FUN_00449400(void* self, int edx, void* msg) { return 0; }
void __fastcall FUN_004494e0(void* self) {}
void* __fastcall FUN_00403620(void*) { return 0; }
void __fastcall FUN_00401130(void*) {}
void __fastcall FUN_00401c80(void*) {}
void __fastcall FUN_004061e0(void*) {}
void __fastcall FUN_00406cc0(void*, int, void*) {}
void __fastcall FUN_00406fd0(void*, int, int) {}
void __fastcall FUN_004070a0(void*, int, int) {}
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
void* __fastcall FUN_00404b80(void*) { return 0; }
void __fastcall FUN_0040b760(void*, int, int) {}
void __fastcall FUN_0044bac0(void*, int, int, int) {}
void* __fastcall FUN_00450b10(void*) { return 0; }
void* __fastcall FUN_0044c660(void*, int, char*) { return 0; }
void __fastcall FUN_00443b90(void*, int, int) {}
void __fastcall FUN_0044d2a0(void*, int, int) {}
void __fastcall FUN_0044c880(void*) {}
void __fastcall FUN_0044d210(void*, int, int, int, int, int) {}
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
void __fastcall FUN_00409f20(void*, int) {}
void __fastcall FUN_004274c0(void*, int, int) {}
void* __fastcall FUN_00410fb0(void*, int, char*, int) { return 0; }
void __fastcall FUN_00411180(void*, int, int) {}
void __fastcall FUN_00420d90(void*) {}
void __fastcall FUN_00424ed0(void*) {}
void __fastcall FUN_00421020(void*) {}
void __fastcall FUN_00421880(void*) {}
void __fastcall FUN_004250e0(void*) {}
void __fastcall FUN_00425200(void*) {}
void __fastcall FUN_00425490(void*) {}
void* __fastcall FUN_00425480(void*) { return 0; }
void __fastcall FUN_00444af0(void*) {}
void __fastcall FUN_004254a0(void*, int, int) {}
void* __fastcall FUN_00440860(void*) { return 0; }
void __fastcall FUN_00421920(void*) {}
void __fastcall FUN_00421930(void*) {}
void __fastcall FUN_00425100(void*, int, int, int) {}
void __fastcall FUN_00427880(void*) {}
void __fastcall FUN_00403fd0(void*, int, void*, int, int, int, int, int, int, int, int, int, int) {}
void __fastcall FUN_004219f0(void*, int, int) {}
int __fastcall FUN_00421a30(void*) { return 0; }
int __fastcall FUN_00424f00(void*, int, char*) { return 0; }
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
void __fastcall FUN_00443ed0(void*, int, int) {}

// ============================================================================
// Remaining __cdecl stubs
// ============================================================================

void __cdecl FUN_00412a50() {}
int __cdecl FUN_00412a50(void*) { return 0; }
void __cdecl FUN_00413e70(void*, int, char*) {}
void __cdecl FUN_00425a90(int, int) {}
void __cdecl FUN_00425c50(char*, ...) {}
void __cdecl FUN_00425d70(char*, ...) {}
void __cdecl FUN_00444e40(void*) {}
void __cdecl FUN_00445450(void*, void*) {}
void* __cdecl FUN_00444a40(void*, int, int, int, int, int, int, int, int, int, int) { return 0; }
char* __cdecl FUN_00426190(char* name) { return name; }
void __cdecl FUN_00425cb0(char*, ...) {}
int __cdecl FUN_00454920() { return 0; }
int __cdecl FUN_0044ccf0(int, int, int, int) { return 0; }
void __cdecl FUN_004344b0() {}
void __cdecl FUN_00434030(void*, int) {}
void __cdecl FUN_00425c50(char*) {}
void __cdecl FUN_00425d70(char*) {}
char* __cdecl FUN_0044e470(char*) { return 0; }
void __cdecl FUN_00425bc0(char*, char*, int) {}

// ============================================================================
// Remaining extern "C" stubs
// ============================================================================

extern "C" {
    int* FUN_004205e0() { static int v = 0; return &v; }
    int* FUN_004205f0() { static int v = 0; return &v; }
    void FUN_00425f10() {}
    int FUN_00425fa0(void*) { return 0; }
    void* FUN_004260f0(char* name) { return name; }
    void FUN_004307b0(int) {}
    char* FUN_0044e3e0(int) { return 0; }
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
