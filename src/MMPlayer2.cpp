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

    queue = (ZBQueue*)AllocateMemory(0x10);
    if (queue != 0) {
        queue->type = 2;
        queue->head = 0;
        queue->tail = 0;
        queue->current = queue->head;
    }
    spriteList = queue;
    field_88 = 0x54;
}

/* Function start: 0x408A40 */
MMPlayer2::~MMPlayer2()
{
    ZBQueueNode* node;
    ZBQueueNode* curr;
    Sprite* sprite;
    ZBQueue* queue;

    queue = spriteList;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                curr = (ZBQueueNode*)queue->current;
                if (curr == 0) {
                    sprite = 0;
                } else {
                    if (queue->head == curr) {
                        queue->head = curr->prev;
                    }
                    if (queue->tail == curr) {
                        queue->tail = curr->next;
                    }
                    if (curr->next != 0) {
                        curr->next->prev = curr->prev;
                    }
                    if (curr->prev != 0) {
                        curr->prev->next = curr->next;
                    }
                    node = curr;
                    sprite = 0;
                    if (node != 0) {
                        sprite = (Sprite*)node->data;
                        delete node;
                        queue->current = 0;
                    }
                    queue->current = queue->head;
                }
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
            if (spriteList->current != 0) spriteList->current = ((ZBQueueNode*)spriteList->current)->prev;
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
            if (spriteList->current != 0) spriteList->current = ((ZBQueueNode*)spriteList->current)->prev;
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
    if (spr == 0) ShowError("queue fault 0101");

    q->current = q->head;
    if (q->type == 1 || q->type == 2) {
        if (q->head != 0) {
            while (1) {
                currNode = q->current;
                if (((Sprite*)currNode->data)->field_0xb0 < spr->field_0xb0) {
                    if (spr == 0) ShowError("queue fault 0102");
                    newNode = new ZBQueueNode();
                    if (newNode != 0) {
                        newNode->data = spr;
                        newNode->next = 0;
                        newNode->prev = 0;
                    }
                    if (q->current == 0) q->current = q->head;
                    if (q->head == 0) {
                        q->head = newNode;
                        q->tail = newNode;
                        q->current = newNode;
                    } else {
                        newNode->prev = q->current;
                        newNode->next = q->current->next;
                        if (q->current->next == 0) {
                            q->head = newNode;
                        } else {
                            q->current->next->prev = newNode;
                        }
                        q->current->next = newNode;
                    }
                    return;
                }
                if (q->tail == currNode) break;
                q->current = currNode->prev;
                if (q->current == 0) return;
            }
            
            // Push to tail case
            if (spr == 0) ShowError("queue fault 0112");
            newNode = new ZBQueueNode();
            if (newNode != 0) {
                newNode->data = spr;
                newNode->next = 0;
                newNode->prev = 0;
            }
            if (q->current == 0) q->current = q->tail;
            if (q->head == 0) {
                q->head = newNode;
                q->tail = newNode;
                q->current = newNode;
            } else {
                if (q->tail == 0 || q->tail->prev != 0) {
                    ShowError("queue fault 0113");
                }
                newNode->prev = 0;
                newNode->next = q->tail;
                q->tail->prev = newNode;
                q->tail = newNode;
            }
        } else {
            // Head was 0 case
            if (spr == 0) ShowError("queue fault 0102");
            newNode = new ZBQueueNode();
            if (newNode != 0) {
                newNode->data = spr;
                newNode->next = 0;
                newNode->prev = 0;
            }
            if (q->current == 0) q->current = q->head;
            if (q->head == 0) {
                q->head = newNode;
                q->tail = newNode;
                q->current = newNode;
            } else {
                newNode->prev = q->current;
                newNode->next = q->current->next;
                if (q->current->next == 0) {
                    q->head = newNode;
                } else {
                    q->current->next->prev = newNode;
                }
                q->current->next = newNode;
            }
        }
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
            if (list->current != 0) list->current = node->prev;
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
            if (list->current != 0) list->current = node->prev;
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
