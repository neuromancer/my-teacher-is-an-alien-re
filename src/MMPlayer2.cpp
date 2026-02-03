#include "MMPlayer2.h"
#include "Sprite.h"
#include "Memory.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

/* Function start: 0x408970 */
MMPlayer2::MMPlayer2()
{
    int* ptr;
    int i;
    ZBQueue* queue;

    ptr = &field_88;
    for (i = 0xe; i != 0; i--) {
        *ptr = 0;
        ptr++;
    }
    field_90 = 1;

    spriteList = new ZBQueue(2);
    field_88 = 0x54;
}

/* Function start: 0x408A40 */
MMPlayer2::~MMPlayer2()
{
    Sprite* sprite;
    ZBQueue* queue;

    queue = spriteList;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                sprite = (Sprite*)queue->Pop();
                if (sprite != 0) {
                    delete sprite;
                }
            }
        }
        delete queue;
        spriteList = 0;
    }
}

/* Function start: 0x408B60 */
void MMPlayer2::StopAll()
{
    spriteList->current = spriteList->head;
    if (spriteList->head != 0) {
        do {
            ZBQueueNode* node = spriteList->current;
            Sprite* spr = (Sprite*)0;
            if (node != 0) spr = (Sprite*)node->data;
            spr->StopAnimationSound();
            if (spriteList->tail == spriteList->current) break;
            if (spriteList->current != 0) spriteList->current = ((ZBQueueNode*)spriteList->current)->next;
        } while (spriteList->head != 0);
    }
    field_8c &= ~0x2000;
}

/* Function start: 0x408BD0 */
void MMPlayer2::PreDraw()
{
    spriteList->current = spriteList->head;
    if (spriteList->head != 0) {
        do {
            ZBQueueNode* node = spriteList->current;
            Sprite* spr = (Sprite*)0;
            if (node != 0) spr = (Sprite*)node->data;
            spr->Init();
            if (spriteList->tail == spriteList->current) break;
            if (spriteList->current != 0) spriteList->current = ((ZBQueueNode*)spriteList->current)->next;
        } while (spriteList->head != 0);
    }
    field_90 = 1;
    field_8c |= 0x2000;
}

/* Function start: 0x408C40 */
void MMPlayer2::AddSprite(Sprite* spr)
{
    ZBQueueNode* newNode;
    ZBQueueNode* currNode;

    if (spr == 0) return;
    spr->FreeAnimation();

    ZBQueue* q = spriteList;
    q->ResetForSortedAdd(spr);
    if (q->type == 1 || q->type == 2) {
        if (q->head != 0) {
            while (1) {
                if (((Sprite*)q->current->data)->field_0xb0 < spr->field_0xb0) {
                    q->Insert(spr);
                    return;
                }
                if (q->tail == q->current) break;
                q->current = q->current->next;
            }
        }
        q->Push(spr);
    } else {
        q->Insert(spr);
    }
}

/* Function start: 0x408EE0 */
int MMPlayer2::Draw()
{
    field_90 = 1;
    if (!(field_8c & 0x2000)) {
        PreDraw();
    }

    ZBQueue* list = spriteList;
    if (list == 0) return 1;

    list->current = list->head;
    if (list->head != 0) {
        do {
            ZBQueueNode* node = list->current;
            Sprite* spr = (Sprite*)0;
            if (node != 0) spr = (Sprite*)node->data;
            if (spr->Do(spr->loc_x, spr->loc_y, 1.0)) {
                field_90 = 0;
            }
            if (list->tail == list->current) break;
            if (list->current != 0) list->current = node->next;
        } while (list->head != 0);
    }
    return field_90;
}

/* Function start: 0x408F80 */
int MMPlayer2::DrawWithStates(int* states)
{
    field_90 = 1;
    if (!(field_8c & 0x2000)) {
        PreDraw();
    }

    ZBQueue* list = spriteList;
    if (list == 0) return 1;

    list->current = list->head;
    if (list->head != 0) {
        int i = 0;
        do {
            int s = states[i++];
            ZBQueueNode* node = list->current;
            if (s != 0) {
                Sprite* spr = (Sprite*)0;
                if (node != 0) spr = (Sprite*)node->data;
                if (spr->Do(spr->loc_x, spr->loc_y, 1.0)) {
                    field_90 = 0;
                }
            }
            if (list->tail == list->current) break;
            if (list->current != 0) list->current = node->next;
        } while (list->head != 0);
    }
    return field_90;
}

/* Function start: 0x409030 */
int MMPlayer2::LBLParse(char* line)
{
    char command[32];
    Sprite* sprite;

    if (sscanf(line, " %s ", command) != 1) {
        return 0;
    }

    if (strcmp(command, "SPRITE") == 0) {
        sprite = new Sprite(0);
        Parser::ProcessFile(sprite, this, 0);
        AddSprite(sprite);
    } else if (strcmp(command, "END") == 0) {
        return 1;
    } else {
        return Parser::LBLParse("MMPlayer2");
    }
    return 0;
}
