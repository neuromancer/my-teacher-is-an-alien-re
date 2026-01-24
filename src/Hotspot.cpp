#include "Hotspot.h"
#include "string.h"
#include "Message.h"
#include "Memory.h"
#include "Sprite.h"
#include "MouseControl.h"
#include <stdio.h>
#include <string.h>

// Helper for Miles Sound System
typedef void* HSAMPLE;
extern "C" int __stdcall AIL_sample_status(HSAMPLE);

/* Function start: 0x409230 */
T_Hotspot::T_Hotspot()
{
    int* ptr;
    int i;

    field_13C = 0;
    rect_x = 0;
    rect_y = 0;
    rect_w = 0;
    rect_h = 0;

    ptr = (int*)&sprite;
    for (i = 0x2e; i != 0; i--) {
        *ptr = 0;
        ptr++;
    }
    enabled = 1;
    state = 1;
    dialogParseFileNumber = -1;
    parseFileIndex = -1;
}

/* Function start: 0x4092E0 */
T_Hotspot::~T_Hotspot()
{
    Sprite* spr;
    MouseControl* mc;

    spr = sprite;
    if (spr != 0) {
        delete spr;
        sprite = 0;
    }

    mc = list1;
    if (mc != 0) {
        delete mc;
        list1 = 0;
    }

    mc = list2;
    if (mc != 0) {
        delete mc;
        list2 = 0;
    }

    mc = list3;
    if (mc != 0) {
        delete mc;
        list3 = 0;
    }
}

// Wrapper for SCI_AfterSchoolMenu compatibility (SEH handler often needs a simpler entry)
void __fastcall FUN_004092e0(void* obj)
{
    if (obj) {
        ((T_Hotspot*)obj)->~T_Hotspot();
    }
}

/* Function start: 0x409400 */
unsigned char T_Hotspot::Do()
{
    if (sprite != 0) {
        return sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
    }
    ShowError("Missing sprite in Hotspot Do()");
    return 0;
}

/* Function start: 0x409440 */
int T_Hotspot::SetState(int newState)
{
    if (sprite != 0) {
        sprite->SetState2(newState);
        return 1;
    }
    ShowError("Error in T_Hotspot::Set_State");
    return 0;
}

/* Function start: 0x409470 */
int T_Hotspot::GetState()
{
    if (sprite == 0) {
        ShowError("Error in T_Hotspot::Get_State");
        return 0;
    }
    return sprite->current_state;
}

/* Function start: 0x4094A0 */
void T_Hotspot::Exit()
{
    if (sprite != 0) {
        sprite->StopAnimationSound();
    }
    if (list1 != 0) {
        list1->StopAll();
    }
    if (list2 != 0) {
        list2->StopAll();
    }
    if (list3 != 0) {
        list3->StopAll();
    }
}

/* Function start: 0x4094F0 */
int T_Hotspot::Update(int param_1, int param_2, int param_3)
{
    int iVar1;
    int* puVar2;

    switch (state) {
    case 1:
        if ((list1 == 0) || (iVar1 = list1->DoAll(), iVar1 != 0)) {
            state = 2;
        }
        if (state != 2) {
            return 0;
        }
    case 2:
        if (list2 == 0) {
            state = 3;
            if (dialog != 0) {
                puVar2 = (int*)AllocateMemory(8);
                *puVar2 = param_1;
                puVar2[1] = param_2;
                SC_Message_Send(9, parseFileIndex, 0xb, param_3, 5, dialogParseFileNumber, 0, (int)puVar2, 0, 0);
            }
        }
        else {
            iVar1 = list2->DoAll();
            if (iVar1 != 0) {
                state = 3;
                return 0;
            }
        }
        return 0;
    case 3:
        if (list3 == 0) {
            state = 1;
        }
        else {
            iVar1 = list3->DoAll();
            if (iVar1 != 0) {
                state = 1;
                return 1;
            }
        }
        return 1;
    case 4:
        return 1;
    }
    ShowError("Error in Thotspot.cpp - Update()");
    return 0;
}

/* Function start: 0x409620 */
int T_Hotspot::LBLParse(char* line)
{
    char command[32];
    sscanf(line, " %s ", command);

    if (strcmp(command, "RECT") == 0) {
        sscanf(line, "%*s %d %d %d %d", &rect_x, &rect_y, &rect_w, &rect_h);
    } else if (strcmp(command, "DIALOG") == 0) {
        dialog = 1;
    } else if (strcmp(command, "DIALOGPARSEFILENUMBER") == 0) {
        sscanf(line, "%*s %d", &dialogParseFileNumber);
    } else if (strcmp(command, "PARSEFILEINDEX") == 0) {
        sscanf(line, "%*s %d", &parseFileIndex);
    } else if (strcmp(command, "SPRITE") == 0) {
        Sprite* newSprite = (Sprite*)AllocateMemory(0xd8);
        if (newSprite) {
            newSprite->Sprite::Sprite(NULL);
        }
        sprite = newSprite;
        Parser::ProcessFile(sprite, this, 0);
    } else if (strcmp(command, "LABEL") == 0) {
        sscanf(line, "%*s %s", label);
    } else if (strcmp(command, "MOUSE") == 0) {
        sscanf(line, "%*s %s", mouse);
    } else if (strcmp(command, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("T_Hotspot");
    }
    return 0;
}

/* Function start: 0x40D300 */
Hotspot::Hotspot()
{
    memset(&sprite, 0, 104);
    field_D0 = 1;
    field_D8 = 1;
}

/* Function start: 0x40D3A0 */
Hotspot::~Hotspot()
{
    // Implementation from FUN_40D3A0
    if (sprite != 0) {
        sprite->~Sprite(); // Or delete? Disassembly shows call 0x41f360 then free 0x00424940
        FreeMemory(sprite);
        sprite = 0;
    }
    if (list1 != 0) {
        list1->~MouseControl();
        FreeMemory(list1);
        list1 = 0;
    }
    if (list2 != 0) {
        list2->~MouseControl();
        FreeMemory(list2);
        list2 = 0;
    }
    // and more... stubs for now to fix build
}

/* Function start: 0x40D710 */
int Hotspot::LBLParse(char* line)
{
    // Stub for build fixing
    return 0;
}

/* Function start: 0x40D610 */
int Hotspot::Draw_40d610()
{
    // Stub for build fixing
    return 0;
}

unsigned char Hotspot::Do()
{
    return 0;
}
