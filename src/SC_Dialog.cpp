#include "SC_Dialog.h"
#include "Sprite.h"
#include "Memory.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

/* Function start: 0x408970 */
SC_Dialog::SC_Dialog()
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
    field_bc = queue;
    field_88 = 0x54;
}

/* Function start: 0x408A40 */
SC_Dialog::~SC_Dialog()
{
    ZBQueueNode* node;
    ZBQueueNode* curr;
    Sprite* sprite;
    ZBQueue* queue;

    queue = field_bc;
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
                        FreeMemory(node);
                        queue->current = 0;
                    }
                    queue->current = queue->head;
                }
                if (sprite != 0) {
                    delete sprite;
                }
            }
        }
        FreeMemory(queue);
        field_bc = 0;
    }
}

/* Function start: 0x408B60 */
void SC_Dialog::StopAll()
{
    field_bc->current = field_bc->head;
    if (field_bc->head != 0) {
        do {
            ZBQueueNode* node = field_bc->current;
            Sprite* spr = (Sprite*)0;
            if (node != 0) spr = (Sprite*)node->data;
            spr->StopAnimationSound();
            if (field_bc->tail == field_bc->current) break;
            if (field_bc->current != 0) field_bc->current = ((ZBQueueNode*)field_bc->current)->prev;
        } while (field_bc->head != 0);
    }
    field_8c &= ~0x2000;
}

/* Function start: 0x408BD0 */
void SC_Dialog::PreDraw()
{
    field_bc->current = field_bc->head;
    if (field_bc->head != 0) {
        do {
            ZBQueueNode* node = field_bc->current;
            Sprite* spr = (Sprite*)0;
            if (node != 0) spr = (Sprite*)node->data;
            spr->Init();
            if (field_bc->tail == field_bc->current) break;
            if (field_bc->current != 0) field_bc->current = ((ZBQueueNode*)field_bc->current)->prev;
        } while (field_bc->head != 0);
    }
    field_90 = 1;
    field_8c |= 0x2000;
}

/* Function start: 0x408C40 */
void SC_Dialog::AddSprite(Sprite* spr)
{
    ZBQueueNode* newNode;
    ZBQueueNode* currNode;

    if (spr == 0) return;
    spr->FreeAnimation();

    ZBQueue* q = field_bc;
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
int SC_Dialog::Draw()
{
    field_90 = 1;
    if (!(field_8c & 0x2000)) {
        PreDraw();
    }

    if (field_bc == 0) return 1;

    field_bc->current = field_bc->head;
    if (field_bc->head != 0) {
        do {
            ZBQueueNode* node = field_bc->current;
            Sprite* spr = (Sprite*)0;
            if (node != 0) spr = (Sprite*)node->data;
            if (spr->Do(spr->loc_x, spr->loc_y, 1.0)) {
                field_90 = 0;
            }
            if (field_bc->tail == field_bc->current) break;
            if (field_bc->current != 0) field_bc->current = ((ZBQueueNode*)field_bc->current)->prev;
        } while (field_bc->head != 0);
    }
    return field_90;
}

/* Function start: 0x408F80 */
int SC_Dialog::DrawWithStates(int* states)
{
    field_90 = 1;
    if (!(field_8c & 0x2000)) {
        PreDraw();
    }

    if (field_bc == 0) return 1;

    field_bc->current = field_bc->head;
    if (field_bc->head != 0) {
        int i = 0;
        do {
            ZBQueueNode* node = field_bc->current;
            Sprite* spr = (Sprite*)0;
            if (node != 0) spr = (Sprite*)node->data;
            if (states[i++] != 0) {
                if (spr->Do(spr->loc_x, spr->loc_y, 1.0)) {
                    field_90 = 0;
                }
            }
            if (field_bc->tail == field_bc->current) break;
            if (field_bc->current != 0) field_bc->current = ((ZBQueueNode*)field_bc->current)->prev;
        } while (field_bc->head != 0);
    }
    return field_90;
}

/* Function start: 0x409030 */
int SC_Dialog::LBLParse(char* line)
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
