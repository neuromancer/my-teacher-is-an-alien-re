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
    unsigned int parameter1; // Priority
    void* data;
    int x;
    int y;
    int mode;
    int scale_low;
    int scale_high;

    CommandType1() : parameter1(0), data(0), x(0), y(0), mode(0), scale_low(0), scale_high(0) {}

    virtual void Execute(GlyphRect* rect) {
        switch(mode) {
            case 0:
                 g_WorkBuffer_00436974->ClipAndBlit(*(int*)((char*)data + 0x28), *(int*)((char*)data + 0x2c), *(int*)((char*)data + 0x20), *(int*)((char*)data + 0x24), x, y, (int)data);
                 break;
            case 1:
                 g_WorkBuffer_00436974->ClipAndPaste(*(int*)((char*)data + 0x28), *(int*)((char*)data + 0x2c), *(int*)((char*)data + 0x20), *(int*)((char*)data + 0x24), x, y, (int)data);
                 break;

            case 2:
                 DrawScaledSprite(x, y, data, *(double*)&scale_low);
                 break;
            case 3:
                 g_WorkBuffer_00436974->ScaleTCCopy(x, y, (VBuffer*)data, *(double*)&scale_low);
                 break;
        }
    }
};

struct CommandType2 : public SoundCommand {
    int duration;
    char* text; 
    int x;
    int y;

    virtual void Execute(GlyphRect* rect) {
        SetDrawPosition(x, y);
        g_TextManager_00436990->RenderText(text, -1);
    }
};

// Stub - checks if palette has changed from the stored one
// Returns 0 if palette changed, non-zero if unchanged
int __fastcall IsPaletteUnchanged(void* storedPalette) { return 0; }

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
            switch(mode) {
                case 0:
                     g_WorkBuffer_00436974->ClipAndBlit(*(int*)((char*)data + 0x28), *(int*)((char*)data + 0x2c), *(int*)((char*)data + 0x20), *(int*)((char*)data + 0x24), x, y, (int)data);
                     break;
                case 1:
                     g_WorkBuffer_00436974->ClipAndPaste(*(int*)((char*)data + 0x28), *(int*)((char*)data + 0x2c), *(int*)((char*)data + 0x20), *(int*)((char*)data + 0x24), x, y, (int)data);
                     break;
                case 2:
                     DrawScaledSprite(x, y, data, *(double*)&scale1);
                     break;
                case 3:
                     g_WorkBuffer_00436974->ScaleTCCopy(x, y, (VBuffer*)data, *(double*)&scale1);
                     break;
            }
        }
    }
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
/* Function start: 0x401810 */
void* ZBQueue::PopNode()
{
    ZBQueueNode* piVar1;
    ZBQueueNode* puVar2;
    void* uVar3;

    piVar1 = (ZBQueueNode*)current;
    if (piVar1 == 0) {
        return 0;
    }
    if (head == piVar1) {
        head = piVar1->prev;
    }
    if (tail == piVar1) {
        tail = piVar1->next;
    }
    if (piVar1->next != 0) {
        ((ZBQueueNode*)piVar1->next)->prev = piVar1->prev;
    }
    puVar2 = (ZBQueueNode*)((ZBQueueNode*)current)->prev;
    if (puVar2 != 0) {
        puVar2->next = ((ZBQueueNode*)current)->next;
    }
    puVar2 = (ZBQueueNode*)current;
    uVar3 = 0;
    if (puVar2 != 0) {
        uVar3 = puVar2->data;
    }
    if (puVar2 != 0) {
        delete puVar2;
        current = 0;
    }
    current = head;
    return uVar3;
}

extern "C" void FlipScreen();

extern VBuffer* g_WorkBuffer_00436974;

/* Function start: 0x41C960 */
void ZBufferManager::UpdateScreen() {
    ZBQueue* queue;
    ZBQueueNode* node;
    ZBQueueNode* nextNode;
    ZBQueueNode* prevNode;
    RenderEntry* local_10;
    GlyphRect local_rect;
    int queueType;

    int mode = m_state;
    if (mode == 1) {
        FlipScreen();
        return;
    }
    if (mode == 2) {
        FlipScreen();
        return;
    }
    if (mode != 3) {
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
            goto process_entry;
        }

process_entry:
        node = (ZBQueueNode*)queue->current;
        if (node == 0) {
            goto do_blit;
        }
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
            local_10 = 0;
        } else {
            local_10 = (RenderEntry*)node->data;
            delete node;
            queue->current = 0;
        }
        queue->current = queue->head;

do_blit:
        {
            local_rect = local_10->rect;

            if (g_WorkBuffer_00436974 != 0) {
                g_WorkBuffer_00436974->CallBlitter4(local_rect.left, local_rect.right, local_rect.top, local_rect.bottom, local_rect.left, local_rect.right);
            }

            delete local_10;
        }

        queue = m_queue9c;
    } while (queue->head != 0);
}


/* Function start: 0x41C000 */
void ZBufferManager::ShowSubtitle(char* text, int x, int y, int duration, int flag)
{
    int iVar1 = m_state;
    if (iVar1 == 0) {
        return;
    }
    if (iVar1 == 1) {
        SetDrawPosition(x, y);
        g_TextManager_00436990->RenderText(text, -1);
        return;
    }
    
    // Allocate 0x14 bytes (CommandType2)
    CommandType2* cmd = new CommandType2();
    if (cmd != 0) {
        // Zero init fields (assembly does this)
        cmd->duration = 0;
        cmd->text = 0;
        cmd->x = 0;
        cmd->y = 0;

        int len = strlen(text) + 1;
        char* newText = (char*)AllocateMemory(len);
        cmd->text = newText;
        
        if (newText != 0) {
            memcpy(newText, text, len);
        }
        
        cmd->duration = duration;
        cmd->x = x;
        cmd->y = y;
        
        QueueCommand(cmd);
    }
}
// Stub for 0x41C130
void ZBufferManager::DrawRect(int p1, int p2, int p3, int p4, int p5, int p6, int p7)
{
    // Stub
}

/* Function start: 0x41C2C0 */
void ZBufferManager::QueueCommand(SoundCommand* cmd)
{
    int h = g_WorkBuffer_00436974->height - 1;
    int w = g_WorkBuffer_00436974->width - 1;
    GlyphRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = w;
    rect.bottom = h;
    
    int mode = m_state;
    if (mode == 1) {
        cmd->Execute(&rect);
        delete cmd;
        return;
    }
    
    if (mode == 2) {
        ZBQueue* queue = m_queueA0;
        if (!cmd) {
            ShowError("queue fault 0101");
        }
        queue->current = queue->head;
        
        int qtype = queue->type;
        if (qtype == 1 || qtype == 2) {
            if (queue->head == 0) {
                // Simple insert using ZBQueue
                ZBQueueNode* newNode = new ZBQueueNode();
                if (newNode) {
                    newNode->data = cmd;
                    queue->head = newNode;
                    queue->tail = newNode;
                    queue->current = newNode;
                }
                return;
            }
            
            while (queue->current) {
                ZBQueueNode* node = (ZBQueueNode*)queue->current;
                SoundCommand* nodeCmd = (SoundCommand*)node->data;
                
                if ((unsigned int)((CommandType1*)nodeCmd)->parameter1 < (unsigned int)((CommandType1*)cmd)->parameter1) {
                    // Insert before current
                    ZBQueueNode* newNode = new ZBQueueNode();
                    if (newNode) {
                        newNode->data = cmd;
                        newNode->next = node;
                        newNode->prev = node->prev;
                        if (node->prev == 0) {
                            queue->head = newNode;
                        } else {
                            ((ZBQueueNode*)node->prev)->next = newNode;
                        }
                        node->prev = newNode;
                    }
                    return;
                }
                
                if (queue->tail == queue->current) {
                    if (!cmd) {
                        ShowError("queue fault 0112");
                    }
                    ZBQueueNode* newNode = new ZBQueueNode();
                    if (newNode) {
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
                        ZBQueueNode* tail = (ZBQueueNode*)queue->tail;
                        if (tail == 0 || tail->next != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->prev = tail;
                        tail->next = newNode;
                        queue->tail = newNode;
                    }
                    return;
                }

                if (node != 0) {
                    queue->current = node->next;
                }
            }
            return;
        }
        // Simple insert
        ZBQueueNode* newNode = new ZBQueueNode();
        if (newNode) {
            newNode->data = cmd;
            if (queue->head == 0) {
                queue->head = newNode;
                queue->tail = newNode;
            } else {
                ((ZBQueueNode*)queue->tail)->next = newNode;
                newNode->prev = queue->tail;
                queue->tail = newNode;
            }
        }
        return;
    }
    
    if (mode == 3) {
        ZBQueue* queue = m_queueA0;
        if (!cmd) {
            ShowError("queue fault 0101");
        }
        queue->current = queue->head;

        int qtype = queue->type;
        if (qtype == 1 || qtype == 2) {
            if (queue->head == 0) {
                ZBQueueNode* newNode = new ZBQueueNode();
                if (newNode) {
                    newNode->data = cmd;
                    queue->head = newNode;
                    queue->tail = newNode;
                    queue->current = newNode;
                }
                return;
            }

            while (queue->current) {
                ZBQueueNode* node = (ZBQueueNode*)queue->current;
                SoundCommand* nodeCmd = (SoundCommand*)node->data;

                if ((unsigned int)((CommandType1*)nodeCmd)->parameter1 < (unsigned int)((CommandType1*)cmd)->parameter1) {
                    ZBQueueNode* newNode = new ZBQueueNode();
                    if (newNode) {
                        newNode->data = cmd;
                        newNode->next = node;
                        newNode->prev = node->prev;
                        if (node->prev == 0) {
                            queue->head = newNode;
                        } else {
                            ((ZBQueueNode*)node->prev)->next = newNode;
                        }
                        node->prev = newNode;
                    }
                    return;
                }

                if (queue->tail == queue->current) {
                    if (!cmd) {
                        ShowError("queue fault 0112");
                    }
                    ZBQueueNode* newNode = new ZBQueueNode();
                    if (newNode) {
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
                        ZBQueueNode* tail = (ZBQueueNode*)queue->tail;
                        if (tail == 0 || tail->next != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->prev = tail;
                        tail->next = newNode;
                        queue->tail = newNode;
                    }
                    return;
                }

                if (node != 0) {
                    queue->current = node->next;
                }
            }
            return;
        }
        ZBQueueNode* newNode = new ZBQueueNode();
        if (newNode) {
            newNode->data = cmd;
            if (queue->head == 0) {
                queue->head = newNode;
                queue->tail = newNode;
            } else {
                ((ZBQueueNode*)queue->tail)->next = newNode;
                newNode->prev = queue->tail;
                queue->tail = newNode;
            }
        }
        return;
    }
}

ZBufferManager::~ZBufferManager()
{
    Cleanup();
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
    while ((int)queue->head != 0) {
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
            nextNode = (ZBQueueNode*)node->next;
            if (nextNode != 0) {
                nextNode->prev = node->prev;
            }
            node = (ZBQueueNode*)queue->current;
            prevNode = (ZBQueueNode*)node->prev;
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
                data = ZBuffer::PopNode((int*)queue);
                if (data != 0) {
                    delete data;
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
                data = ZBuffer::PopNode_2((int*)queue);
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
                data = ZBuffer::PopNode((int*)queue);
            }

            if (data != 0) {
                // Virtual call - Execute on SoundCommand
                ((SoundCommand*)data)->Execute((GlyphRect*)&local_20);

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
                data = ZBuffer::PopNode_2((int*)queue);
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
                data = ZBuffer::PopNode_2((int*)queue);
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
