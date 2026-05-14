#include "SC_SaveLoad.h"
#include "T_MenuHotspot.h"
#include "Sprite.h"
#include "Palette.h"
#include "SpriteAction.h"
#include "GameState.h"
#include "ZBufferManager.h"
#include "MouseControl.h"
#include "Memory.h"
#include <string.h>
#include <new>

#include "globals.h"
extern "C" void ShowError(const char* format, ...);
extern "C" void WriteToLog(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x421C40 */
SC_SaveLoad::SC_SaveLoad()
{
    int rect[4];

    memset(&timer, 0, 0xC * 4);
    handlerId = 0x2F;
    timer.Reset();

    sprite = new Sprite("mainmenu\\save_yn.smk");
    sprite->priority = 0;

    palette = new Palette("mainmenu\\menu.col");

    rect[0] = 0x9B;
    rect[1] = 0xFB;
    rect[2] = 0xEA;
    rect[3] = 0x11A;
    btnYes = new T_MenuButton("mainmenu\\ansr_y.smk", rect);

    rect[0] = 0x11D;
    rect[1] = 0xFD;
    rect[2] = 0x16C;
    rect[3] = 0x11B;
    btnNo = new T_MenuButton("mainmenu\\ansr_n.smk", rect);

    rect[0] = 0x19C;
    rect[1] = 0xFC;
    rect[2] = 0x1EB;
    rect[3] = 0x11B;
    btnCancel = new T_MenuButton("mainmenu\\ansr_c.smk", rect);
}

/* Function start: 0x421F80 */
SC_SaveLoad::~SC_SaveLoad()
{
    {
        Sprite* spr = sprite;
        if (spr != 0) {
            spr->~Sprite();
            FreeMemory(spr);
            sprite = 0;
        }
    }
    if (btnYes != 0) {
        Sprite* spr = btnYes->sprite;
        if (spr != 0) {
            spr->~Sprite();
            FreeMemory(spr);
            btnYes->sprite = 0;
        }
        FreeMemory(btnYes);
        btnYes = 0;
    }
    if (btnNo != 0) {
        Sprite* spr = btnNo->sprite;
        if (spr != 0) {
            spr->~Sprite();
            FreeMemory(spr);
            btnNo->sprite = 0;
        }
        FreeMemory(btnNo);
        btnNo = 0;
    }
    if (btnCancel != 0) {
        Sprite* spr = btnCancel->sprite;
        if (spr != 0) {
            spr->~Sprite();
            FreeMemory(spr);
            btnCancel->sprite = 0;
        }
        FreeMemory(btnCancel);
        btnCancel = 0;
    }
}

/* Function start: 0x422280 */
int SC_SaveLoad::ShutDown(SC_MessageParser* msg) {
    (void)msg;
    if (sprite != 0) {
        sprite->StopAnimationSound();
    }
    if (btnYes != 0 && btnYes->sprite != 0) {
        btnYes->sprite->StopAnimationSound();
    }
    if (btnNo != 0 && btnNo->sprite != 0) {
        btnNo->sprite->StopAnimationSound();
    }
    if (btnCancel != 0 && btnCancel->sprite != 0) {
        btnCancel->sprite->StopAnimationSound();
    }
    return 0;
}

/* Function start: 0x4221A0 */
void SC_SaveLoad::Init(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    CopyCommandData(msg);
    if (action != 0) {
        returnCommand = action->extra1;
        returnData = action->extra2;
        moduleParam = action->addressValue;
        GameState* gs = g_GameState_0046aa30;
        int idx = gs->FindState("MUST_SAVEGAME");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        if (gs->stateValues[idx] == 0) {
            SendGameMessage(handlerId, moduleParam, 0, 0, 2, 2, 0, 0, 0, 0);
        }
    }
    if (palette != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = palette;
    }
    if (g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0, 0);
    }
}

/* Function start: 0x4222F0 */
void SC_SaveLoad::Update(int param1, int param2) {
    GlyphRect rect;
    unsigned int elapsed = timer.Update();
    if (elapsed > 30000) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    if (handlerId != param2) return;
    timer.Reset();
    if (sprite != 0) {
        sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
    }
    btnYes->SimpleUpdate();
    btnNo->SimpleUpdate();
    btnCancel->SimpleUpdate();
    g_Mouse_0046aa18->DrawCursor();
}

/* Function start: 0x4223F0 */
int SC_SaveLoad::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    WriteMessageAddress(msg);
    timer.Reset();
    if (action->lastKey != 0) {
        if (action->lastKey == 0xD || action->lastKey == 0x59 || action->lastKey == 0x79) {
            action->instruction = 2;
            action->extra1 = 1;
            return 1;
        }
        if (action->lastKey == 0x4E || action->lastKey == 0x6E) {
            action->instruction = 2;
            action->extra1 = 0;
            return 1;
        }
        if (action->lastKey == 0x1B) {
            action->instruction = 2;
            action->extra1 = 2;
            return 1;
        }
    } else {
        if (action->button1 <= 1) return 1;
        int bHit;
        {
            SlimeDim pt = action->mousePos;
            T_MenuButton* btn = btnYes;
            if (btn->bounds.left > pt.x ||
                btn->bounds.right < pt.x ||
                btn->bounds.top > pt.y ||
                btn->bounds.bottom < pt.y) {
                bHit = 0;
            } else {
                bHit = 1;
            }
        }
        if (bHit) {
            action->instruction = 2;
            action->extra1 = 1;
            return 1;
        }
        {
            SlimeDim pt = action->mousePos;
            T_MenuButton* btn = btnNo;
            if (btn->bounds.left > pt.x ||
                btn->bounds.right < pt.x ||
                btn->bounds.top > pt.y ||
                btn->bounds.bottom < pt.y) {
                bHit = 0;
            } else {
                bHit = 1;
            }
        }
        if (bHit) {
            action->instruction = 2;
            action->extra1 = 2;
            return 1;
        }
        {
            SlimeDim pt = action->mousePos;
            T_MenuButton* btn = btnCancel;
            if (btn->bounds.left > pt.x ||
                btn->bounds.right < pt.x ||
                btn->bounds.top > pt.y ||
                btn->bounds.bottom < pt.y) {
                bHit = 0;
            } else {
                bHit = 1;
            }
        }
        if (bHit) {
            action->instruction = 2;
            action->extra1 = 0;
            return 1;
        }
    }
    return 1;
}

/* Function start: 0x4226C0 */
int SC_SaveLoad::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (handlerId != action->addressType) {
        return 0;
    }
    timer.Reset();
    switch (action->instruction) {
    case 2: {
        switch (action->extra1) {
        case 0:
            SendGameMessage(savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
            return 1;
        case 1: {
            GameState* gs = g_GameState_0046aa30;
            int idx = gs->FindState("MUST_SAVEGAME");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 0;
            SendGameMessage(0x2E, 0, returnCommand, returnData, 4, 0, 0, 0, 0, 0);
            return 1;
        }
        case 2: {
            GameState* gs = g_GameState_0046aa30;
            int idx = gs->FindState("MUST_SAVEGAME");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 0;
            SendGameMessage(returnCommand, returnData, savedCommand, savedMsgData, 4, 0, 0, 0, 0, 0);
            return 1;
        }
        }
        return 1;
    }
    case 7:
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    }
    return 1;
}
