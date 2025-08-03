#include "Hotspot.h"
#include <stdio.h>
#include <string.h>
#include "Memory.h"

extern "C" {
    void ShowError(const char* message, ...);
    int __fastcall FUN_0041f800(void*);
    void __cdecl SC_Message_Send(int, int, int, int, int, int, int, void*, int, int);
}

/* Function start: 0x409400 */
unsigned char Hotspot::Do()
{
    if (this->sprite != (Sprite *)0x0) {
        return this->sprite->Do(this->sprite->loc_x, this->sprite->loc_y, 0, 0x3ff00000);
    }
    // WARNING: Subroutine does not return
    ShowError("Missing sprite in Hotspot Do()");
    return 0;
}

/* Function start: 0x4094F0 */
int Hotspot::Update(int param_1, int param_2, int param_3)
{
    int iVar1;
    void* puVar2;

    switch (this->state) {
    case 1:
        if ((this->list1 == 0) || (iVar1 = FUN_0041f800(this->list1), iVar1 != 0)) {
            this->state = 2;
        }
        if (this->state != 2) {
            return 0;
        }
    case 2:
        if (this->list2 == 0) {
            this->state = 3;
            if (this->dialog != 0) {
                puVar2 = AllocateMemory_Wrapper(8);
                *(int*)puVar2 = param_1;
                *((int*)puVar2 + 1) = param_2;
                SC_Message_Send(9, this->parseFileIndex, 0xb, param_3, 5, this->dialogParseFileNumber, 0, puVar2, 0, 0);
            }
        }
        else {
            iVar1 = FUN_0041f800(this->list2);
            if (iVar1 != 0) {
                this->state = 3;
                return 0;
            }
        }
        return 0;
    case 3:
        if (this->list3 == 0) {
            this->state = 1;
        }
        else {
            iVar1 = FUN_0041f800(this->list3);
            if (iVar1 != 0) {
                this->state = 1;
                return 1;
            }
        }
        return 1;
    case 4:
        return 1;
    default:
        ShowError("Error in Thotspot.cpp - Update()");
    }
    return 0;
}

/* Function start: 0x409620 */
int Hotspot::LBLParse(char* line)
{
    try {
        char command[32];
        sscanf(line, "%s", command);

        if (strcmp(command, "HOTSPOT") == 0) {
            sscanf(line, "%*s %d %d %d %d", &this->rect_x, &this->rect_y, &this->rect_w, &this->rect_h);
        } else if (strcmp(command, "DIALOG") == 0) {
            this->dialog = 1;
        } else if (strcmp(command, "DIALOGPARSEFILENUMBER") == 0) {
            sscanf(line, "%*s %d", &this->dialogParseFileNumber);
        } else if (strcmp(command, "PARSEFILEINDEX") == 0) {
            sscanf(line, "%*s %d", &this->parseFileIndex);
        } else if (strcmp(command, "SPRITE") == 0) {
            Sprite* newSprite = (Sprite*)AllocateMemory_Wrapper(sizeof(Sprite));
            if (newSprite) {
                newSprite->Sprite::Sprite(NULL);
                this->sprite = newSprite;
                this->sprite->Copy(this);
            }
        } else if (strcmp(command, "LABEL") == 0) {
            sscanf(line, "%*s %s", this->label);
        } else if (strcmp(command, "MOUSE") == 0) {
            sscanf(line, "%*s %s", this->mouse);
        } else if (strcmp(command, "END") == 0) {
            return 1;
        } else {
            this->LBLParse("Hotspot");
        }
    } catch (...) {
        return 0;
    }
    return 0;
}
