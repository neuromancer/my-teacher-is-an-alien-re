#include "MMPlayer.h"
#include "Memory.h"
#include "Sprite.h"
#include "string.h"
#include <new.h>
#include <string.h>
#include <stdio.h>

/* Function start: 0x41F280 */
MMPlayer::MMPlayer()
{
    field_0x88 = 0;
    field_0x8c = 0;
    field_0x90 = 0;
    field_0x90 = 1;
    Queue* queue = new Queue();
    queue->type = 2;
    m_queue = queue;
    field_0x88 = 0x54;
}

/* Function start: 0x41F360 */
MMPlayer::~MMPlayer()
{
    QueueNode* node;
    QueueNode* current;
    void* sprite;
    Queue* queue;

    queue = m_queue;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                current = (QueueNode*)queue->current;
                if (current == 0) {
                    sprite = 0;
                } else {
                    if (queue->head == current) {
                        queue->head = current->next;
                    }
                    if (queue->tail == current) {
                        queue->tail = current->next;
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
                        queue->current = 0;
                    }
                    queue->current = queue->head;
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
void MMPlayer::StopAll()
{
    Queue* queue;
    QueueNode* current;
    Sprite* sprite;

    queue = m_queue;
    queue->current = queue->head;
    if (queue->head != 0) {
        do {
            current = (QueueNode*)queue->current;
            sprite = 0;
            if (current != 0) {
                sprite = (Sprite*)current->data;
            }
            sprite->StopAnimationSound();

            queue = m_queue;
            current = (QueueNode*)queue->current;
            if (queue->tail == current) {
                break;
            }
            if (current != 0) {
                queue->current = current->next;
            }
        } while (queue->head != 0);
    }
    field_0x8c = field_0x8c & ~0x2000;
}

/* Function start: 0x41F4F0 */
void MMPlayer::Init()
{
    Queue* queue;
    QueueNode* current;
    Sprite* sprite;

    queue = m_queue;
    queue->current = queue->head;
    if (queue->head != 0) {
        do {
            current = (QueueNode*)queue->current;
            sprite = 0;
            if (current != 0) {
                sprite = (Sprite*)current->data;
            }
            sprite->Init();

            queue = m_queue;
            current = (QueueNode*)queue->current;
            if (queue->tail == current) {
                break;
            }
            if (current != 0) {
                queue->current = current->next;
            }
        } while (queue->head != 0);
    }
    field_0x90 = 1;
    field_0x8c = field_0x8c | 0x2000;
}

/* Function start: 0x41F560 */
void MMPlayer::AddSprite(Sprite* s)
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

    queue->current = queue->head;

    if (queue->type == 1 || queue->type == 2)
    {
        if (queue->head == 0)
        {
            if (s == 0) ShowError("queue fault 0102");
            QueueNode* newNode = new QueueNode(s);

            if (queue->current == 0)
            {
                queue->current = queue->head;
            }

            if (queue->head == 0)
            {
                queue->head = newNode;
                queue->tail = newNode;
                queue->current = newNode;
            }
            else
            {
                newNode->next = (QueueNode*)queue->current;
                newNode->prev = ((QueueNode*)queue->current)->prev;
                if (((QueueNode*)queue->current)->prev == 0)
                {
                    queue->head = newNode;
                }
                else
                {
                    ((QueueNode*)((QueueNode*)queue->current)->prev)->next = newNode;
                }
                ((QueueNode*)queue->current)->prev = newNode;
            }
        }
        else
        {
            while (queue->current != 0)
            {
                QueueNode* current = (QueueNode*)queue->current;
                Sprite* currSprite = (Sprite*)current->data;

                if ((unsigned int)currSprite->field_0xb0 < (unsigned int)s->field_0xb0)
                {
                    if (s == 0) ShowError("queue fault 0102");
                    QueueNode* newNode = new QueueNode(s);

                    if (queue->current == 0)
                    {
                        queue->current = queue->head;
                    }

                    if (queue->head == 0)
                    {
                        queue->head = newNode;
                        queue->tail = newNode;
                        queue->current = newNode;
                    }
                    else
                    {
                        newNode->next = (QueueNode*)queue->current;
                        newNode->prev = ((QueueNode*)queue->current)->prev;
                        if (((QueueNode*)queue->current)->prev == 0)
                        {
                            queue->head = newNode;
                        }
                        else
                        {
                            ((QueueNode*)((QueueNode*)queue->current)->prev)->next = newNode;
                        }
                        ((QueueNode*)queue->current)->prev = newNode;
                    }
                    return;
                }

                if (queue->tail == current)
                {
                    if (s == 0) ShowError("queue fault 0112");
                    QueueNode* newNode = new QueueNode(s);

                    if (queue->current == 0)
                    {
                        queue->current = queue->tail;
                    }

                    if (queue->head == 0)
                    {
                        queue->head = newNode;
                        queue->tail = newNode;
                        queue->current = newNode;
                    }
                    else
                    {
                        if (queue->tail == 0 || ((QueueNode*)queue->tail)->next != 0)
                        {
                            ShowError("queue fault 0113");
                        }
                        newNode->next = 0;
                        newNode->prev = (QueueNode*)queue->tail;
                        ((QueueNode*)queue->tail)->next = newNode;
                        queue->tail = newNode;
                    }
                    return;
                }

                queue->current = current->next;
            }
        }
    }
    else
    {
        queue->Insert(s);
    }
}

/* Function start: 0x41F800 */
int MMPlayer::Draw()
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
    queue->current = queue->head;
    if (queue->head != 0) {
        do {
            current = (QueueNode*)queue->current;
            sprite = 0;
            if (current != 0) {
                sprite = (Sprite*)current->data;
            }
            result = sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
            if (result != 0) {
                field_0x90 = 0;
            }

            queue = m_queue;
            current = (QueueNode*)queue->current;
            if (queue->tail == current) {
                break;
            }
            if (current != 0) {
                queue->current = current->next;
            }
        } while (queue->head != 0);
    }
    return field_0x90;
}

/* Function start: 0x41F8A0 */
int MMPlayer::LBLParse(char* param_1)
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
