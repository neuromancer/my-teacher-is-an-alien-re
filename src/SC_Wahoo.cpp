#include "SC_Wahoo.h"
#include "stubs.h"
#include "MouseControl.h"
#include "SpriteAction.h"
#include "MouseControl.h"
#include "Sprite.h"
#include "Projectile.h"
#include "MouseControl.h"
#include "Palette.h"
#include "MouseControl.h"
#include "InputManager.h"
#include "MouseControl.h"
#include "Sample.h"
#include "MouseControl.h"
#include "SoundList.h"
#include "MouseControl.h"
#include "mss.h"
#include "MouseControl.h"
#include "Memory.h"
#include "MouseControl.h"
#include "GameState.h"
#include "MouseControl.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new.h>
#include "VBuffer.h"
#include "MouseControl.h"
#include "main.h"
#include "MouseControl.h"
#include "SC_Question.h"


// FUN_00421930 = TimeOut::~TimeOut (full game)

// FUN_00412a50 = Parser::LBLParse in Parser.h
#include "string.h"

// FUN_00410fb0 = VBuffer ctor — callers updated to use new
#include "RockThrower.h"
#include "CombatWeapon.h"
#include "SC_CombatBase.h"


// FUN_00426ce0 is thiscall with 1 param (not fastcall)
// Use local class wrapper for correct calling convention

// TimerObj = TimeOut (ctor 0x421920 wraps 0x421960, dtor 0x421930)
// Check() = IsTimeOut() @ 0x421A30, Set() = Start() @ 0x4219F0
#include "TimeOut.h"
#include "MouseControl.h"
#include "AILSample.h"

#include "CDData.h"
class DetectionObj {
public:
    void Render();            // 0x427880
};

// DetMask IS VBuffer — CheckHit at 0x411330 now in VBuffer.h
void DetectionObj::Render() {}

// FUN_00413e70 = Parser::ProcessFile in Parser.cpp
// FUN_00413e10 = ParseFile in Parser.h

#include "globals.h"


/* Function start: 0x437830 */
SC_Wahoo::SC_Wahoo() {
    // Rect/SlimeDim constructors zero their fields automatically via SEH
    memset(&spriteAction, 0, 0xC0);
    handlerId = 0x3D;
}

/* Function start: 0x437980 */
SC_Wahoo::~SC_Wahoo() {
    ShutDown(0);
}

/* Function start: 0x437A40 */
void SC_Wahoo::Init(SC_MessageParser* msg) {
    int savedId = handlerId;
    memset(&spriteAction, 0, 0x30 * 4);
    handlerId = savedId;

    CopyCommandData(msg);

    gameFlags = 0;

    if (!FileExists("CB_Bridge")) {
        ShowLoadingScreen();
    }

    screenSize.x = 0x140;
    screenSize.y = 0xf0;
    SetVideoRes(0x140, 0xf0);

    moduleParam = ((SpriteAction*)msg)->addressValue;

    GameState* gameState = g_GameState_0046aa30;
    unsigned int stateIdx = gameState->FindLabel("PLAY_RIGHT_BRIDGE");
    if ((int)stateIdx < 0 || gameState->maxStates - 1 < (int)stateIdx) {
        ShowError("Invalid gamestate %d", stateIdx);
    }
    playRightBridge = gameState->stateValues[stateIdx];

    ParseFile(this, "mis\\cb_bridge.mis", (char*)0);

    gameState = g_GameState_0046aa30;
    stateIdx = gameState->FindLabel("PLAY_RIGHT_BRIDGE");
    if ((int)stateIdx < 0 || gameState->maxStates - 1 < (int)stateIdx) {
        ShowError("Invalid gamestate %d", stateIdx);
    }
    playRightBridge = gameState->stateValues[stateIdx];

    if (spriteAction == 0) {
        spriteAction = new SpriteAction(
            savedCommand,
            savedMsgData,
            handlerId,
            moduleParam,
            4, 0, 0, 0, 0, 0
        );
    }

    if (palette != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToMessageLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = palette;
    }

    timer = new TimeOut();
}

/* Function start: 0x437C60 */
void SC_Wahoo::ShutDown(SC_MessageParser* msg) {
    void* ptr;
    int i;

    ptr = palette;
    if (ptr != 0) {
        delete (Palette*)ptr;
        palette = 0;
    }

    ptr = bgSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        bgSprite = 0;
    }

    ptr = resetSwitchSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        resetSwitchSprite = 0;
    }

    ptr = detectionMask;
    if (ptr != 0) {
        delete (VBuffer*)ptr;
        detectionMask = 0;
    }

    ptr = innerSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        innerSprite = 0;
    }

    ptr = middleSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        middleSprite = 0;
    }

    ptr = outerSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        outerSprite = 0;
    }

    if (g_WahooEngine_0046bbfc != 0) {
        delete g_WahooEngine_0046bbfc;
        g_WahooEngine_0046bbfc = 0;
    }

    ptr = consoleSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        consoleSprite = 0;
    }

    ptr = timer;
    if (ptr != 0) {
        delete (TimeOut*)ptr;
        timer = 0;
    }

    ptr = spriteAction;
    if (ptr != 0) {
        delete (SpriteAction*)ptr;
        spriteAction = 0;
    }

    ptr = soundList;
    if (ptr != 0) {
        delete (SoundList*)ptr;
        soundList = 0;
    }

    winSound = 0;
    matchSound = 0;
    startSound = 0;
    resetSound = 0;
    missSound = 0;
    sound1 = 0;

    for (i = 0xe; i != 0; i--) {
        ptr = sampleSlots[0xe - i];
        if (ptr != 0) {
            ((Sample*)ptr)->Unload();
            operator delete(ptr);
            sampleSlots[0xe - i] = 0;
        }
    }

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return;
}

/* Function start: 0x4381E0 */
int SC_Wahoo::AddMessage(SC_MessageParser* msg) {
    SpriteAction* m = (SpriteAction*)msg;

    if (m->lastKey == 0x1b) {
        m->fromType = handlerId;
        m->fromValue = moduleParam;
        m->instruction = 0;
        ProcessState();
        return 1;
    }

    m->fromType = handlerId;
    m->fromValue = moduleParam;
    m->instruction = 0;

    if (m->button1 >= 1) {
        if (m->mousePos.x < cursorHitbox.left || cursorHitbox.right < m->mousePos.x ||
            m->mousePos.y < cursorHitbox.top || cursorHitbox.bottom < m->mousePos.y) {
            // Original quirk: Wahoo mode packs a mouse-over flag into bit 0 of the
            // hotspotPool slot (0xB8); kept as a raw int access on the pointer field.
            *(int*)&g_WahooEngine_0046bbfc->hotspotPool = *(int*)&g_WahooEngine_0046bbfc->hotspotPool & 0xFFFFFFFE;
        } else {
            *(int*)&g_WahooEngine_0046bbfc->hotspotPool = *(int*)&g_WahooEngine_0046bbfc->hotspotPool | 1;
            if (m->button1 > 1) {
                gameFlags = gameFlags | 2;
                ProcessState();
                return 1;
            }
        }
    }

    return 1;
}

/* Function start: 0x438280 */
int SC_Wahoo::Exit(SC_MessageParser* msg) {
    SpriteAction* m = (SpriteAction*)msg;
    int id = handlerId;

    if (m->addressType != id) {
        return 0;
    }

    switch (m->instruction) {
    case 0:
        break;
    case 7:
        SendGameMessage(1, id, id, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        break;
    default:
        return 0;
    }

    return 1;
}

/* Function start: 0x438470 */
void SC_Wahoo::OnProcessEnd() {
    int cursorX;

    cursorHitbox.left = 0;
    cursorHitbox.top = 200;
    cursorHitbox.right = 0x140;
    cursorHitbox.bottom = 0xF0;

    if (resetSwitchSprite != 0) {
        resetSwitchSprite->ResetAnimation(0, 0);
    }
    innerSprite->ResetAnimation(0, 0);
    outerSprite->ResetAnimation(4, 0);

    if (resetSwitchSprite != 0) {
        resetSwitchSprite->ResetAnimation(1, 0);
    }
    innerSprite->ResetAnimation(3, 0);
    middleSprite->ResetAnimation(4, 0);
    outerSprite->ResetAnimation(1, 0);

    if (playRightBridge != 0) {
        switchHitbox.left = 0x116;
        switchHitbox.top = 0x40;
        switchHitbox.right = 0x121;
        switchHitbox.bottom = 0x4B;
        playAreaHitbox.left = 0x11;
        playAreaHitbox.top = 0x15;
        playAreaHitbox.right = 0x79;
    } else {
        switchHitbox.left = 0x1F;
        switchHitbox.top = 0x40;
        switchHitbox.right = 0x2A;
        switchHitbox.bottom = 0x4B;
        playAreaHitbox.left = 0xBB;
        playAreaHitbox.top = 0x15;
        playAreaHitbox.right = 0x12F;
    }

    playAreaHitbox.bottom = 0x91;

    if (g_InputManager_0046aa08 != 0) {
        (g_InputManager_0046aa08)->Refresh(1);
    }

    if (consoleSprite != 0) {
        cursorX = 0;
        // Original bug at 0x438470: DAT_0046aa08 is dereferenced after only the earlier Refresh guard.
        if (g_InputManager_0046aa08->pMouse != 0) {
            cursorX = g_InputManager_0046aa08->pMouse->x;
        }
        consoleSprite->ResetAnimation(cursorX / (screenSize.x / 5), 0);
    }

    if (startSound != 0) {
        startSound->Play(100, 0);
    }

    if (sampleSlots[9] != 0) {
        sampleSlots[9]->Play(100, 1);
    }
}

/* Function start: 0x437EA0 */
void SC_Wahoo::Update(int param1, int param2) {
    Sprite* spr;
    InputState* cursor;
    int cursorX;
    int cursorY;
    int frame;

    if (handlerId != param2) {
        return;
    }

    if (timer->m_isActive == 1) {
        if (timer->IsTimeOut() == 0) {
            return;
        }
        int val = playRightBridge;
        GameState* gs = g_GameState_0046aa30;
        int idx = gs->FindLabel("COMBAT_BRIDGE_STATE");
        gs->ValidateIndex(idx);
        gs->stateValues[idx] = val + 1;
        ProcessState();
        return;
    }

    spr = bgSprite;
    if ((gameFlags & 1) != 0) {
        if (spr->Do(spr->loc.x, spr->loc.y, 1.0) != 0) {
            timer->Start(2000);
        }
        spr = innerSprite;
        if (spr != 0) {
            spr->Do(spr->loc.x, spr->loc.y, 1.0);
        }
        spr = middleSprite;
        if (spr != 0) {
            spr->Do(spr->loc.x, spr->loc.y, 1.0);
        }
        spr = outerSprite;
        if (spr == 0) {
            return;
        }
        spr->Do(spr->loc.x, spr->loc.y, 1.0);
        return;
    }

    if (spr != 0) {
        spr->Do(spr->loc.x, spr->loc.y, 1.0);
    }
    spr = resetSwitchSprite;
    if (spr != 0) {
        spr->Do(spr->loc.x, spr->loc.y, 1.0);
    }
    spr = innerSprite;
    if (spr != 0) {
        spr->Do(spr->loc.x, spr->loc.y, 1.0);
    }
    spr = middleSprite;
    if (spr != 0) {
        spr->Do(spr->loc.x, spr->loc.y, 1.0);
    }
    spr = outerSprite;
    if (spr != 0) {
        spr->Do(spr->loc.x, spr->loc.y, 1.0);
    }

    cursorY = 0;
    cursor = g_InputManager_0046aa08->pMouse;
    if (cursor != 0) {
        cursorY = cursor->y;
    }
    cursorX = 0;
    if (cursor != 0) {
        cursorX = cursor->x;
    }

    if (cursorX < cursorHitbox.left || cursorHitbox.right < cursorX ||
        cursorY < cursorHitbox.top || cursorHitbox.bottom < cursorY) {
        spr = g_Mouse_0046aa18->m_sprite;
        if (spr == 0) goto label_done;
        frame = 0xe;
    } else if (cursor == 0 || cursor->x < 0xa0) {
        spr = g_Mouse_0046aa18->m_sprite;
        if (spr == 0) goto label_done;
        frame = 0xa;
    } else {
        spr = g_Mouse_0046aa18->m_sprite;
        if (spr == 0) goto label_done;
        frame = 0xb;
    }
    spr->ResetAnimation(frame, 0);

label_done:
    if (g_WahooEngine_0046bbfc == 0) return;
    if (consoleSprite == 0) return;

    ((Weapon*)g_WahooEngine_0046bbfc)->UpdateProjectiles();

    if (g_WahooEngine_0046bbfc->palette != 0) {
        cursorX = 0;
        if (g_InputManager_0046aa08->pMouse != 0) {
            cursorX = g_InputManager_0046aa08->pMouse->x;
        }
        consoleSprite->ResetAnimation(cursorX / (screenSize.x / 3) + 5, 0);
    }

    spr = consoleSprite;
    if (spr->Do(spr->loc.x, spr->loc.y, 1.0) != 0) {
        cursorX = 0;
        if (g_InputManager_0046aa08->pMouse != 0) {
            cursorX = g_InputManager_0046aa08->pMouse->x;
        }
        consoleSprite->ResetAnimation(cursorX / (screenSize.x / 5), 0);
    }
}

/* Function start: 0x4382F0 */
void SC_Wahoo::ProcessState() {
    SpriteAction* sa;

    if (savedCommand == 0x2B && (gameFlags & 1) != 0) {
        sa = spriteAction;
        if (sa != 0) {
            delete sa;
            spriteAction = 0;
        }
        spriteAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        {
            SC_MessageParser temp;
            ParseFile(&temp, "mis\\cb_bridge.mis", "[WIN_LBL_PR]");
        }
    }

    EnqueueSpriteAction(spriteAction);
    sa = spriteAction;
    if (sa != 0) {
        delete sa;
        spriteAction = 0;
    }
}

/* Function start: 0x438630 */
/* Function start: 0x438660 */
int SC_Wahoo::LBLParse(char* param_1) { // prologue at 0x438630
    char local_b8[128];
    char local_38[32];
    int local_18;

    local_18 = 0;
    local_b8[0] = 0;
    local_38[0] = 0;
    sscanf(param_1, "%s", local_38);

    if (strcmp(local_38, "PALETTE") == 0) {
        sscanf(param_1, "%s %s", local_38, local_b8);
        palette = new Palette();
        palette->LoadFile(local_b8);
    }
    else if (strcmp(local_38, "DETECTION_MASK") == 0) {
        sscanf(param_1, "%s %s", local_38, local_b8);
        detectionMask = new VBuffer(ResolveAssetPath(local_b8), 0);
    }
    else if (strcmp(local_38, "BACKGROUND_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        bgSprite = spr;
        Parser::ProcessFile((Parser*)spr, this, (char*)0);
    }
    else if (strcmp(local_38, "RESET_SWITCH_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        resetSwitchSprite = spr;
        Parser::ProcessFile((Parser*)spr, this, (char*)0);
    }
    else if (strcmp(local_38, "INNER_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        innerSprite = spr;
        Parser::ProcessFile((Parser*)spr, this, (char*)0);
    }
    else if (strcmp(local_38, "MIDDLE_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        middleSprite = spr;
        Parser::ProcessFile((Parser*)spr, this, (char*)0);
    }
    else if (strcmp(local_38, "OUTER_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        outerSprite = spr;
        Parser::ProcessFile((Parser*)spr, this, (char*)0);
    }
    else if (strcmp(local_38, "CONSOLE_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        consoleSprite = spr;
        Parser::ProcessFile((Parser*)spr, this, (char*)0);
    }
    else if (strcmp(local_38, "SET_GAMESTATE") == 0) {
        sscanf(param_1, " %s %s %d ", local_38, local_b8, &local_18);
        int val = local_18;
        GameState* gs = g_GameState_0046aa30;
        unsigned int idx = gs->FindLabel(local_b8);
        if ((int)idx < 0 || gs->maxStates - 1 < (int)idx) {
            // Original bug at 0x438660: missing gamestate index argument for "%d".
            ShowError("Invalid gamestate %d");
        }
        gs->stateValues[idx] = val;
    }
    else if (strcmp(local_38, "SOUND") == 0) {
        sscanf(param_1, " %s %d %s ", local_38, &local_18, local_b8);
        if (soundList == 0) {
            soundList = new SoundList(0xf);
        }
        if (local_18 == 0) {
            int iVar3 = atoi(local_b8);
            SendGameMessage(5, iVar3, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
        }
        else if (local_18 >= 1 && local_18 <= 6) {
            char* path = MakeAudioName(local_b8);
            Sample* snd = (Sample*)soundList->Register(path);
            if (local_18 == 1) {
                sound1 = snd;
            }
            else if (local_18 == 2) {
                missSound = snd;
            }
            else if (local_18 == 3) {
                resetSound = snd;
            }
            else if (local_18 == 4) {
                startSound = snd;
            }
            else if (local_18 == 5) {
                matchSound = snd;
            }
            else if (local_18 == 6) {
                winSound = snd;
            }
        }
        else if (local_18 >= 7 && local_18 <= 0xd) {
            Sample* smp = new Sample();
            sampleSlots[local_18] = smp;
            char* path = MakeAudioName(local_b8);
            int err = smp->Load(path);
            if (err != 0 && sampleSlots[local_18] != 0) {
                sampleSlots[local_18]->Unload();
                operator delete(sampleSlots[local_18]);
                sampleSlots[local_18] = 0;
            }
        }
        else {
            Parser::ReportUnknownLabel("SC_Wahoo");
        }
    }
    else if (strcmp(local_38, "WEAPON") == 0) {
        int ret = sscanf(param_1, " %s %s ", local_38, local_b8);
        if (ret == 2) {
            if (strcmp(local_b8, "ROCKTHROWER2") == 0) {
                g_WahooEngine_0046bbfc = (SC_CombatBase*)new CombatWeapon(this);
            }
        }
    }
    else if (strcmp(local_38, "END") == 0) {
        return 1;
    }
    else {
        Parser::ReportUnknownLabel("SC_Wahoo");
    }

    return 0;
}

/* Function start: 0x438EF0 */
void SC_Wahoo::OnCombatResult() {
    Sample* s = sound1;
    if (s != 0) {
        s->Play(100, 1);
    }
}

/* Function start: 0x438F10 */
int SC_Wahoo::ProcessClick(Projectile* proj) {
    Sample* resultSnd;
    int iVar3;
    int iVar5;
    int iVar7;
    int* piVar4;
    int* piVar8;
    int uVar10;
    int local_c[3];

    iVar7 = proj->ranges[proj->handle].dim.y - 1;
    if (proj->animation_data == 0) {
        if (iVar7 != 0) {
            return 0;
        }
    }
    else if (proj->animation_data->smk->FrameNum != (unsigned int)iVar7) {
        return 0;
    }

    iVar7 = proj->nextPos.x;
    int bVar1;
    if (iVar7 < switchHitbox.left || switchHitbox.right < iVar7 ||
        proj->nextPos.y < switchHitbox.top || switchHitbox.bottom < proj->nextPos.y) {
        bVar1 = 0;
    } else {
        bVar1 = 1;
    }

    if (!bVar1) {
        if (iVar7 < playAreaHitbox.left || playAreaHitbox.right < iVar7 ||
            proj->nextPos.y < playAreaHitbox.top || playAreaHitbox.bottom < proj->nextPos.y) {
            bVar1 = 0;
        } else {
            bVar1 = 1;
        }
        if (!bVar1) goto LAB_004392b0;

        {
        unsigned int hitZone = detectionMask->CheckHit(
            iVar7 - playAreaHitbox.left,
            proj->nextPos.y - playAreaHitbox.top);

        Sprite* zoneSprite;
        if ((int)hitZone < 1 || 3 < (int)hitZone ||
            (zoneSprite = (&resetSwitchSprite)[hitZone],
             zoneSprite->handle != 0)) {
            if (missSound != 0) {
                missSound->Play(100, 1);
            }
            return 1;
        }

        if (zoneSprite->animation_data == 0) {
            iVar7 = 0;
        } else {
            iVar7 = zoneSprite->animation_data->smk->FrameNum;
        }

        if (hitZone != 2) {
            iVar3 = (iVar7 - 1) / 4 + 1;
        } else {
            if (iVar7 >= 0 && iVar7 <= 3) {
                iVar3 = 1;
            } else if (iVar7 >= 4 && iVar7 <= 9) {
                iVar3 = 4;
            } else if (iVar7 >= 8 && iVar7 <= 0xb) {
                iVar3 = 3;
            } else {
                iVar3 = 2;
            }
        }

        zoneSprite->ResetAnimation(iVar3, 0);

        if (matchSound != 0) {
            matchSound->Play(100, 1);
        }

        iVar3 = 0;
        piVar8 = local_c;
        Sprite** sprSlot = &innerSprite;
        iVar7 = 3;
        do {
            if ((*sprSlot)->handle != 0) {
                *piVar8 = (*sprSlot)->handle;
                piVar8 = piVar8 + 1;
                iVar3 = iVar3 + 1;
            }
            sprSlot = sprSlot + 1;
            iVar7 = iVar7 - 1;
        } while (iVar7 != 0);

        if (iVar3 == 3 && resetSound != 0 &&
            resetSound->m_sample != 0 &&
            resetSound->m_size == ((AILSampleData*)resetSound->m_sample)->len) {
            if (AIL_sample_status(resetSound->m_sample) == 4) {
                resetSound->Stop();
            }
        }

        iVar7 = 1;
        if (1 < iVar3) {
            iVar5 = iVar3 - 1;
            piVar4 = local_c;
            do {
                if (*piVar4 == piVar4[1]) {
                    iVar7 = iVar7 + 1;
                }
                iVar5 = iVar5 - 1;
                piVar4 = piVar4 + 1;
            } while (iVar5 != 0);
        }

        if (resetSwitchSprite->handle == 0 && 1 < iVar3) {
            if (iVar7 == 3) {
                gameFlags = gameFlags | 1;
                bgSprite->ResetAnimation(1, 0);
                resultSnd = winSound;
                if (resultSnd != 0) {
                    uVar10 = 1;
                    goto LAB_00439160;
                }
            }
            else if (iVar7 != iVar3) {
                resetSwitchSprite->ResetAnimation(1, 0);
                resultSnd = startSound;
                if (resultSnd != 0) {
                    uVar10 = 0;
LAB_00439160:
                    resultSnd->Play(100, uVar10);
                }
            }
        }

        local_c[0] = 0xe;
        Sample** smpSlot = sampleSlots;
        do {
            if (*smpSlot != 0) {
                (*smpSlot)->Stop();
            }
            smpSlot = smpSlot + 1;
            local_c[0] = local_c[0] - 1;
        } while (local_c[0] != 0);

        if (iVar7 == iVar3) {
            resultSnd = sampleSlots[iVar3 + 10];
            if (resultSnd != 0) {
                resultSnd->Play(100, 1);
            }
        }
        else if (sampleSlots[10] != 0) {
            sampleSlots[10]->Play(100, 1);
        }
        }
    } else {
        if (resetSwitchSprite->handle == 1) {
            GameState* gs = g_GameState_0046aa30;
            {
            unsigned int stateIdx = gs->FindLabel("NUM_ACTIONS");
            if ((int)stateIdx < 0 || gs->maxStates - 1 < (int)stateIdx) {
                ShowError("Invalid gamestate %d", stateIdx);
            }
            piVar4 = &gs->stateValues[stateIdx];
            }
            *piVar4 = *piVar4 + 5;
            resetSwitchSprite->ResetAnimation(0, 0);
            if (startSound != 0) {
                startSound->Stop();
            }
            {
            Sprite** puVar9 = &innerSprite;
            iVar7 = 3;
            do {
                if ((*puVar9)->handle != 0) {
                    (*puVar9)->ResetAnimation(0, 0);
                    if (resetSound != 0) {
                        if (resetSound->m_sample != 0 &&
                            resetSound->m_size == ((AILSampleData*)resetSound->m_sample)->len) {
                            if (AIL_sample_status(resetSound->m_sample) == 4) goto LAB_004392aa;
                        }
                        resetSound->Play(100, 0);
                    }
                }
LAB_004392aa:
                puVar9 = puVar9 + 1;
                iVar7 = iVar7 - 1;
            } while (iVar7 != 0);
            }
        }
    }

LAB_004392b0:
    if (missSound != 0) {
        missSound->Play(100, 1);
    }
    return 1;
}
