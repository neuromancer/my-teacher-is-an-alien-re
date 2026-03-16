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
#include "string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern InputManager* DAT_0046aa08;
extern "C" GameState* DAT_0046aa30;
extern int DAT_0046cb90;
extern "C" int DAT_00473e18;

extern "C" void* DAT_0046ae4c;
extern "C" void* DAT_0046ae50;
extern "C" void* DAT_0046ae58;
extern "C" void* DAT_0046ae60;
extern "C" void* DAT_0046ae68;
extern "C" void* DAT_0046ae6c;

extern char* __cdecl FUN_0044e470(char*);
static char* FormatSoundPath(char* path);

class TargetListFull {
public:
    void ProcessTargets();  // 0x443660
};

class WeaponParser {
public:
    void UpdateProjectiles();  // 0x427880
};

class InputManagerFull {
public:
    void PollEvents(int param);  // 0x426ce0
};

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
    EngineB::field_0x108 = 0;
    EngineB::field_0x10C = 0;
    EngineB::field_0x110 = 0;
    EngineB::m_completionSound = 0;
    EngineB::field_0x120 = 0;
    EngineB::field_0x11C = 0;
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
void EngineB::method10() {
    ((TargetListFull*)DAT_0046ae58)->ProcessTargets();
    if (DAT_00473e18 == 1) {
        ((WeaponParser*)EngineB::m_weaponParser)->UpdateProjectiles();
        method11();
        method12();
    }
}

/* Function start: 0x450DB0 */
void EngineB::method11() {
    int hitCount;
    int missCount;
    int r;
    Sample* snd;
    int weaponHit;
    int mouseX;
    int divisor;

    if (DAT_0046ae50 == 0) {
        return;
    }

    hitCount = ((int*)DAT_0046ae6c)[5];
    if (EngineB::m_prevHitCount != hitCount) {
        EngineB::m_prevHitCount = hitCount;
        ((Sprite*)DAT_0046ae50)->ResetAnimation(8, 0);
        EngineB::m_progress.start += EngineB::m_targetConfig[1];
        r = rand() % 3;
        snd = (&m_hitSound1)[r];
        EngineB::m_localSoundList->StopAll();
        if (snd != 0) {
            snd->Play(100, 1);
        }
    } else {
        missCount = ((int*)DAT_0046ae6c)[3];
        if (EngineB::m_prevMissCount != missCount) {
            EngineB::m_prevMissCount = missCount;
            ((Sprite*)DAT_0046ae50)->ResetAnimation(9, 0);
            r = rand() % 2;
            snd = (&m_tauntSound1)[r];
            if (snd != 0) {
                snd->Play(100, 1);
            }
        }
    }

    weaponHit = ((int*)EngineB::m_weaponParser)[0x30];
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

    if (((int*)DAT_0046ae60)[0x2A] != 0) {
        mouseX = 0;
        if (((int*)DAT_0046aa08)[0x68] != 0) {
            mouseX = *(int*)((int*)DAT_0046aa08)[0x68];
        }
        divisor = ((int*)DAT_0046ae4c)[0x26] / 3;
        ((Sprite*)DAT_0046ae50)->ResetAnimation(mouseX / divisor + 5, 0);
    }

    if (((Sprite*)DAT_0046ae50)->Do(((Sprite*)DAT_0046ae50)->num_states, ((Sprite*)DAT_0046ae50)->field_0xb0, 1.0)) {
        mouseX = 0;
        if (((int*)DAT_0046aa08)[0x68] != 0) {
            mouseX = *(int*)((int*)DAT_0046aa08)[0x68];
        }
        divisor = ((int*)DAT_0046ae4c)[0x26] / 5;
        ((Sprite*)DAT_0046ae50)->ResetAnimation(mouseX / divisor, 0);
    }
}

/* Function start: 0x450F70 */
int EngineB::method12() {
    return 0;
}

/* Function start: 0x4511C0 */
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

    config = (int*)operator new(8);
    if (config != 0) {
        char* p = (char*)config + 1;
        *(int*)p = 0;
        *(short*)(p + 4) = 0;
        p[6] = 0;

        gs = DAT_0046aa30;
        if (DAT_0046cb90 < 0 || gs->maxStates - 1 < DAT_0046cb90) {
            ShowError("Invalid gamestate %d", DAT_0046cb90);
        }
        stateVal = gs->stateValues[DAT_0046cb90];
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

    if (DAT_0046aa08 != 0) {
        ((InputManagerFull*)DAT_0046aa08)->PollEvents(1);
    }

    if (DAT_0046ae50 != 0) {
        mouseX = 0;
        if (((int*)DAT_0046aa08)[0x68] != 0) {
            mouseX = *(int*)((int*)DAT_0046aa08)[0x68];
        }
        divisor = ((int*)DAT_0046ae4c)[0x26] / 5;
        ((Sprite*)DAT_0046ae50)->ResetAnimation(mouseX / divisor, 0);
    }

    EngineB::m_weaponParser = (int)DAT_0046ae60;
    for (i = 0; i < ((int*)DAT_0046ae58)[0x24]; i++) {
        int* target = (int*)*(int*)((char*)DAT_0046ae58 + 0x94 + i * 4);
        target[0x4A] = EngineB::m_targetConfig[0];
    }

    EngineB::m_localSoundList = new SoundList(10);

    anim = new Animation(FormatStringVA("combats\\nmeter.smk"));
    EngineB::m_meterAnimation = anim;
    anim->DoFrame();

    EngineB::m_meterBuffer = (int)EngineB::m_meterAnimation->targetBuffer;
    EngineB::m_meterEmptyRect.right = 0x10D;
    EngineB::m_meterEmptyRect.left = 0;
    EngineB::m_meterEmptyRect.top = 0;
    EngineB::m_meterEmptyRect.bottom = 0x13;

    gs = DAT_0046aa30;
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

    if (DAT_0046ae68 != 0) {
        EngineB::m_missSound = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\snd3012"));
        EngineB::m_completionSound = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\snd5009"));
    }

    sList = EngineB::m_localSoundList;
    if (sList != 0) {
        EngineB::m_tauntSound1 = (Sample*)sList->Register(FormatSoundPath("cb_rats\\snd5007"));
        EngineB::m_tauntSound2 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\snd5008"));
        EngineB::m_hitSound1 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\snd5004"));
        EngineB::m_hitSound2 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\snd5005"));
        EngineB::m_hitSound3 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\snd5006"));
        EngineB::m_milestoneSound = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\snd5002"));
        EngineB::m_ambientSound = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\snd5001"));
        EngineB::field_0x110 = (Sample*)EngineB::m_localSoundList->Register(FormatSoundPath("cb_rats\\snd5010"));
    }

    DAT_00473e18 = 0;
}

/* Function start: 0x451680 */
static char* FormatSoundPath(char* path) {
    return FUN_0044e470(path);
}
