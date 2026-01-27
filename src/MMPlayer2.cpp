#include "MMPlayer2.h"
#include "Sprite.h"
#include "ZBufferManager.h"
#include "string.h"
#include <stdio.h>
#include <string.h>

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
