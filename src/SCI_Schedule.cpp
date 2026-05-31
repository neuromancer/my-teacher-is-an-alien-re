#include "SCI_Schedule.h"
#include "Sprite.h"
#include "Palette.h"
#include "SpriteAction.h"
#include "T_MenuHotspot.h"
#include "Memory.h"
#include "SC_Question.h"
#include "GameState.h"
#include "MouseControl.h"
#include "InputManager.h"
#include "ZBufferManager.h"
#include <stdio.h>
#include <string.h>

#include "globals.h"

extern "C" void WriteToLog(const char*, ...);
extern "C" void ShowError(const char* format, ...);

/* Function start: 0x434C10 */
SCI_Schedule::SCI_Schedule()
{
    memset(&palette, 0, 0xC * 4);

    handlerId = 0x27;
    ParseFile(this, "mis\\\\schedule.mis", (char*)0);

    Palette* pal = new Palette();
    palette = pal;
    pal->Load("schedual\\\\schedule.col");

    Sprite* spr = new Sprite("schedual\\\\schedule.smk");
    bgSprite = spr;
    spr->ConfigStates(3);
    spr->ConfigRange(0, 1, 1, 1);
    spr->ConfigRange(1, 2, 2, 1);
    spr->ConfigRange(2, 3, 3, 1);
    bgSprite->priority = 0;

    Sprite* spr2 = new Sprite("schedual\\\\shedbox1.smk");
    selBox1 = spr2;
    selBox1->priority = 5;
    selBox1->flags |= 0x40;

    Sprite* spr3 = new Sprite("schedual\\\\shedbox2.smk");
    selBox2 = spr3;
    selBox2->priority = 5;
    selBox2->flags |= 0x40;

    Sprite* spr4 = new Sprite("schedual\\\\tardies.smk");
    Sprite** ebx = &tardiesSprite;
    *ebx = spr4;
    (*ebx)->loc.x = 0x54;
    (*ebx)->loc.y = 0x190;

    Sprite* spr5 = new Sprite("schedual\\\\cuts.smk");
    cutsSprite = spr5;
    cutsSprite->loc.x = 0x105;
    cutsSprite->loc.y = 0x190;

    Sprite* spr6 = new Sprite("schedual\\\\detents.smk");
    scheduleSprite = spr6;
    scheduleSprite->loc.x = 0x1BB;
    scheduleSprite->loc.y = 0x190;

    // Configure tardies/cuts/schedule sprites (loop 3 times)
    int count = 3;
    do {
        (*ebx)->ConfigStates(3);
        (*ebx)->ConfigRange(0, 1, 1, 1);
        (*ebx)->ConfigRange(1, 2, 2, 1);
        (*ebx)->ConfigRange(2, 3, 3, 1);
        (*ebx)->priority = 5;
        (*ebx)->flags |= 0x40;
        ebx++;
        count--;
    } while (count != 0);
}

/* Function start: 0x434FB0 */
SCI_Schedule::~SCI_Schedule()
{
    if (palette != 0) {
        delete palette;
        palette = 0;
    }

    if (bgSprite != 0) {
        delete bgSprite;
        bgSprite = 0;
    }

    Sprite** ptr = &selBox1;
    int i = 6;
    do {
        Sprite* s = *ptr;
        if (s != 0) {
            delete s;
            *ptr = 0;
        }
        ptr++;
        i--;
    } while (i != 0);

    if (mapHotspot != 0) {
        delete mapHotspot;
        mapHotspot = 0;
    }
}

/* Function start: 0x4350C0 */
void SCI_Schedule::Init(SC_MessageParser* msg)
{
    int iVar8;
    int iVar3;
    char cVar4;
    GameState* pvVar1;
    GameState* pvVar2;
    unsigned int uVar6;
    char local_40[64];

    IconBar::Init(msg);

    int* puVar5 = &g_IconBarState_00473334;
    do {
        *puVar5 = 0;
        puVar5 = puVar5 + 9;
    } while ((unsigned int)puVar5 < (unsigned int)&g_InventoryState_004733e8);
    g_InventoryState_004733e8 = 0;

    iVar8 = (int)palette;
    if (iVar8 != 0) {
        int* piVar7 = (int*)((int)g_ZBufferManager_0046aa24 + 0xa8);
        if (*piVar7 != 0) {
            WriteToLog("ddouble palette");
        }
        *piVar7 = iVar8;
    }

    iVar8 = g_PeriodStateIdx_0046cb90;
    pvVar1 = g_GameState_0046aa30;
    if (iVar8 < 0 || g_GameState_0046aa30->maxStates - 1 < iVar8) {
        ShowError("Invalid gamestate %d", iVar8);
    }
    bgSprite->ResetAnimation(pvVar1->stateValues[iVar8], 0);

    pvVar1 = g_GameState_0046aa30;
    uVar6 = pvVar1->FindState("PERIOD");
    if ((int)uVar6 < 0 || pvVar1->maxStates - 1 < (int)uVar6) {
        ShowError("Invalid gamestate %d", uVar6);
    }
    iVar8 = pvVar1->stateValues[uVar6];

    if (iVar8 % 5 == 0) {
        selectionState = -1;
    } else {
        selectionState = (unsigned int)((iVar8 + 2) % 5 == 0);
    }

    iVar8 = iVar8 + iVar8 / -5;
    renderX = g_SchedulePositionTable_0046b940[iVar8 * 2];
    renderY = g_SchedulePositionTable_0046b940[iVar8 * 2 + 1];

    iVar8 = g_PeriodStateIdx_0046cb90;
    pvVar1 = g_GameState_0046aa30;
    if (g_PeriodStateIdx_0046cb90 < 0 || g_GameState_0046aa30->maxStates - 1 < g_PeriodStateIdx_0046cb90) {
        ShowError("Invalid gamestate %d", g_PeriodStateIdx_0046cb90);
    }
    iVar3 = g_PeriodStateIdx_0046cb90;
    pvVar2 = g_GameState_0046aa30;
    iVar8 = pvVar1->stateValues[iVar8];
    if (iVar8 == -1) {
        if (g_PeriodStateIdx_0046cb90 < 0 || g_GameState_0046aa30->maxStates - 1 < g_PeriodStateIdx_0046cb90) {
            ShowError("Invalid gamestate %d", g_PeriodStateIdx_0046cb90);
        }
        cVar4 = g_PeriodCharTable_0046cb94[pvVar2->stateValues[iVar3]];
    } else {
        cVar4 = g_PeriodCharTable_0046cb94[iVar8];
    }

    sprintf(local_40, "%c_LATE_CUT", (int)cVar4);
    pvVar1 = g_GameState_0046aa30;
    uVar6 = pvVar1->FindState(local_40);
    if ((int)uVar6 < 0 || pvVar1->maxStates - 1 < (int)uVar6) {
        ShowError("Invalid gamestate %d", uVar6);
    }
    iVar8 = pvVar1->stateValues[uVar6];
    tardiesSprite->ResetAnimation(iVar8 % 3, 0);
    cutsSprite->ResetAnimation(iVar8 / 3 % 3, 0);

    iVar8 = g_PeriodStateIdx_0046cb90;
    pvVar1 = g_GameState_0046aa30;
    if (g_PeriodStateIdx_0046cb90 < 0 || g_GameState_0046aa30->maxStates - 1 < g_PeriodStateIdx_0046cb90) {
        ShowError("Invalid gamestate %d", g_PeriodStateIdx_0046cb90);
    }
    iVar3 = g_PeriodStateIdx_0046cb90;
    pvVar2 = g_GameState_0046aa30;
    iVar8 = pvVar1->stateValues[iVar8];
    if (iVar8 == -1) {
        if (g_PeriodStateIdx_0046cb90 < 0 || g_GameState_0046aa30->maxStates - 1 < g_PeriodStateIdx_0046cb90) {
            ShowError("Invalid gamestate %d", g_PeriodStateIdx_0046cb90);
        }
        cVar4 = g_PeriodCharTable_0046cb94[pvVar2->stateValues[iVar3]];
    } else {
        cVar4 = g_PeriodCharTable_0046cb94[iVar8];
    }

    sprintf(local_40, "%c_DETENTION_SUSPENSION", (int)cVar4);
    pvVar1 = g_GameState_0046aa30;
    uVar6 = pvVar1->FindState(local_40);
    if ((int)uVar6 < 0 || pvVar1->maxStates - 1 < (int)uVar6) {
        ShowError("Invalid gamestate %d", uVar6);
    }
    scheduleSprite->ResetAnimation(pvVar1->stateValues[uVar6] % 3, 0);
}

/* Function start: 0x4353E0 */
void SCI_Schedule::ShutDown(SC_MessageParser* msg)
{
    int i = 6;
    Sprite** ptr = &selBox1;
    do {
        if (*ptr != 0) {
            (*ptr)->StopAnimationSound();
        }
        ptr++;
        i--;
    } while (i != 0);

    if (bgSprite != 0) {
        bgSprite->StopAnimationSound();
    }

    g_InventoryState_004733e8 = 1;
    IconBar::ShutDown(msg);

    if (mapHotspot != 0) {
        mapHotspot->StopCursorSound();
    }

    return;
}

/* Function start: 0x435440 */
int SCI_Schedule::AddMessage(SC_MessageParser* msg)
{
    SpriteAction* action = (SpriteAction*)msg;

    if (IconBar::AddMessage(msg) != 0) {
        return 1;
    }

    if (action->lastKey == 0x1B) {
        action->addressType = savedCommand;
        action->addressValue = savedMsgData;
        action->instruction = 4;
        return 1;
    }

    if (action->button1 >= 2) {
        int mouseX, mouseY;
        InputState* mouse = g_InputManager_0046aa08->pMouse;
        mouseY = 0;
        if (mouse != 0) {
            mouseY = mouse->y;
        }
        mouseX = 0;
        if (mouse != 0) {
            mouseX = mouse->x;
        }

        T_MenuHotspot* hs = mapHotspot;
        if (hs->sprite != 0 &&
            hs->bounds.left <= mouseX && hs->bounds.right >= mouseX &&
            hs->bounds.top <= mouseY && hs->bounds.bottom >= mouseY) {
            action->addressType = savedCommand;
            action->instruction = 4;
            action->addressValue = savedMsgData;
        }
    }

    return 1;
}

/* Function start: 0x435500 */
int SCI_Schedule::Exit(SC_MessageParser* msg) {
    return handlerId == ((SpriteAction*)msg)->addressType;
}

/* Function start: 0x435520 */
void SCI_Schedule::Update(int param1, int param2)
{
    if (handlerId != param2) {
        return;
    }

    IconBar::Update(param1, param2);

    bgSprite->Do(bgSprite->loc.x, bgSprite->loc.y, 1.0);

    if (selectionState == 1) {
        selBox2->Do(renderX, renderY, 1.0);
    } else if (selectionState == 0) {
        selBox1->Do(renderX, renderY, 1.0);
    }

    Sprite** sprPtr = &tardiesSprite;
    int count = 4;
    do {
        Sprite* s = *sprPtr;
        if (s != 0) {
            s->Do(s->loc.x, s->loc.y, 1.0);
        }
        sprPtr++;
        count--;
    } while (count != 0);

    int mouseX, mouseY;
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseY = g_InputManager_0046aa08->pMouse->y;
    } else {
        mouseY = 0;
    }
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseX = g_InputManager_0046aa08->pMouse->x;
    } else {
        mouseX = 0;
    }

    T_MenuHotspot* hs = mapHotspot;
    if (hs->sprite != 0) {
        if (hs->bounds.left <= mouseX && hs->bounds.right >= mouseX &&
            hs->bounds.top <= mouseY && hs->bounds.bottom >= mouseY) {
            hs->state = 1;
            if (hs->cursor != 0) {
                hs->cursor->ResetAnimation(1, 0);
            }
        } else {
            hs->state = 0;
            if (hs->cursor != 0) {
                hs->cursor->ResetAnimation(0, 0);
            }
        }
    }

    tardiesSprite->Do(0x55, 0x190, 1.0);
    cutsSprite->Do(0x105, 0x190, 1.0);
    scheduleSprite->Do(0x1BB, 0x190, 1.0);

    mapHotspot->Update();
    g_Mouse_0046aa18->DrawCursor();
}

/* Function start: 0x4356E0 */
int SCI_Schedule::LBLParse(char* param_1)
{
    char token[32];

    sscanf(param_1, " %s ", token);

    if (strcmp(token, "EXIT") == 0) {
        mapHotspot = new T_MenuHotspot(0);
        Parser::ProcessFile(mapHotspot, this, (char*)0);
    }
    else if (strcmp(token, "END") == 0) {
        return 1;
    }
    else {
        ReportUnknownLabel("SCI_Schedule");
    }

    return 0;
}
