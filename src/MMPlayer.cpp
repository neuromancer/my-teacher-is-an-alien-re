#include "MMPlayer.h"
#include "Memory.h"
#include "Sprite.h"
#include "string.h"
#include "GameEngine.h"
#include "globals.h"
#include <new.h>
#include <string.h>
#include <stdio.h>

/* Function start: 0x4438A0 */
MMPlayer::MMPlayer()
{
    int* ptr = &m_frameTime;
    ptr[0] = 0;
    ptr[1] = 0;
    ptr[2] = 0;
    ptr[3] = 0;
    m_drawResult = 1;
    m_queue = new ZBQueue(2);
    m_frameTime = g_GameEngine_0046a6ec->m_frameTime;
}

/* Function start: 0x443990 */
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

/* Function start: 0x443AB0 */
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
    m_flags = m_flags & ~0x2000;
}

/* Function start: 0x443B20 */
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
            sprite->InitAnimation();

            if (m_queue->tail == m_queue->current) {
                break;
            }
            if (m_queue->current != 0) {
                m_queue->current = m_queue->current->next;
            }
        } while (m_queue->head != 0);
    }
    m_drawResult = 1;
    m_flags = m_flags | 0x2000;
}

/* Function start: 0x443B90 */
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
                if ((unsigned int)((Sprite*)queue->current->data)->handleValue < (unsigned int)s->handleValue) {
                    if (s == 0) ShowError("queue fault 0102");
                    ListNode* newNode = new ListNode(s);
                    if (queue->current == 0) queue->current = queue->head;
                    if (queue->head == 0) {
                        queue->head = newNode;
                        queue->tail = newNode;
                        queue->current = newNode;
                    } else {
                        newNode->next = queue->current;
                        newNode->prev = queue->current->prev;
                        if (queue->current->prev != 0) {
                            queue->current->prev->next = newNode;
                            queue->current->prev = newNode;
                        } else {
                            queue->head = newNode;
                            queue->current->prev = newNode;
                        }
                    }
                    return;
                }
                if (queue->tail == queue->current) {
                    if (s == 0) ShowError("queue fault 0112");
                    ListNode* newNode = new ListNode(s);
                    if (queue->current == 0) queue->current = queue->tail;
                    if (queue->head == 0) {
                        queue->head = newNode;
                        queue->tail = newNode;
                        queue->current = newNode;
                    } else {
                        if (queue->tail == 0 || queue->tail->next != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->next = 0;
                        newNode->prev = queue->tail;
                        queue->tail->next = newNode;
                        queue->tail = newNode;
                    }
                    return;
                }
                if (queue->current != 0) {
                    queue->current = queue->current->next;
                }
            } while (queue->current != 0);
        } else {
            if (s == 0) ShowError("queue fault 0102");
            ListNode* newNode = new ListNode(s);
            if (queue->current == 0) queue->current = queue->head;
            if (queue->head == 0) {
                queue->head = newNode;
                queue->tail = newNode;
                queue->current = newNode;
            } else {
                newNode->next = queue->current;
                newNode->prev = queue->current->prev;
                if (queue->current->prev != 0) {
                    queue->current->prev->next = newNode;
                    queue->current->prev = newNode;
                } else {
                    queue->head = newNode;
                    queue->current->prev = newNode;
                }
            }
        }
    } else {
        queue->InsertBeforeCurrent(s);
    }
}

/* Function start: 0x443E30 */
int MMPlayer::Draw()
{
    Sprite* sprite;

    m_drawResult = 1;
    if ((m_flags & 0x2000) == 0) {
        Init();
    }
    m_queue->current = m_queue->head;
    if (m_queue->head != 0) {
        do {
            sprite = 0;
            if (m_queue->current != 0) {
                sprite = (Sprite*)m_queue->current->data;
            }
            if (sprite->Do(sprite->loc.x, sprite->loc.y, 1.0) != 0) {
                m_drawResult = 0;
            }

            if (m_queue->tail == m_queue->current) {
                break;
            }
            if (m_queue->current != 0) {
                m_queue->current = m_queue->current->next;
            }
        } while (m_queue->head != 0);
    }
    return m_drawResult;
}

/* Function start: 0x443ED0 */
void MMPlayer::ResetAnimations(int param_1)
{
    Sprite* sprite;

    m_queue->current = m_queue->head;
    if (m_queue->head != 0) {
        do {
            sprite = 0;
            if (m_queue->current != 0) {
                sprite = (Sprite*)m_queue->current->data;
            }
            sprite->ResetAnimation(param_1, 0);

            if (m_queue->tail == m_queue->current) {
                break;
            }
            if (m_queue->current != 0) {
                m_queue->current = m_queue->current->next;
            }
        } while (m_queue->head != 0);
    }
}

/* Function start: 0x443F40 */
int MMPlayer::LBLParse(char* param_1)
{
    char local_34[32];
    Sprite* sprite;
    char local_b0[128];

    sscanf(param_1, " %s ", local_34);

    if (strcmp(local_34, "SPRITE") == 0)
    {
        sprite = new Sprite(0);
        Parser::ProcessFile(sprite, this, 0);
        AddSprite(sprite);
    }
    else if (strncmp(local_34, "SPR", 3) == 0)
    {
        if (sscanf(param_1, " %s %s ", local_34, local_b0) < 2) {
            Parser::ReportUnknownLabel("MMPlayer");
        }
        sprite = new Sprite(local_b0);
        sprite->LBLParse(param_1);
        AddSprite(sprite);
    }
    else if (strcmp(local_34, "END") == 0)
    {
        return 1;
    }
    else
    {
        Parser::ReportUnknownLabel("MMPlayer");
    }
    return 0;
}
