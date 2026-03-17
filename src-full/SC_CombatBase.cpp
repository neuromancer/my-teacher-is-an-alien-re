#include "SC_CombatBase.h"
#include "SC_Question.h"
#include "Sprite.h"
#include "GameState.h"
#include "Memory.h"
#include "Viewport.h"
#include "Target.h"
#include "SoundList.h"
#include "EngineSubsystems.h"
#include "mCNavigator.h"
#include "Palette.h"
#include "CombatSprite.h"
#include "ScoreDisplay.h"
#include "CombatDisplay.h"
#include "WeaponDisplay.h"
#include <string.h>

class InputManager;
extern InputManager* DAT_0046aa08;
extern "C" extern GameState* DAT_0046aa30;

extern void __fastcall FUN_00409730(void*, int, int);  // CombatSprite::ProcessFrame
extern int __fastcall FUN_0044be70(void*, int);         // WeaponParser::UpdateProjectiles
extern void __fastcall FUN_0042d1a0(void*, int, int*);    // HotspotPool::PopEvent
extern void __fastcall FUN_00444a40(void*, int, int, int, int, int, int, int, int, int, int, int); // HotspotEvent ctor
extern void __fastcall FUN_00444920(void*, int, int*);   // HotspotEvent::CopyFrom
extern void __fastcall FUN_00444af0(void*, int);          // HotspotEvent dtor

/* Function start: 0x42BCD0 */
SC_CombatBase::SC_CombatBase()
{
    hotspotX = 0;
    hotspotY = 0;
    combatBonus = 0;
    field_0xC8 = 0;
    field_0xCC = 0;
    field_0xD0 = 0;
    field_0xD4 = 0;
    field_0xD8 = 0;
    memset((void*)&targetList, 0, 0x18 * 4);
    Initialize();
}

/* Function start: 0x42BDA0 */
SC_CombatBase::~SC_CombatBase()
{
    CleanupAll();
}

/* Function start: 0x42BF20 */
int SC_CombatBase::StopAndCleanup()
{
    BeginFrame();

    if (ProcessEvents() != 0) {
        return 1;
    }

    if (FUN_0044be70(DAT_0046ae70, 0) != 0) {
        return 1;
    }

    if (UpdateSprites() != 0) {
        return 1;
    }

    ProcessFrame();

    GameState* gs = DAT_0046aa30;
    if (gs->maxStates - 1 < 4) {
        ShowError("Invalid gamestate %d", 4);
    }
    if (gs->stateValues[4] != 0) {
        ProcessInput();
    }

    frameCount++;
    spriteFrameCount++;
    return 0;
}

/* Function start: 0x42BFC0 */
void SC_CombatBase::ProcessFrame()
{
    void* target = DAT_0046ae58->ProcessTargets();

    int* obj60 = (int*)DAT_0046ae60;
    int* vtbl60 = (int*)*obj60;
    ((void (__fastcall *)(int*, int))vtbl60[5])(obj60, 0);

    int* pMouse = *(int**)((char*)DAT_0046aa08 + 0x1a0);
    if (pMouse == 0) {
        ((int*)DAT_0046ae60)[0x2a] = 0;
    } else {
        int clicked = (*(int*)(pMouse + 4) >= 2) ? 1 : 0;
        ((int*)DAT_0046ae60)[0x2a] = clicked;
    }

    if (((int*)DAT_0046ae60)[0x2a] != 0) {
        int* vtbl60b = (int*)*(int*)DAT_0046ae60;
        ((void (__fastcall *)(int*, int))vtbl60b[4])((int*)DAT_0046ae60, 0);
        if (target != 0) {
            ((Target*)target)->UpdateProgress(1);
        }
    }

    RenderBackground();
    PostRender();
}

/* Function start: 0x42C050 */
int SC_CombatBase::UpdateSprites()
{
    if (DAT_0046ae5c != 0) {
        FUN_00409730(DAT_0046ae5c, 0, spriteFrameCount);
    }
    return 0;
}

/* Function start: 0x42C120 */
void SC_CombatBase::RenderBackground()
{
    if (DAT_0046ae50 != 0) {
        DAT_0046ae50->RenderAt(
            DAT_0046ae50->num_states,
            DAT_0046ae50->field_0xb0,
            0, 0x3FF00000);
    }
}

/* Function start: 0x42C8A0 */
void SC_CombatBase::SetupViewport()
{
    DAT_0046ae50 = bgSprite;
    DAT_0046ae60 = combatDisplay;
    DAT_0046ae68 = soundList;
    DAT_0046ae4c = weaponParser;
    DAT_0046ae70 = navigator;
    DAT_0046ae64 = palette;
    DAT_0046ae5c = combatSprite;
    DAT_0046ae58 = targetList;
    DAT_0046ae6c = scoreDisplay;
    DAT_0046ae54 = viewport;
    DAT_0046ae74 = hotspotPool;
}

/* Function start: 0x42C240 */
void SC_CombatBase::Initialize()
{
    combatDisplay = new CombatDisplay();

    // SoundList (0x10 bytes)
    soundList = new SoundList(0x32);

    weaponParser = new EngineInfoParser();
    navigator = new mCNavigator();
    palette = new Palette();
    combatSprite = new CombatSprite();
    targetList = new TargetList();
    scoreDisplay = new ScoreDisplay();
    viewport = new Viewport();
    hotspotPool = new HotspotListData();

    SetupViewport();
}

extern void __fastcall FUN_0044c740(void*);  // Sprite dtor
extern void __fastcall FUN_00443360(void*, int);  // TargetList dtor
extern void __fastcall FUN_00434740(void*, int);  // EngineInfoParser dtor
extern void __fastcall FUN_00409020(void*, int);  // CombatSprite dtor

/* Function start: 0x42C630 */
void SC_CombatBase::CleanupAll()
{
    if (bgSprite != 0) {
        FUN_0044c740(bgSprite);
        FreeMemory(bgSprite);
        bgSprite = 0;
    }
    if (navigator != 0) {
        delete navigator;
        navigator = 0;
    }
    if (palette != 0) {
        delete palette;
        palette = 0;
    }
    if (scoreDisplay != 0) {
        delete scoreDisplay;
        scoreDisplay = 0;
    }
    if (targetList != 0) {
        FUN_00443360(targetList, 0);
        FreeMemory(targetList);
        targetList = 0;
    }
    if (weaponParser != 0) {
        FUN_00434740(weaponParser, 0);
        FreeMemory(weaponParser);
        weaponParser = 0;
    }
    if (combatSprite != 0) {
        FUN_00409020(combatSprite, 0);
        FreeMemory(combatSprite);
        combatSprite = 0;
    }
    if (viewport != 0) {
        delete viewport;
        viewport = 0;
    }
    if (soundList != 0) {
        delete soundList;
        soundList = 0;
    }
    if (combatDisplay != 0) {
        delete combatDisplay;
        combatDisplay = 0;
    }
    if (hotspotPool != 0) {
        hotspotPool->~HotspotListData();
        FreeMemory(hotspotPool);
        hotspotPool = 0;
    }

    RenderState();
}


/* Function start: 0x42CB2E */
int SC_CombatBase::LBLParse(char* line)
{
    char token[32];
    char arg[32];
    int value;

    sscanf(line, "%s", token);

    if (strcmp(token, "ENGINE_INFO") == 0) {
        Parser::ProcessFile((Parser*)DAT_0046ae4c, this, (char*)0);
    } else if (strcmp(token, "UPDATE_DIRS") == 0) {
        extern void* DAT_0046aa1c;
        Parser::ProcessFile((Parser*)DAT_0046aa1c, this, (char*)0);
    } else if (strcmp(token, "TARGETS") == 0) {
        Parser::ProcessFile((Parser*)DAT_0046ae58, this, (char*)0);
    } else if (strcmp(token, "SPRITELIST") == 0) {
        Parser::ProcessFile((Parser*)DAT_0046ae5c, this, (char*)0);
    } else if (strcmp(token, "NAVIGATION") == 0) {
        Parser::ProcessFile((Parser*)DAT_0046ae70, this, (char*)0);
    } else if (strcmp(token, "SET_GAMESTATE") == 0) {
        sscanf(line, " %s %s %d", token, arg, &value);
        GameState* gs = DAT_0046aa30;
        int idx = gs->FindState(arg);
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] = value;
    } else if (strcmp(token, "WEAPON") == 0) {
        int n = sscanf(line, " %s %s", token, arg);
        if (n == 2) {
            if (combatDisplay != 0) {
                delete combatDisplay;
                combatDisplay = 0;
            }
            if (strcmp(arg, "ROCKTHROWER") == 0) {
                combatDisplay = new RockThrower(this);
                DAT_0046ae60 = combatDisplay;
            } else {
                combatDisplay = new WeaponDisplay();
                DAT_0046ae60 = combatDisplay;
            }
        }
    } else if (strcmp(token, "CONSOLE") == 0) {
        bgSprite = new Sprite((char*)0);
        DAT_0046ae50 = bgSprite;
        Parser::ProcessFile(bgSprite, this, (char*)0);
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("Engine");
    }

    return 0;
}
/* Function start: 0x42C9D0 */
int SC_CombatBase::ProcessEvents()
{
    int localEvent[14];
    int tempEvent[14];

    FUN_00444a40((void*)localEvent, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    int result = 0;
    HotspotListData* pool = DAT_0046ae74;

    if (pool->count != 0) {
        int* vtbl = *(int**)this;
        int (__fastcall *handleAction)(void*, int, int*) =
            (int (__fastcall *)(void*, int, int*))vtbl[13];

        do {
            FUN_0042d1a0(pool, 0, tempEvent);
            FUN_00444920((void*)localEvent, 0, tempEvent);
            FUN_00444af0((void*)tempEvent, 0);
            result |= handleAction(this, 0, localEvent);
        } while (pool->count != 0);
    }

    FUN_00444af0((void*)localEvent, 0);
    return result;
}

void SC_CombatBase::ResetState() {}
void SC_CombatBase::ProcessInput() {}
void SC_CombatBase::BeginFrame() {}
int SC_CombatBase::PostRender() { return 0; }
int SC_CombatBase::HandleAction(int*) { return 0; }
void SC_CombatBase::RenderState() {}
int SC_CombatBase::UpdateAndCheck() { return 0; }
