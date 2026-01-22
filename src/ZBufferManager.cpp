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
    int scale_low;           // 0x18
    int scale_high;          // 0x1c

    CommandType1() : parameter1(0), data(0), x(0), y(0), mode(0), scale_low(0), scale_high(0) {}

    virtual void Execute(GlyphRect* rect);
};

struct CommandType2 : public SoundCommand {
    unsigned int parameter1; // 0x04 - priority field
    char* text;              // 0x08
    int x;                   // 0x0c
    int y;                   // 0x10

    virtual void Execute(GlyphRect* rect);
};

// Stub - checks if palette has changed from the stored one
// Returns 0 if palette changed, non-zero if unchanged
int __fastcall IsPaletteUnchanged(void* storedPalette) { return 0; }



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



ZBufferManager::~ZBufferManager()
{
    Cleanup();
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
        DrawScaledSprite(x, y, data, *(double*)&scale_low);
        return;
    case 3:
        g_WorkBuffer_00436974->ScaleTCCopy(x, y, (VBuffer*)data, *(double*)&scale_low);
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
    ZBQueue* puVar1;
    ZBQueue* puVar2;
    
    WriteToMessageLogIfEnabled(L"declaring ZBuffer");
    memset(this, 0, 0x2b * 4);
    
    puVar1 = (ZBQueue*)operator new(sizeof(ZBQueue));
    puVar2 = 0;
    if (puVar1 != 0) {
        puVar1->type = 2;
        puVar1->head = 0;
        puVar1->tail = 0;
        puVar1->current = puVar1->head;
        puVar2 = puVar1;
    }
    m_queueA0 = puVar2;
    
    puVar1 = (ZBQueue*)operator new(sizeof(ZBQueue));
    puVar2 = 0;
    if (puVar1 != 0) {
        puVar1->type = 0;
        puVar1->head = 0;
        puVar1->tail = 0;
        puVar1->current = puVar1->head;
        puVar2 = puVar1;
    }
    m_queueA4 = puVar2;
    
    puVar1 = (ZBQueue*)operator new(sizeof(ZBQueue));
    puVar2 = 0;
    if (puVar1 != 0) {
        puVar1->type = 1;
        puVar1->head = 0;
        puVar1->tail = 0;
        puVar1->current = puVar1->head;
        puVar2 = puVar1;
    }
    m_queue9c = puVar2;
    
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
                queue->head = node->prev;
            }
            node = (ZBQueueNode*)queue->current;
            if (queue->tail == node) {
                queue->tail = node->next;
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
                node->CleanupNode(1);
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
                data = ZBuffer::PopNode(queue);
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
                data = ZBuffer::PopNode_2(queue);
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
                local_18 = local_14->PopNode();
                if (local_18 != 0) {
                    delete local_18;
                }
            }
        }
        delete local_14;
        m_queue9c = 0;
    }
}

/* Function start: 0x41BB10 */
void* ZBQueue::GetCurrentData()
{
    ZBQueueNode* node = (ZBQueueNode*)current;
    if (node != 0) {
        return node->data;
    }
    return 0;
}

/* Function start: 0x41BE20 */
void ZBufferManager::PlayAnimationSound(void* data, int priority, int x, int y, int mode, int scale1, int scale2)
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
            cmd->scale_high = scale2;
            cmd->scale_low = scale1;
            
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
                     DrawScaledSprite(x, y, data, *(double*)&scale1);
                     break;
                case 3:
                     g_WorkBuffer_00436974->ScaleTCCopy(x, y, vbuf, *(double*)&scale1);
                     break;
            }
        }
    }
}

/* Function start: 0x41C000 */
void ZBufferManager::ShowSubtitle(char* text, int x, int y, int duration, int flag)
{
    CommandType2* cmd;
    char* newText;
    int len;

    if (m_state == 0) {
        return;
    }
    if (m_state == 1) {
        SetDrawPosition(x, y);
        g_TextManager_00436990->RenderText(text, -1);
        return;
    }

    cmd = new CommandType2();
    if (cmd != 0) {
        cmd->parameter1 = 0;
        cmd->text = 0;
        cmd->x = 0;
        cmd->y = 0;

        len = strlen(text) + 1;
        newText = (char*)operator new(len);
        cmd->text = newText;
        if (newText != 0) {
            strcpy(newText, text);
        }
        cmd->parameter1 = duration;
        cmd->x = x;
        cmd->y = y;
        QueueCommand(cmd);
    }
}

/* Function start: 0x41C130 */
void ZBufferManager::DrawRect(int p1, int p2, int p3, int p4, int p5, int p6, int p7)
{
    if (m_state != 0) {
        if (m_state != 1) {
            // Queued rendering path
            CommandType3* cmd = new CommandType3();
            if (cmd == 0) {
                cmd = 0;
            }
            cmd->priority = p5;
            cmd->field_1c = p6;
            cmd->field_18 = p7;
            cmd->left = p1;
            cmd->top = p2;
            cmd->right = p3;
            cmd->bottom = p4;

            int temp;
            if (cmd->right < cmd->left) {
                temp = cmd->left;
                cmd->left = cmd->right;
                cmd->right = temp;
            }
            if (cmd->bottom < cmd->top) {
                temp = cmd->top;
                cmd->top = cmd->bottom;
                cmd->bottom = temp;
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
    int qtype;
    GlyphRect rect;

    rect.bottom = g_WorkBuffer_00436974->height - 1;
    rect.right = g_WorkBuffer_00436974->width - 1;
    rect.left = 0;
    rect.top = 0;

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
        qtype = queue->type;
        if (qtype == 1 || qtype == 2) {
            if (queue->head == 0) {
                queue->InsertBeforeCurrent(cmd);
                return;
            }
            while (queue->current != 0) {
                node = (ZBQueueNode*)queue->current;
                if (*(unsigned int*)((char*)node->data + 4) < ((CommandType1*)cmd)->parameter1) {
                    queue->InsertBeforeCurrent(cmd);
                    return;
                }
                if (queue->tail == node) {
                    if (cmd == 0) {
                        ShowError("queue fault 0112");
                    }
                    newNode = new ZBQueueNode();
                    if (newNode != 0) {
                        newNode->data = cmd;
                    }
                    if (queue->current == 0) {
                        queue->current = queue->tail;
                    }
                    if (queue->head == 0) {
                        queue->head = newNode;
                        queue->tail = newNode;
                        queue->current = newNode;
                    } else {
                        if (queue->tail == 0 || ((ZBQueueNode*)queue->tail)->prev != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->prev = 0;
                        newNode->next = queue->tail;
                        ((ZBQueueNode*)queue->tail)->prev = newNode;
                        queue->tail = newNode;
                    }
                    return;
                }
                if (node != 0) {
                    queue->current = node->prev;
                }
            }
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
        qtype = queue->type;
        if (qtype == 1 || qtype == 2) {
            if (queue->head == 0) {
                queue->InsertBeforeCurrent(cmd);
                return;
            }
            while (queue->current != 0) {
                node = (ZBQueueNode*)queue->current;
                if (*(unsigned int*)((char*)node->data + 4) < ((CommandType1*)cmd)->parameter1) {
                    queue->InsertBeforeCurrent(cmd);
                    return;
                }
                if (queue->tail == node) {
                    if (cmd == 0) {
                        ShowError("queue fault 0112");
                    }
                    newNode = new ZBQueueNode();
                    if (newNode != 0) {
                        newNode->data = cmd;
                    }
                    if (queue->current == 0) {
                        queue->current = queue->tail;
                    }
                    if (queue->head == 0) {
                        queue->head = newNode;
                        queue->tail = newNode;
                        queue->current = newNode;
                    } else {
                        if (queue->tail == 0 || ((ZBQueueNode*)queue->tail)->prev != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->prev = 0;
                        newNode->next = queue->tail;
                        ((ZBQueueNode*)queue->tail)->prev = newNode;
                        queue->tail = newNode;
                    }
                    return;
                }
                if (node != 0) {
                    queue->current = node->prev;
                }
            }
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
    int local_height;
    int local_width;
    int local_1c;
    int local_20;
    GlyphRect rect;

    m_head = 0;

    if (m_state == 2) {
        // State type 2 processing
        if (m_fieldA8 != 0) {
            if (IsPaletteUnchanged(m_fieldA8) == 0) {
                BlankScreen();
                ((Palette*)m_fieldA8)->SetPalette(0, 0x100);
            }
            m_fieldA8 = 0;
        }

        local_height = g_WorkBuffer_00436974->height - 1;
        local_width = g_WorkBuffer_00436974->width - 1;
        local_20 = 0;
        local_1c = 0;
        rect.right = local_width;
        rect.bottom = local_height;

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
                data = ZBuffer::PopNode(queue);
            }

            if (data != 0) {
                // Virtual call - Execute on SoundCommand
                ((SoundCommand*)data)->Execute(&rect);

                if (data != 0) {
                    delete data;
                }
            }
        }

        // Process queue at 0xa4
        queue = m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = ZBuffer::PopNode_2(queue);
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
    }
    else if (m_state == 3) {
        // State type 3 processing
        if (m_fieldA8 != 0) {
            if (IsPaletteUnchanged(m_fieldA8) == 0) {
                BlankScreen();
                ((Palette*)m_fieldA8)->SetPalette(0, 0x100);
            }
            m_fieldA8 = 0;
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
                    queue->head = node->prev;
                }
                node = (ZBQueueNode*)queue->current;
                if (queue->tail == node) {
                    queue->tail = node->next;
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
                    // Virtual call - Execute on SoundCommand
                    cmd->Execute(iterPtr);

                    queue = m_queueA0;
                    node = (ZBQueueNode*)queue->current;
                    if (queue->tail == node) {
                        break;
                    }
                    if (node != 0) {
                        queue->current = node->prev;
                    }
                } while (m_queueA0->head != 0);
            }
            if (local_14 != 0) {
                delete local_14;
            }
        }

        // Process queue at 0xa4
        queue = m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = ZBuffer::PopNode_2(queue);
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
    }
}

/* Function start: 0x41C960 */
void ZBufferManager::UpdateScreen() {
    ZBQueue* queue;
    ZBQueueNode* node;
    ZBQueueNode* nextNode;
    ZBQueueNode* prevNode;
    RenderEntry* local_10;
    int queueType;

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
                queue->head = node->prev;
            }
            node = (ZBQueueNode*)queue->current;
            if (queue->tail == node) {
                queue->tail = node->next;
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
            node = (ZBQueueNode*)queue->current;
            if (node != 0) {
                local_10 = (RenderEntry*)node->data;
                node->data = 0;
                node->next = 0;
                node->prev = 0;
                operator delete(node);
                queue->current = 0;
            }
            queue->current = queue->head;
        }

        {
            GlyphRect local_rect;
            local_rect = local_10->rect;
            if (g_WorkBuffer_00436974 != 0) {
                g_WorkBuffer_00436974->CallBlitter4(local_rect.left, local_rect.right, local_rect.top, local_rect.bottom, local_rect.left, local_rect.right);
            }

            if (local_10 != 0) {
                local_10->RenderEntry::~RenderEntry();
                operator delete(local_10);
            }
        }

        queue = m_queue9c;
    } while (queue->head != 0);
}

/* Function start: 0x409160 */
void ZBQueue::Insert(void* data)
{
    ZBQueueNode* newNode;
    ZBQueueNode* node;

    if (data == 0) {
        ShowError("queue fault 0102");
    }

    newNode = (ZBQueueNode*)operator new(sizeof(ZBQueueNode));
    node = 0;
    if (newNode != 0) {
        newNode->data = data;
        newNode->next = 0;
        newNode->prev = 0;
        node = newNode;
    }

    if (current == 0) {
        current = head;
    }

    if (head == 0) {
        head = node;
        tail = node;
        current = node;
        return;
    }

    node->prev = current;
    node->next = current->next;
    if (current->next == 0) {
        head = node;
        current->next = node;
    } else {
        current->next->prev = node;
        current->next = node;
    }
}

/* Function start: 0x41CB40 */
void ZBQueue::InsertBeforeCurrent(void* data)
{
    ZBQueueNode* newNode;
    ZBQueueNode* result;

    if (data == 0) {
        ShowError("queue fault 0115");
    }

    newNode = (ZBQueueNode*)operator new(sizeof(ZBQueueNode));
    result = 0;
    if (newNode != 0) {
        result = newNode->Init(data);
    }

    if (current == 0) {
        current = head;
    }

    if (head == 0) {
        head = result;
        tail = result;
        current = result;
        return;
    }

    result->prev = current;
    result->next = current->next;
    if (current->next == 0) {
        head = result;
        current->next = result;
    } else {
        current->next->prev = result;
        current->next = result;
    }
}

/* Function start: 0x4189A0 */
void* ZBQueueNode::Cleanup(int flag)
{
    ZBQueueNode::data = 0;
    ZBQueueNode::next = 0;
    ZBQueueNode::prev = 0;
    if ((flag & 1) != 0) {
        delete this;
    }
    return this;
}

/* Function start: 0x41CCE0 */
void* ZBQueueNode::CleanupNode(int flag)
{
    data = 0;
    next = 0;
    prev = 0;
    if ((flag & 1) != 0) {
        FreeMemory(this);
    }
    return this;
}

