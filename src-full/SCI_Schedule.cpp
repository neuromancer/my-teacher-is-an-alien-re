#include "SCI_Schedule.h"
#include "Sprite.h"
#include "Palette.h"
#include "T_MenuHotspot.h"
#include "Memory.h"
#include "SC_Question.h"
#include "GameState.h"
#include "MouseControl.h"
#include "InputManager.h"
#include <stdio.h>
#include <string.h>

extern InputManager* DAT_0046aa08;
extern MouseControl* DAT_0046aa18;
extern "C" extern GameState* DAT_0046aa30;
extern int DAT_004733e8;

/* Function start: 0x434C10 */
SCI_Schedule::SCI_Schedule()
{
    memset(&palette, 0, 0xC * 4);

    handlerId = 0x27;
    ParseFile(this, "mis\\schedule.mis", (char*)0);

    Palette* pal = new Palette();
    palette = pal;
    pal->Load("schedual\\schedule.col");

    Sprite* spr = new Sprite("schedual\\schedule.smk");
    bgSprite = spr;
    spr->ConfigStates(3);
    spr->ConfigRange(0, 1, 1, 1);
    spr->ConfigRange(1, 2, 2, 1);
    spr->ConfigRange(2, 3, 3, 1);
    bgSprite->handle = 0;

    Sprite* spr2 = new Sprite("schedual\\shedbox1.smk");
    selBox1 = spr2;
    selBox1->handle = 5;
    selBox1->flags |= 0x40;

    Sprite* spr3 = new Sprite("schedual\\shedbox2.smk");
    selBox2 = spr3;
    selBox2->handle = 5;
    selBox2->flags |= 0x40;

    Sprite* spr4 = new Sprite("schedual\\tardies.smk");
    Sprite** ebx = &tardiesSprite;
    *ebx = spr4;
    (*ebx)->loc_x = 0x54;
    (*ebx)->loc_y = 0x190;

    Sprite* spr5 = new Sprite("schedual\\cuts.smk");
    cutsSprite = spr5;
    cutsSprite->loc_x = 0x105;
    cutsSprite->loc_y = 0x190;

    Sprite* spr6 = new Sprite("schedual\\schedule.smk");
    scheduleSprite = spr6;
    scheduleSprite->loc_x = 0x1BB;
    scheduleSprite->loc_y = 0x190;

    // Configure tardies/cuts/schedule sprites (loop 3 times)
    int count = 3;
    do {
        (*ebx)->ConfigStates(3);
        (*ebx)->ConfigRange(0, 1, 1, 1);
        (*ebx)->ConfigRange(1, 2, 2, 1);
        (*ebx)->ConfigRange(2, 3, 3, 1);
        (*ebx)->handle = 5;
        (*ebx)->flags |= 0x40;
        ebx++;
        count--;
    } while (count != 0);
}

/* Function start: 0x434FB0 */
SCI_Schedule::~SCI_Schedule()
{
    if (palette != 0) {
        palette->~Palette();
        FreeMemory(palette);
        palette = 0;
    }

    if (bgSprite != 0) {
        bgSprite->~Sprite();
        FreeMemory(bgSprite);
        bgSprite = 0;
    }

    Sprite** ptr = &selBox1;
    int i = 6;
    do {
        Sprite* s = *ptr;
        if (s != 0) {
            s->~Sprite();
            FreeMemory(s);
            *ptr = 0;
        }
        ptr++;
        i--;
    } while (i != 0);

    if (mapHotspot != 0) {
        mapHotspot->~T_MenuHotspot();
        FreeMemory(mapHotspot);
        mapHotspot = 0;
    }
}

/* Function start: 0x4350C0 */ /* No assembly extracted */
void SCI_Schedule::Init(SC_Message* msg)
{
    IconBar::Init(msg);
}

/* Function start: 0x4353E0 */
int SCI_Schedule::ShutDown(SC_Message* msg)
{
    Sprite** ptr = &selBox1;
    int i = 6;
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

    DAT_004733e8 = 1;
    IconBar::ShutDown(msg);

    if (mapHotspot != 0) {
        if (mapHotspot->cursor != 0) {
            mapHotspot->cursor->StopAnimationSound();
        }
    }

    return 0;
}

/* Function start: 0x435440 */
int SCI_Schedule::AddMessage(SC_Message* msg)
{
    int* msgData = (int*)msg;

    if (IconBar::AddMessage(msg) != 0) {
        return 1;
    }

    if (msgData[0xB] == 0x1B) {
        msgData[0] = savedCommand;
        msgData[4] = 4;
        msgData[1] = savedMsgData;
        return 1;
    }

    if (msgData[9] >= 2) {
        int mouseX, mouseY;
        InputState* mouse = DAT_0046aa08->pMouse;
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
            msgData[0] = savedCommand;
            msgData[4] = 4;
            msgData[1] = savedMsgData;
        }
    }

    return 1;
}

/* Function start: 0x435500 */ /* No assembly extracted */
int SCI_Schedule::Exit(SC_Message* msg)
{
    return WriteMessageAddress(msg);
}

/* Function start: 0x435520 */
void SCI_Schedule::Update(int param1, int param2)
{
    if (handlerId != param2) {
        return;
    }

    IconBar::Update(param1, param2);

    bgSprite->RenderAt(bgSprite->num_states, bgSprite->field_0xb0, 0, 0x3FF00000);

    if (selectionState == 1) {
        selBox2->RenderAt(renderX, renderY, 0, 0x3FF00000);
    } else if (selectionState == 0) {
        selBox1->RenderAt(renderX, renderY, 0, 0x3FF00000);
    }

    Sprite** sprPtr = &tardiesSprite;
    int count = 4;
    do {
        Sprite* s = *sprPtr;
        if (s != 0) {
            s->RenderAt(s->num_states, s->field_0xb0, 0, 0x3FF00000);
        }
        sprPtr++;
        count--;
    } while (count != 0);

    int mouseX, mouseY;
    InputState* mouse = DAT_0046aa08->pMouse;
    mouseY = 0;
    if (mouse != 0) {
        mouseY = mouse->y;
    }
    if (mouse != 0) {
        mouseX = mouse->x;
    } else {
        mouseX = 0;
    }

    T_MenuHotspot* hs = mapHotspot;
    if (hs->sprite != 0) {
        if (hs->bounds.left <= mouseX && hs->bounds.right >= mouseX &&
            hs->bounds.top <= mouseY && hs->bounds.bottom >= mouseY) {
            hs->field_94 = 1;
            if (hs->cursor != 0) {
                hs->cursor->ResetAnimation(1, 0);
            }
        } else {
            hs->field_94 = 0;
            if (hs->cursor != 0) {
                hs->cursor->ResetAnimation(0, 0);
            }
        }
    }

    tardiesSprite->RenderAt(0x55, 0x190, 0, 0x3FF00000);
    cutsSprite->RenderAt(0x105, 0x190, 0, 0x3FF00000);
    scheduleSprite->RenderAt(0x1BB, 0x190, 0, 0x3FF00000);

    mapHotspot->Update();
    DAT_0046aa18->DrawCursor();
}

/* Function start: 0x4356E0 */
int SCI_Schedule::LBLParse(char* param_1)
{
    char token[32];

    token[0] = '\0';
    sscanf(param_1, " %s ", token);

    if (strcmp(token, "HOTSPOT") == 0) {
        mapHotspot = new T_MenuHotspot(0);
        Parser::ProcessFile(mapHotspot, this, (char*)0);
    }
    else if (strcmp(token, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("SCI_Schedule");
    }

    return 0;
}
