#include "SoundManager.h"
#include "globals.h"
#include "Memory.h"
#include "VBuffer.h"
#include "GlyphRect.h"
#include "string.h"
#include <string.h>
#include <stdlib.h>


extern "C" {
    void FUN_0041b0a0(int, int, void*, int, int);
    void FUN_004229ea(int, int);
    void FUN_00421700(void*, char*, int);
    int _rand();
}

void __stdcall FUN_0041b110(int, int, void*, int, int);

// Base SoundCommand
struct SoundCommand {
    virtual void Execute(GlyphRect* rect) = 0;
    
    void* operator new(size_t size) {
        return AllocateMemory(size);
    }
    void operator delete(void* ptr) {
        FreeFromGlobalHeap(ptr);
    }
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

/* Function start: 0x41BE20 */
void SoundManager::PlayAnimationSound(void* data, int priority, int x, int y, int mode, int scale1, int scale2)
{
    if (m_mode != 0) {
        if (m_mode != 1) {
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

/* Function start: 0x41C000 */
void SoundManager::ShowSubtitle(char* text, int x, int y, int duration, int flag)
{
    if (m_mode == 0) return;

    if (m_mode == 1) {
        FUN_004229ea(x, y);
        FUN_00421700(g_TextManager_00436990, text, -1);
        return;
    }

    CommandType2* cmd = new CommandType2();
    if (!cmd) return;

    // Copy text
    int len = strlen(text);
    char* buf = (char*)AllocateMemory(len + 1);
    if (buf) strcpy(buf, text);
    cmd->text = buf;
    
    cmd->duration = duration;
    cmd->x = x;
    cmd->y = y;

    QueueCommand(cmd);
}

/* Function start: 0x41C2C0 */
void SoundManager::QueueCommand(SoundCommand* cmd)
{
    int h = g_WorkBuffer_00436974->height - 1;
    int w = g_WorkBuffer_00436974->width - 1;
    GlyphRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = w;
    rect.bottom = h;
    
    int mode = m_mode;
    if (mode == 1) {
        cmd->Execute(&rect);
        delete cmd;
        return;
    }
    
    if (mode == 2) {
        Queue* queue = m_commandQueue;
        if (!cmd) {
            ShowError("queue fault 0101");
        }
        queue->m_current = queue->m_head;
        
        int qtype = queue->m_field_0xc;
        if (qtype == 1 || qtype == 2) {
            if (queue->m_head == 0) {
                queue->Insert(cmd);
                return;
            }
            
            while (queue->m_current) {
                QueueNode* node = (QueueNode*)queue->m_current;
                SoundCommand* nodeCmd = (SoundCommand*)node->data;
                
                if ((unsigned int)((CommandType1*)nodeCmd)->parameter1 < (unsigned int)((CommandType1*)cmd)->parameter1) {
                    queue->Insert(cmd);
                    return;
                }
                
                if (queue->m_tail == queue->m_current) {
                    if (!cmd) {
                        ShowError("queue fault 0112");
                    }
                    QueueNode* newNode = (QueueNode*)AllocateMemory(12);
                    if (newNode) {
                        newNode->prev = 0;
                        newNode->next = 0;
                        newNode->data = cmd;
                    } else {
                        newNode = 0;
                    }
                    
                    if (queue->m_current == 0) {
                        queue->m_current = queue->m_tail;
                    }
                    if (queue->m_head == 0) {
                        queue->m_head = newNode;
                        queue->m_tail = newNode;
                        queue->m_current = newNode;
                    } else {
                        QueueNode* tail = (QueueNode*)queue->m_tail;
                        if (tail == 0 || tail->next != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->next = 0;
                        newNode->prev = tail;
                        tail->next = newNode;
                        queue->m_tail = newNode;
                    }
                    return;
                }
                
                if (node != 0) {
                    queue->m_current = node->next;
                }
            }
            return;
        }
        queue->Insert(cmd);
        return;
    }
    
    if (mode == 3) {
        Queue* queue = m_commandQueue;
        if (!cmd) {
            ShowError("queue fault 0101");
        }
        queue->m_current = queue->m_head;
        
        int qtype = queue->m_field_0xc;
        if (qtype == 1 || qtype == 2) {
            if (queue->m_head == 0) {
                queue->Insert(cmd);
                return;
            }
            
            while (queue->m_current) {
                QueueNode* node = (QueueNode*)queue->m_current;
                SoundCommand* nodeCmd = (SoundCommand*)node->data;
                
                if ((unsigned int)((CommandType1*)nodeCmd)->parameter1 < (unsigned int)((CommandType1*)cmd)->parameter1) {
                    queue->Insert(cmd);
                    return;
                }
                
                if (queue->m_tail == queue->m_current) {
                    if (!cmd) {
                        ShowError("queue fault 0112");
                    }
                    QueueNode* newNode = (QueueNode*)AllocateMemory(12);
                    if (newNode) {
                        newNode->prev = 0;
                        newNode->next = 0;
                        newNode->data = cmd;
                    } else {
                        newNode = 0;
                    }
                    
                    if (queue->m_current == 0) {
                        queue->m_current = queue->m_tail;
                    }
                    if (queue->m_head == 0) {
                        queue->m_head = newNode;
                        queue->m_tail = newNode;
                        queue->m_current = newNode;
                    } else {
                        QueueNode* tail = (QueueNode*)queue->m_tail;
                        if (tail == 0 || tail->next != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->next = 0;
                        newNode->prev = tail;
                        tail->next = newNode;
                        queue->m_tail = newNode;
                    }
                    return;
                }
                
                if (node != 0) {
                    queue->m_current = node->next;
                }
            }
            return;
        }
        queue->Insert(cmd);
        return;
    }
}
