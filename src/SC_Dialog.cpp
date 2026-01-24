#include "SC_Dialog.h"
#include "Queue.h"
#include "Sprite.h"
#include "Memory.h"
#include <string.h>

/* Function start: 0x408970 */
SC_Dialog::SC_Dialog()
{
    int* ptr;
    int i;
    Queue* queue;

    ptr = &field_88;
    for (i = 0xe; i != 0; i--) {
        *ptr = 0;
        ptr++;
    }
    field_90 = 1;

    queue = (Queue*)AllocateMemory(0x10);
    if (queue != 0) {
        queue->m_field_0xc = 2;
        queue->m_head = 0;
        queue->m_tail = 0;
        queue->m_current = queue->m_head;
    }
    field_bc = queue;
    field_88 = 0x54;
}

/* Function start: 0x408A40 */
SC_Dialog::~SC_Dialog()
{
    QueueNode* node;
    QueueNode* current;
    Sprite* sprite;
    Queue* queue;

    queue = field_bc;
    if (queue != 0) {
        if (queue->m_head != 0) {
            queue->m_current = queue->m_head;
            while (queue->m_head != 0) {
                current = (QueueNode*)queue->m_current;
                if (current == 0) {
                    sprite = 0;
                } else {
                    if (queue->m_head == current) {
                        queue->m_head = current->next;
                    }
                    if (queue->m_tail == current) {
                        queue->m_tail = current->prev;
                    }
                    if (current->prev != 0) {
                        current->prev->next = current->next;
                    }
                    if (current->next != 0) {
                        current->next->prev = current->prev;
                    }
                    node = current;
                    sprite = 0;
                    if (node != 0) {
                        sprite = (Sprite*)node->data;
                        delete node;
                        queue->m_current = 0;
                    }
                    queue->m_current = queue->m_head;
                }
                if (sprite != 0) {
                    delete sprite;
                }
            }
        }
        delete queue;
        field_bc = 0;
    }
}

/* Function start: 0x408B60 */
void SC_Dialog::StopAll()
{
    if (field_bc == 0) return;

    QueueNode* node = (QueueNode*)field_bc->m_head;
    while (node != 0) {
        Sprite* sprite = (Sprite*)node->data;
        if (sprite != 0) {
            sprite->StopAnimationSound();
        }
        node = node->next;
    }
    field_8c &= ~0x2000;
}

/* Function start: 0x408EE0 */
void SC_Dialog::Draw()
{
    field_90 = 1;
    if (!(field_8c & 0x2000)) {
        PreDraw();
    }

    if (field_bc == 0) return;

    if (field_bc->m_head != 0) {
        field_bc->m_current = field_bc->m_head;
        while (1) {
            QueueNode* node = (QueueNode*)field_bc->m_current;
            Sprite* spr = (node != 0) ? (Sprite*)node->data : 0;
            if (spr->Do(spr->loc_x, spr->loc_y, 1.0)) {
                field_90 = 0;
            }
            if (field_bc->m_tail == field_bc->m_current) break;
            field_bc->m_current = node->next;
            if (field_bc->m_current == 0) break;
        }
    }
}

/* Function start: 0x408F80 */
int SC_Dialog::DrawWithStates(int* states)
{
    field_90 = 1;
    if (!(field_8c & 0x2000)) {
        PreDraw();
    }

    Queue* queue = field_bc;
    if (queue == 0) return 1;

    void* head = queue->m_head;
    queue->m_current = head;
    if (head != 0) {
        int i = 0;
        do {
            QueueNode* node = (QueueNode*)queue->m_current;
            if (states[i++] != 0) {
                Sprite* spr = (Sprite*)0;
                if (node != 0) spr = (Sprite*)node->data;
                if (spr->Do(spr->loc_x, spr->loc_y, 1.0)) {
                    field_90 = 0;
                }
            }
            if (queue->m_tail == queue->m_current) break;
            if (node != 0) queue->m_current = node->next;
        } while (queue->m_head != 0);
    }
    return field_90;
}

/* Function start: 0x408BD0 */
void SC_Dialog::PreDraw()
{
    Queue* queue = field_bc;
    if (queue == 0) return;

    void* head = queue->m_head;
    queue->m_current = head;
    if (head != 0) {
        do {
            QueueNode* node = (QueueNode*)queue->m_current;
            Sprite* spr = (Sprite*)0;
            if (node != 0) spr = (Sprite*)node->data;
            spr->Init();
            if (queue->m_tail == queue->m_current) break;
            if (node != 0) queue->m_current = node->next;
        } while (queue->m_head != 0);
    }
    field_90 = 1;
    field_8c |= 0x2000;
}

/* Function start: 0x408C40 */
void SC_Dialog::AddSprite(Sprite* spr)
{
    field_bc->Insert(spr);
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
        AddSprite(sprite);
        Parser::ProcessFile(sprite, this, 0);
    } else if (strcmp(command, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("MMPlayer2");
    }
    return 0;
}
