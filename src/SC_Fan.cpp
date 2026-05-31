#include "SC_Fan.h"
#include "stubs.h"
#include "Sprite.h"
#include "Projectile.h"
#include "Animation.h"
#include "SpriteAction.h"
#include "Palette.h"
#include "Sample.h"
#include "GameState.h"
#include "RockThrower.h"
#include "mss.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <new.h>
#include "TimeOut.h"
#include "VBuffer.h"
#include "ZBufferManager.h"
#include "SC_Question.h"
#include "InputManager.h"
#include "MouseControl.h"
#include "main.h"
#include "string.h"

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" int FileExists(const char*);
extern "C" void WriteToLog(const char* format, ...);
extern char* __cdecl ResolveAssetPath(char* name, ...);
// FUN_00427880 = Weapon::UpdateProjectiles — callers updated

#include "globals.h"

/* Function start: 0x40EFF0 */
SC_Fan::SC_Fan()
{
    memset(&mainAction, 0, 0x120);
    handlerId = 0x3E;
}

/* Function start: 0x40F190 */
SC_Fan::~SC_Fan() {
    Cleanup(0);
}

/* Function start: 0x40F290 */
void SC_Fan::Init(SC_MessageParser* msg) {
    int savedId;
    void* mem;
    void* ptr;
    int* target;

    savedId = handlerId;
    memset(&mainAction, 0, 0x120);
    handlerId = savedId;

    if (!FileExists("CB_Fan")) {
        ShowLoadingScreen();
    }

    Handler::Init(msg);

    field_B0 = 0;
    dim_B4.x = 0x140;
    dim_B4.y = 0xF0;

    SetVideoRes(0x140, 0xF0);

    moduleParam = ((SpriteAction*)msg)->addressValue;

    ParseFile(this, "mis\\cb_fan.mis", (char*)0);

    if (mainAction == 0) {
        mainAction = new SpriteAction(savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }

    ptr = palette;
    if (ptr != 0) {
        target = (int*)&g_ZBufferManager_0046aa24->m_palette;
        if (*target != 0) {
            WriteToLog("ddouble palette");
        }
        *target = (int)ptr;
    }

    timer = new TimeOut();
}

/* Function start: 0x40F420 */
void SC_Fan::Cleanup(int flag) {
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

    ptr = stopSwitchSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        stopSwitchSprite = 0;
    }

    ptr = tlSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        tlSprite = 0;
    }

    ptr = blSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        blSprite = 0;
    }

    ptr = trSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        trSprite = 0;
    }

    ptr = brSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        brSprite = 0;
    }

    if (g_ActiveWeapon_00468ef0 != 0) {
        delete g_ActiveWeapon_00468ef0;
        g_ActiveWeapon_00468ef0 = 0;
    }

    ptr = consoleSprite;
    if (ptr != 0) {
        delete (Sprite*)ptr;
        consoleSprite = 0;
    }

    ptr = noiseMeterBuffer;
    if (ptr != 0) {
        delete (VBuffer*)ptr;
        noiseMeterBuffer = 0;
    }

    ptr = timer;
    if (ptr != 0) {
        delete (TimeOut*)ptr;
        timer = 0;
    }

    ptr = loseAction;
    if (ptr != 0) {
        delete (SpriteAction*)ptr;
        loseAction = 0;
    }

    ptr = mainAction;
    if (ptr != 0) {
        delete (SpriteAction*)ptr;
        mainAction = 0;
    }

    for (i = 0xb; i != 0; i--) {
        ptr = samples[0xb - i];
        if (ptr != 0) {
            samples[0xb - i]->Unload();
            operator delete(samples[0xb - i]);
            samples[0xb - i] = 0;
        }
    }

    bgSoundId = 0;

    if (flag != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
}

/* Function start: 0x40F660 */
void SC_Fan::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }

    switch (state) {
    case 0:
        State0Handler();
        return;
    case 1:
        RenderFan();
        return;
    case 2:
        State2Handler();
        return;
    case 3:
        ProcessRound();
        return;
    case 4:
        State4Handler();
        return;
    }
}

/* Function start: 0x40F6C0 */
int SC_Fan::AddMessage(SC_MessageParser* msg) {
    WriteMessageAddress(msg);

    if (((SpriteAction*)msg)->lastKey == 0x1B) {
        state = 4;
    }

    return 1;
}

/* Function start: 0x40F6F0 */
int SC_Fan::Exit(SC_MessageParser* msg) {
    SpriteAction* p;

    p = (SpriteAction*)msg;
    if (p->addressType != handlerId) {
        return 0;
    }

    switch (p->instruction) {
    case 0:
        break;
    case 7:
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        ShowError("SCMI_INSERT");
        break;
    default:
        return 0;
    }
    return 1;
}

/* Function start: 0x40F760 */
void SC_Fan::ProcessRound() {
    void* ptr;

    if (savedCommand == 0x2B) {
        SendGameMessage(4, bgSoundId, handlerId, moduleParam, 0x13, 0, 100, 0, 0, 0);

        if (state == 2) {
            ptr = mainAction;
            if (ptr != 0) {
                delete (SpriteAction*)ptr;
                mainAction = 0;
            }
            {
                mainAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                Parser temp;
                ParseFile(&temp, "mis\\cb_fan.mis", "[WIN_LBL_PR]");
            }
        } else if (state == 3) {
            ptr = mainAction;
            if (ptr != 0) {
                delete (SpriteAction*)ptr;
                mainAction = 0;
            }
            {
                mainAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                Parser temp;
                ParseFile(&temp, "mis\\cb_fan.mis", "[LOSE_LBL_PR]");
            }
        }
    } else {
        SendGameMessage(4, bgSoundId, handlerId, moduleParam, 0x13, 0, 1000, 0, 0, 0);
        SendGameMessage(5, 0, handlerId, moduleParam, 0x13, 0, 0, 0, 0, 0);

        if (state == 4) {
            mainAction->extra1 = 1;
            mainAction->mousePos.x = 0x78;
            mainAction->mousePos.y = 0;

            GameState* gs = g_GameState_0046aa30;
            int idx = gs->FindLabel("NUM_ACTIONS");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] += 0x14;
        } else if (state == 3) {
            EnqueueSpriteAction(loseAction);
            ptr = loseAction;
            if (ptr != 0) {
                delete (SpriteAction*)ptr;
                loseAction = 0;
            }
            ptr = mainAction;
            if (ptr != 0) {
                delete (SpriteAction*)ptr;
                mainAction = 0;
            }
            return;
        } else if (state == 2) {
            GameState* gs = g_GameState_0046aa30;
            int idx = gs->FindLabel("NUM_ACTIONS");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] += 0x1E;

            gs = g_GameState_0046aa30;
            idx = gs->FindLabel("COMBAT_FAN_WON");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 1;

            mainAction->extra1 = 1;
            mainAction->mousePos.x = 0x78;
            mainAction->mousePos.y = 0;
        }
    }

    EnqueueSpriteAction(mainAction);
    ptr = mainAction;
    if (ptr != 0) {
        delete (SpriteAction*)ptr;
        mainAction = 0;
    }
}

/* Function start: 0x40FBD0 */
void SC_Fan::DisplaySprites(int frame) {
    int i;
    Sprite** ptr;

    bgSprite->ResetAnimation(frame, 0);

    ptr = &tlSprite;
    i = 4;
    do {
        if (*ptr != 0) {
            int val;
            val = *((int*)ptr + 4) ? -1 : 0;
            val = (val & 5) + frame;
            (*ptr)->ResetAnimation(val, 0);
        }
        ptr++;
        i--;
    } while (i != 0);
}

/* Function start: 0x40FC30 */
void SC_Fan::ProcessHit() {
    Sample* s = samples[2];
    if (s != 0) {
        s->Play(100, 1);
    }
}

/* Function start: 0x40FC50 */
int SC_Fan::HandleInput(Sprite* spr) {
    Projectile* p = (Projectile*)spr;
    int finalFrame = p->ranges[p->handle].dim.y - 1;
    Animation* anim = p->animation_data;
    if (anim == 0) {
        if (finalFrame != 0) return 0;
    } else {
        if (anim->smk->FrameNum != finalFrame) return 0;
    }

    dim_168.x++;

    int inSlot;
    if (invSlot_D0.left <= p->nextPos.x && invSlot_D0.right >= p->nextPos.x &&
        invSlot_D0.top <= p->nextPos.y && invSlot_D0.bottom >= p->nextPos.y) {
        inSlot = 1;
    } else {
        inSlot = 0;
    }

    if (!inSlot) {
    if (bgSprite->handle != 3) goto end_processing;

    {
        int inGlobalRect;
        if (g_FanField1_00472be0 <= p->nextPos.x && g_FanField3_00472be8 >= p->nextPos.x &&
            g_FanField2_00472be4 <= p->nextPos.y && g_FanField4_00472bec >= p->nextPos.y) {
            inGlobalRect = 1;
        } else {
            inGlobalRect = 0;
        }
        if (!inGlobalRect) goto end_processing;
    }

    {
        int i;
        Rect* slotRect = &invSlots_100[0];
        int* flagPtr = &field_F0;
        for (i = 0; i < 4; i++) {
            int hit;
            if (slotRect->left <= p->nextPos.x && slotRect->right >= p->nextPos.x &&
                slotRect->top <= p->nextPos.y && slotRect->bottom >= p->nextPos.y) {
                hit = 1;
            } else {
                hit = 0;
            }
            if (hit && *flagPtr == 0) {
                (&field_F0)[i] = 1;
                (&tlSprite)[i]->ResetAnimation(4, 0);
                if (samples[5] != 0) {
                    samples[5]->Play(100, 1);
                }
                field_140++;
                if (field_140 == 4) {
                    state = 2;
                    bgSprite->ResetAnimation(4, 0);
                    if (samples[8] != 0) {
                        samples[8]->Play(100, 1);
                    }
                    if (samples[9] != 0) {
                        samples[9]->Play(100, 1);
                    }
                    return 1;
                }
                int r = rand() % 2;
                if ((&samples[6])[r] != 0) {
                    (&samples[6])[r]->Play(100, 1);
                }
                return 1;
            }
            slotRect++;
            flagPtr++;
        }
    }
    } else {
        if (bgSprite->handle != 1) goto end_processing;
        DisplaySprites(2);
        if (samples[4] != 0) {
            samples[4]->Play(100, 1);
        }
        SendGameMessage(4, bgSoundId, handlerId, moduleParam, 0x13, 1, 0x3e8, 0, 0, 0);
        return 1;
    }

end_processing:
    if (samples[3] != 0) {
        samples[3]->Play(100, 1);
    }
    return 1;
}

/* Function start: 0x40FEA0 */
void SC_Fan::State0Handler() {
    Sample* sample;

    RenderFan();

    if (introPhase == 0) {
        introPhase = 1;
        SendGameMessage(4, bgSoundId, handlerId, moduleParam, 0x13, 0x32, 0x3E8, 0, 0, 0);

        sample = samples[1];
        if (sample != 0) {
            sample->Play(100, 1);
        }
        return;
    }

    if (introPhase == 1) {
        sample = samples[1];
        if (sample != 0) {
            if ((int)sample->m_sample != 0) {
                if (sample->m_size == *(int*)((int)sample->m_sample + 0xC)) {
                    if (AIL_sample_status(sample->m_sample) == 4) {
                        return;
                    }
                }
            }
        }
        introPhase++;
        state = 1;
        inputActive = 1;
    }
}

/* Function start: 0x40FF40 */
void SC_Fan::State2Handler() {
    Sprite* sprite;
    Sample* sample;

    sprite = bgSprite;
    if (sprite->Do(sprite->loc_x, sprite->loc_y, 1.0)) {
        bgSprite->ResetAnimation(5, 0);
    }

    sample = samples[8];
    if (sample != 0) {
        if ((int)sample->m_sample != 0) {
            if (sample->m_size == *(int*)((int)sample->m_sample + 0xC)) {
                if (AIL_sample_status(sample->m_sample) == 4) {
                    return;
                }
            }
        }
    }

    ProcessRound();
}

/* Function start: 0x40FFC0 */
void SC_Fan::State4Handler() {
    Sample* sample;
    int i;

    if (exitPhase == 0) {
        i = 0;
        do {
            if (samples[i] != 0) {
                samples[i]->~Sample();
            }
            i++;
        } while (i < 0xB);

        sample = samples[10];
        if (sample != 0) {
            sample->Play(100, 1);
        }

        exitPhase++;
        inputActive = 0;
        RenderFan();
        return;
    }

    if (exitPhase == 1) {
        sample = samples[10];
        if (sample != 0) {
            if ((int)sample->m_sample != 0) {
                if (sample->m_size == *(int*)((int)sample->m_sample + 0xC)) {
                    if (AIL_sample_status(sample->m_sample) == 4) {
                        goto end;
                    }
                }
            }
        }
        exitPhase++;
        ProcessRound();
    }

end:
    RenderFan();
}

/* Function start: 0x410060 */
void SC_Fan::RenderFan() {
    Sprite* sprite;
    void** ptr;
    int state;
    int i;

    sprite = bgSprite;
    if (sprite == 0) {
        return;
    }

    state = sprite->handle;

    if (state == 3) {
        if (stopSwitchSprite != 0) {
            if (stopSwitchSprite->Do(stopSwitchSprite->loc_x, stopSwitchSprite->loc_y, 1.0)) {
                DisplaySprites(0);
                SendGameMessage(4, bgSoundId, handlerId, moduleParam, 0x13, 0x32, 0x3E8, 0, 0, 0);
            }
        }
    }

    {
        Sprite** sp = &tlSprite;
        i = 4;
        do {
            if (*sp != 0) {
                (*sp)->Do((*sp)->loc_x, (*sp)->loc_y, 1.0);
            }
            sp++;
            i--;
        } while (i != 0);
    }

    sprite = bgSprite;
    if (sprite->Do(sprite->loc_x, sprite->loc_y, 1.0)) {
        if (state == 0) {
            DisplaySprites(1);
        } else if (state == 2) {
            if (stopSwitchSprite != 0) {
                stopSwitchSprite->ResetAnimation(1, 0);
            }
            DisplaySprites(3);
        }
    }

    if (noiseMeterBuffer != 0) {
        int dimVal;
        dimVal = dim_168.y;
        if (dimVal != 0 && dim_168.x >= dimVal) {
            (g_ZBufferManager_0046aa24)->DrawVBufferRegion(
                noiseMeterBuffer, 0x7531, dim_170.x, dim_170.y, 2, 1.0,
                invSlot_158.left, invSlot_158.top, invSlot_158.right, invSlot_158.bottom);

            if (timer->m_isActive == 1) {
                if (timer->IsTimeOut()) {
                    state = 3;
                }
            } else {
                timer->Start(0x5DC);
            }
        } else {
            int offset;
            int rnd;
            offset = dim_168.x * 0x36 / dimVal;
            rnd = rand();
            offset = offset - rnd % 3 + 1;
            if (offset >= 0) {
                if (offset > 0x36) {
                    offset = 0x36;
                }
            } else {
                offset = 0;
            }
            offset = offset * 4 + 0x12;

            (g_ZBufferManager_0046aa24)->DrawVBufferRegion(
                noiseMeterBuffer, 0x7531, dim_170.x, dim_170.y, 2, 1.0,
                invSlot_158.left, invSlot_158.top, offset, invSlot_158.bottom);

            (g_ZBufferManager_0046aa24)->DrawVBufferRegion(
                noiseMeterBuffer, 0x7531, dim_170.x + offset, dim_170.y, 2, 1.0,
                offset, invSlot_148.top, invSlot_148.right, invSlot_148.bottom);
        }
    }

    if (inputActive != 0) {
        int frames;
        InputState* p;

        (g_Mouse_0046aa18)->DrawCursor();
        g_ActiveWeapon_00468ef0->UpdateProjectiles();

        if (g_ActiveWeapon_00468ef0->m_clicked != 0) {
            frames = 0;
            p = g_InputManager_0046aa08->pMouse;
            if (p != 0) {
                frames = p->x;
            }
            consoleSprite->ResetAnimation(frames / (dim_B4.x / 3) + 5, 0);
        }

        sprite = consoleSprite;
        if (sprite->Do(sprite->loc_x, sprite->loc_y, 1.0)) {
            frames = 0;
            p = g_InputManager_0046aa08->pMouse;
            if (p != 0) {
                frames = p->x;
            }
            sprite->ResetAnimation(frames / (dim_B4.x / 5), 0);
        }
    }
}

/* Function start: 0x410390 */
void SC_Fan::OnProcessEnd()
{
    invSlot_D0.left = 0xDB;
    invSlot_D0.top = 0xBA;
    invSlot_D0.right = 0xEC;
    invSlot_D0.bottom = 0xC7;
    g_FanField1_00472be0 = 0x90;
    g_FanField2_00472be4 = 0x18;
    g_FanField3_00472be8 = 0xF0;
    g_FanField4_00472bec = 0x64;
    invSlots_100[0].left = 0x99;
    invSlots_100[0].right = 0x9C;
    invSlots_100[1].left = 0x99;
    invSlots_100[1].right = 0x9C;
    invSlots_100[1].bottom = 0x5E;
    invSlots_100[2].left = 0xE3;
    invSlots_100[0].top = 0x1E;
    invSlots_100[0].bottom = 0x21;
    invSlots_100[1].top = 0x5A;
    invSlots_100[2].top = 0x1E;
    invSlots_100[2].right = 0xE7;
    invSlots_100[3].left = 0xE3;
    invSlots_100[3].bottom = 0x5E;
    invSlots_100[2].bottom = 0x22;
    invSlots_100[3].top = 0x5A;
    invSlots_100[3].right = 0xE6;
    field_F4 = 0;
    field_F0 = 0;
    field_FC = 0;
    field_F8 = 0;
    field_140 = 0;
    dim_168.x = 0;
    inputActive = 0;
    state = 0;
}

/* Function start: 0x4104B0 */
/* Function start: 0x4104E0 */
int SC_Fan::LBLParse(char* param_1) { // prologue at 0x4104B0
    char local_b8[128];
    char local_38[32];
    int local_18;

    local_18 = 0;
    local_b8[0] = 0;
    local_38[0] = 0;
    sscanf(param_1, "%s", local_38);

    if (strcmp(local_38, "PALETTE") == 0) {
        sscanf(param_1, "%s %s", local_38, local_b8);
        Palette* pal = new Palette();
        palette = pal;
        pal->Load(local_b8);
    }
    else if (strcmp(local_38, "BACKGROUND_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        bgSprite = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "STOP_SWITCH_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        stopSwitchSprite = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "TL_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        tlSprite = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "BL_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        blSprite = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "TR_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        trSprite = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "BR_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        brSprite = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "CONSOLE_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        consoleSprite = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (strcmp(local_38, "NOISE_METER_VBUFFER") == 0) {
        sscanf(param_1, " %s %s %d %d %d ", local_38, local_b8,
               &dim_170.x, &dim_170.y, &dim_168.y);
        noiseMeterBuffer = new VBuffer(ResolveAssetPath(local_b8), 0);
        invSlot_148.right = 0x10D;
        invSlot_148.left = 0;
        invSlot_148.top = 0;
        invSlot_158.left = 0;
        invSlot_158.right = 0x10D;
        invSlot_148.bottom = 0x13;
        invSlot_158.top = 0x14;
        invSlot_158.bottom = 0x27;
    }
    else if (strcmp(local_38, "NUM_BOLTS_REMAINING") == 0) {
        /* recognized but no action */
    }
    else if (strcmp(local_38, "SET_GAMESTATE") == 0) {
        sscanf(param_1, " %s %s %d ", local_38, local_b8, &local_18);
        int val = local_18;
        GameState* gs = g_GameState_0046aa30;
        int idx = gs->FindLabel(local_b8);
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] = val;
    }
    else if (strcmp(local_38, "SOUND") == 0) {
        int ret = sscanf(param_1, " %s %d %s ", local_38, &local_18, local_b8);
        if (ret != 3 || local_18 < -1 || 10 < local_18) {
            ReportUnknownLabel("SC_Fan");
        }
        if (local_18 == 0) {
            SendGameMessage(5, atoi(local_b8), handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
        }
        else if (local_18 == -1) {
            bgSoundId = atoi(local_b8);
            SendGameMessage(4, bgSoundId, handlerId, moduleParam, 0x1b, 1, 0, 0, 0, 0);
        }
        else {
            Sample* smp = new Sample();
            samples[local_18] = smp;
            char* path = MakeAudioName(local_b8);
            int err = smp->Load(path);
            if (err != 0 && samples[local_18] != 0) {
                samples[local_18]->Unload();
                operator delete(samples[local_18]);
                samples[local_18] = 0;
            }
        }
    }
    else if (strcmp(local_38, "WEAPON") == 0) {
        int ret = sscanf(param_1, " %s %s ", local_38, local_b8);
        if (ret == 2) {
            if (strcmp(local_b8, "ROCKTHROWER2") == 0) {
                g_ActiveWeapon_00468ef0 = new RockThrower(this);
            } else {
                ReportUnknownLabel("SC_Fan");
            }
        }
    }
    else if (strcmp(local_38, "LOSE_MESSAGE") == 0) {
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        loseAction = action;
        ParseSpriteAction(action, this);
    }
    else if (strcmp(local_38, "END") == 0) {
        return 1;
    }
    else {
        ReportUnknownLabel("SC_Fan");
    }

    return 0;
}
