#include "MouseControl.h"
#include "Memory.h"
#include "Sprite.h"
#include <new.h>

// C wrapper for MouseControl constructor (used by SCI_AfterSchoolMenu)
extern "C" void* MouseControl_Constructor(void* mem) {
    return new(mem) MouseControl();
}

/* Function start: 0x41F280 */
MouseControl::MouseControl()
{
    field_0x88 = 0;
    field_0x8c = 0;
    field_0x90 = 0;
    field_0x90 = 1;
    Queue* queue = (Queue*)AllocateMemory(0x10);
    if (queue != 0) {
        queue->m_head = 0;
        queue->m_tail = 0;
        queue->m_current = 0;
        queue->m_field_0xc = 2;
    }
    m_queue = queue;
    field_0x88 = 0x54;
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

/* Function start: 0x41F480 */
void MouseControl::StopAll()
{
    Queue* queue;
    QueueNode* current;
    Sprite* sprite;

    queue = m_queue;
    queue->m_current = queue->m_head;
    if (queue->m_head != 0) {
        do {
            current = (QueueNode*)queue->m_current;
            sprite = 0;
            if (current != 0) {
                sprite = (Sprite*)current->data;
            }
            sprite->StopAnimationSound();

            queue = m_queue;
            current = (QueueNode*)queue->m_current;
            if (queue->m_tail == current) {
                break;
            }
            if (current != 0) {
                queue->m_current = current->next;
            }
        } while (queue->m_head != 0);
    }
    field_0x8c = field_0x8c & ~0x2000;
}

/* Function start: 0x41F4F0 */
void MouseControl::Init()
{
    Queue* queue;
    QueueNode* current;
    Sprite* sprite;

    queue = m_queue;
    queue->m_current = queue->m_head;
    if (queue->m_head != 0) {
        do {
            current = (QueueNode*)queue->m_current;
            sprite = 0;
            if (current != 0) {
                sprite = (Sprite*)current->data;
            }
            sprite->Init();

            queue = m_queue;
            current = (QueueNode*)queue->m_current;
            if (queue->m_tail == current) {
                break;
            }
            if (current != 0) {
                queue->m_current = current->next;
            }
        } while (queue->m_head != 0);
    }
    field_0x90 = 1;
    field_0x8c = field_0x8c | 0x2000;
}

/* Function start: 0x41F800 */
int MouseControl::DoAll()
{
    Queue* queue;
    QueueNode* current;
    Sprite* sprite;
    int result;

    field_0x90 = 1;
    if ((field_0x8c & 0x2000) == 0) {
        Init();
    }
    queue = m_queue;
    queue->m_current = queue->m_head;
    if (queue->m_head != 0) {
        do {
            current = (QueueNode*)queue->m_current;
            sprite = 0;
            if (current != 0) {
                sprite = (Sprite*)current->data;
            }
            result = sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
            if (result != 0) {
                field_0x90 = 0;
            }

            queue = m_queue;
            current = (QueueNode*)queue->m_current;
            if (queue->m_tail == current) {
                break;
            }
            if (current != 0) {
                queue->m_current = current->next;
            }
        } while (queue->m_head != 0);
    }
    return field_0x90;
}
