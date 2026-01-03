#include "SoundManager.h"
#include "globals.h"
#include "Memory.h"
#include "VBuffer.h"
#include <string.h>
#include <stdlib.h>

extern "C" {
    void __stdcall FUN_0041b0a0(int, int, void*, int, int);
    void FUN_004229ea(int, int);
    void FUN_00421700(void*, char*, int);
    int _rand();
}

void __stdcall FUN_0041b110(int, int, void*, int, int);

// Base SoundCommand
struct SoundCommand {
    virtual void Execute(SoundManager* mgr) = 0;
    
    void* operator new(size_t size) {
        return AllocateMemory(size);
    }
    void operator delete(void* ptr) {
        FreeFromGlobalHeap(ptr);
    }
};

struct CommandType1 : public SoundCommand {
    int parameter1; // Priority
    void* data;
    int x;
    int y;
    int mode;
    int scale_low;
    int scale_high;

    CommandType1() : parameter1(0), data(0), x(0), y(0), mode(0), scale_low(0), scale_high(0) {}

    virtual void Execute(SoundManager* mgr) {
        switch(mode) {
            case 0:
                 g_WorkBuffer_00436974->ClipAndBlit(*(int*)((char*)data + 0x28), *(int*)((char*)data + 0x2c), *(int*)((char*)data + 0x20), *(int*)((char*)data + 0x24), x, y, (int)data);
                 break;
            case 1:
                 g_WorkBuffer_00436974->ClipAndPaste(*(int*)((char*)data + 0x28), *(int*)((char*)data + 0x2c), *(int*)((char*)data + 0x20), *(int*)((char*)data + 0x24), x, y, (int)data);
                 break;

            case 2:
                 FUN_0041b0a0(x, y, data, scale_low, scale_high);
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

    virtual void Execute(SoundManager* mgr) {
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
                     FUN_0041b0a0(x, y, data, scale1, scale2);
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
    if (m_mode == 1) {
        cmd->Execute(this);
        // cmd is explicitly deleted or handled?
        // Decompiled code at 41C2C0 for mode 1: calls func, then resets vtable and deletes.
        // But m_mode==1 case in 41BE20/41C000 handles exec directly without allocating.
        // So this function is only called if we decided to queue, but mode CHANGED to 1?
        // Or if we call QueueCommand directly?
        // Assuming we are deleting it.
        delete cmd;
        return;
    }
    

    if (m_mode == 2 || m_mode == 3) {
        if (!m_commandQueue) {
            // ShowError("queue fault 0101");
            return;
        }

        m_commandQueue->m_current = m_commandQueue->m_head;
        
        // Check sort mode
        if (m_commandQueue->m_field_0xc == 1 || m_commandQueue->m_field_0xc == 2) {
             if (m_commandQueue->m_head == 0) {
                 m_commandQueue->Push(cmd);
                 return;
             }
             
             while(m_commandQueue->m_current) {
                 SoundCommand* currCmd = (SoundCommand*)((QueueNode*)m_commandQueue->m_current)->data;
                 // parameter1 is at offset 4 for both types
                 int currPriority = ((CommandType1*)currCmd)->parameter1;
                 int newPriority = ((CommandType1*)cmd)->parameter1;
                 
                 if (currPriority < newPriority) {
                     m_commandQueue->Insert(cmd);
                     // Logic in 41c2c0 seems to break after insert?
                     // Yes, "FUN_0041cb40(piVar2,(int)param_1); break;"
                     return;
                 }
                 
                 m_commandQueue->m_current = ((QueueNode*)m_commandQueue->m_current)->next;
             }
             // If accessed end, push
             m_commandQueue->Push(cmd);
        } else {
             // Just insert (at current? or push?)
             // 41c2c0 calls FUN_0041cb40(piVar2, param_1).
             // FUN_0041cb40 is at 41C.map.
             // FUN_0041c2c0 uses it.
             // It's presumably Queue::Insert.
             m_commandQueue->Insert(cmd);
        }
    }

}
