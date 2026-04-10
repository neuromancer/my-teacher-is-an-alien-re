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
#include "GlyphFont.h"
#include "string.h"
#include "globals.h"
#include <string.h>
#include <stdlib.h>

#include "VideoTable.h"
#include "AnimatedAsset.h"
#include "InvSlotItem.h"

void __stdcall DrawScaledSprite(int x, int y, void* data, double scale);

// g_BackBuffer_0046aa14 now in globals.h



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

struct BlitCommand : public SoundCommand {
    unsigned int priority; // 0x04
    void* data;            // 0x08
    int x;                 // 0x0c
    int y;                 // 0x10
    int left;              // 0x14
    int top;               // 0x18
    int right;             // 0x1c
    int bottom;            // 0x20
    int mode;              // 0x24
    double scale;          // 0x28

    BlitCommand() { memset(&priority, 0, 0x2c); }

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
int __cdecl ClipRectBottomUp(int* param_1, int* param_2, int* param_3, int* param_4);

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

// 0x409160: ZBQueue::InsertBeforeCurrent COMDAT duplicate (same as 0x41CB40, from different obj)

/* Function start: 0x403CD0 */
void ZBufferManager::QueueAnimationCleanup(void* anim)
{
    void* wrapper;
    ZBQueue* queue;

    switch (m_state) {
    case 1:
        if (anim != 0) {
            delete (Animation*)anim;
        }
        return;
    case 2:
    case 3:
        break;
    default:
        return;
    }

    if (anim == 0) {
        return;
    }

    // Create 12-byte wrapper: [prev=0, next=0, data=Animation*]
    wrapper = ::operator new(0xc);
    if (wrapper != 0) {
        ((int*)wrapper)[0] = 0;
        ((int*)wrapper)[1] = 0;
        ((void**)wrapper)[2] = anim;
    } else {
        wrapper = 0;
    }

    queue = m_queueA4;
    if (wrapper == 0) {
        ShowError("queue fault 0101");
    }
    queue->current = queue->head;

    if (queue->type == 1 || queue->type == 2) {
        if (queue->head != 0) {
            do {
                if (*(unsigned int*)queue->current->data < *(unsigned int*)wrapper) {
                    queue->InsertNode(wrapper);
                    return;
                }
                if (queue->tail == queue->current) {
                    queue->PushNode(wrapper);
                    return;
                }
                if (queue->current != 0) {
                    queue->current = queue->current->next;
                }
            } while (queue->current != 0);
            return;
        }
        queue->InsertNode(wrapper);
    } else {
        queue->InsertBeforeCurrent(wrapper);
    }
}

/* Function start: 0x41B5D0 */ /* No assembly extracted */
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

void BlitCommand::Execute(GlyphRect* rect)
{
    GlyphRect clipRect;
    GlyphRect srcRect;
    int destX;
    int destY;
    VBuffer* vbuf;

    vbuf = (VBuffer*)g_BackBuffer_0046aa14;
    switch (mode) {
    case 0:
        vbuf->ClipAndBlit(left, right, top, bottom, x, y, (int)data);
        return;
    case 1:
        vbuf->ClipAndPaste(left, right, top, bottom, x, y, (int)data);
        return;
    case 2:
        vbuf->ClipAndBlitRegion(left, right, top, bottom, x, y, (int)data);
        return;
    case 3:
        clipRect.left = vbuf->clip_x1;
        clipRect.top = vbuf->clip_y1;
        clipRect.right = vbuf->clip_x2;
        clipRect.bottom = vbuf->clip_y2;
        srcRect.left = left;
        srcRect.top = top;
        srcRect.right = right;
        srcRect.bottom = bottom;
        destX = x;
        destY = y;
        if (ClipRectBottomUp(&clipRect.left, &srcRect.left, &destX, &destY) != 0) {
            vbuf->CallBlitter3(srcRect.left, srcRect.right, srcRect.top, srcRect.bottom,
                               destX, destY, (VBuffer*)data, 0, 1);
        }
        return;
    case 4:
        DrawScaledSprite(x, y, data, scale);
        return;
    case 5:
        vbuf->ScaleTCCopy(x, y, (VBuffer*)data, scale);
        return;
    }
}

/* Function start: 0x41B690 */ /* No assembly extracted */
void CommandType2::Execute(GlyphRect* rect)
{
    SetDrawPosition(x, y);
    if (g_TextManager_00436990 != 0) {
        g_TextManager_00436990->RenderText(text, -1);
    }
    return;
}

/* Function start: 0x41B6D0 */ /* No assembly extracted */
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

/* Function start: 0x403910 */
ZBufferManager::ZBufferManager() {
    WriteToLog("declaring ZBuffer");
    memset(this, 0, 0x2b * 4);

    m_queueA0 = new ZBQueue(2);
    m_queueA4 = new ZBQueue(0);
    m_queue9c = new ZBQueue(1);

    m_state = 2;
}

/* Function start: 0x403A90 */
void ZBufferManager::Cleanup() {
    ZBQueue* queue;
    void* data;
    int queueType;

    // First: manual drain of m_queueA0 nodes (matches LAB_0041b916 sequence)
    queue = m_queueA0;
    if (queue->head != 0) {
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

            ZBQueueNode* node = (ZBQueueNode*)queue->current;
            if (node != 0) {
                if (queue->head == node) {
                    queue->head = node->next;
                }
                if (queue->tail == node) {
                    queue->tail = node->prev;
                }
                if (node->prev != 0) {
                    node->prev->next = node->next;
                }
                if (node->next != 0) {
                    node->next->prev = node->prev;
                }

                queue->GetCurrentData();
                node = (ZBQueueNode*)queue->current;
                if (node != 0) {
                    delete node;
                    queue->current = 0;
                }
                queue->current = queue->head;
            }
        }
    }

    // Second: drain items from m_queueA0 (SoundCommand - non-virtual)
    queue = m_queueA0;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = queue->Pop();
                if (data != 0) {
                    SoundCommand* cmd = (SoundCommand*)data;
                    cmd->SoundCommand::~SoundCommand();
                    operator delete(cmd);
                }
            }
        }
        operator delete(queue);
        m_queueA0 = 0;
    }

    // Third: drain items from m_queueA4 (ZBuffer)
    queue = m_queueA4;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = queue->Pop();
                if (data != 0) {
                    ((ZBuffer*)data)->CleanUpVBuffer();
                    operator delete(data);
                }
            }
        }
        operator delete(queue);
        m_queueA4 = 0;
    }

    // Fourth: drain items from m_queue9c (RenderEntry)
    queue = m_queue9c;
    if (queue != 0) {
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = queue->Pop();
                if (data != 0) {
                    RenderEntry* entry = (RenderEntry*)data;
                    entry->RenderEntry::~RenderEntry();
                    operator delete(entry);
                }
            }
        }
        operator delete(queue);
        m_queue9c = 0;
    }
}

ZBufferManager::~ZBufferManager()
{
    // Original destructor at 0x430B90 does NOT call Cleanup.
    // It cleans up buffers at offsets 0x08, 0x0c, 0x10 instead.
    // Cleanup (0x403A90) handles queues at 0x9C/0xA0/0xA4 separately.
}

/* Function start: 0x404B80 */
void* ZBQueue::GetCurrentData()
{
    return LinkedList::GetCurrentData();
}

/* Function start: 0x41BE20 */ /* No assembly extracted */
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

/* Function start: 0x403FD0 */
void ZBufferManager::DrawVBufferRegion(void* data, int priority, int x, int y, int mode, double scale,
                                       int left, int top, int right, int bottom)
{
    if (m_state == 0) {
        return;
    }

    if (m_state != 1) {
        BlitCommand* cmd = new BlitCommand();

        cmd->data = data;
        cmd->priority = priority;

        if ((m_flags & 2) != 0) {
            x = rand() % 5 - 2 + x;
            y = rand() % 5 - 2 + y;
        }

        cmd->x = x;
        cmd->y = y;
        cmd->left = left;
        cmd->top = top;
        cmd->right = right;
        cmd->bottom = bottom;
        cmd->mode = mode;
        cmd->scale = scale;
        QueueCommand(cmd);
        return;
    }

    switch (mode) {
    case 0:
        g_BackBuffer_0046aa14->ClipAndBlit(left, right, top, bottom, x, y, (int)data);
        break;
    case 1:
        g_BackBuffer_0046aa14->ClipAndPaste(left, right, top, bottom, x, y, (int)data);
        break;
    case 2:
        g_BackBuffer_0046aa14->ClipAndBlitRegion(left, right, top, bottom, x, y, (int)data);
        break;
    case 3:
        g_BackBuffer_0046aa14->ClipAndBlitReversed(left, right, top, bottom, x, y, (int)data);
        break;
    case 4:
        DrawScaledSprite(x, y, data, scale);
        break;
    case 5:
        g_BackBuffer_0046aa14->ScaleTCCopy(x, y, (VBuffer*)data, scale);
        break;
    }
}

/* Function start: 0x41C000 */ /* No assembly extracted */
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

/* Function start: 0x404350 */
void ZBufferManager::DrawRect(int p1, int p2, int p3, int p4, int p5, int p6, int p7)
{
    if (m_state != 0) {
        if (m_state != 1) {
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
            SetFillColor(p6);
            ((VBuffer*)g_BackBuffer_0046aa14)->SetVideoMode();
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
            if (m_state == 2) {
                DrawRectOutline(p1, p3, p2, p4);
            } else {
                VideoFillRect(p1, p3, p2, p4);
            }
        }
    }
}

extern "C" void SetFontPosition(int, int);

// TextRenderEntry: 0x64-byte entry for queued text rendering (vtable 0x461040)
struct TextRenderEntry {
    int field_00;       // 0x00 (vtable set by new)
    int priority;       // 0x04
    char text[80];      // 0x08-0x57
    int posX;           // 0x58
    int posY;           // 0x5C
    int color;          // 0x60
    virtual ~TextRenderEntry() {}
};

/* Function start: 0x404230 */
void ZBufferManager::ShowText(char* text, int x, int y, int priority, int color) {
    if (m_state == 0) return;

    if (m_state == 1) {
        SetFontPosition(x, y);
        g_GlyphFont_0046aa28->RenderText(text, color);
        return;
    }

    TextRenderEntry* entry = new TextRenderEntry();
    strcpy(entry->text, text);
    entry->priority = priority;
    entry->posX = x;
    entry->posY = y;
    entry->color = color;
    QueueCommand((SoundCommand*)entry);
}

/* Function start: 0x4044E0 */
void ZBufferManager::QueueCommand(SoundCommand* cmd)
{
    ZBQueue* queue;
    ZBQueueNode* node;
    ListNode* newNode;
    VBuffer* vbuf = (VBuffer*)g_BackBuffer_0046aa14;
    int maxY = vbuf->height - 1;
    int maxX = vbuf->width - 1;
    GlyphRect rect(0, 0, maxX, maxY);
    switch (m_state) {
    case 1:
        cmd->Execute(&rect);
        if (cmd != 0) {
            
            FreeMemory(cmd);
        }
        break;
    case 2:
        queue = m_queueA0;
        if (cmd == 0) {
            ShowError("queue fault 0101");
        }
        queue->current = queue->head;
        if (queue->type == 1 || queue->type == 2) {
            if (queue->head == 0) {
                queue->InsertBeforeCurrent(cmd);
            } else {
                do {
                    node = (ZBQueueNode*)queue->current;
                    if (*(unsigned int*)((char*)node->data + 4) < *(unsigned int*)((char*)cmd + 4)) {
                        queue->InsertBeforeCurrent(cmd);
                        break;
                    }
                    if (queue->tail == node) {
                        if (cmd == 0) {
                            ShowError("queue fault 0112");
                        }
                        newNode = new ListNode(cmd);
                        if (queue->current == 0) {
                            queue->current = queue->tail;
                        }
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
                        break;
                    }
                    if (node != 0) {
                        queue->current = node->next;
                    }
                } while (queue->current != 0);
            }
        } else {
            queue->InsertBeforeCurrent(cmd);
        }
        break;
    case 3:
        queue = m_queueA0;
        if (cmd == 0) {
            ShowError("queue fault 0101");
        }
        queue->current = queue->head;
        if (queue->type == 1 || queue->type == 2) {
            if (queue->head == 0) {
                queue->InsertBeforeCurrent(cmd);
            } else {
                do {
                    node = (ZBQueueNode*)queue->current;
                    if (*(unsigned int*)((char*)node->data + 4) < *(unsigned int*)((char*)cmd + 4)) {
                        queue->InsertBeforeCurrent(cmd);
                        break;
                    }
                    if (queue->tail == node) {
                        if (cmd == 0) {
                            ShowError("queue fault 0112");
                        }
                        newNode = new ListNode(cmd);
                        if (queue->current == 0) {
                            queue->current = queue->tail;
                        }
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
                        break;
                    }
                    if (node != 0) {
                        queue->current = node->next;
                    }
                } while (queue->current != 0);
            }
        } else {
            queue->InsertBeforeCurrent(cmd);
        }
        break;
    }
}

/* Function start: 0x4047C0 */
void ZBufferManager::ProcessRenderQueues()
{
    ZBQueue* queue;
    ZBQueueNode* node;
    void* data;
    unsigned int elapsed;
    int queueType;
    void* local_14;

    m_head = 0;

    if (m_state == 1) {
        if (m_palette != 0) {
            m_palette->SetPalette(0, 0x100);
            m_palette = 0;
        }
    } else if (m_state == 2) {
        if (m_palette != 0) {
            m_palette->SetPalette(0, 0x100);
            m_palette = 0;
        }

        {
            VBuffer* vbuf = (VBuffer*)g_BackBuffer_0046aa14;
            GlyphRect rect(0, 0, vbuf->width - 1, vbuf->height - 1);

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
                        
                        FreeMemory(data);
                    }
                }
            }
        }

        queue = m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = queue->Pop();
                if (data != 0) {
                    ((ZBuffer*)data)->CleanUpVBuffer();
                    FreeMemory(data);
                }
            }
        }

        if ((m_flags & 2) != 0) {
            elapsed = timer.Update();
            if (elapsed > 2000) {
                m_flags = m_flags & ~2;
            }
        }
    } else if (m_state == 3) {
        if (m_palette != 0) {
            m_palette->SetPalette(0, 0x100);
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
                if (queue->head == node) {
                    queue->head = node->next;
                }
                if (queue->tail == (ZBQueueNode*)queue->current) {
                    queue->tail = ((ZBQueueNode*)queue->current)->prev;
                }
                node = (ZBQueueNode*)queue->current;
                if (node->prev != 0) {
                    node->prev->next = node->next;
                }
                node = (ZBQueueNode*)queue->current;
                if (node->next != 0) {
                    node->next->prev = node->prev;
                }

                local_14 = queue->GetCurrentData();
                node = (ZBQueueNode*)queue->current;
                if (node != 0) {
                    delete (ListNode*)node;
                    queue->current = 0;
                }
                queue->current = queue->head;
            }

            queue = m_queueA0;
            if (queue == 0 || queue->head == 0) {
                break;
            }
            queue->current = queue->head;

            if (m_queueA0->head != 0) {
                GlyphRect* iterPtr = (GlyphRect*)((int*)local_14 + 1);
                do {
                    SoundCommand* cmd;
                    node = (ZBQueueNode*)m_queueA0->current;
                    if (node != 0) {
                        cmd = (SoundCommand*)node->data;
                    } else {
                        cmd = 0;
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
                ((RenderEntry*)local_14)->~RenderEntry();
                FreeMemory(local_14);
            }
        }

        queue = m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                data = queue->Pop();
                if (data != 0) {
                    ((ZBuffer*)data)->CleanUpVBuffer();
                    FreeMemory(data);
                }
            }
        }

        if ((m_flags & 2) != 0) {
            elapsed = timer.Update();
            if (elapsed > 2000) {
                m_flags = m_flags & ~2;
            }
        }
    }
}

/* Function start: 0x404B90 */
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

/* Function start: 0x404DA0 */
void ZBQueue::InsertBeforeCurrent(void* data)
{
    LinkedList::InsertNode(data);
}

/* Function start: 0x431C30 */
// Same pattern as 0x41CB40 - InsertNode COMDAT
