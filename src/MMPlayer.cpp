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
    int* ptr = &field_0x88;
    ptr[0] = 0;
    ptr[1] = 0;
    ptr[2] = 0;
    ptr[3] = 0;
    field_0x90 = 1;
    m_queue = new ZBQueue(2);
    field_0x88 = 0x54;
}

/* Function start: 0x41F360 */
MMPlayer::~MMPlayer()
{
    Sprite* sprite;
    ZBQueue* queue;

    queue = m_queue;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                sprite = (Sprite*)queue->RemoveCurrent();
                if (sprite != 0) {
                    delete sprite;
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
    Sprite* sprite;

    m_queue->current = m_queue->head;
    if (m_queue->head != 0) {
        do {
            sprite = 0;
            if (m_queue->current != 0) {
                sprite = (Sprite*)m_queue->current->data;
            }
            sprite->StopAnimationSound();

            if (m_queue->tail == m_queue->current) {
                break;
            }
            if (m_queue->current != 0) {
                m_queue->current = m_queue->current->next;
            }
        } while (m_queue->head != 0);
    }
    field_0x8c = field_0x8c & ~0x2000;
}

/* Function start: 0x41F4F0 */
void MMPlayer::Init()
{
    Sprite* sprite;

    m_queue->current = m_queue->head;
    if (m_queue->head != 0) {
        do {
            sprite = 0;
            if (m_queue->current != 0) {
                sprite = (Sprite*)m_queue->current->data;
            }
            sprite->Init();

            if (m_queue->tail == m_queue->current) {
                break;
            }
            if (m_queue->current != 0) {
                m_queue->current = m_queue->current->next;
            }
        } while (m_queue->head != 0);
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
    ZBQueue* queue = m_queue;
    queue->ResetForSortedAdd(s);

    if (queue->type == 1 || queue->type == 2) {
        if (queue->head != 0) {
            do {
                if (((Sprite*)queue->current->data)->field_0xb0 < s->field_0xb0) {
                    queue->InsertNode(s);
                    return;
                }
                if (queue->tail == queue->current) {
                    queue->PushNode(s);
                    return;
                }
                if (queue->current != 0) {
                    queue->current = queue->current->next;
                }
            } while (queue->current != 0);
        } else {
            queue->InsertNode(s);
        }
    } else {
        queue->InsertBeforeCurrent(s);
    }
}

/* Function start: 0x41F800 */
int MMPlayer::Draw()
{
    Sprite* sprite;

    field_0x90 = 1;
    if ((field_0x8c & 0x2000) == 0) {
        Init();
    }
    m_queue->current = m_queue->head;
    if (m_queue->head != 0) {
        do {
            sprite = 0;
            if (m_queue->current != 0) {
                sprite = (Sprite*)m_queue->current->data;
            }
            if (sprite->Do(sprite->loc_x, sprite->loc_y, 1.0) != 0) {
                field_0x90 = 0;
            }

            if (m_queue->tail == m_queue->current) {
                break;
            }
            if (m_queue->current != 0) {
                m_queue->current = m_queue->current->next;
            }
        } while (m_queue->head != 0);
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
