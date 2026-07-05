#include "SC_CombatBase.h"
#include "SC_Question.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "AnimatedAsset.h"
#include "RockThrower.h"
#include "GameState.h"
#include "Memory.h"
#include "Viewport.h"
#include "Target.h"
#include "SoundList.h"
#include "Sound.h"
#include "EngineSubsystems.h"
#include "mCNavigator.h"
#include "Palette.h"
#include "CombatSprite.h"
#include "ScoreDisplay.h"
#include "CombatDisplay.h"
#include "WeaponDisplay.h"
#include <string.h>
#include <new>

#include "InputManager.h"
#include "globals.h"

#include "CombatSprite.h"
#include "mCNavigator.h"
#include "TimedEvent.h"

#include "VideoTable.h"
/* Function start: 0x42BCD0 */
SC_CombatBase::SC_CombatBase()
{
    memset((void*)&targetList, 0, 0x18 * 4);
    Initialize();
}

/* Function start: 0x42BDA0 */
SC_CombatBase::~SC_CombatBase()
{
    CleanupAll();
}

/* Function start: 0x42BF00 */
void SC_CombatBase::StopSoundsAndReset()
{
    g_EngineSound_0046aa0c->StopAllSamples();
    ResetState();
}

/* Function start: 0x42BF20 */
int SC_CombatBase::StopAndCleanup()
{
    BeginFrame();

    if (ProcessEvents() != 0) {
        return 1;
    }

    if (g_Navigator_0046ae70->Update() != 0) {
        return 1;
    }

    if (UpdateSprites() != 0) {
        return 1;
    }

    ProcessFrame();

    GameState* gs = g_GameState_0046aa30;
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
    Target* target = (Target*)g_TargetList_0046ae58->ProcessTargets();

    g_CombatWeapon_0046ae60->DrawCrosshairs();

    InputState* pMouse = g_InputManager_0046aa08->pMouse;
    if (pMouse != 0) {
        int clicked = (pMouse->ext1 >= 2) ? 1 : 0;
        g_CombatWeapon_0046ae60->m_clicked = clicked;
    } else {
        g_CombatWeapon_0046ae60->m_clicked = 0;
    }

    if (g_CombatWeapon_0046ae60->m_clicked != 0) {
        g_CombatWeapon_0046ae60->OnHit();
        if (target != 0) {
            target->UpdateProgress(1);
        }
    }

    RenderBackground();
    PostRender();
}

/* Function start: 0x42C050 */
int SC_CombatBase::UpdateSprites()
{
    if (g_CombatSprite_0046ae5c != 0) {
        g_CombatSprite_0046ae5c->ProcessFrame(spriteFrameCount);
    }
    return 0;
}

/* Function start: 0x42C120 */
void SC_CombatBase::RenderBackground()
{
    if (g_BgSprite_0046ae50 != 0) {
        Sprite* spr = g_BgSprite_0046ae50;
        spr->Do(spr->loc.x, spr->loc.y, 1.0);
    }
}

/* Function start: 0x42C150 */
void SC_CombatBase::EnqueueHotspotAction(SpriteAction* param)
{
    int* pool;
    int* tailPtr;
    int* freePtr;
    int oldTail;
    int* node;
    SpriteAction* dataPtr;

    pool = (int*)g_HotspotPool_0046ae74;
    tailPtr = pool + 1;
    freePtr = pool + 3;
    oldTail = *tailPtr;

    if (*freePtr == 0) {
        int* sizePtr = pool + 5;
        int sz = *sizePtr;
        int* block = (int*)operator new(sz * 0x40 + 4);
        block[0] = pool[4];
        pool[4] = (int)block;
        {
            int count = *sizePtr;
            int* nodeAddr = (int*)((char*)block + count * 0x40 - 0x3C);
            count--;
            if (count >= 0) {
                do {
                    int old = *freePtr;
                    count--;
                    nodeAddr[0] = old;
                    *freePtr = (int)nodeAddr;
                    nodeAddr = (int*)((char*)nodeAddr - 0x40);
                } while (count >= 0);
            }
        }
    }

    node = (int*)*freePtr;
    dataPtr = (SpriteAction*)(node + 2);
    *freePtr = node[0];
    node[1] = oldTail;
    node[0] = 0;
    pool[2]++;
    memset(node + 2, 0, 14 * 4);

    {
        int i = 0;
        do {
            if (dataPtr != 0) {
                new (dataPtr) SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            }
            dataPtr = dataPtr + 1;
        } while (i-- != 0);
    }

    ((SpriteAction*)(node + 2))->CopyFrom(param);

    if (*tailPtr != 0) {
        ((int*)*tailPtr)[0] = (int)node;
    } else {
        pool[0] = (int)node;
    }
    *tailPtr = (int)node;
}

/* Function start: 0x42C8A0 */
void SC_CombatBase::SetupViewport()
{
    g_BgSprite_0046ae50 = bgSprite;
    g_CombatWeapon_0046ae60 = combatDisplay;
    g_SoundList_0046ae68 = soundList;
    g_WeaponParser_0046ae4c = weaponParser;
    g_Navigator_0046ae70 = navigator;
    g_Palette_0046ae64 = palette;
    g_CombatSprite_0046ae5c = combatSprite;
    g_TargetList_0046ae58 = targetList;
    g_ScoreDisplay_0046ae6c = scoreDisplay;
    g_Viewport_0046ae54 = viewport;
    g_HotspotPool_0046ae74 = hotspotPool;
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
    hotspotPool = new TimedEventPool(10);

    SetupViewport();
}

#include "Target.h"

/* Function start: 0x42C630 */
void SC_CombatBase::CleanupAll()
{
    if (bgSprite != 0) {
        delete bgSprite;
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
        TargetList* t = targetList;
        t->TargetList::~TargetList();
        FreeMemory(t);
        targetList = 0;
    }
    if (weaponParser != 0) {
        EngineInfoParser* w = weaponParser;
        w->EngineInfoParser::~EngineInfoParser();
        FreeMemory(w);
        weaponParser = 0;
    }
    if (combatSprite != 0) {
        CombatSprite* c = combatSprite;
        c->CombatSprite::~CombatSprite();
        FreeMemory(c);
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
        delete hotspotPool;
        hotspotPool = 0;
    }

    RenderState();
}


/* Function start: 0x42CB10 */
/* Function start: 0x42CB2E */
int SC_CombatBase::LBLParse(char* line)
{
    char token[32];
    char arg[32];
    int value;

    sscanf(line, "%s", token);

    if (strcmp(token, "[ENGINE_INFO]") == 0) {
        Parser::ProcessFile((Parser*)g_WeaponParser_0046ae4c, this, (char*)0);
    } else if (strcmp(token, "[UPDATE_DIRS]") == 0) {
        Parser::ProcessFile((Parser*)g_PathResolver_0046aa1c, this, (char*)0);
    } else if (strcmp(token, "[TARGETS]") == 0) {
        Parser::ProcessFile((Parser*)g_TargetList_0046ae58, this, (char*)0);
    } else if (strcmp(token, "[SPRITELIST]") == 0) {
        Parser::ProcessFile((Parser*)g_CombatSprite_0046ae5c, this, (char*)0);
    } else if (strcmp(token, "[NAVIGATION]") == 0) {
        Parser::ProcessFile((Parser*)g_Navigator_0046ae70, this, (char*)0);
    } else if (strcmp(token, "SET_GAMESTATE") == 0) {
        sscanf(line, " %s %s %d", token, arg, &value);
        GameState* gs = g_GameState_0046aa30;
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
                g_CombatWeapon_0046ae60 = combatDisplay;
            } else if (strcmp(arg, "RAY_GUN") == 0) {
                combatDisplay = new WeaponDisplay();
                g_CombatWeapon_0046ae60 = combatDisplay;
            } else if (strcmp(arg, "NONE") == 0) {
                combatDisplay = new Weapon();
                g_CombatWeapon_0046ae60 = combatDisplay;
            }
        }
    } else if (strcmp(token, "CONSOLE") == 0) {
        bgSprite = new Sprite((char*)0);
        g_BgSprite_0046ae50 = bgSprite;
        Parser::ProcessFile(bgSprite, this, (char*)0);
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        Parser::ReportUnknownLabel("Engine");
    }

    return 0;
}
/* Function start: 0x42C9D0 */
int SC_CombatBase::ProcessEvents()
{
    SpriteAction localEvent(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    int result = 0;

    if (g_HotspotPool_0046ae74->m_count != 0) {
        do {
            localEvent.CopyFrom((SpriteAction*)&((TimedEventPool*)g_HotspotPool_0046ae74)->PopSafe());
            result |= HandleAction((int*)&localEvent);
        } while (g_HotspotPool_0046ae74->m_count != 0);
    }

    return result;
}

/* Function start: 0x42C230 */
void SC_CombatBase::ResetState() {}

/* Function start: 0x42C960 */
void SC_CombatBase::ProcessInput() {
    char buffer[128];
    SetDrawColors(-1, -1);
    SetDrawPosition(0, 0xc7);
    sprintf(buffer, "frames: a=%d l=%d", frameCount, spriteFrameCount);
    if (g_GlyphFont_0046aa28 != 0) {
        g_GlyphFont_0046aa28->RenderText(buffer, -1);
    }
}
/* Function start: 0x42C070 */
void SC_CombatBase::BeginFrame() {
    InputState* mouse = g_InputManager_0046aa08->pMouse;
    if (mouse != 0) {
        g_CombatWeapon_0046ae60->m_crosshair.x = mouse->x;
    } else {
        g_CombatWeapon_0046ae60->m_crosshair.x = 0;
    }
    mouse = g_InputManager_0046aa08->pMouse;
    if (mouse != 0) {
        g_CombatWeapon_0046ae60->m_crosshair.y = mouse->y;
    } else {
        g_CombatWeapon_0046ae60->m_crosshair.y = 0;
    }

    mouse = g_InputManager_0046aa08->pMouse;
    if (mouse != 0 && mouse->x > 0xAA) {
        g_Viewport_0046ae54->SetCenterX(g_Viewport_0046ae54->center.a + 4);
        return;
    }
    if (mouse != 0 && mouse->x >= 0x96) {
        return;
    }
    g_Viewport_0046ae54->SetCenterX(g_Viewport_0046ae54->center.a - 4);
}
/* Function start: 0x40BC80 */
void SC_CombatBase::PostRender() {}
/* Function start: 0x42BD70 */
int SC_CombatBase::HandleAction(int*) { return 0; }
/* Function start: 0x42C920 */
void SC_CombatBase::RenderState() {
    g_BgSprite_0046ae50 = 0;
    g_CombatWeapon_0046ae60 = 0;
    g_SoundList_0046ae68 = 0;
    g_WeaponParser_0046ae4c = 0;
    g_Navigator_0046ae70 = 0;
    g_Palette_0046ae64 = 0;
    g_CombatSprite_0046ae5c = 0;
    g_TargetList_0046ae58 = 0;
    g_ScoreDisplay_0046ae6c = 0;
    g_Viewport_0046ae54 = 0;
    g_HotspotPool_0046ae74 = 0;
}
/* Function start: 0x40BC90 */
void SC_CombatBase::UpdateAndCheck() {}
