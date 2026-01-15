#include "OptionMenu.h"
#include "Memory.h"
#include "Sprite.h"
#include "SpriteList.h"

#include <stdio.h>
#include <string.h>

// OptionMenu method stubs
void OptionMenu::SetOptionState(int param_1, int param_2) {}
void OptionMenu::GetOptionQ(int param_1) {}

/* Function start: 0x409940 */
OptionMenu::OptionMenu()
{
    for (int i = 0; i < 6; i++) {
        options[i] = 0;
    }
    selected_option = 5;

    Sprite* option1 = new Sprite("demo/option1.smk");
    if (option1) {
        option1->flags &= ~2;
        option1->loc_x = 216;
        option1->loc_y = 202;
        option1->priority = 10;
        option1->SetState(4);
        option1->SetRange(0, 1, 1);
        option1->SetRange(1, 2, 2);
        option1->SetRange(2, 3, 12);
        option1->SetRange(3, 13, 13);
    }

    Sprite* option2 = new Sprite("demo/option2.smk");
    if (option2) {
        option2->flags &= ~2;
        option2->loc_x = 216;
        option2->loc_y = 202;
        option2->priority = 10;
        option2->SetState(4);
        option2->SetRange(0, 1, 1);
        option2->SetRange(1, 2, 2);
        option2->SetRange(2, 3, 12);
        option2->SetRange(3, 13, 13);
    }

    Sprite* option3 = new Sprite("demo/option3.smk");
    if (option3) {
        option3->flags &= ~2;
        option3->loc_x = 216;
        option3->loc_y = 202;
        option3->priority = 10;
        option3->SetState(4);
        option3->SetRange(0, 1, 1);
        option3->SetRange(1, 2, 2);
        option3->SetRange(2, 3, 12);
        option3->SetRange(3, 13, 13);
    }
}

/* Function start: 0x409BF0 */
OptionMenu::~OptionMenu()
{
    delete options[0];
    options[0] = 0;
    delete options[1];
    options[1] = 0;
    delete options[2];
    options[2] = 0;
}

/* Function start: 0x409CD0 */
int OptionMenu::LBLParse(char* command)
{
    char command_copy[32];
    sscanf(command, "%s", command_copy);

    if (strcmp(command_copy, "END") == 0) {
        return 1;
    }

    Parser::LBLParse("DemoOption_Menu");
    return 0;
}

/* Function start: 0x409D50 */
void OptionMenu::UpdateSpriteStates(int sprite_count, int sprite_index)
{
    int i = 0;
    int iVar1;
    int iVar2;
    Sprite* sprite;

    if (sprite_count > selected_option) {
        sprite_count = selected_option;
    }

    spriteList->current = spriteList->head;

    if (*(int*)spriteList == 0) {
        return;
    }

    do {
        if (i >= sprite_count) {
            return;
        }

        if (sprite_index == 0) {
            iVar1 = (int)spriteList->current;
            if (iVar1 != 0) {
                iVar1 = *(int*)(iVar1 + 8);
            }
            options[0]->SetState2(*(int*)(iVar1 + 0x48));
            sprite = options[0];
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }
        if (sprite_index == 1) {
            iVar1 = (int)spriteList->current;
            if (iVar1 != 0) {
                iVar1 = *(int*)(iVar1 + 8);
            }
            options[1]->SetState2(*(int*)(iVar1 + 0x48));
            sprite = options[1];
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }
        if (sprite_index == 2) {
            iVar1 = (int)spriteList->current;
            if (iVar1 != 0) {
                iVar1 = *(int*)(iVar1 + 8);
            }
            options[2]->SetState2(*(int*)(iVar1 + 0x48));
            sprite = options[2];
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }

        // Set bounding box values on the current list item
        iVar1 = (int)spriteList->current;
        if (iVar1 != 0) {
            iVar1 = *(int*)(iVar1 + 8);
        }
        *(int*)(iVar1 + 0x4c) = 0xda;

        iVar1 = (int)spriteList->current;
        if (iVar1 != 0) {
            iVar1 = *(int*)(iVar1 + 8);
        }
        *(int*)(iVar1 + 0x50) = 0xcc;

        iVar1 = (int)spriteList->current;
        if (iVar1 != 0) {
            iVar1 = *(int*)(iVar1 + 8);
        }
        *(int*)(iVar1 + 0x54) = 0x1b3;

        iVar1 = (int)spriteList->current;
        if (iVar1 != 0) {
            iVar1 = *(int*)(iVar1 + 8);
        }
        *(int*)(iVar1 + 0x58) = 0xee;

        // Advance iterator
        iVar1 = (int)spriteList;
        iVar2 = *(int*)(iVar1 + 8);
        if (*(int*)(iVar1 + 4) == iVar2) break;
        if (iVar2 != 0) {
            *(int*)(iVar1 + 8) = *(int*)(iVar2 + 4);
        }
        i++;
    } while (*(int*)spriteList != 0);
}
