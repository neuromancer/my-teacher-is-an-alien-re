#include "ZBufferManager.h"
#include "VBuffer.h"
#include "Timer.h"
#include "ZBuffer.h"
#include "Palette.h"
#include "Memory.h"
#include "Animation.h"
#include "GlyphRect.h"
#include "RenderEntry.h"
#include "SoundCommand.h"
#include "string.h"
#include "globals.h"
#include <string.h>
#include <stdlib.h>

#include "VideoTable.h"
#include "AnimatedAsset.h"

void __stdcall DrawScaledSprite(int x, int y, void* data, double scale);


struct CommandType1 : public SoundCommand {
    unsigned int parameter1; // 0x04 - Priority
    void* data;              // 0x08
    int x;                   // 0x0c
    int y;                   // 0x10
    int mode;                // 0x14
    double scale;            // 0x18

    CommandType1() : parameter1(0), data(0), x(0), y(0), mode(0), scale(0) {}

    virtual void Execute(GlyphRect* rect);
};

struct CommandType2 : public SoundCommand {
    unsigned int parameter1; // 0x04 - priority field
    char* text;              // 0x08
    int x;                   // 0x0c
    int y;                   // 0x10

    CommandType2() : parameter1(0), text(0), x(0), y(0) {}

    virtual void Execute(GlyphRect* rect);
};



extern "C" void FlipScreen();
extern VBuffer* g_WorkBuffer_00436974;

// Functions for rectangle drawing
extern "C" int __cdecl SetFillColor(unsigned char color);
extern "C" int __cdecl DrawRectOutline(int left, int right, int top, int bottom);
extern "C" int __cdecl VideoFillRect(int left, int right, int top, int bottom);

struct CommandType3 : public SoundCommand {
    unsigned int priority;  // 0x04
    int left;               // 0x08
    int top;                // 0x0c
    int right;              // 0x10
    int bottom;             // 0x14
    int field_18;           // 0x18
    int field_1c;           // 0x1c - color

    CommandType3() : priority(0), left(0), top(0), right(0), bottom(0), field_18(0), field_1c(0) {}

    virtual void Execute(GlyphRect* rect);
};

/* Function start: 0x409160 */
// void ZBQueue::Insert(void* data) - Moved to LinkedList.h as inline

/* Function start: 0x41BB20 */
void ZBufferManager::QueueAnimationCleanup(void* anim)
{
    ZBQueueNode* node;
    ZBQueueNode* newNode;
    ZBQueue* queue;
    int qtype;

    if (m_state == 1) {
        if (anim != 0) {
            delete (Animation*)anim;
        }
        return;
    }
    if (m_state < 2 || m_state > 3) {
        return;
    }

    if (anim == 0) {
        return;
    }

    node = new ZBQueueNode(anim);

    queue = m_queueA4;
    queue->ResetForSortedAdd(node);

    if (queue->type == 1 || queue->type == 2) {
        if (queue->head != 0) {
            while (1) {
                if (*(unsigned int*)queue->current->data < *(unsigned int*)node) {
                    queue->Insert(node);
                    return;
                }
                if (queue->tail == queue->current) break;
                queue->current = queue->current->next;
            }
        }
        queue->Push(node);
    } else {
        queue->Insert(node);
    }
}

/* Function start: 0x41B5D0 */
void CommandType1::Execute(GlyphRect* rect)
{
    VBuffer* vbuf;

    //ShowMessage("CommandType1::Execute mode=%d data=%x", mode, (int)data);
    switch(mode) {
    case 0:
        vbuf = (VBuffer*)data;
        g_WorkBuffer_00436974->ClipAndBlit(vbuf->clip_x1, vbuf->clip_x2, vbuf->clip_y1, vbuf->clip_y2, x, y, (int)vbuf);
        return;
    case 1:
        vbuf = (VBuffer*)data;
        g_WorkBuffer_00436974->ClipAndPaste(vbuf->clip_x1, vbuf->clip_x2, vbuf->clip_y1, vbuf->clip_y2, x, y, (int)vbuf);
        return;
    case 2:
        DrawScaledSprite(x, y, data, scale);
        return;
    case 3:
        g_WorkBuffer_00436974->ScaleTCCopy(x, y, (VBuffer*)data, scale);
    }
    return;
}

/* Function start: 0x41B690 */
void CommandType2::Execute(GlyphRect* rect)
{
    SetDrawPosition(x, y);
    if (g_TextManager_00436990 != 0) {
        g_TextManager_00436990->RenderText(text, -1);
    }
    return;
}

/* Function start: 0x41B6D0 */
void CommandType3::Execute(GlyphRect* rect)
{
    SetFillColor((unsigned char)field_1c);
    g_WorkBuffer_00436974->SetVideoMode();
    if (left < 0) {
        left = 0;
    }
    if (top < 0) {
        top = 0;
    }
    if (0x27f < right) {
        right = 0x27f;
    }
    if (0x1df < bottom) {
        bottom = 0x1df;
    }
    if (field_18 == 1) {
        DrawRectOutline(left, right, top, bottom);
        return;
    }
    VideoFillRect(left, right, top, bottom);
    return;
}

/* Function start: 0x41B760 */
ZBufferManager::ZBufferManager() {
    WriteToMessageLogIfEnabled(L"declaring ZBuffer");
    memset(this, 0, 0x2b * 4);

    m_queueA0 = new ZBQueue(2);
    m_queueA4 = new ZBQueue(0);
    m_queue9c = new ZBQueue(1);

    m_state = 2;
}

/* Function start: 0x41B8E0 */
void ZBufferManager::Cleanup() {
    ZBQueue* queue;
    ZBQueueNode* node;
    ZBQueueNode* nextNode;
    ZBQueueNode* prevNode;
    void* data;
    int queueType;
    ZBQueue* local_14;
    void* local_18;

    // First: drain queue at 0xA0 with special handling
    queue = m_queueA0;
    while (queue->head != 0) {
        queue = m_queueA0;
        queueType = queue->type;
        if (queueType == 1 || queueType == 4) {
            queue->current = queue->head;
        } else if (queueType == 2 || queueType == 0) {
            queue->current = queue->tail;
        } else {
            ShowError("bad queue type %lu", queueType);
        }

        node = (ZBQueueNode*)queue->current;
        if (node != 0) {
            if (queue->head == node) {
                queue->head = node->next;
            }
            node = (ZBQueueNode*)queue->current;
            if (queue->tail == node) {
                queue->tail = node->prev;
            }
            node = (ZBQueueNode*)queue->current;
            nextNode = node->next;
            if (nextNode != 0) {
                nextNode->prev = node->prev;
            }
            node = (ZBQueueNode*)queue->current;
            prevNode = node->prev;
            if (prevNode != 0) {
                prevNode->next = node->next;
            }
            queue->GetCurrentData();
            node = (ZBQueueNode*)queue->current;
            if (node != 0) {
                delete node;
                queue->current = 0;
            }
            queue->current = queue->head;
        }
        queue = m_queueA0;
    }

    // Second: drain queue at 0xA0 (again), freeing remaining items
    queue = m_queueA0;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = queue->Pop();
                if (data != 0) {
                    delete (SoundCommand*)data;
                }
            }
        }
        delete queue;
        m_queueA0 = 0;
    }

    // Third: drain queue at 0xA4
    queue = m_queueA4;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = queue->Pop();
                if (data != 0) {
                    ((ZBuffer*)data)->CleanUpVBuffer();
                    delete data;
                }
            }
        }
        delete queue;
        m_queueA4 = 0;
    }

    // Fourth: drain queue at 0x9C
    local_14 = m_queue9c;
    if (local_14 != 0) {
        if (local_14->head != 0) {
            local_14->current = local_14->head;
            while (local_14->head != 0) {
                local_18 = local_14->Pop();
                if (local_18 != 0) {
                    delete local_18;
                }
            }
        }
        delete local_14;
        m_queue9c = 0;
    }
}

ZBufferManager::~ZBufferManager()
{
    Cleanup();
}

/* Function start: 0x41BB10 */
// void* ZBQueue::GetCurrentData() - Moved to LinkedList.h as inline

/* Function start: 0x41BE20 */
void ZBufferManager::PlayAnimationSound(void* data, int priority, int x, int y, int mode, double scale)
{
    if (m_state != 0) {
        if (m_state != 1) {
            // Queue logic
            CommandType1* cmd = new CommandType1();
            if (!cmd) return;

            cmd->data = data;
            cmd->parameter1 = priority;

            // Apply jitter if flags & 2
            if ((m_flags & 2) != 0) {
                 x = rand() % 5 - 2 + x;
                 y = rand() % 5 - 2 + y;
            }

            cmd->x = x;
            cmd->y = y;
            cmd->mode = mode;
            cmd->scale = scale;

            QueueCommand(cmd);
        }
        else {
            VBuffer* vbuf = (VBuffer*)data;
            switch(mode) {
                case 0:
                     g_WorkBuffer_00436974->ClipAndBlit(vbuf->clip_x1, vbuf->clip_x2, vbuf->clip_y1, vbuf->clip_y2, x, y, (int)vbuf);
                     break;
                case 1:
                     g_WorkBuffer_00436974->ClipAndPaste(vbuf->clip_x1, vbuf->clip_x2, vbuf->clip_y1, vbuf->clip_y2, x, y, (int)vbuf);
                     break;
                case 2:
                     DrawScaledSprite(x, y, data, scale);
                     break;
                case 3:
                     g_WorkBuffer_00436974->ScaleTCCopy(x, y, vbuf, scale);
                     break;
            }
        }
    }
}

/* Function start: 0x41C000 */
void ZBufferManager::ShowSubtitle(char* text, int x, int y, int duration, int flag)
{
    CommandType2* cmd;

    if (m_state == 0) {
        return;
    }
    if (m_state != 1) {
        cmd = new CommandType2();
        cmd->text = new char[strlen(text) + 1];
        strcpy(cmd->text, text);
        cmd->parameter1 = duration;
        cmd->x = x;
        cmd->y = y;
        QueueCommand(cmd);
        return;
    }
    SetDrawPosition(x, y);
    g_TextManager_00436990->RenderText(text, -1);
}

/* Function start: 0x41C130 */
void ZBufferManager::DrawRect(int p1, int p2, int p3, int p4, int p5, int p6, int p7)
{
    if (m_state != 0) {
        if (m_state != 1) {
            // Queued rendering path
            CommandType3* cmd = new CommandType3();
            cmd->priority = p5;
            cmd->field_1c = p6;
            cmd->field_18 = p7;
            cmd->left = p1;
            cmd->top = p2;
            cmd->right = p3;
            cmd->bottom = p4;

            int rig = cmd->right;
            int lef = cmd->left;
            if (lef > rig) {
                cmd->left = rig;
                cmd->right = lef;
            }
            int bot = cmd->bottom;
            int tp = cmd->top;
            if (bot < tp) {
                cmd->top = bot;
                cmd->bottom = tp;
            }

            QueueCommand(cmd);
        }
        else {
            // Direct rendering path (m_state == 1)
            SetFillColor(p6);
            g_WorkBuffer_00436974->SetVideoMode();
            if (p1 < 0) {
                p1 = 0;
            }
            if (p2 < 0) {
                p2 = 0;
            }
            if (p3 > 0x27f) {
                p3 = 0x27f;
            }
            if (p4 > 0x1df) {
                p4 = 0x1df;
            }
            if (m_state == 1) {
                DrawRectOutline(p1, p3, p2, p4);
            } else {
                VideoFillRect(p1, p3, p2, p4);
            }
        }
    }
}

/* Function start: 0x41C2C0 */
void ZBufferManager::QueueCommand(SoundCommand* cmd)
{
    ZBQueue* queue;
    ZBQueueNode* node;
    ZBQueueNode* newNode;
    GlyphRect rect(0, 0, g_WorkBuffer_00436974->width - 1, g_WorkBuffer_00436974->height - 1);

    switch (m_state) {
    case 1:
        cmd->Execute(&rect);
        if (cmd != 0) {
            delete cmd;
        }
        return;
    case 2:
        queue = m_queueA0;
        if (cmd == 0) {
            ShowError("queue fault 0101");
        }
        queue->current = queue->head;
        if (queue->type == 1 || queue->type == 2) {
            if (queue->current == 0) {
                queue->InsertBeforeCurrent(cmd);
                return;
            }
            do {
                node = (ZBQueueNode*)queue->current;
                if (((CommandType1*)node->data)->parameter1 < ((CommandType1*)cmd)->parameter1) {
                    queue->InsertBeforeCurrent(cmd);
                    return;
                }
                if (queue->tail == node) {
                    if (cmd == 0) {
                        ShowError("queue fault 0112");
                    }
                    newNode = (ZBQueueNode*)::operator new(sizeof(ZBQueueNode));
                    if (newNode != 0) {
                        newNode = (ZBQueueNode*)newNode->Init(cmd);
                    }
                    if (queue->current == 0) {
                        queue->current = queue->tail;
                    }
                    if (queue->head == 0) {
                        queue->head = newNode;
                        queue->tail = newNode;
                        queue->current = newNode;
                    } else {
                        ZBQueueNode* tail = queue->tail;
                        if (tail == 0 || tail->next != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->next = 0;
                        newNode->prev = tail;
                        tail->next = newNode;
                        queue->tail = newNode;
                    }
                    return;
                }
                if (node != 0) {
                    queue->current = node->next;
                }
            } while (queue->current != 0);
            return;
        }
        queue->InsertBeforeCurrent(cmd);
        return;
    case 3:
        queue = m_queueA0;
        if (cmd == 0) {
            ShowError("queue fault 0101");
        }
        queue->current = queue->head;
        if (queue->type == 1 || queue->type == 2) {
            if (queue->current == 0) {
                queue->InsertBeforeCurrent(cmd);
                return;
            }
            do {
                node = (ZBQueueNode*)queue->current;
                if (((CommandType1*)node->data)->parameter1 < ((CommandType1*)cmd)->parameter1) {
                    queue->InsertBeforeCurrent(cmd);
                    return;
                }
                if (queue->tail == node) {
                    if (cmd == 0) {
                        ShowError("queue fault 0112");
                    }
                    newNode = (ZBQueueNode*)::operator new(sizeof(ZBQueueNode));
                    if (newNode != 0) {
                        newNode = (ZBQueueNode*)newNode->Init(cmd);
                    }
                    if (queue->current == 0) {
                        queue->current = queue->tail;
                    }
                    if (queue->head == 0) {
                        queue->head = newNode;
                        queue->tail = newNode;
                        queue->current = newNode;
                    } else {
                        ZBQueueNode* tail = queue->tail;
                        if (tail == 0 || tail->next != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->next = 0;
                        newNode->prev = tail;
                        tail->next = newNode;
                        queue->tail = newNode;
                    }
                    return;
                }
                if (node != 0) {
                    queue->current = node->next;
                }
            } while (queue->current != 0);
            return;
        }
        queue->InsertBeforeCurrent(cmd);
        return;
    }
}

/* Function start: 0x41C5A0 */
void ZBufferManager::ProcessRenderQueues()
{
    ZBQueue* queue;
    ZBQueueNode* node;
    ZBQueueNode* nextNode;
    ZBQueueNode* prevNode;
    void* data;
    unsigned int elapsed;
    int queueType;
    void* local_14;

    m_head = 0;

    switch (m_state) {
    case 2:
    {
        // State type 2 processing
        if (m_palette != 0) {
            if (m_palette->IsPaletteUnchanged() == 0) {
                BlankScreen();
                m_palette->SetPalette(0, 0x100);
            }
            m_palette = 0;
        }

        {
            GlyphRect rect;
            rect.left = 0;
            rect.top = 0;
            rect.right = g_WorkBuffer_00436974->width - 1;
            rect.bottom = g_WorkBuffer_00436974->height - 1;

            while (m_queueA0->head != 0) {
                data = 0;
                queue = m_queueA0;
                queueType = queue->type;
                if (queueType == 1 || queueType == 4) {
                    queue->current = queue->head;
                } else if (queueType == 2 || queueType == 0) {
                    queue->current = queue->tail;
                } else {
                    ShowError("bad queue type %lu", queueType);
                }

                if (queue->current != 0) {
                    data = queue->Pop();
                }

                if (data != 0) {
                    ((SoundCommand*)data)->Execute(&rect);

                    if (data != 0) {
                        ((SoundCommand*)data)->~SoundCommand();
                        delete data;
                    }
                }
            }
        }

        // Process queue at 0xa4
        queue = m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = queue->Pop();
                if (data != 0) {
                    ((ZBuffer*)data)->CleanUpVBuffer();
                    delete data;
                }
            }
        }

        if ((m_flags & 2) != 0) {
            elapsed = timer.Update();
            if (elapsed > 2000) {
                m_flags = m_flags & 0xfffffffd;
            }
        }
        break;
    }
    case 3:
    {
        // State type 3 processing
        if (m_palette != 0) {
            if (m_palette->IsPaletteUnchanged() == 0) {
                BlankScreen();
                m_palette->SetPalette(0, 0x100);
            }
            m_palette = 0;
        }

        while (m_queue9c->head != 0) {
            local_14 = 0;
            queue = m_queue9c;
            queueType = queue->type;
            if (queueType == 1 || queueType == 4) {
                queue->current = queue->head;
            } else if (queueType == 2 || queueType == 0) {
                queue->current = queue->tail;
            } else {
                ShowError("bad queue type %lu", queueType);
            }

            node = (ZBQueueNode*)queue->current;
            if (node != 0) {
                // Unlink node from double linked list
                if (queue->head == node) {
                    queue->head = node->next;
                }
                node = (ZBQueueNode*)queue->current;
                if (queue->tail == node) {
                    queue->tail = node->prev;
                }
                node = (ZBQueueNode*)queue->current;
                nextNode = (ZBQueueNode*)node->next;
                if (nextNode != 0) {
                    nextNode->prev = node->prev;
                }
                node = (ZBQueueNode*)queue->current;
                prevNode = (ZBQueueNode*)node->prev;
                if (prevNode != 0) {
                    prevNode->next = node->next;
                }

                node = (ZBQueueNode*)queue->current;
                if (node == 0) {
                    data = 0;
                } else {
                    data = node->data;
                    delete node;
                    queue->current = 0;
                }
                queue->current = queue->head;
                local_14 = data;
            }

            // Check queue at 0xa0
            queue = m_queueA0;
            if (queue == 0 || queue->head == 0) {
                break;
            }
            queue->current = queue->head;

            if (m_queueA0->head != 0) {
                GlyphRect* iterPtr = (GlyphRect*)((unsigned int*)local_14 + 1);
                do {
                    SoundCommand* cmd = 0;
                    node = (ZBQueueNode*)m_queueA0->current;
                    if (node != 0) {
                        cmd = (SoundCommand*)node->data;
                    }
                    cmd->Execute(iterPtr);

                    queue = m_queueA0;
                    node = (ZBQueueNode*)queue->current;
                    if (queue->tail == node) {
                        break;
                    }
                    if (node != 0) {
                        queue->current = node->next;
                    }
                } while (m_queueA0->head != 0);
            }
            if (local_14 != 0) {
                ((RenderEntry*)local_14)->RenderEntry::~RenderEntry();
                delete local_14;
            }
        }

        // Process queue at 0xa4
        queue = m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = queue->Pop();
                if (data != 0) {
                    ((ZBuffer*)data)->CleanUpVBuffer();
                    delete data;
                }
            }
        }

        if ((m_flags & 2) != 0) {
            elapsed = timer.Update();
            if (elapsed > 2000) {
                m_flags = m_flags & 0xfffffffd;
            }
        }
        break;
    }
    }
}

/* Function start: 0x41C960 */
void ZBufferManager::UpdateScreen() {
    ZBQueue* queue;
    ZBQueueNode* node;
    int queueType;
    RenderEntry* local_10;

    switch (m_state) {
    case 1:
        FlipScreen();
        return;
    case 2:
        FlipScreen();
        return;
    case 3:
        break;
    default:
        return;
    }

    queue = m_queue9c;
    if (queue->head == 0) {
        return;
    }

    do {
        local_10 = 0;
        queue = m_queue9c;
        queueType = queue->type;
        if (queueType == 1 || queueType == 4) {
            queue->current = queue->head;
        } else if (queueType == 2 || queueType == 0) {
            queue->current = queue->tail;
        } else {
            ShowError("bad queue type %lu", queueType);
        }

        node = (ZBQueueNode*)queue->current;
        if (node != 0) {
            if (queue->head == node) {
                queue->head = node->next;
            }
            node = (ZBQueueNode*)queue->current;
            if (queue->tail == node) {
                queue->tail = node->prev;
            }
            node = (ZBQueueNode*)queue->current;
            if (node->prev != 0) {
                node->prev->next = node->next;
            }
            node = (ZBQueueNode*)queue->current;
            if (node->next != 0) {
                node->next->prev = node->prev;
            }

            node = (ZBQueueNode*)queue->current;
            if (node != 0) {
                local_10 = (RenderEntry*)node->data;
            }
            if (node != 0) {
                delete node;
                queue->current = 0;
            }
            queue->current = queue->head;
        }

        {
            GlyphRect local_rect = local_10->rect;
            if (g_WorkBuffer_00436974 != 0) {
                g_WorkBuffer_00436974->CallBlitter4(local_rect.left, local_rect.right, local_rect.top, local_rect.bottom, local_rect.left, local_rect.right);
            }

            if (local_10 != 0) {
                local_10->RenderEntry::~RenderEntry();
                delete (void*)local_10;
            }
        }

        queue = m_queue9c;
    } while (queue->head != 0);
}

/* Function start: 0x41CB40 */
void ZBQueue::InsertBeforeCurrent(void* data)
{
    LinkedList::InsertNode(data);
}

/* Function start: 0x41CC10 */
// Same pattern as 0x41CB40 - InsertNode COMDAT

