#include "MMPlayer2.h"
#include "Sprite.h"
#include "Memory.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

/* Function start: 0x408970 */
MMPlayer2::MMPlayer2()
{
    ZBQueue* queue;

    memset(&field_88, 0, 0xe * 4);
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
                sprite = (Sprite*)queue->RemoveCurrent();
                if (sprite != 0) {
                    sprite->~Sprite();
                    operator delete(sprite);
                }
            }
        }
        operator delete(queue);
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
    spr->StopAnimationSound();

    ZBQueue* q = spriteList;
    q->ResetForSortedAdd(spr);
    if (q->type == 1 || q->type == 2) {
        if (q->head != 0) {
            do {
                if ((unsigned int)((Sprite*)q->current->data)->field_0xb0 < (unsigned int)spr->field_0xb0) {
                    q->InsertNode(spr);
                    return;
                }
                if (q->tail == q->current) {
                    q->PushNode(spr);
                    return;
                }
                if (q->current != 0) q->current = q->current->next;
            } while (q->current != 0);
            return;
        }
        q->InsertNode(spr);
    } else {
        q->InsertBeforeCurrent(spr);
    }
}

/* Function start: 0x408EE0 */
int MMPlayer2::Draw()
{
    Sprite* spr;

    field_90 = 1;
    if (!(field_8c & 0x2000)) {
        PreDraw();
    }

    spriteList->current = spriteList->head;
    if (spriteList->head != 0) {
        do {
            spr = (Sprite*)0;
            if (spriteList->current != 0) spr = (Sprite*)spriteList->current->data;
            if (spr->Do(spr->loc_x, spr->loc_y, 1.0)) {
                field_90 = 0;
            }
            if (spriteList->tail == spriteList->current) break;
            if (spriteList->current != 0) spriteList->current = spriteList->current->next;
        } while (spriteList->head != 0);
    }
    return field_90;
}

/* Function start: 0x408F80 */
int MMPlayer2::DrawWithStates(int* states)
{
    Sprite* spr;
    int i;

    i = 0;
    field_90 = 1;
    if (!(field_8c & 0x2000)) {
        PreDraw();
    }

    spriteList->current = spriteList->head;
    if (spriteList->head != 0) {
        do {
            if (states[i++] != 0) {
                spr = (Sprite*)0;
                if (spriteList->current != 0) spr = (Sprite*)spriteList->current->data;
                if (spr->Do(spr->loc_x, spr->loc_y, 1.0)) {
                    field_90 = 0;
                }
            }
            if (spriteList->tail == spriteList->current) break;
            if (spriteList->current != 0) spriteList->current = spriteList->current->next;
        } while (spriteList->head != 0);
    }
    return field_90;
}

/* Function start: 0x409030 */
int MMPlayer2::LBLParse(char* line)
{
    char command[32];
    Sprite* sprite;

    command[0] = 0;
    sscanf(line, " %s ", command);

    if (strcmp(command, "SPRITE") == 0) {
        sprite = new Sprite(0);
        Parser::ProcessFile(sprite, this, 0);
        AddSprite(sprite);
    } else if (strcmp(command, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("MMPlayer2");
    }
    return 0;
}
