#include "EngineB.h"
#include "SoundList.h"
#include "Memory.h"
#include "Sample.h"
#include "Animation.h"
#include "VBuffer.h"
#include "Sprite.h"
#include "GameState.h"
#include "globals.h"
#include "Target.h"
#include "InputManager.h"
#include "EngineSubsystems.h"
#include "RockThrower.h"
#include "ScoreDisplay.h"
#include "TimeOut.h"
#include "string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "string.h"
extern void __stdcall EnqueueHotspotAction(SpriteAction*);
static char* FormatSoundPath(char* path);
void StartScheduleTimer2();

#include "Target.h"
#include "InputManager.h"

#include "RockThrower.h"

/* Function start: 0x450B10 */
EngineB::EngineB() {
    memset(&m_targetConfig, 0, 0x88);
}

/* Function start: 0x450C10 */
EngineB::~EngineB() {
    if (EngineB::m_meterAnimation != 0) {
        delete EngineB::m_meterAnimation;
        EngineB::m_meterAnimation = 0;
    }

    if (EngineB::m_localSoundList != 0) {
        delete EngineB::m_localSoundList;
        EngineB::m_localSoundList = 0;
    }

    if (EngineB::m_targetConfig != 0) {
        operator delete(EngineB::m_targetConfig);
        EngineB::m_targetConfig = 0;
    }

    EngineB::m_meterBuffer = 0;
    EngineB::m_missSound = 0;
    EngineB::reserved_0x108 = 0;
    EngineB::reserved_0x10C = 0;
    EngineB::m_ambientSound2 = 0;
    EngineB::m_completionSound = 0;
    EngineB::reserved_0x120 = 0;
    EngineB::reserved_0x11C = 0;
    EngineB::m_ambientSound = 0;
    EngineB::m_hitSound3 = 0;
    EngineB::m_hitSound2 = 0;
    EngineB::m_hitSound1 = 0;
    EngineB::m_tauntSound2 = 0;
    EngineB::m_tauntSound1 = 0;
    EngineB::m_milestoneSound = 0;
}

/* Function start: 0x451690 */
int EngineB::LBLParse(char* line) {
    char token[32];
    sscanf(line, "%s", token);
    if (strcmp(token, "END_DERIVED_ENGINE_INFO") == 0) {
        return 1;
    }
    SC_CombatBase::LBLParse(line);
    return 0;
}

/* Function start: 0x451180 */
void EngineB::ProcessFrame() {
    ((TargetList*)g_TargetList_0046ae58)->ProcessTargets();
    if (g_RatsState_00473e18 == 1) {
        EngineB::m_weaponParser->UpdateProjectiles();
        RenderBackground();
        PostRender();
    }
}

/* Function start: 0x450DB0 */
void EngineB::RenderBackground() {
    int hitCount;
    int missCount;
    int r;
    Sample* snd;
    int weaponHit;
    int mouseX;
    int divisor;

    if (g_BgSprite_0046ae50 == 0) {
        return;
    }

    hitCount = g_ScoreDisplay_0046ae6c->hitCount;
    if (EngineB::m_prevHitCount != hitCount) {
        EngineB::m_prevHitCount = hitCount;
        g_BgSprite_0046ae50->ResetAnimation(8, 0);
        EngineB::m_progress.start += EngineB::m_targetConfig[1];
        r = rand() % 3;
        snd = (&m_hitSound1)[r];
        EngineB::m_localSoundList->StopAll();
        if (snd != 0) {
            snd->Play(100, 1);
        }
    } else {
        missCount = g_ScoreDisplay_0046ae6c->missCount;
        if (EngineB::m_prevMissCount != missCount) {
            EngineB::m_prevMissCount = missCount;
            g_BgSprite_0046ae50->ResetAnimation(9, 0);
            r = rand() % 2;
            snd = (&m_tauntSound1)[r];
            if (snd != 0) {
                snd->Play(100, 1);
            }
        }
    }

    weaponHit = ((RockThrower*)m_weaponParser)->m_hitCountFull;
    if (weaponHit != 0) {
        EngineB::m_progress.start += weaponHit;
        if (EngineB::m_missSound != 0) {
            EngineB::m_missSound->Play(100, 1);
        }
        if (EngineB::m_progress.start == 0x13 || EngineB::m_progress.start == 0x25) {
            if (EngineB::m_milestoneSound != 0) {
                EngineB::m_milestoneSound->Play(100, 1);
            }
        }
    }

    if (g_CombatWeapon_0046ae60->m_clicked != 0) {
        mouseX = 0;
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        }
        divisor = g_WeaponParser_0046ae4c->dimensions.x / 3;
        g_BgSprite_0046ae50->ResetAnimation(mouseX / divisor + 5, 0);
    }

    if (g_BgSprite_0046ae50->Do(g_BgSprite_0046ae50->loc.x, g_BgSprite_0046ae50->loc.y, 1.0)) {
        mouseX = 0;
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        }
        divisor = g_WeaponParser_0046ae4c->dimensions.x / 5;
        g_BgSprite_0046ae50->ResetAnimation(mouseX / divisor, 0);
    }
}

/* Function start: 0x450F70 */
void EngineB::PostRender() {
    if (m_meterBuffer == 0) {
        return;
    }

    if (m_progress.end != 0 && m_progress.start >= m_progress.end) {
        static TimeOut completionTimeout;

        g_BackBuffer_0046aa14->CallBlitter2(
            0,
            m_meterFullRect.right,
            m_meterFullRect.top,
            m_meterFullRect.bottom,
            m_meterPosition.x,
            m_meterPosition.y,
            m_meterBuffer);

        if (completionTimeout.m_isActive == 1) {
            if (completionTimeout.IsTimeOut() == 0) {
                return;
            }
            StartScheduleTimer2();
            return;
        }

        completionTimeout.Start(0x5dc);
        return;
    }

    // Original bug at 0x450F70: zero m_progress.end reaches this divide.
    int fillRight = (m_progress.start * 0x36) / m_progress.end;
    fillRight = (fillRight - (rand() % 3)) + 1;
    if (fillRight >= 0) {
        if (fillRight > 0x36) {
            fillRight = 0x36;
        }
    } else {
        fillRight = 0;
    }
    fillRight = 0x12 + fillRight * 4;

    g_BackBuffer_0046aa14->CallBlitter2(
        0,
        fillRight,
        m_meterFullRect.top,
        m_meterFullRect.bottom,
        m_meterPosition.x,
        m_meterPosition.y,
        m_meterBuffer);

    g_BackBuffer_0046aa14->CallBlitter2(
        fillRight,
        m_meterFullRect.right,
        m_meterEmptyRect.top,
        m_meterEmptyRect.bottom,
        m_meterPosition.x + fillRight,
        m_meterPosition.y,
        m_meterBuffer);
}

/* Function start: 0x4511C0 */ /* No assembly extracted */
int EngineB::HandleAction(int* param) {
    return 0;
}

/* Function start: 0x451230 */
void EngineB::OnProcessEnd() {
    int* config;
    int stateVal;
    GameState* gs;
    int idx;
    int i;
    int mouseX;
    int divisor;
    SoundList* sList;
    Animation* anim;

    config = (int*)new IntPair();
    if (config != 0) {
        gs = g_GameState_0046aa30;
        if (g_PeriodStateIdx_0046cb90 < 0 || gs->maxStates - 1 < g_PeriodStateIdx_0046cb90) {
            ShowError("Invalid gamestate %d", g_PeriodStateIdx_0046cb90);
        }
        stateVal = gs->stateValues[g_PeriodStateIdx_0046cb90];
        switch (stateVal) {
        case 0:
            config[0] = 1;
            config[1] = 3;
            break;
        case 1:
            config[0] = 1;
            config[1] = 2;
            break;
        case 2:
            config[0] = 1;
            config[1] = 1;
            break;
        }
    } else {
        config = 0;
    }
    EngineB::m_targetConfig = config;

    if (g_InputManager_0046aa08 != 0) {
        ((InputManager*)g_InputManager_0046aa08)->Refresh(1);
    }

    if (g_BgSprite_0046ae50 != 0) {
        mouseX = 0;
        // Original bug at 0x451230: DAT_0046aa08 is dereferenced after only the earlier Refresh guard.
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        }
        divisor = g_WeaponParser_0046ae4c->dimensions.x / 5;
        g_BgSprite_0046ae50->ResetAnimation(mouseX / divisor, 0);
    }

    EngineB::m_weaponParser = g_CombatWeapon_0046ae60;
    for (i = 0; i < g_TargetList_0046ae58->count; i++) {
        int* target = (int*)*(int*)((char*)g_TargetList_0046ae58 + 0x94 + i * 4);
        target[0x4A] = EngineB::m_targetConfig[0];
    }

    EngineB::m_localSoundList = new SoundList(10);

    anim = new Animation(FormatStringVA("combats\\\\nmeter.smk"));
    EngineB::m_meterAnimation = anim;
    anim->DoFrame();

    EngineB::m_meterBuffer = EngineB::m_meterAnimation->targetBuffer;
    EngineB::m_meterEmptyRect.right = 0x10D;
    EngineB::m_meterEmptyRect.left = 0;
    EngineB::m_meterEmptyRect.top = 0;
    EngineB::m_meterEmptyRect.bottom = 0x13;

    gs = g_GameState_0046aa30;
    idx = gs->FindState("MAX_NOISE_ALLOWED_RATS");

    EngineB::m_meterFullRect.right = 0x10D;
    EngineB::m_meterFullRect.left = 0;
    EngineB::m_meterFullRect.top = 0x14;
    EngineB::m_meterFullRect.bottom = 0x27;

    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    EngineB::m_progress.start = 0;
    EngineB::m_meterPosition.x = 0x19;
    EngineB::m_meterPosition.y = 0x16;
    EngineB::m_progress.end = gs->stateValues[idx];

    if (g_SoundList_0046ae68 != 0) {
        EngineB::m_missSound = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\\\snd3012"));
        EngineB::m_completionSound = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\\\snd5009"));
    }

    sList = EngineB::m_localSoundList;
    if (sList != 0) {
        EngineB::m_tauntSound1 = (Sample*)sList->Register(FormatSoundPath("cb_rats\\\\snd5007"));
        EngineB::m_tauntSound2 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\\\snd5008"));
        EngineB::m_hitSound1 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\\\snd5004"));
        EngineB::m_hitSound2 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\\\snd5005"));
        EngineB::m_hitSound3 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\\\snd5006"));
        EngineB::m_milestoneSound = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\\\snd5002"));
        EngineB::m_ambientSound = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\\\snd5001"));
        EngineB::m_ambientSound2 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\\\snd5010"));
    }

    g_RatsState_00473e18 = 0;
}

/* Function start: 0x451680 */
static char* FormatSoundPath(char* path) {
    return MakeAudioName(path);
}

/* Function start: 0x4510E0 */
void StartScheduleTimer2() {
    SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    action.instruction = 0x37;
    action.extra1 = 1;
    EnqueueHotspotAction(&action);
}
