#include "ZBuffer.h"
#include "globals.h"
#include "Message.h"

char s_MustDefine[] = "Parser:: Must define an LBLParse func";
char s_hIam[] = "hIam %d";

#include "string.h"
#include <stdlib.h>
#include "VBuffer.h"
#include "Animation.h"
#include "Memory.h"
#include "Timer.h"
#include "string.h"

// ZBQueue is defined in ZBufferManager.h
#include "ZBufferManager.h"
#include "RenderEntry.h"
#include "SoundCommand.h"

extern void LogToMessageFile(char* format, ...);

/* Function start: 0x4010C0 */
int ZBuffer::LBLParse(char* param_1)
{
    ShowError("Parser:: Must define an LBLParse func");
    return 0;
}

/* Function start: 0x401150 */
int ZBuffer::ShutDown(Message* msg)
{
    return 0;
}

/* Function start: 0x401160 */
int ZBuffer::Exit(Message* msg)
{
    return 0;
}

/* Function start: 0x401170 */
void ZBuffer::OnInput(Message* msg)
{
    //LogToMessageFile("hIam %d", m_address);
}

/* Function start: 0x4012D0 */
void ZBuffer::Update(int param_1, int param_2)
{
    if (timer.Update() > 10000) {
        SC_Message_Send(3, m_address, m_address, m_from, 0x14, 0, 0, 0, 0, 0);
    }
    if (m_address == param_2) {
        ShowError("SC_ZBuffer::Update");
    }
}

/* Function start: 0x401330 */
void ZBuffer::AddMessage(int param_1)
{
    //VFunc5(param_1);
    ShowError("SC_ZBuffer::AddMessage");
}

/* Function start: 0x401350 */
int ZBuffer::ProcessMessage(Message* msg)
{
    if (msg->targetAddress != m_address) {
        return 0;
    }
    timer.Reset();
    int iVar1 = msg->priority;
    if (iVar1 == 3) {
        g_ZBufferManager_0043698c->m_flags |= 2;
        g_ZBufferManager_0043698c->timer.Reset();
        return 1;
    }
    if (iVar1 == 0xe) {
        g_ZBufferManager_0043698c->m_state = 2;
        ZBQueue* queue = g_ZBufferManager_0043698c->m_queueA0;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                SoundCommand* cmd = (SoundCommand*)queue->Pop();
                if (cmd != 0) {
                    cmd->~SoundCommand();
                    FreeFromGlobalHeap(cmd);
                }
            }
        }
        queue = g_ZBufferManager_0043698c->m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                ZBuffer* zb = (ZBuffer*)queue->Pop();
                if (zb != 0) {
                    zb->CleanUpVBuffer();
                    FreeFromGlobalHeap(zb);
                }
            }
        }
        queue = g_ZBufferManager_0043698c->m_queue9c;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                void* data = queue->Pop();
                if (data != 0) {
                    FreeFromGlobalHeap(data);
                }
            }
        }
    } else {
        if (iVar1 != 0xf) {
            return 0;
        }
        g_ZBufferManager_0043698c->m_state = 1;
        ZBQueue* queue = g_ZBufferManager_0043698c->m_queueA0;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                SoundCommand* cmd = (SoundCommand*)queue->Pop();
                if (cmd != 0) {
                    cmd->~SoundCommand();
                    FreeFromGlobalHeap(cmd);
                }
            }
        }
        queue = g_ZBufferManager_0043698c->m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                ZBuffer* zb = (ZBuffer*)queue->Pop();
                if (zb != 0) {
                    zb->CleanUpVBuffer();
                    FreeFromGlobalHeap(zb);
                }
            }
        }
        g_ZBufferManager_0043698c->m_queue9c->ClearList();
    }
    return 1;
}

/* Function start: 0x401500 */
void ZBuffer::CleanUpVBuffer()
{
    VBuffer* vb = m_vbuffer;
    if (vb != 0) {
        vb->~VBuffer();
        FreeFromGlobalHeap(vb);
        m_vbuffer = 0;
    }
    Animation* ptr = (Animation*)m_subObject;
    if (ptr != 0) {
        delete ptr;
        m_subObject = 0;
    }
}

/* Function start: 0x401560 */
void ZBQueue::ClearList()
{
    void* data;

    if (head == 0) return;
    current = head;
    do {
        ZBQueueNode* node = current;
        if (node == 0) {
            data = 0;
        } else {
            if (head == node) {
                head = node->next;
            }
            if (tail == node) {
                tail = node->prev;
            }
            if (node->prev != 0) {
                node->prev->next = node->next;
            }
            if (current->next != 0) {
                current->next->prev = current->prev;
            }
            data = 0;
            if (current != 0) {
                data = current->data;
            }
            if (current != 0) {
                delete current;
                current = 0;
            }
            current = head;
        }
        if (data != 0) {
            ((RenderEntry*)data)->RenderEntry::~RenderEntry();
            ::operator delete(data);
        }
    } while (head != 0);
}

/* Function start: 0x4016A0 */
ZBuffer::~ZBuffer()
{
    CleanUpVBuffer();
}

/* Function start: 0x401710 */
/* Function start: 0x401790 */
/* Function start: 0x401810 */
void* ZBQueue::Pop()
{
    return LinkedList::RemoveCurrent();
}
