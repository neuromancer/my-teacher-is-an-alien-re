#include "SC_Wahoo.h"
#include "stubs.h"
#include "MouseControl.h"
#include "SpriteAction.h"
#include "MouseControl.h"
#include "Sprite.h"
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

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

// FUN_00421930 = TimeOut::~TimeOut (full game)

extern "C" void ShowError(const char* format, ...);
// FUN_00412a50 = Parser::LBLParse in Parser.h
extern char* __cdecl ResolveAssetPath(char* name, ...);
#include "string.h"

// FUN_00410fb0 = VBuffer ctor — callers updated to use new
#include "RockThrower.h"
#include "SC_CombatBase.h"


// FUN_00426ce0 is thiscall with 1 param (not fastcall)
// Use local class wrapper for correct calling convention

// TimerObj = TimeOut (ctor 0x421920 wraps 0x421960, dtor 0x421930)
// Check() = IsTimeOut() @ 0x421A30, Set() = Start() @ 0x4219F0
#include "TimeOut.h"
#include "MouseControl.h"

class DetectionObj {
public:
    void Render();            // 0x427880
};

// DetMask IS VBuffer — CheckHit at 0x411330 now in VBuffer.h
void DetectionObj::Render() {}

// FUN_00413e70 = Parser::ProcessFile in Parser.cpp
// FUN_00413e10 = ParseFile in Parser.h

#include "globals.h"

extern "C" int FileExists(const char*);
extern "C" void SetVideoRes(int, int);
extern "C" void WriteToLog(const char* format, ...);

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
    int uVar1 = handlerId;
    int* puVar6 = (int*)&spriteAction;
    int iVar7;
    for (iVar7 = 0x30; iVar7 != 0; iVar7--) {
        *puVar6 = 0;
        puVar6++;
    }
    handlerId = uVar1;

    CopyCommandData((SC_MessageParser*)msg);

    gameFlags = 0;

    if (!FileExists("CB_Bridge")) {
        ShowLoadingScreen();
    }

    screenSize.x = 0x140;
    screenSize.y = 0xf0;
    SetVideoRes(0x140, 0xf0);

    moduleParam = ((SpriteAction*)msg)->addressValue;

    void* pvVar4 = g_GameState_0046aa30;
    unsigned int uVar3 = ((GameState*)pvVar4)->FindLabel("PLAY_RIGHT_BRIDGE");
    if ((int)uVar3 < 0 || ((GameState*)pvVar4)->maxStates - 1 < (int)uVar3) {
        ShowError("Invalid gamestate %d", uVar3);
    }
    playRightBridge = ((GameState*)pvVar4)->stateValues[uVar3];

    ParseFile(this, "mis\\cb_bridge.mis", (char*)0);

    pvVar4 = g_GameState_0046aa30;
    uVar3 = ((GameState*)pvVar4)->FindLabel("PLAY_RIGHT_BRIDGE");
    if ((int)uVar3 < 0 || ((GameState*)pvVar4)->maxStates - 1 < (int)uVar3) {
        ShowError("Invalid gamestate %d", uVar3);
    }
    playRightBridge = ((GameState*)pvVar4)->stateValues[uVar3];

    if (spriteAction == 0) {
        spriteAction = new SpriteAction(
            savedCommand,
            savedMsgData,
            handlerId,
            moduleParam,
            4, 0, 0, 0, 0, 0
        );
    }

    iVar7 = (int)palette;
    if (iVar7 != 0) {
        int* piVar8 = (int*)((int)g_ZBufferManager_0046aa24 + 0xa8);
        if (*piVar8 != 0) {
            WriteToLog("ddouble palette");
        }
        *piVar8 = iVar7;
    }

    timer = new TimeOut();
}

/* Function start: 0x437C60 */
int SC_Wahoo::ShutDown(SC_MessageParser* msg) {
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

    return 0;
}

/* Function start: 0x4381E0 */
int SC_Wahoo::AddMessage(SC_MessageParser* msg) {
    int* m = (int*)msg;

    if (m[0xB] == 0x1b) {
        m[2] = handlerId;
        m[3] = moduleParam;
        m[4] = 0;
        ProcessState();
        return 1;
    }

    m[2] = handlerId;
    m[3] = moduleParam;
    m[4] = 0;

    if (m[9] >= 1) {
        if (m[7] < cursorHitbox.left || cursorHitbox.right < m[7] ||
            m[8] < cursorHitbox.top || cursorHitbox.bottom < m[8]) {
            *(int*)&g_WahooEngine_0046bbfc->hotspotPool = *(int*)&g_WahooEngine_0046bbfc->hotspotPool & 0xFFFFFFFE;
        } else {
            *(int*)&g_WahooEngine_0046bbfc->hotspotPool = *(int*)&g_WahooEngine_0046bbfc->hotspotPool | 1;
            if (m[9] > 1) {
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
    int* m = (int*)msg;
    int id = handlerId;

    if (m[0] != id) {
        return 0;
    }

    switch (m[4]) {
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
    int iVar1;

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
        iVar1 = 0;
        if (*(int*)((int)g_InputManager_0046aa08 + 0x1A0) != 0) {
            iVar1 = **(int**)((int)g_InputManager_0046aa08 + 0x1A0);
        }
        consoleSprite->ResetAnimation(iVar1 / (screenSize.x / 5), 0);
    }

    if (startSound != 0) {
        startSound->Play(100, 0);
    }

    if (sampleSlots[9] != 0) {
        ((Sample*)sampleSlots[9])->Play(100, 1);
    }
}

/* Function start: 0x437EA0 */
void SC_Wahoo::Update(int param1, int param2) {
    Sprite* spr;
    int* cursorData;
    int cursorX;
    int cursorY;
    int frame;

    if (handlerId != param2) {
        return;
    }

    if (*(int*)(timer) == 1) {
        if (timer->IsTimeOut() == 0) {
            return;
        }
        int val = playRightBridge;
        void* gs = g_GameState_0046aa30;
        int idx = ((GameState*)gs)->FindLabel("COMBAT_BRIDGE_STATE");
        ((GameState*)gs)->ValidateIndex(idx);
        ((GameState*)gs)->stateValues[idx] = val + 1;
        ProcessState();
        return;
    }

    spr = bgSprite;
    if ((*(char*)((int)&gameFlags) & 1) != 0) {
        if (spr->Do(spr->loc_x, spr->loc_y, 1.0) != 0) {
            timer->Start(2000);
        }
        spr = innerSprite;
        if (spr != 0) {
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
        }
        spr = middleSprite;
        if (spr != 0) {
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
        }
        spr = (Sprite*)outerSprite;
        if (spr == 0) {
            return;
        }
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
        return;
    }

    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }
    spr = (Sprite*)resetSwitchSprite;
    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }
    spr = (Sprite*)innerSprite;
    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }
    spr = (Sprite*)middleSprite;
    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }
    spr = (Sprite*)outerSprite;
    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }

    cursorY = 0;
    cursorData = *(int**)((int)g_InputManager_0046aa08 + 0x1A0);
    if (cursorData != 0) {
        cursorY = cursorData[1];
    }
    cursorX = 0;
    if (cursorData != 0) {
        cursorX = cursorData[0];
    }

    if (cursorX < cursorHitbox.left || cursorHitbox.right < cursorX ||
        cursorY < cursorHitbox.top || cursorHitbox.bottom < cursorY) {
        spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
        if (spr == 0) goto label_done;
        frame = 0xe;
    } else if (cursorData == 0 || cursorData[0] < 0xa0) {
        spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
        if (spr == 0) goto label_done;
        frame = 0xa;
    } else {
        spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
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
        if (*(int**)((int)g_InputManager_0046aa08 + 0x1A0) != 0) {
            cursorX = **(int**)((int)g_InputManager_0046aa08 + 0x1A0);
        }
        consoleSprite->ResetAnimation(cursorX / (screenSize.x / 3) + 5, 0);
    }

    spr = (Sprite*)consoleSprite;
    if (spr->Do(spr->loc_x, spr->loc_y, 1.0) != 0) {
        cursorX = 0;
        if (*(int**)((int)g_InputManager_0046aa08 + 0x1A0) != 0) {
            cursorX = **(int**)((int)g_InputManager_0046aa08 + 0x1A0);
        }
        consoleSprite->ResetAnimation(cursorX / (screenSize.x / 5), 0);
    }
}

/* Function start: 0x4382F0 */
void SC_Wahoo::ProcessState() {
    void* sa;

    if (savedCommand == 0x2B && (*(char*)(&gameFlags) & 1) != 0) {
        sa = spriteAction;
        if (sa != 0) {
            delete (SpriteAction*)sa;
            spriteAction = 0;
        }
        spriteAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        {
            SC_MessageParser temp;
            ParseFile(&temp, "mis\\cb_bridge.mis", "_WIN_LBL_PR_");
        }
    }

    EnqueueSpriteAction(spriteAction);
    sa = spriteAction;
    if (sa != 0) {
        delete (SpriteAction*)sa;
        spriteAction = 0;
    }
}

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
        palette->Load(local_b8);
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
        void* gs = g_GameState_0046aa30;
        unsigned int idx = ((GameState*)gs)->FindLabel(local_b8);
        if ((int)idx < 0 || ((GameState*)gs)->maxStates - 1 < (int)idx) {
            ShowError("Invalid gamestate %d");
        }
        ((GameState*)gs)->stateValues[idx] = val;
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
                sound1 = (int)snd;
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
                matchSound = (int)snd;
            }
            else if (local_18 == 6) {
                winSound = (int)snd;
            }
        }
        else if (local_18 >= 7 && local_18 <= 0xd) {
            Sample* smp = new Sample();
            sampleSlots[local_18] = (void*)smp;
            char* path = MakeAudioName(local_b8);
            int err = smp->Load(path);
            if (err != 0 && sampleSlots[local_18] != 0) {
                ((Sample*)sampleSlots[local_18])->Unload();
                operator delete(sampleSlots[local_18]);
                sampleSlots[local_18] = 0;
            }
        }
        else {
            Parser::LBLParse("SC_Wahoo");
        }
    }
    else if (strcmp(local_38, "WEAPON") == 0) {
        int ret = sscanf(param_1, " %s %s ", local_38, local_b8);
        if (ret == 2) {
            if (strcmp(local_b8, "ROCKTHROWER2") == 0) {
                g_WahooEngine_0046bbfc = (SC_CombatBase*)new RockThrower(this);
            }
        }
    }
    else if (strcmp(local_38, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("SC_Wahoo");
    }

    return 0;
}

/* Function start: 0x438EF0 */
int SC_Wahoo::OnCombatResult() {
    TODO("SC_Wahoo::OnCombatResult");
    return 0;
}

/* Function start: 0x438F10 */
int SC_Wahoo::ProcessClick(int param_1) {
    void* pvVar6;
    int iVar3;
    int iVar5;
    int iVar7;
    int* piVar4;
    int* piVar8;
    int uVar10;
    int local_c[3];

    iVar7 = ((Sprite*)param_1)->ranges[((Sprite*)param_1)->handle].dim.y - 1;
    if (((Sprite*)param_1)->animation_data == 0) {
        if (iVar7 != 0) {
            return 0;
        }
    }
    else if (*(int*)((int)((Sprite*)param_1)->animation_data->smk + 0x374) != iVar7) {
        return 0;
    }

    iVar7 = *(int*)(param_1 + 0x120);
    int bVar1;
    if (iVar7 < switchHitbox.left || switchHitbox.right < iVar7 ||
        *(int*)(param_1 + 0x124) < switchHitbox.top || switchHitbox.bottom < *(int*)(param_1 + 0x124)) {
        bVar1 = 0;
    } else {
        bVar1 = 1;
    }

    if (!bVar1) {
        if (iVar7 < playAreaHitbox.left || playAreaHitbox.right < iVar7 ||
            *(int*)(param_1 + 0x124) < playAreaHitbox.top || playAreaHitbox.bottom < *(int*)(param_1 + 0x124)) {
            bVar1 = 0;
        } else {
            bVar1 = 1;
        }
        if (!bVar1) goto LAB_004392b0;

        {
        unsigned int uVar2 = detectionMask->CheckHit(
            iVar7 - playAreaHitbox.left,
            *(int*)(param_1 + 0x124) - playAreaHitbox.top);

        if ((int)uVar2 < 1 || 3 < (int)uVar2 ||
            (pvVar6 = (&resetSwitchSprite)[uVar2],
             ((Sprite*)pvVar6)->handle != 0)) {
            if (missSound != 0) {
                missSound->Play(100, 1);
            }
            return 1;
        }

        if (((Sprite*)pvVar6)->animation_data == 0) {
            iVar7 = 0;
        } else {
            iVar7 = *(int*)((int)((Sprite*)pvVar6)->animation_data->smk + 0x374);
        }

        if (uVar2 != 2) {
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

        ((Sprite*)pvVar6)->ResetAnimation(iVar3, 0);

        if (matchSound != 0) {
            ((Sample*)matchSound)->Play(100, 1);
        }

        iVar3 = 0;
        piVar8 = local_c;
        piVar4 = (int*)&innerSprite;
        iVar7 = 3;
        do {
            if (((Sprite*)*piVar4)->handle != 0) {
                *piVar8 = ((Sprite*)*piVar4)->handle;
                piVar8 = piVar8 + 1;
                iVar3 = iVar3 + 1;
            }
            piVar4 = piVar4 + 1;
            iVar7 = iVar7 - 1;
        } while (iVar7 != 0);

        if (iVar3 == 3 && (iVar7 = (int)resetSound, iVar7 != 0) &&
            (iVar5 = *(int*)(iVar7 + 0xc), iVar5 != 0) &&
            *(int*)(iVar5 + 0xc) == *(int*)(iVar7 + 4)) {
            iVar7 = AIL_sample_status((HSAMPLE)iVar5);
            if (iVar7 == 4) {
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
                pvVar6 = (void*)winSound;
                if (pvVar6 != 0) {
                    uVar10 = 1;
                    goto LAB_00439160;
                }
            }
            else if (iVar7 != iVar3) {
                resetSwitchSprite->ResetAnimation(1, 0);
                pvVar6 = startSound;
                if (pvVar6 != 0) {
                    uVar10 = 0;
LAB_00439160:
                    ((Sample*)pvVar6)->Play(100, uVar10);
                }
            }
        }

        local_c[0] = 0xe;
        piVar4 = (int*)sampleSlots;
        do {
            if (*piVar4 != 0) {
                ((Sample*)*piVar4)->Stop();
            }
            piVar4 = piVar4 + 1;
            local_c[0] = local_c[0] - 1;
        } while (local_c[0] != 0);

        if (iVar7 == iVar3) {
            pvVar6 = sampleSlots[iVar3 + 10];
            if (pvVar6 != 0) {
                ((Sample*)pvVar6)->Play(100, 1);
            }
        }
        else if (sampleSlots[10] != 0) {
            ((Sample*)sampleSlots[10])->Play(100, 1);
        }
        }
    } else {
        if (resetSwitchSprite->handle == 1) {
            pvVar6 = g_GameState_0046aa30;
            {
            unsigned int uVar2 = ((GameState*)pvVar6)->FindLabel("NUM_ACTIONS");
            if ((int)uVar2 < 0 || ((GameState*)pvVar6)->maxStates - 1 < (int)uVar2) {
                ShowError("Invalid gamestate %d", uVar2);
            }
            piVar4 = &((GameState*)pvVar6)->stateValues[uVar2];
            }
            *piVar4 = *piVar4 + 5;
            resetSwitchSprite->ResetAnimation(0, 0);
            if (startSound != 0) {
                startSound->Stop();
            }
            {
            void** puVar9 = (void**)&innerSprite;
            iVar7 = 3;
            do {
                if (((Sprite*)*puVar9)->handle != 0) {
                    ((Sprite*)*puVar9)->ResetAnimation(0, 0);
                    iVar3 = (int)resetSound;
                    if (iVar3 != 0) {
                        iVar5 = *(int*)(iVar3 + 0xc);
                        if (iVar5 != 0 && *(int*)(iVar3 + 4) == *(int*)(iVar5 + 0xc)) {
                            iVar3 = AIL_sample_status((HSAMPLE)iVar5);
                            if (iVar3 == 4) goto LAB_004392aa;
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
