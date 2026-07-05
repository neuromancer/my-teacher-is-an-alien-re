#include "T_Object.h"
#include "SpriteAction.h"
#include "GameState.h"
#include "Memory.h"
#include "SC_Question.h"
#include <stdio.h>
#include <string.h>

#include "globals.h"


/* Function start: 0x40C630 */
T_Object::T_Object(int id) : Parser()
{
    memset(&objectFlags, 0, 0xE * 4);
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
        delete sprite;
        sprite = 0;
    }

    LinkedList* list = actionList;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                void* data = list->RemoveCurrent();
                if (data != 0) {
                    delete (SpriteAction*)data;
                }
            }
        }
        delete list;
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
    sprite->Do(x, y, 1.0);
}

/* Function start: 0x40C890 */
void T_Object::Reset()
{
    objectFlags |= 2;
    sprintf(g_Buffer_0046aa00, "OBJ%3.3d", itemId);

    GameState* gs = g_GameState_0046aa30;
    int idx = gs->FindState(g_Buffer_0046aa00);
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] = 1;

    LinkedList* list = actionList;
    if (list == 0) {
        return;
    }
    if (list->head == 0) {
        return;
    }

    do {
        list = actionList;
        SpriteAction* data = 0;
        int listType = list->type;
        if (listType == 1 || listType == 4) {
            list->current = list->head;
        } else if (listType == 2 || listType == 0) {
            list->current = list->tail;
        } else {
            ShowError("bad queue type %lu", listType);
        }

        if (list->current != 0) {
            data = (SpriteAction*)list->RemoveCurrent();
        }
        EnqueueSpriteAction(data);
        if (data != 0) {
            delete data;
        }
    } while (actionList->head != 0);
}

/* Function start: 0x40C9E0 */
int T_Object::HandleMsg(SpriteAction* msg)
{
    if (msg->instruction != 6) {
        return 0;
    }
    SendGameMessage(4, soundHandle, 0, 0, 2, 0, 0, 0, 0, 0);
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
        sscanf(param_1, " %s %d", value, &descriptionSound);
    }
    else if (strcmp(token, "ILLEGAL") == 0) {
        sscanf(param_1, " %s %d", value, &soundHandle);
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
            ((Queue*)list)->InsertAtCurrent((void*)action);
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
        Parser::ReportUnknownLabel("T_Object");
    }

    return 0;
}
