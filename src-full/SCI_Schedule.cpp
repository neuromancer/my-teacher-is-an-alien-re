#include "SCI_Schedule.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include "SC_Question.h"
#include "GameState.h"
#include "MouseControl.h"
#include <stdio.h>
#include <string.h>

class InputManager;
extern InputManager* DAT_0046aa08;
class MouseControl;
extern MouseControl* DAT_0046aa18;
extern "C" extern GameState* DAT_0046aa30;
class ZBufferManager;
extern ZBufferManager* DAT_0046aa24;
extern int DAT_004733e8;

extern void* __fastcall FUN_00420ce0(void*, int, int);  // MapScene ctor
// FUN_00420ef0 inlined: calls StopAnimationSound on [this+0x19C]
extern void __fastcall FUN_00420f00(void*);            // MapScene render
extern void __fastcall FUN_00420d90(void*);            // MapScene destructor

/* Function start: 0x434C10 */
SCI_Schedule::SCI_Schedule()
{
    memset(&field_A8, 0, 0xC * 4);

    handlerId = 0x27;
    ParseFile(this, "mis\\schedule.mis", (char*)0);

    Palette* pal = new Palette();
    field_A8[0] = (int)pal;
    pal->Load("schedual\\schedule.col");

    Sprite* spr = new Sprite("schedual\\schedule.smk");
    field_A8[1] = (int)spr;
    spr->ConfigStates(3);
    spr->ConfigRange(0, 1, 1, 1);
    spr->ConfigRange(1, 2, 2, 1);
    spr->ConfigRange(2, 3, 3, 1);
    ((Sprite*)field_A8[1])->handle = 0;

    Sprite* spr2 = new Sprite("schedual\\shedbox1.smk");
    field_A8[2] = (int)spr2;
    ((Sprite*)field_A8[2])->handle = 5;
    ((Sprite*)field_A8[2])->flags |= 0x40;

    Sprite* spr3 = new Sprite("schedual\\shedbox2.smk");
    field_A8[3] = (int)spr3;
    ((Sprite*)field_A8[3])->handle = 5;
    ((Sprite*)field_A8[3])->flags |= 0x40;

    Sprite* spr4 = new Sprite("schedual\\tardies.smk");
    int* ebx = &field_A8[4];
    *ebx = (int)spr4;
    ((Sprite*)*ebx)->loc_x = 0x54;
    ((Sprite*)*ebx)->loc_y = 0x190;

    Sprite* spr5 = new Sprite("schedual\\cuts.smk");
    field_A8[5] = (int)spr5;
    ((Sprite*)field_A8[5])->loc_x = 0x105;
    ((Sprite*)field_A8[5])->loc_y = 0x190;

    Sprite* spr6 = new Sprite("schedual\\schedule.smk");
    field_A8[6] = (int)spr6;
    ((Sprite*)field_A8[6])->loc_x = 0x1BB;
    ((Sprite*)field_A8[6])->loc_y = 0x190;

    // Configure sprites B8-C0 (loop 3 times)
    int count = 3;
    do {
        ((Sprite*)*ebx)->ConfigStates(3);
        ((Sprite*)*ebx)->ConfigRange(0, 1, 1, 1);
        ((Sprite*)*ebx)->ConfigRange(1, 2, 2, 1);
        ((Sprite*)*ebx)->ConfigRange(2, 3, 3, 1);
        ((Sprite*)*ebx)->handle = 5;
        ((Sprite*)*ebx)->flags |= 0x40;
        ebx++;
        count--;
    } while (count != 0);
}

/* Function start: 0x434FB0 */
SCI_Schedule::~SCI_Schedule()
{
    Palette* pal = (Palette*)field_A8[0];
    if (pal != 0) {
        pal->~Palette();
        FreeMemory(pal);
        field_A8[0] = 0;
    }

    Sprite* spr = (Sprite*)field_A8[1];
    if (spr != 0) {
        spr->~Sprite();
        FreeMemory(spr);
        field_A8[1] = 0;
    }

    int* ptr = &field_A8[2];
    int i = 6;
    do {
        Sprite* s = (Sprite*)*ptr;
        if (s != 0) {
            s->~Sprite();
            FreeMemory(s);
            *ptr = 0;
        }
        ptr++;
        i--;
    } while (i != 0);

    void* mapScene = (void*)field_A8[11];
    if (mapScene != 0) {
        FUN_00420d90(mapScene);
        FreeMemory(mapScene);
        field_A8[11] = 0;
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
    int* ptr = &field_A8[2];
    int i = 6;
    do {
        Sprite* spr = (Sprite*)*ptr;
        if (spr != 0) {
            spr->StopAnimationSound();
        }
        ptr++;
        i--;
    } while (i != 0);

    Sprite* mainSpr = (Sprite*)field_A8[1];
    if (mainSpr != 0) {
        mainSpr->StopAnimationSound();
    }

    DAT_004733e8 = 1;
    IconBar::ShutDown(msg);

    void* mapScene = (void*)field_A8[11];
    if (mapScene != 0) {
        // FUN_00420ef0 inlined: StopAnimationSound on cursor sprite
        Sprite* cursor = (Sprite*)((int*)mapScene)[0x19C/4];
        if (cursor != 0) {
            cursor->StopAnimationSound();
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
        int* mouse = *(int**)((char*)DAT_0046aa08 + 0x1A0);
        mouseY = 0;
        if (mouse != 0) {
            mouseY = mouse[1];
        }
        mouseX = 0;
        if (mouse != 0) {
            mouseX = mouse[0];
        }

        int* ms = (int*)field_A8[11];
        if (ms[0x90/4] != 0 &&
            ms[0x9C/4] <= mouseX && ms[0xA4/4] >= mouseX &&
            ms[0xA0/4] <= mouseY && ms[0xA8/4] >= mouseY) {
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

    Sprite* mainSpr = (Sprite*)field_A8[1];
    mainSpr->RenderAt(mainSpr->num_states, mainSpr->field_0xb0, 0, 0x3FF00000);

    int selState = field_A8[8];
    if (selState == 1) {
        ((Sprite*)field_A8[3])->RenderAt(field_A8[9], field_A8[10], 0, 0x3FF00000);
    } else if (selState == 0) {
        ((Sprite*)field_A8[2])->RenderAt(field_A8[9], field_A8[10], 0, 0x3FF00000);
    }

    int* sprPtr = &field_A8[4];
    int count = 4;
    do {
        Sprite* s = (Sprite*)*sprPtr;
        if (s != 0) {
            s->RenderAt(s->num_states, s->field_0xb0, 0, 0x3FF00000);
        }
        sprPtr++;
        count--;
    } while (count != 0);

    int mouseX, mouseY;
    int* mouse = *(int**)((char*)DAT_0046aa08 + 0x1A0);
    mouseY = 0;
    if (mouse != 0) {
        mouseY = mouse[1];
    }
    if (mouse != 0) {
        mouseX = mouse[0];
    } else {
        mouseX = 0;
    }

    int* ms = (int*)field_A8[11];
    if (ms[0x90/4] != 0) {
        if (ms[0x9C/4] <= mouseX && ms[0xA4/4] >= mouseX &&
            ms[0xA0/4] <= mouseY && ms[0xA8/4] >= mouseY) {
            ms[0x94/4] = 1;
            Sprite* cursor = (Sprite*)ms[0x19C/4];
            if (cursor != 0) {
                cursor->ResetAnimation(1, 0);
            }
        } else {
            ms[0x94/4] = 0;
            Sprite* cursor = (Sprite*)ms[0x19C/4];
            if (cursor != 0) {
                cursor->ResetAnimation(0, 0);
            }
        }
    }

    ((Sprite*)field_A8[4])->RenderAt(0x55, 0x190, 0, 0x3FF00000);
    ((Sprite*)field_A8[5])->RenderAt(0x105, 0x190, 0, 0x3FF00000);
    ((Sprite*)field_A8[6])->RenderAt(0x1BB, 0x190, 0, 0x3FF00000);

    FUN_00420f00((void*)field_A8[11]);
    (DAT_0046aa18)->DrawCursor();
}

/* Function start: 0x4356E0 */
int SCI_Schedule::LBLParse(char* param_1)
{
    char token[32];

    token[0] = '\0';
    sscanf(param_1, " %s ", token);

    if (strcmp(token, "HOTSPOT") == 0) {
        void* mapScene = (void*)new char[0x1A8];
        if (mapScene != 0) {
            FUN_00420ce0(mapScene, 0, 0);
        }
        field_A8[11] = (int)mapScene;
        Parser::ProcessFile((Parser*)mapScene, this, (char*)0);
    }
    else if (strcmp(token, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("SCI_Schedule");
    }

    return 0;
}
