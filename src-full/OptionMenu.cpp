#include "OptionMenu.h"
#include "Memory.h"
#include "Sprite.h"
#include "SpriteList.h"
#include "Character.h"
#include "string.h"
#include "globals.h"

#include <stdio.h>
#include <string.h>

/* Function start: 0x409940 */ /* DEMO ONLY - no full game match */
OptionMenu::OptionMenu()
{
    // The original code zeroes 6 dwords starting at offset 0x88
    // This includes options[0..2], field_94, spriteList, and selected_option
    memset(&options[0], 0, 6 * sizeof(void*));
    
    selected_option = 5;

    options[0] = new Sprite("demo/option1.smk");
    options[0]->flags &= ~2;
    options[0]->loc_x = 216;
    options[0]->loc_y = 202;
    options[0]->priority = 10;
    options[0]->SetState(4);
    options[0]->SetRange(0, 1, 1);
    options[0]->SetRange(1, 2, 2);
    options[0]->SetRange(2, 3, 12);
    options[0]->SetRange(3, 13, 13);

    options[1] = new Sprite("demo/option2.smk");
    options[1]->flags &= ~2;
    options[1]->loc_x = 216;
    options[1]->loc_y = 202;
    options[1]->priority = 10;
    options[1]->SetState(4);
    options[1]->SetRange(0, 1, 1);
    options[1]->SetRange(1, 2, 2);
    options[1]->SetRange(2, 3, 12);
    options[1]->SetRange(3, 13, 13);

    options[2] = new Sprite("demo/option3.smk");
    options[2]->flags &= ~2;
    options[2]->loc_x = 216;
    options[2]->loc_y = 202;
    options[2]->priority = 10;
    options[2]->SetState(4);
    options[2]->SetRange(0, 1, 1);
    options[2]->SetRange(1, 2, 2);
    options[2]->SetRange(2, 3, 12);
    options[2]->SetRange(3, 13, 13);
}

/* Function start: 0x409BF0 */ /* DEMO ONLY - no full game match */
OptionMenu::~OptionMenu()
{
    spriteList = 0;
    if (options[0] != 0) {
        delete options[0];
        options[0] = 0;
    }
    if (options[1] != 0) {
        delete options[1];
        options[1] = 0;
    }
    if (options[2] != 0) {
        delete options[2];
        options[2] = 0;
    }
}

/* Function start: 0x409CD0 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x409D50 */ /* DEMO ONLY - no full game match */
void OptionMenu::UpdateSpriteStates(int sprite_count, int sprite_index)
{
    if (sprite_count > selected_option) {
        sprite_count = selected_option;
    }

    int i = 0;
    spriteList->current = spriteList->head;
    if (spriteList->head == 0) return;

    while (i < sprite_count) {
        if (sprite_index == 0) {
            options[0]->SetState2((spriteList->current ? (SpriteData*)spriteList->current->data : 0)->state);
            options[0]->Do(options[0]->loc_x, options[0]->loc_y, 1.0);
        }
        if (sprite_index == 1) {
            options[1]->SetState2((spriteList->current ? (SpriteData*)spriteList->current->data : 0)->state);
            options[1]->Do(options[1]->loc_x, options[1]->loc_y, 1.0);
        }
        if (sprite_index == 2) {
            options[2]->SetState2((spriteList->current ? (SpriteData*)spriteList->current->data : 0)->state);
            options[2]->Do(options[2]->loc_x, options[2]->loc_y, 1.0);
        }

        (spriteList->current ? (SpriteData*)spriteList->current->data : 0)->rect_x = 0xda;
        (spriteList->current ? (SpriteData*)spriteList->current->data : 0)->rect_y = 0xcc;
        (spriteList->current ? (SpriteData*)spriteList->current->data : 0)->rect_w = 0x1b3;
        (spriteList->current ? (SpriteData*)spriteList->current->data : 0)->rect_h = 0xee;

        SpriteListNode* node = spriteList->current;
        if (spriteList->tail == node) break;
        if (node != 0) {
            spriteList->current = node->next;
        }
        i++;
        if (spriteList->head == 0) break;
    }
}

/* Function start: 0x409F00 */ /* DEMO ONLY - no full game match */
void OptionMenu::Render(int characterIndex)
{
    SpriteNode* pList;
    int count;

    pList = spriteList;
    if (pList != 0) {
        count = 0;
        pList->current = pList->head;
        if (pList->head != 0) {
            do {
                count++;
                if (pList->tail == pList->current) break;
                if (pList->current != 0) {
                    pList->current = pList->current->next;
                }
            } while (pList->current != 0);
        }
        UpdateSpriteStates(count, characterIndex);
    }
}

/* Function start: 0x409F40 */ /* DEMO ONLY - no full game match */
void* OptionMenu::GetOptionByIndex(int index)
{
    SpriteNode* pList;
    SpriteListNode* node;
    int i;

    i = 0;
    pList = spriteList;
    if (pList != 0) {
        pList->current = pList->head;
        pList = spriteList;
        if (pList->head != 0) {
            do {
                if (i == index) {
                    node = spriteList->current;
                    if (node == 0) {
                        return 0;
                    }
                    return node->data;
                }
                pList = spriteList;
                node = pList->current;
                if (pList->tail == node) break;
                if (node != 0) {
                    pList->current = node->next;
                }
                i++;
            } while (spriteList->head != 0);
        }
    }
    return 0;
}

/* Function start: 0x409FB0 */ /* DEMO ONLY - no full game match */
void OptionMenu::SetOptionState(int param_1, int param_2)
{
    SpriteListNode* node;
    SpriteData* data;
    int i;

    i = 0;
    if (param_1 == -5) {
        spriteList->current = spriteList->head;
        if (spriteList->head != 0) {
            while (1) {
                SetOptionState(i, param_2);
                node = spriteList->current;
                if (spriteList->tail == node) break;
                if (node != 0) {
                    spriteList->current = node->next;
                }
                i = i + 1;
                if (spriteList->head == 0) {
                    return;
                }
            }
        }
    }
    else {
        data = (SpriteData*)GetOptionByIndex(param_1);
        if (data != 0) {
            data->state = param_2;
            return;
        }
        ShowError("Invalid option number!");
    }
}

/* Function start: 0x40A040 */ /* DEMO ONLY - no full game match */
int OptionMenu::HitTest(MousePoint pt, int* indexOut, int* hitOut)
{
    SpriteListNode* node;
    SpriteData* data;
    SpriteNode* pList;
    int inRect;

    pList = spriteList;
    if (pList != 0) {
        pList->current = pList->head;
        pList = spriteList;
        if (pList->head != 0) {
            do {
                pList = spriteList;
                data = 0;
                node = pList->current;
                if (node != 0) {
                    data = (SpriteData*)node->data;
                }

                if (data->rect_x > pt.x || data->rect_w < pt.x ||
                    data->rect_y > pt.y || data->rect_h < pt.y) {
                    inRect = 0;
                } else {
                    inRect = 1;
                }

                if (inRect != 0) {
                    node = spriteList->current;
                    if (node == 0) {
                        data = 0;
                    } else {
                        data = (SpriteData*)node->data;
                    }
                    data->state = 3;
                    *hitOut = 0;
                    break;
                }

                if (pList->tail == node) {
                    break;
                }

                if (node != 0) {
                    pList->current = node->next;
                }

                ++*indexOut;

            } while (spriteList->head != 0);
        }
    }

    return *hitOut == 0;
}

/* Function start: 0x40A150 */ /* DEMO ONLY - no full game match */
void OptionMenu::SelectCharacter(int characterIndex)
{
    switch (characterIndex) {
    case 0:
        spriteList = (SpriteNode*)g_PeterCharacter_00435a74->queue;
        break;
    case 1:
        spriteList = (SpriteNode*)g_SusanCharacter_00435a78->queue;
        break;
    case 2:
        spriteList = (SpriteNode*)g_DuncanCharacter_00435a7c->queue;
        break;
    }
}

/* Function start: 0x40A1A0 */ /* DEMO ONLY - no full game match */
void OptionMenu::Deactivate()
{
    options[0]->StopAnimationSound();
    options[1]->StopAnimationSound();
    options[2]->StopAnimationSound();
}
