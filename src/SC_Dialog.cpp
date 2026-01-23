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
    spriteQueue = queue;
    field_88 = 0x54;
}

/* Function start: 0x408A40 */
SC_Dialog::~SC_Dialog()
{
    QueueNode* node;
    QueueNode* current;
    Sprite* sprite;
    Queue* queue;

    queue = spriteQueue;
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
                        node->data = 0;
                        node->prev = 0;
                        node->next = 0;
                        FreeMemory(node);
                        queue->m_current = 0;
                    }
                    queue->m_current = queue->m_head;
                }
                if (sprite != 0) {
                    sprite->~Sprite();
                    FreeMemory(sprite);
                }
            }
        }
        FreeMemory(queue);
        spriteQueue = 0;
    }
}
