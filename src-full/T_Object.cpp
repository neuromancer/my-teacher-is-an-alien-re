#include "T_Object.h"
#include "SpriteAction.h"
#include "GameState.h"
#include "Memory.h"
#include "SC_Question.h"
#include <stdio.h>
#include <string.h>

extern "C" extern GameState* DAT_0046aa30;
extern char* DAT_0046aa00;

extern void __fastcall FUN_00406fd0(void*, int, int); // LinkedList::Add

/* Function start: 0x40C630 */
T_Object::T_Object(int id) : Parser()
{
    memset(&field_90, 0, 0xE * 4);
    itemId = id;
    if (id == 0) {
        ShowError("illegal handle");
    }
    ParseFile(this, "mis\\obj0001.mis", "[OBJ%5.5d]", itemId);
}

/* Function start: 0x40C6E0 */
T_Object::~T_Object()
{
    if (sprite != 0) {
        sprite->~Sprite();
        FreeMemory(sprite);
        sprite = 0;
    }

    LinkedList* list = actionList;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                void* data = list->RemoveCurrent();
                if (data != 0) {
                    ((SpriteAction*)data)->~SpriteAction();
                    FreeMemory(data);
                }
            }
        }
        FreeMemory(list);
        actionList = 0;
    }
}

/* Function start: 0x40C830 */
void T_Object::StopSound()
{
    if (sprite != 0) {
        sprite->StopAnimationSound();
    }
}

/* Function start: 0x40C840 */
void T_Object::Display(int x, int y, int visible)
{
    if (y < 0 || x < 0) {
        return;
    }
    if (sprite == 0) {
        return;
    }
    if (visible != 0) {
        sprite->ResetAnimation(1, 0);
    } else {
        sprite->ResetAnimation(0, 0);
    }
    sprite->RenderAt(x, y, 0, 0x3FF00000);
}

/* Function start: 0x40C890 */
void T_Object::Reset()
{
    field_90 |= 2;
    sprintf(DAT_0046aa00, "OBJ%3.3d", itemId);

    GameState* gs = DAT_0046aa30;
    int idx = gs->FindState(DAT_0046aa00);
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] = 1;

    // Enqueue all actions from the action list
    LinkedList* list = actionList;
    if (list == 0) {
        return;
    }
    if (list->head == 0) {
        return;
    }

    list->current = list->head;
    while (list->head != 0) {
        void* data = 0;
        if (list->current != 0) {
            data = list->current->data;
        }
        EnqueueSpriteAction(data);
        if (list->tail == list->current) {
            return;
        }
        if (list->current != 0) {
            list->current = list->current->next;
        }
    }
}

/* Function start: 0x40C9E0 */
int T_Object::HandleMsg(int* msg)
{
    if (msg[4] != 6) {
        return 0;
    }
    SendGameMessage(4, field_98, 0, 0, 2, 0, 0, 0, 0, 0);
    return 1;
}

/* Function start: 0x40CA20 */
int T_Object::LBLParse(char* param_1)
{
    char value[32];
    char token[32];

    value[0] = '\0';
    token[0] = '\0';
    sscanf(param_1, " %s ", token);

    if (strcmp(token, "INVENTORYSPR") == 0) {
        if (sprite != 0) {
            ShowError("illegal %s", param_1);
        }
        sprite = new Sprite((char*)0);
        Parser::ProcessFile(sprite, this, (char*)0);
    }
    else if (strcmp(token, "DISCRIPTION") == 0) {
        sscanf(param_1, " %s %d", token, &description);
    }
    else if (strcmp(token, "ILLEGAL") == 0) {
        sscanf(param_1, " %s %d", token, &field_98);
    }
    else if (strcmp(token, "MESSAGE") == 0) {
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        if (actionList == 0) {
            actionList = new LinkedList();
        }

        LinkedList* list = actionList;
        if (action == 0) {
            ShowError("queue fault 0101");
        }

        list->current = list->head;
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                list->InsertNode(action);
            } else {
                while (list->current != 0) {
                    ListNode* node = list->current;
                    if (*(int*)node->data < *(int*)action) {
                        list->InsertNode(action);
                        goto parse_action;
                    }
                    if (list->tail == node) {
                        list->PushNode(action);
                        goto parse_action;
                    }
                    if (node != 0) {
                        list->current = node->next;
                    }
                }
            }
        } else {
            FUN_00406fd0(list, 0, (int)action);
        }

    parse_action:
        ParseSpriteAction(action, this);
    }
    else if (strcmp(token, "END") == 0) {
        if (sprite != 0) {
            sprite->StopAnimationSound();
        }
        return 1;
    }
    else {
        Parser::LBLParse("T_Object");
    }

    return 0;
}
