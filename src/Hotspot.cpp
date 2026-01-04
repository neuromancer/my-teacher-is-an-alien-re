#include "Hotspot.h"
#include "string.h"
#include "Message.h"
#include "Memory.h"
#include "SpriteList.h"
#include <stdio.h>
#include <string.h>

/* Function start: 0x409400 */
unsigned char Hotspot::Do()
{
    if (sprite != (Sprite *)0x0) {
        return sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
    }
    // WARNING: Subroutine does not return
    ShowError("Missing sprite in Hotspot Do()");
    return 0;
}

/* Function start: 0x4094F0 */
int Hotspot::Update(int param_1, int param_2, int param_3)
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
int Hotspot::LBLParse(char* line)
{
    try {
        char command[32];
        sscanf(line, " %s ", command);

        if (strcmp(command, "HOTSPOT") == 0) {
            sscanf(line, "%*s %d %d %d %d", &rect_x, &rect_y, &rect_w, &rect_h);
        } else if (strcmp(command, "DIALOG") == 0) {
            dialog = 1;
        } else if (strcmp(command, "DIALOGPARSEFILENUMBER") == 0) {
            sscanf(line, "%*s %d", &dialogParseFileNumber);
        } else if (strcmp(command, "PARSEFILEINDEX") == 0) {
            sscanf(line, "%*s %d", &parseFileIndex);
        } else if (strcmp(command, "SPRITE") == 0) {
            Sprite* newSprite = (Sprite*)AllocateMemory(sizeof(Sprite));
            if (newSprite) {
                newSprite->Sprite::Sprite(NULL);
                sprite = newSprite;
                sprite->Copy(this);
            }
        } else if (strcmp(command, "LABEL") == 0) {
            sscanf(line, "%*s %s", label);
        } else if (strcmp(command, "MOUSE") == 0) {
            sscanf(line, "%*s %s", mouse);
        } else if (strcmp(command, "END") == 0) {
            return 1;
        } else {
            Parser::LBLParse("T_Hotspot");
        }
    } catch (...) {
        return 0;
    }
    return 0;
}
