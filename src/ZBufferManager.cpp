#include "ZBufferManager.h"
#include "VBuffer.h"
#include "Timer.h"
#include "ZBuffer.h"
#include "Palette.h"
#include "Memory.h"
#include "Animation.h"
#include "GlyphRect.h"
#include "RenderEntry.h"
#include "string.h"
#include "globals.h"
#include <string.h>
#include <stdlib.h>

extern "C" {
    void FUN_0041b0a0(int, int, void*, int, int);
    int _rand();
    void FUN_004229ea(int, int);
    void FUN_00421700(void*, char*, int);
}

void __stdcall FUN_0041b110(int, int, void*, int, int);


// Base SoundCommand
struct SoundCommand {
    virtual void Execute(GlyphRect* rect) = 0;
    
};

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
                 //FUN_0041b0a0(x, y, data, scale_low, scale_high);
                 break;
            case 3:
                 g_WorkBuffer_00436974->FUN_0041b110(x, y, data, scale_low, scale_high);
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
        FUN_004229ea(x, y);
        FUN_00421700(g_TextManager_00436990, text, -1);
    }
};

// Stub - checks if palette has changed from the stored one
// Returns 0 if palette changed, non-zero if unchanged
int __fastcall IsPaletteUnchanged(void* storedPalette) { return 0; }

/* Function start: 0x41BB10 */
void* ZBQueue::GetCurrentData()
{
    ZBQueueNode* node = (ZBQueueNode*)this->current;
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
            if ((*(char*)this & 2) != 0) {
                 x = _rand() % 5 - 2 + x;
                 y = _rand() % 5 - 2 + y;
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
                     //FUN_0041b0a0(x, y, data, scale1, scale2);
                     break;
                case 3:
                     g_WorkBuffer_00436974->FUN_0041b110(x, y, data, scale1, scale2);
                     break;
            }
        }
    }
}
/* Function start: 0x41CCE0 */
void* ZBQueueNode::CleanupNode(int flag)
{
    this->data = 0;
    this->next = 0;
    this->prev = 0;
    if ((flag & 1) != 0) {
        FreeMemory(this);
    }
    return this;
}
/* Function start: 0x401810 */
void* ZBQueue::PopNode()
{
    ZBQueueNode* piVar1 = (ZBQueueNode*)this->current;
    if (piVar1 != 0) {
        if (this->head == piVar1) {
            this->head = piVar1->prev;
        }
        if (this->tail == piVar1) {
            this->tail = piVar1->next;
        }
        if (piVar1->next != 0) {
            ((ZBQueueNode*)piVar1->next)->prev = piVar1->prev;
        }
        ZBQueueNode* puVar2 = (ZBQueueNode*)((ZBQueueNode*)this->current)->prev;
        if (puVar2 != 0) {
            puVar2->next = ((ZBQueueNode*)this->current)->next;
        }
        ZBQueueNode* puVar2_2 = (ZBQueueNode*)this->current;
        void* uVar3;
        uVar3 = 0;
        if (puVar2_2 != 0) {
            uVar3 = puVar2_2->data;
            puVar2_2->data = 0;
            puVar2_2->next = 0;
            puVar2_2->prev = 0;
            FreeMemory(puVar2_2);
            this->current = 0;
        }
        this->current = this->head;
        return uVar3;
    }
    return 0;
}

extern "C" void FlipScreen();

extern VBuffer* g_WorkBuffer_00436974;

/* Function start: 0x41C960 */
void ZBufferManager::UpdateScreen() {
    int* piVar1;
    int* puVar3;
    int iVar4;
    RenderEntry* local_14;
    GlyphRect local_rect;
    
    int mode = this->m_state;
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
    
    piVar1 = (int*)this->m_queue9c;
    if (*piVar1 == 0) {
        return;
    }
    
    do {
        local_14 = (RenderEntry*)0;
        piVar1 = (int*)this->m_queue9c;
        int queueType = piVar1[3];
        if (queueType == 1 || queueType == 4) {
            iVar4 = *piVar1;
        } else if (queueType == 2 || queueType == 0) {
            iVar4 = piVar1[1];
        } else {
            ShowError("\"bad queue type %lu\"", queueType);
            goto process_entry;
        }
        piVar1[2] = iVar4;
        
process_entry:
        iVar4 = piVar1[2];
        if (iVar4 == 0) {
            goto do_blit;
        }
        if (*piVar1 == iVar4) {
            *piVar1 = *(int*)(iVar4 + 4);
        }
        iVar4 = piVar1[2];
        if (piVar1[1] == iVar4) {
            piVar1[1] = *(int*)iVar4;
        }
        iVar4 = *(int*)piVar1[2];
        if (iVar4 != 0) {
            *(int*)(iVar4 + 4) = ((int*)piVar1[2])[1];
        }
        puVar3 = (int*)((int*)piVar1[2])[1];
        if (puVar3 != 0) {
            *puVar3 = *(int*)piVar1[2];
        }
        iVar4 = piVar1[2];
        if (iVar4 == 0) {
            local_14 = (RenderEntry*)0;
        } else {
            local_14 = (RenderEntry*)((int*)iVar4)[2];
            ((int*)iVar4)[2] = 0;
            *(int*)iVar4 = 0;
            ((int*)iVar4)[1] = 0;
            FreeMemory((void*)iVar4);
            piVar1[2] = 0;
        }
        piVar1[2] = *piVar1;
        
do_blit:
        {
            local_rect = local_14->rect;
            
            if (g_WorkBuffer_00436974 != 0) {
                g_WorkBuffer_00436974->CallBlitter4(local_rect.left, local_rect.right, local_rect.top, local_rect.bottom, local_rect.left, local_rect.right);
            }
            
            delete local_14;
        }
        
        piVar1 = (int*)this->m_queue9c;
    } while (*piVar1 != 0);
}


/* Function start: 0x41C000 */
void ZBufferManager::ShowSubtitle(char* text, int x, int y, int duration, int flag)
{
    int iVar1 = m_state;
    if (iVar1 == 0) {
        return;
    }
    if (iVar1 == 1) {
        FUN_004229ea(x, y);
        FUN_00421700(g_TextManager_00436990, text, -1);
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
                ZBQueueNode* newNode = (ZBQueueNode*)AllocateMemory(12);
                if (newNode) {
                    newNode->prev = 0;
                    newNode->next = 0;
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
                    ZBQueueNode* newNode = (ZBQueueNode*)AllocateMemory(12);
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
                    ZBQueueNode* newNode = (ZBQueueNode*)AllocateMemory(12);
                    if (newNode) {
                        newNode->prev = 0;
                        newNode->next = 0;
                        newNode->data = cmd;
                    } else {
                        newNode = 0;
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
            }
            return;
        }
        // Simple insert
        ZBQueueNode* newNode = (ZBQueueNode*)AllocateMemory(12);
        if (newNode) {
            newNode->prev = 0;
            newNode->next = 0;
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
                ZBQueueNode* newNode = (ZBQueueNode*)AllocateMemory(12);
                if (newNode) {
                    newNode->prev = 0;
                    newNode->next = 0;
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
                    ZBQueueNode* newNode = (ZBQueueNode*)AllocateMemory(12);
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
                    ZBQueueNode* newNode = (ZBQueueNode*)AllocateMemory(12);
                    if (newNode) {
                        newNode->prev = 0;
                        newNode->next = 0;
                        newNode->data = cmd;
                    } else {
                        newNode = 0;
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
            }
            return;
        }
        ZBQueueNode* newNode = (ZBQueueNode*)AllocateMemory(12);
        if (newNode) {
            newNode->prev = 0;
            newNode->next = 0;
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
    
    WriteToMessageLogIfEnabled(L"\"declaring ZBuffer\"");
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
    int* piVar1;
    int iVar3;
    unsigned int* puVar4;
    void* pvVar5;
    int* local_14;
    unsigned int* local_18;
    unsigned int* local_1c;
    
    // First: drain queue at 0xA0 with special handling
    piVar1 = (int*)this->m_queueA0;
    iVar3 = *piVar1;
    while (iVar3 != 0) {
        piVar1 = (int*)this->m_queueA0;
        int queueType = piVar1[3];
        if (queueType == 1 || queueType == 4) {
            iVar3 = *piVar1;
        } else if (queueType == 2 || queueType == 0) {
            iVar3 = piVar1[1];
        } else {
            ShowError("\"bad queue type %lu\"", queueType);
        }
        piVar1[2] = iVar3;
        
        iVar3 = piVar1[2];
        if (iVar3 != 0) {
            if (*piVar1 == iVar3) {
                *piVar1 = *(int*)(iVar3 + 4);
            }
            if (piVar1[1] == piVar1[2]) {
                piVar1[1] = *(int*)piVar1[2];
            }
            iVar3 = *(int*)piVar1[2];
            if (iVar3 != 0) {
                *(int*)(iVar3 + 4) = ((int*)piVar1[2])[1];
            }
            int* pNode = (int*)((int*)piVar1[2])[1];
            if (pNode != 0) {
                *pNode = *(int*)piVar1[2];
            }
            ((ZBQueue*)piVar1)->GetCurrentData();
            ZBQueueNode* nodePtr = (ZBQueueNode*)piVar1[2];
            if (nodePtr != 0) {
                nodePtr->CleanupNode(1);
                piVar1[2] = 0;
            }
            piVar1[2] = *piVar1;
        }
        piVar1 = (int*)this->m_queueA0;
        iVar3 = *piVar1;
    }
    
    // Second: drain queue at 0xA0 (again), freeing remaining items
    piVar1 = (int*)this->m_queueA0;
    if (piVar1 != 0) {
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                puVar4 = (unsigned int*)ZBuffer::PopNode(piVar1);
                if (puVar4 != 0) {
                    *puVar4 = 0x431050;
                    FreeMemory(puVar4);
                }
            }
        }
        FreeMemory(piVar1);
        this->m_queueA0 = 0;
    }
    
    // Third: drain queue at 0xA4
    piVar1 = (int*)this->m_queueA4;
    if (piVar1 != 0) {
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                pvVar5 = ZBuffer::PopNode_2(piVar1);
                if (pvVar5 != 0) {
                    ((ZBuffer*)pvVar5)->CleanUpVBuffer();
                    FreeMemory(pvVar5);
                }
            }
        }
        FreeMemory(piVar1);
        this->m_queueA4 = 0;
    }
    
    // Fourth: drain queue at 0x9C
    local_14 = (int*)this->m_queue9c;
    if (local_14 != 0) {
        if (*local_14 != 0) {
            local_14[2] = *local_14;
            while (*local_14 != 0) {
                local_18 = (unsigned int*)((ZBQueue*)local_14)->PopNode();
                if (local_18 != 0) {
                    local_1c = local_18;
                    *local_18 = 0x431058;
                    try {
                        ((GlyphRect*)(local_1c + 1))->~GlyphRect();
                    } catch (...) {
                    }
                    FreeMemory(local_18);
                }
            }
        }
        FreeMemory(local_14);
        this->m_queue9c = 0;
    }
}

/* Function start: 0x41C5A0 */
void ZBufferManager::ProcessRenderQueues()
{
    unsigned int* pThis = (unsigned int*)this;
    int* piVar1;
    int iVar3;
    void* pvVar4;
    unsigned int uVar5;
    unsigned int* puVar6;
    unsigned int* puVar7;
    unsigned int* local_14;  // saved this pointer / [EBP-0x10]
    int local_height;        // [EBP-0x14] 
    int local_width;         // [EBP-0x18]
    int local_1c;            // [EBP-0x1c]
    int local_20;            // [EBP-0x20]
    
    pThis[1] = 0;
    local_14 = pThis;
    
    if (pThis[0x26] == 2) {
        // State type 2 processing
        if ((void*)pThis[0x2a] != 0) {
            iVar3 = IsPaletteUnchanged((void*)pThis[0x2a]);
            if (iVar3 == 0) {
                BlankScreen();
                ((Palette*)local_14[0x2a])->SetPalette(0, 0x100);
            }
            local_14[0x2a] = 0;
        }
        
        local_height = g_WorkBuffer_00436974->height - 1;
        local_width = g_WorkBuffer_00436974->width - 1;
        local_20 = 0;
        local_1c = 0;
        
        try {
            iVar3 = *(int*)local_14[0x28];
            while (iVar3 != 0) {
                puVar7 = 0;
                piVar1 = (int*)local_14[0x28];
                int queueType = piVar1[3];
                if (queueType == 1 || queueType == 4) {
                    iVar3 = *piVar1;
                    piVar1[2] = iVar3;
                } else if (queueType == 2 || queueType == 0) {
                    iVar3 = piVar1[1];
                    piVar1[2] = iVar3;
                } else {
                    ShowError("\"bad queue type %lu\"", queueType);
                }
                
                if (piVar1[2] != 0) {
                    puVar7 = (unsigned int*)ZBuffer::PopNode(piVar1);
                }
                
                if (puVar7 != 0) {
                    // Virtual call through vtable [puVar7[0]], passing &local_20
                    (*(void (**)(unsigned int*, int*))*puVar7)(puVar7, &local_20);
                    
                    if (puVar7 != 0) {
                        *puVar7 = 0x431050;  // vtable for destructor
                        FreeMemory(puVar7);
                    }
                }
                
                iVar3 = *(int*)local_14[0x28];
            }
        } catch (...) {
        }
        
        // Process queue at 0xa4
        piVar1 = (int*)local_14[0x29];
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                pvVar4 = ZBuffer::PopNode_2(piVar1);
                if (pvVar4 != 0) {
                    ((ZBuffer*)pvVar4)->CleanUpVBuffer();
                    FreeMemory(pvVar4);
                }
            }
        }
        
        if ((*local_14 & 2) != 0) {
            uVar5 = ((Timer*)(local_14 + 0x21))->Update();
            if (uVar5 > 2000) {
                *local_14 = *local_14 & 0xfffffffd;
            }
        }
    }
    else if (pThis[0x26] == 3) {
        // State type 3 processing
        if ((void*)pThis[0x2a] != 0) {
            iVar3 = IsPaletteUnchanged((void*)pThis[0x2a]);
            if (iVar3 == 0) {
                BlankScreen();
                ((Palette*)local_14[0x2a])->SetPalette(0, 0x100);
            }
            local_14[0x2a] = 0;
        }
        
        iVar3 = *(int*)local_14[0x27];
        while (iVar3 != 0) {
            puVar7 = 0;
            piVar1 = (int*)local_14[0x27];
            int queueType = piVar1[3];
            if (queueType == 1 || queueType == 4) {
                iVar3 = *piVar1;
                piVar1[2] = iVar3;
            } else if (queueType == 2 || queueType == 0) {
                iVar3 = piVar1[1];
                piVar1[2] = iVar3;
            } else {
                ShowError("\"bad queue type %lu\"", queueType);
            }
            
            iVar3 = piVar1[2];
            if (iVar3 != 0) {
                // Unlink node from double linked list
                if (*piVar1 == iVar3) {
                    *piVar1 = *(int*)(iVar3 + 4);
                }
                if (piVar1[1] == piVar1[2]) {
                    piVar1[1] = *(int*)piVar1[2];
                }
                iVar3 = *(int*)piVar1[2];
                if (iVar3 != 0) {
                    *(int*)(iVar3 + 4) = ((int*)piVar1[2])[1];
                }
                puVar6 = (unsigned int*)((unsigned int*)piVar1[2])[1];
                if (puVar6 != 0) {
                    *puVar6 = *(unsigned int*)piVar1[2];
                }
                
                ZBQueueNode* nodePtr = (ZBQueueNode*)piVar1[2];
                if (nodePtr == 0) {
                    puVar7 = 0;
                } else {
                    puVar7 = (unsigned int*)nodePtr->data;
                    nodePtr->CleanupNode(1);
                    piVar1[2] = 0;
                }
                piVar1[2] = *piVar1;
                local_height = (int)puVar7;
            }
            
            // Check queue at 0xa0
            piVar1 = (int*)local_14[0x28];
            if (piVar1 == 0 || *piVar1 == 0) {
                break;
            }
            piVar1[2] = *piVar1;
            
            if (*(int*)local_14[0x28] != 0) {
                unsigned int* iterPtr = puVar7 + 1;
                do {
                    puVar6 = 0;
                    if (*(int*)(local_14[0x28] + 8) != 0) {
                        puVar6 = *(unsigned int**)(*(int*)(local_14[0x28] + 8) + 8);
                    }
                    // Virtual call
                    (*(void (**)(unsigned int*, unsigned int*))*puVar6)(puVar6, iterPtr);
                    
                    unsigned int queuePtr = local_14[0x28];
                    iVar3 = *(int*)(queuePtr + 8);
                    if (*(int*)(queuePtr + 4) == iVar3) {
                        break;
                    }
                    if (iVar3 != 0) {
                        *(unsigned int*)(queuePtr + 8) = *(unsigned int*)(iVar3 + 4);
                    }
                } while (*(int*)local_14[0x28] != 0);
            }
            // TODO: convert to delete
            if (puVar7 != 0) {
                *puVar7 = 0x431058;  // vtable for destructor
                //SEH_Cleanup_0041c94c();
                FreeMemory(puVar7);
            }
            
            iVar3 = *(int*)local_14[0x27];
        }
        
        // Process queue at 0xa4
        piVar1 = (int*)local_14[0x29];
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                pvVar4 = ZBuffer::PopNode_2(piVar1);
                if (pvVar4 != 0) {
                    ((ZBuffer*)pvVar4)->CleanUpVBuffer();
                    FreeMemory(pvVar4);
                }
            }
        }
        
        if ((*local_14 & 2) != 0) {
            uVar5 = ((Timer*)(local_14 + 0x21))->Update();
            if (uVar5 > 2000) {
                *local_14 = *local_14 & 0xfffffffd;
            }
        }
    }
}
