#include "OptionMenu.h"
#include "Memory.h"
#include "Sprite.h"
#include "SpriteList.h"
#include "Character.h"
#include "string.h"
#include "globals.h"

#include <stdio.h>
#include <string.h>

/* Function start: 0x409940 */
OptionMenu::OptionMenu()
{
    // The original code zeroes 6 dwords starting at offset 0x88
    // This includes options[0..2], field_94, spriteList, and selected_option
    memset(&options[0], 0, 6 * sizeof(void*));
    
    selected_option = 5;

    options[0] = new Sprite("demo/option1.smk");
    if (options[0]) {
        options[0]->flags &= ~2;
        options[0]->loc_x = 216;
        options[0]->loc_y = 202;
        options[0]->priority = 10;
        options[0]->SetState(4);
        options[0]->SetRange(0, 1, 1);
        options[0]->SetRange(1, 2, 2);
        options[0]->SetRange(2, 3, 12);
        options[0]->SetRange(3, 13, 13);
    }

    options[1] = new Sprite("demo/option2.smk");
    if (options[1]) {
        options[1]->flags &= ~2;
        options[1]->loc_x = 216;
        options[1]->loc_y = 202;
        options[1]->priority = 10;
        options[1]->SetState(4);
        options[1]->SetRange(0, 1, 1);
        options[1]->SetRange(1, 2, 2);
        options[1]->SetRange(2, 3, 12);
        options[1]->SetRange(3, 13, 13);
    }

    options[2] = new Sprite("demo/option3.smk");
    if (options[2]) {
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
}

/* Function start: 0x409BF0 */
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
    SpriteListNode* node;
    SpriteData* data;
    Sprite* sprite;

    if (sprite_count > selected_option) {
        sprite_count = selected_option;
    }

    if (spriteList == 0 || spriteList->head == 0) {
        return;
    }

    spriteList->current = spriteList->head;

    do {
        if (i >= sprite_count) {
            break;
        }

        node = spriteList->current;
        data = (node != 0) ? (SpriteData*)node->data : 0;

        if (data != 0 && sprite_index >= 0 && sprite_index < 3) {
            sprite = options[sprite_index];
            if (sprite != 0) {
                sprite->SetState2(data->state);
                sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
            }
        }

        if (data != 0) {
            data->rect_x = 0xda;
            data->rect_y = 0xcc;
            data->rect_w = 0x1b3;
            data->rect_h = 0xee;
        }

        // Advance iterator
        if (spriteList->tail == node) break;
        if (node != 0) {
            spriteList->current = node->prev;
        }
        i++;
    } while (spriteList->current != 0);
}

/* Function start: 0x409F00 */
void OptionMenu::Render(int characterIndex)
{
    SpriteListNode* node;
    int count;

    if (spriteList != 0) {
        count = 0;
        node = spriteList->head;
        spriteList->current = node;
        while (node != 0) {
            count = count + 1;
            node = spriteList->current;
            if (spriteList->tail == node) break;
            if (node != 0) {
                spriteList->current = node->prev;
            }
            node = spriteList->current;
        }
        UpdateSpriteStates(count, characterIndex);
    }
}

/* Function start: 0x409F40 */
void* OptionMenu::GetOptionByIndex(int index)
{
    SpriteListNode* node;
    int i;

    i = 0;
    if (spriteList != 0) {
        spriteList->current = spriteList->head;
        while (spriteList->head != 0) {
            if (i == index) {
                node = spriteList->current;
                if (node == 0) {
                    return 0;
                }
                return node->data;
            }
            node = spriteList->current;
            if (spriteList->tail == node) {
                return 0;
            }
            if (node != 0) {
                spriteList->current = node->prev;
            }
            i = i + 1;
            if (spriteList->head == 0) break;
        }
    }
    return 0;
}

/* Function start: 0x409FB0 */
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
                    spriteList->current = node->prev;
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

/* Function start: 0x40A040 */
int OptionMenu::HitTest(MousePoint pt, int* indexOut, int* hitOut)
{
    SpriteListNode* node;
    SpriteData* data;
    SpriteNode* pList;
    int hit;

    pList = spriteList;
    if (pList != 0) {
        pList->current = pList->head;
        pList = spriteList;
        if (pList->head != 0) {
            while (1) {
                pList = spriteList;
                data = 0;
                node = pList->current;
                if (node != 0) {
                    data = (SpriteData*)node->data;
                }

                if (data->rect_x > pt.x) {
                    hit = 0;
                } else if (data->rect_w < pt.x) {
                    hit = 0;
                } else if (data->rect_y > pt.y) {
                    hit = 0;
                } else {
                    hit = 1;
                    if (data->rect_h < pt.y) {
                        hit = 0;
                    }
                }

                if (hit != 0) {
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
                    pList->current = node->prev;
                }

                ++*indexOut;

                if (spriteList->head == 0) {
                    break;
                }
            }
        }
    }

    return *hitOut == 0;
}

/* Function start: 0x40A150 */
void OptionMenu::SelectCharacter(int characterIndex)
{
    switch (characterIndex) {
    case 0:
        spriteList = (SpriteNode*)DAT_00435a74->queue;
        break;
    case 1:
        spriteList = (SpriteNode*)DAT_00435a78->queue;
        break;
    case 2:
        spriteList = (SpriteNode*)DAT_00435a7c->queue;
        break;
    }
}

/* Function start: 0x40A1A0 */
void OptionMenu::Deactivate()
{
    options[0]->StopAnimationSound();
    options[1]->StopAnimationSound();
    options[2]->StopAnimationSound();
}
