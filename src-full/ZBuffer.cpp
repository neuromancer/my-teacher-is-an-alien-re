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
#include "Handler.h"
#include "SC_Question.h"

/* Function start: 0x4010C0 */ /* DEMO ONLY - no full game match */
int ZBuffer::LBLParse(char* param_1)
{
    ShowError("Parser:: Must define an LBLParse func");
    return 0;
}

/* Function start: 0x401150 */ /* DEMO ONLY - no full game match */
int ZBuffer::ShutDown(Message* msg)
{
    return 0;
}

/* Function start: 0x44B5A0 */
int ZBuffer::Exit(Message* msg)
{
    return 0;
}

/* Function start: 0x401170 */ /* DEMO ONLY - no full game match */
void ZBuffer::OnInput(Message* msg)
{
    WriteToMessageLog("hIam %d", m_address);
}

/* Function start: 0x4012D0 */ /* DEMO ONLY - no full game match */
void ZBuffer::Update(int param_1, int param_2)
{
    if (timer.Update() > 10000) {
        SC_Message_Send(3, m_address, m_address, m_from, 0x14, 0, 0, 0, 0, 0);
    }
    if (m_address == param_2) {
        ShowError("SC_ZBuffer::Update");
    }
}

/* Function start: 0x401330 */ /* DEMO ONLY - no full game match */
int ZBuffer::AddMessage(int param_1)
{
    ((Handler*)this)->WriteMessageAddress((SC_Message*)param_1);
    ShowError("SC_ZBuffer::AddMessage");
    return 1;
}

/* Function start: 0x401350 */ /* DEMO ONLY - no full game match */
int ZBuffer::ProcessMessage(Message* msg)
{
    if (msg->targetAddress != m_address) {
        return 0;
    }
    timer.Reset();
    switch (msg->priority) {
    case 3:
        g_ZBufferManager_0043698c->m_flags |= 2;
        g_ZBufferManager_0043698c->timer.Reset();
        return 1;
    case 0xe:
    {
        ZBufferManager* mgr = g_ZBufferManager_0043698c;
        mgr->m_state = 2;
        ZBQueue* queue = mgr->m_queueA0;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                SoundCommand* cmd = (SoundCommand*)queue->Pop();
                if (cmd != 0) {
                    cmd->~SoundCommand();
                    operator delete(cmd);
                }
            }
        }
        ZBQueue* queueA4 = mgr->m_queueA4;
        if (queueA4->head != 0) {
            queueA4->current = queueA4->head;
            while (queueA4->head != 0) {
                ZBuffer* zb = (ZBuffer*)queueA4->Pop();
                if (zb != 0) {
                    zb->CleanUpVBuffer();
                    operator delete(zb);
                }
            }
        }
        queue = mgr->m_queue9c;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                RenderEntry* entry = (RenderEntry*)queue->Pop();
                if (entry != 0) {
                    delete entry;
                }
            }
        }
        break;
    }
    case 0xf:
    {
        ZBufferManager* mgr = g_ZBufferManager_0043698c;
        mgr->m_state = 1;
        ZBQueue* queue = mgr->m_queueA0;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                SoundCommand* cmd = (SoundCommand*)queue->Pop();
                if (cmd != 0) {
                    cmd->~SoundCommand();
                    operator delete(cmd);
                }
            }
        }
        ZBQueue* queueA4 = mgr->m_queueA4;
        if (queueA4->head != 0) {
            queueA4->current = queueA4->head;
            while (queueA4->head != 0) {
                ZBuffer* zb = (ZBuffer*)queueA4->Pop();
                if (zb != 0) {
                    zb->CleanUpVBuffer();
                    operator delete(zb);
                }
            }
        }
        mgr->m_queue9c->ClearList();
        break;
    }
    default:
        return 0;
    }
    return 1;
}

/* Function start: 0x401C80 */
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

/* Function start: 0x4145F0 */
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

/* Function start: 0x414710 */
ZBuffer::~ZBuffer()
{
}

/* Function start: 0x42BC50 */
/* Function start: 0x4036A0 */
/* Function start: 0x4035A0 */
void* ZBQueue::Pop()
{
    return LinkedList::RemoveCurrent();
}
