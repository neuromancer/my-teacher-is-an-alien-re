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

extern "C" int DAT_0046a6ec;
extern "C" int ProcessMessages();
extern Timer g_leftClickTimer;
extern Timer g_rightClickTimer;

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

class TargetListFull {
public:
    void ProcessTargets();
};
void TargetListFull::ProcessTargets() {}

class WeaponParser {
public:
    void UpdateProjectiles();
};
void WeaponParser::UpdateProjectiles() {}

class InputManagerFull {
public:
    void PollEvents(int);
};
void InputManagerFull::PollEvents(int) {}

class PaletteLoader {
public:
    void Load(char*);
};
void PaletteLoader::Load(char*) {}

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

// Engine base class methods (called via fastcall trick from SC_DodgeOrville)
void __fastcall FUN_00449320(void* self, int edx, int flag) {}
int __fastcall FUN_00449400(void* self, int edx, void* msg) { return 0; }
void __fastcall FUN_004494e0(void* self) {}

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
// SC_Detention::ResetAnimations -> SC_Detention.cpp
// SC_Detention::ProcessPeriodAction -> SC_Detention.cpp

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
extern "C" extern void* DAT_0046aa30;
#define g_GameState_0046aa30 ((GameState*)DAT_0046aa30)
GameState* g_StringTable_0046aa34 = 0; // DAT_0046aa34
char* DAT_0046aa2c = 0;               // DAT_0046aa2c - string buffer for ProcessMessage
GameState* DAT_0046aa3c = 0;          // DAT_0046aa3c - GameState for ProcessMessage

extern "C" void WriteToLog(const char* format, ...) {}  // FUN_00425d70 stub
/* Function start: 0x426A90 */
void __fastcall FUN_00426a90(void* self) {
    InputState* mouse;

    mouse = ((InputManager*)self)->pMouse;
    mouse->ext2 = 0;
    mouse->ext1 = mouse->ext2;
}

// Functions referenced by SCI_Inventory
void __fastcall FUN_00432da0(void* self) {}
void __fastcall FUN_00421bc0(void* self) {}
void* __fastcall FUN_004407c0(void* self) { return 0; }
void __fastcall FUN_0040c6e0(void* self) {}

extern "C" {
void FUN_00444d90(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    SpriteAction action(a, b, c, d, e, f, g, h, i, j);

    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction(&action);
        action.field_34 = 0;
    }
}
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
/* Function start: 0x403520 */
void* __fastcall FUN_00403520(void* self) { return ((ZBQueue*)self)->Pop(); }
/* Function start: 0x4035A0 */
void* __fastcall FUN_004035a0(void* self) { return ((ZBQueue*)self)->Pop(); }
void* __fastcall FUN_00403620(void*) { return 0; }
void __fastcall FUN_00401130(void*) {}
void __fastcall FUN_00401c80(void*) {}
void __fastcall FUN_004061e0(void*) {}
void __fastcall FUN_00406cc0(void*, int, void*) {}
void __fastcall FUN_00406fd0(void*, int, int) {}
void __fastcall FUN_004070a0(void*, int, int) {}
/* Function start: 0x4145F0 */
void __fastcall FUN_004145f0(void* self) { ((ZBQueue*)self)->ClearList(); }
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

// --- Parser link stubs ---
int g_ParserCount = 0;              // DAT_00469288
void* g_FilePosCache = 0;           // DAT_00469144
void __fastcall FUN_004128f0(void*) {}  // destructor for g_FilePosCache
// Parser::ProcessFile -> Parser.cpp
// Parser::GetKey -> Parser.cpp

// Parser::GetTokenType -> Parser.cpp
void Parser::HandleToken(int tokenType, char* line) {}
void Parser::BeginComment(char* line, int flag) {}
int Parser::EndComment() { return 0; }
int Parser::DoCommentsMatch(char* line) { return 0; }
void Parser::SubstituteVars(char* src, char* dst) {}
int DAT_00469160 = 0;  // preprocessor variable substitution flag

// Parser::ProcessFile timing globals
int DAT_00469148 = 0;
int DAT_0046914c = 0;
int DAT_00469150 = 0;
int DAT_00469154 = 0;
int DAT_00469158 = 0;
int DAT_0046915c = 0;

// --- C++ __cdecl stubs ---
// FUN_00412a50 -> Parser::ReportUnknownLabel in Parser.cpp
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

// T_MenuHotspot stub
#include "T_MenuHotspot.h"
int T_MenuHotspot::LBLParse(char*) { return 0; }

// --- C++ globals ---
SpriteAction DAT_00472d58;
SpriteAction DAT_00472d90;  // needed by IconBar and HotspotAction

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

// PaletteObj stubs
class PaletteObj {
public:
    void Load(char*);
    void PlaySound(int);
    int CheckSound(int);
};
void PaletteObj::Load(char*) {}
void PaletteObj::PlaySound(int) {}
int PaletteObj::CheckSound(int) { return 0; }

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
}
SpriteAction DAT_00472d20;

// extern "C" functions
extern "C" {
    void FUN_00413e10(void*, char*, char*, ...) {}
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

extern "C" {
    void FUN_004309c0(void*) {}
}

// ============================================================================
// SC_Message method stubs (engine message class, used by many systems)
// ============================================================================

SC_Message::SC_Message(int, int, int, int, int, int, int, int, int, int) {}
SC_Message::~SC_Message() {}
/* Function start: 0x444E60 */
int SC_Message::LBLParse(char* param_1)
{
    extern void* DAT_0046aa38;
    char local_30[32];
    char local_54[32];
    char local_74[32];
    int idx;

    local_54[0] = '\0';
    local_74[0] = '\0';
    local_30[0] = '\0';
    sscanf(param_1, "%s", local_30);

    if (strcmp(local_30, "ADDRESS") == 0) {
        sscanf(param_1, "%s %s %s", local_30, local_54, local_74);
        if (g_StringTable_0046aa34 != 0) {
            idx = g_StringTable_0046aa34->FindState(local_54);
        } else {
            sscanf(local_54, "%d", &idx);
        }
        targetAddress = idx;
        if (targetAddress < 0) {
            FUN_00425c50("SC_MessageParser::ADDRESS illegal index %s %s", local_54, param_1);
        }
        if (targetAddress == 2) {
            if (g_GameState_0046aa30 != 0) {
                idx = g_GameState_0046aa30->FindState(local_74);
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    FUN_00425c50("Invalid gamestate %d", idx);
                }
                sourceAddress = idx;
            }
        } else {
            sscanf(local_74, "%d", &sourceAddress);
        }
    } else if (strcmp(local_30, "FROM") == 0) {
        sscanf(param_1, "%s %s %s", local_30, local_54, local_74);
        if (g_StringTable_0046aa34 != 0) {
            idx = g_StringTable_0046aa34->FindState(local_54);
        } else {
            sscanf(local_54, "%d", &idx);
        }
        command = idx;
        if (command < 0) {
            FUN_00425c50("FROM illegal index %s %s", local_54, param_1);
        }
        if (command == 2) {
            if (g_GameState_0046aa30 != 0) {
                idx = g_GameState_0046aa30->FindState(local_74);
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    FUN_00425c50("Invalid gamestate %d", idx);
                }
                data = idx;
            }
        } else {
            sscanf(local_74, "%d", &data);
        }
    } else if (strcmp(local_30, "INSTRUCTION") == 0) {
        sscanf(param_1, "%s %s", local_30, local_54);
        if (DAT_0046aa38 != 0) {
            idx = ((GameState*)DAT_0046aa38)->FindState(local_54);
        } else {
            sscanf(local_54, "%d", &idx);
        }
        priority = idx;
        if (priority < 0) {
            FUN_00425c50("INSTRUCTION illegal index %d %s", idx, param_1);
        }
    } else if (strcmp(local_30, "MESSAGE") == 0) {
        if (userPtr != 0) {
            FUN_00425c50("double reserve in Message %s", param_1);
        }
        SC_Message* msg = new SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        userPtr = (int)msg;
        Parser::ProcessFile((Parser*)msg, (Parser*)this, 0);
    } else if (strcmp(local_30, "MOUSE") == 0) {
        sscanf(param_1, "%s %d %d", local_30, &clickPos.x, &clickPos.y);
    } else if (strcmp(local_30, "BUTTON1") == 0) {
        sscanf(param_1, "%s %d", local_30, &mouseX);
    } else if (strcmp(local_30, "BUTTON2") == 0) {
        sscanf(param_1, "%s %d", local_30, &mouseY);
    } else if (strcmp(local_30, "LASTKEY") == 0) {
        sscanf(param_1, "%s %d", local_30, &lastKey);
    } else if (strcmp(local_30, "TIME") == 0) {
        sscanf(param_1, "%s %lu", local_30, &time);
    } else if (strcmp(local_30, "EXTRA1") == 0) {
        sscanf(param_1, "%s %lu", local_30, &param1);
    } else if (strcmp(local_30, "EXTRA2") == 0) {
        sscanf(param_1, "%s %lu", local_30, &param2);
    } else if (strcmp(local_30, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_Message");
    }

    return 0;
}
void SC_Message::Dump(int) {}

// ============================================================================
// SC_Question extern stubs (full game)
// ============================================================================

int __fastcall FUN_0044c350(void*, int, char*) { return 0; }
void __cdecl FUN_00425cb0(char*, ...) {}
int __fastcall FUN_00420a00(void*, int, int) { return 0; }
void __fastcall FUN_00420a50(void*, int, int) {}
// FUN_004047c0 = ZBufferManager::ProcessRenderQueues (implemented)
void __fastcall FUN_00404b90(void*) {}
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
/* Function start: 0x4218C0 */
void __fastcall FUN_004218c0(void* self) {
    ((Timer*)self)->Reset();
}
/* Function start: 0x4218E0 */
unsigned int __fastcall FUN_004218e0(void* self) {
    return ((Timer*)self)->Update();
}
void __fastcall FUN_00411180(void*, int, int) {}
// FUN_00404350 = ZBufferManager::DrawRect (implemented)

// Missing __cdecl stubs
char* __cdecl FUN_0044e470(char*) { return 0; }

// Missing C++ globals
int DAT_0046ad6c = 0;
void* DAT_0046bbfc = 0;
void* DAT_00468ef0 = 0;
char* DAT_0046bacc = 0;

// Class method stubs for SC_Wahoo local classes
class InputObj { public: int Refresh(int); };
int InputObj::Refresh(int) { return 0; }

class DetectionObj { public: void Render(); };
void DetectionObj::Render() {}

class DetMask { public: int CheckHit(int, int); };
int DetMask::CheckHit(int, int) { return 0; }

// SC_WordSearch virtual method stubs
#include "SC_WordSearch.h"
int SC_WordSearch::AddMessage(SC_Message*) { return 0; }
int SC_WordSearch::LBLParse(char*) { return 0; }

// ============================================================================
// SC_FireAlarm / SC_Pods link stubs
// ============================================================================

void* __fastcall FUN_00425480(void*) { return 0; }
void __fastcall FUN_00444af0(void*) {}
void __fastcall FUN_004254a0(void*, int, int) {}
void* __fastcall FUN_00440860(void*) { return 0; }
int DAT_004685ac = 0;
void* DAT_0046bf28 = 0;

// ============================================================================
// Engine link stubs
// ============================================================================

#include "Sound.h"
#include "SpriteAction.h"

void __stdcall FUN_004309C0(int*) {}
void FUN_00444E40(SpriteAction* action) {
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction(action);
        action->field_34 = 0;
    }
}
Sound* DAT_0046AA0C = 0;
int DAT_0046AA24 = 0;

// ============================================================================
// SC_CombatBase / EngineA link stubs
// ============================================================================

#include "SC_CombatBase.h"
#include "GameState.h"

SC_CombatBase::SC_CombatBase() {}
SC_CombatBase::~SC_CombatBase() {}
int SC_CombatBase::LBLParse(char*) { return 0; }
void SC_CombatBase::Initialize() {}
void SC_CombatBase::CleanupAll() {}
void SC_CombatBase::ResetState() {}
void SC_CombatBase::ProcessInput() {}
void SC_CombatBase::method8() {}
void SC_CombatBase::method9() {}
void SC_CombatBase::method10() {}
void SC_CombatBase::method11() {}
int SC_CombatBase::method12() { return 0; }
int SC_CombatBase::HandleAction(int*) { return 0; }
void SC_CombatBase::StopAndCleanup() {}
void SC_CombatBase::SetupViewport() {}
void SC_CombatBase::RenderState() {}
int SC_CombatBase::UpdateAndCheck() { return 0; }

// ============================================================================
// SC_Fan link stubs
// ============================================================================

void __fastcall FUN_00421920(void*) {}
void __fastcall FUN_00421930(void*) {}
void __fastcall FUN_00425100(void*, int, int, int) {}
void __fastcall FUN_00427880(void*) {}
void __fastcall FUN_00403fd0(void*, int, void*, int, int, int, int, int, int, int, int, int, int) {}
int __cdecl FUN_00454920() { return 0; }  // C++ linkage for SC_Fan
void __fastcall FUN_004219f0(void*, int, int) {}
int __fastcall FUN_00421a30(void*) { return 0; }
int __fastcall FUN_00424f00(void*, int, char*) { return 0; }

// GameStateObj::FindLabel stub (SC_Fan uses this wrapper for __thiscall FUN_00433ae0)
class GameStateObj { public: int FindLabel(char* name); };
int GameStateObj::FindLabel(char*) { return 0; }

// SC_Fan externs that need C++ linkage versions
int __cdecl FUN_0044ccf0(int, int, int, int) { return 0; }
void __fastcall FUN_0044cb40(void*, int, int, int) {}
void __fastcall FUN_004127c0(void*) {}
void __fastcall FUN_00407b60(void*) {}

// ============================================================================
// InitMemoryCache / CleanupMemoryCache link stubs
// ============================================================================

void __cdecl FUN_004344b0() {}
void __cdecl FUN_00434030(void*, int) {}

// ============================================================================
// Additional link stubs for TEACHER-FULL.EXE
// ============================================================================

// IconBar.obj: __stdcall stubs
void __stdcall FUN_004309a0(int) {}
// FUN_004309c0 __stdcall version conflicts with extern "C" cdecl version
// Cannot provide both from same TU

// SpriteRender::RenderAt - SC_FireAlarm wants int, IconBar wants void
// Provide int version (SC_FireAlarm uses return value)
class SpriteRender { public: int RenderAt(int, int, int, int); };
int SpriteRender::RenderAt(int, int, int, int) { return 0; }

// IconBar.obj: FUN_00444e40 with SpriteAction* param (different mangling from void* version)
void FUN_00444e40(SpriteAction* action) {
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction(action);
        action->field_34 = 0;
    }
}

// IconBar.obj: FUN_00412a50 as __fastcall
void __fastcall FUN_00412a50(void*, int, char*) {}

// IconBar.obj: SpriteSetup stubs
class SpriteSetup {
public:
    void ConfigRange(int, int, int, int);
    void ConfigStates(int);
};
void SpriteSetup::ConfigRange(int, int, int, int) {}
void SpriteSetup::ConfigStates(int) {}

// GameEngine.obj: FUN_004047c0
void __fastcall FUN_004047c0(void*) {}

// HotspotAction.obj: FUN_004036a0
void* __fastcall FUN_004036a0(void*) { return 0; }

// HotspotAction.obj: GSObj::FUN_00433bb0
class GSObj { public: int FUN_00433bb0(int*); };
int GSObj::FUN_00433bb0(int*) { return 0; }

// HotspotAction.obj: DAT_0046aa34 as void* (C++ linkage)
void* DAT_0046aa34 = 0;

// HotspotAction.obj: FUN_00425bc0
void __cdecl FUN_00425bc0(char*, char*, int) {}

// HotspotAction.obj: LookupObj::FUN_00432e20
class LookupObj { public: int FUN_00432e20(char*); };
int LookupObj::FUN_00432e20(char*) { return 0; }

// SCI_IconBarModule.obj: CursorControl::SetCursor
class CursorControl { public: void SetCursor(int, int); };
void CursorControl::SetCursor(int, int) {}

// SCI_IconBarModule.obj: __fastcall stubs
void __fastcall FUN_00445970(void*) {}
void __fastcall FUN_004459a0(void*, int, int) {}

// FUN_00443e30: SCI_IconBarModule wants void return, others want int return
// Can't provide both from same TU - void version unsatisfied from stubs alone

// SCI_IconBarModule.obj + SC_Detention.obj: DAT_00468764
int DAT_00468764 = 0;

// SC_FireAlarm.obj: SpriteAnim::ResetAnimation
class SpriteAnim { public: void ResetAnimation(int, int); };
void SpriteAnim::ResetAnimation(int, int) {}

// SC_FireAlarm.obj: globals
int DAT_00472bd8 = 0;
int DAT_004685a0 = 0;
int DAT_00472bdc = 0;

// SC_FireAlarm.obj: FUN_004279a0
void __fastcall FUN_004279a0(void*) {}

// FUN_00454920 extern "C" needed by SC_FireAlarm, but conflicts with C++ version above

// SC_WordSearch.obj: FUN_00404350
void __fastcall FUN_00404350(void*, int, int, int, int, int, int, int, int) {}
