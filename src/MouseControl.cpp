#include "MouseControl.h"
#include "Memory.h"
#include "Sprite.h"

/* Function start: 0x41F280 */
MouseControl::MouseControl()
{
    field_0x88[0] = 0;
    field_0x88[1] = 0;
    field_0x88[2] = 0;
    field_0x88[2] = 1;
    Queue* queue = (Queue*)AllocateMemory(0x10);
    if (queue != 0) {
        queue->m_head = 0;
        queue->m_tail = 0;
        queue->m_current = 0;
        queue->m_field_0xc = 2;
    }
    m_queue = queue;
    field_0x88[0] = 0x54;
}

/* Function start: 0x41F360 */
MouseControl::~MouseControl()
{
    QueueNode* node;
    QueueNode* current;
    void* sprite;
    Queue* queue;

    queue = m_queue;
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
                        queue->m_tail = current->next;
                    }
                    if (current->next != 0) {
                        current->next->prev = current->prev;
                    }
                    if (current->prev != 0) {
                        current->prev->next = current->next;
                    }
                    node = current;
                    sprite = 0;
                    if (node != 0) {
                        sprite = node->data;
                        node->data = 0;
                        node->next = 0;
                        node->prev = 0;
                        FreeMemory(node);
                        queue->m_current = 0;
                    }
                    queue->m_current = queue->m_head;
                }
                if (sprite != 0) {
                    ((Sprite*)sprite)->~Sprite();
                    FreeMemory(sprite);
                }
            }
        }
        FreeMemory(queue);
        m_queue = 0;
    }
}
