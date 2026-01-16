#include "OptionMenu.h"
#include "Memory.h"
#include "Sprite.h"
#include "SpriteList.h"
#include "Character.h"
#include "string.h"

#include <stdio.h>
#include <string.h>

extern void* DAT_00435a74;
extern void* DAT_00435a78;
extern void* DAT_00435a7c;

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

    spriteList->current = spriteList->head;

    if (spriteList->head == 0) {
        return;
    }

    do {
        if (i >= sprite_count) {
            return;
        }

        if (sprite_index == 0) {
            node = spriteList->current;
            data = (node != 0) ? (SpriteData*)node->data : 0;
            options[0]->SetState2(data->state);
            sprite = options[0];
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }
        if (sprite_index == 1) {
            node = spriteList->current;
            data = (node != 0) ? (SpriteData*)node->data : 0;
            options[1]->SetState2(data->state);
            sprite = options[1];
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }
        if (sprite_index == 2) {
            node = spriteList->current;
            data = (node != 0) ? (SpriteData*)node->data : 0;
            options[2]->SetState2(data->state);
            sprite = options[2];
            sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        }

        // Set bounding box values on the current list item
        node = spriteList->current;
        data = (node != 0) ? (SpriteData*)node->data : 0;
        data->rect_x = 0xda;

        node = spriteList->current;
        data = (node != 0) ? (SpriteData*)node->data : 0;
        data->rect_y = 0xcc;

        node = spriteList->current;
        data = (node != 0) ? (SpriteData*)node->data : 0;
        data->rect_w = 0x1b3;

        node = spriteList->current;
        data = (node != 0) ? (SpriteData*)node->data : 0;
        data->rect_h = 0xee;

        // Advance iterator
        node = spriteList->current;
        if (spriteList->tail == node) break;
        if (node != 0) {
            spriteList->current = node->prev;
        }
        i++;
    } while (spriteList->head != 0);
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

/* Function start: 0x40A150 */
void OptionMenu::SelectCharacter(int characterIndex)
{
    switch (characterIndex) {
    case 0:
        spriteList = (SpriteNode*)((Character*)DAT_00435a74)->queue;
        break;
    case 1:
        spriteList = (SpriteNode*)((Character*)DAT_00435a78)->queue;
        break;
    case 2:
        spriteList = (SpriteNode*)((Character*)DAT_00435a7c)->queue;
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
