#include "MouseControl.h"
#include "Memory.h"
#include "Sprite.h"
#include "string.h"
#include <new.h>
#include <string.h>
#include <stdio.h>

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
                        delete node;
                        queue->m_current = 0;
                    }
                    queue->m_current = queue->m_head;
                }
                if (sprite != 0) {
                    delete (Sprite*)sprite;
                }
            }
        }
        delete queue;
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

/* Function start: 0x41F560 */
void MouseControl::AddSprite(Sprite* s)
{
    if (s == 0)
    {
        return;
    }

    s->StopAnimationSound();
    Queue* queue = m_queue;

    if (s == 0)
    {
        ShowError("queue fault 0101");
    }

    queue->m_current = queue->m_head;

    if (queue->m_field_0xc == 1 || queue->m_field_0xc == 2)
    {
        if (queue->m_head == 0)
        {
            if (s == 0) ShowError("queue fault 0102");
            QueueNode* newNode = new QueueNode(s);

            if (queue->m_current == 0)
            {
                queue->m_current = queue->m_head;
            }

            if (queue->m_head == 0)
            {
                queue->m_head = newNode;
                queue->m_tail = newNode;
                queue->m_current = newNode;
            }
            else
            {
                newNode->next = (QueueNode*)queue->m_current;
                newNode->prev = ((QueueNode*)queue->m_current)->prev;
                if (((QueueNode*)queue->m_current)->prev == 0)
                {
                    queue->m_head = newNode;
                }
                else
                {
                    ((QueueNode*)((QueueNode*)queue->m_current)->prev)->next = newNode;
                }
                ((QueueNode*)queue->m_current)->prev = newNode;
            }
        }
        else
        {
            while (queue->m_current != 0)
            {
                QueueNode* current = (QueueNode*)queue->m_current;
                Sprite* currSprite = (Sprite*)current->data;

                if ((unsigned int)currSprite->field_0xb0 < (unsigned int)s->field_0xb0)
                {
                    if (s == 0) ShowError("queue fault 0102");
                    QueueNode* newNode = new QueueNode(s);

                    if (queue->m_current == 0)
                    {
                        queue->m_current = queue->m_head;
                    }

                    if (queue->m_head == 0)
                    {
                        queue->m_head = newNode;
                        queue->m_tail = newNode;
                        queue->m_current = newNode;
                    }
                    else
                    {
                        newNode->next = (QueueNode*)queue->m_current;
                        newNode->prev = ((QueueNode*)queue->m_current)->prev;
                        if (((QueueNode*)queue->m_current)->prev == 0)
                        {
                            queue->m_head = newNode;
                        }
                        else
                        {
                            ((QueueNode*)((QueueNode*)queue->m_current)->prev)->next = newNode;
                        }
                        ((QueueNode*)queue->m_current)->prev = newNode;
                    }
                    return;
                }

                if (queue->m_tail == current)
                {
                    if (s == 0) ShowError("queue fault 0112");
                    QueueNode* newNode = new QueueNode(s);

                    if (queue->m_current == 0)
                    {
                        queue->m_current = queue->m_tail;
                    }

                    if (queue->m_head == 0)
                    {
                        queue->m_head = newNode;
                        queue->m_tail = newNode;
                        queue->m_current = newNode;
                    }
                    else
                    {
                        if (queue->m_tail == 0 || ((QueueNode*)queue->m_tail)->next != 0)
                        {
                            ShowError("queue fault 0113");
                        }
                        newNode->next = 0;
                        newNode->prev = (QueueNode*)queue->m_tail;
                        ((QueueNode*)queue->m_tail)->next = newNode;
                        queue->m_tail = newNode;
                    }
                    return;
                }

                queue->m_current = current->next;
            }
        }
    }
    else
    {
        queue->Insert(s);
    }
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

/* Function start: 0x41F8A0 */
int MouseControl::LBLParse(char* param_1)
{
    char local_34[32];
    Sprite* sprite;

    local_34[0] = 0;
    sscanf(param_1, " %s ", local_34);

    if (strcmp(local_34, "SPRITE") == 0)
    {
        sprite = new Sprite(0);
        Parser::ProcessFile(sprite, this, 0);
        AddSprite(sprite);
    }
    else if (strcmp(local_34, "END") == 0)
    {
        return 1;
    }
    else
    {
        Parser::LBLParse("MMPlayer");
    }
    return 0;
}
