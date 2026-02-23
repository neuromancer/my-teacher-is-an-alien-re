// ZBuffer.cpp - Combined compilation unit for ZBuffer, Handler12, and ScriptHandler destructor
// Original binary has these interleaved in 0x401000-0x401810 range
#include "Handler12.h"
#include "ZBuffer.h"
#include "ScriptHandler.h"
#include "SC_Question.h"
#include "globals.h"
#include "Message.h"
#include "string.h"
#include <string.h>
#include <stdlib.h>
#include "VBuffer.h"
#include "Animation.h"
#include "Memory.h"
#include "Timer.h"
#include "ZBufferManager.h"
#include "RenderEntry.h"
#include "SoundCommand.h"
#include "Handler.h"

char s_MustDefine[] = "Parser:: Must define an LBLParse func";
char s_hIam[] = "hIam %d";

extern void LogToMessageFile(char* format, ...);

/* Function start: 0x401000 */
Handler12::Handler12() {
    targetAddress = 12;
    timer.Reset();
}

/* Function start: 0x4010C0 */
int Handler::LBLParse(char* param_1)
{
    ShowError("Parser:: Must define an LBLParse func");
    return 0;
}

/* Function start: 0x4010E0 */
void Parser::OnProcessStart() {}

/* Function start: 0x4010F0 */
void Parser::OnProcessEnd() {}

/* Function start: 0x401100 */
Handler::~Handler() {
}

/* Function start: 0x401150 */
int Handler::ShutDown(SC_Message* msg)
{
    return 0;
}

/* Function start: 0x401160 */
int Handler::Exit(SC_Message* msg)
{
    return 0;
}

/* Function start: 0x401170 */
void Handler::OnInput(SC_Message* msg)
{
    WriteToMessageLog("hIam %d", targetAddress);
}

/* Function start: 0x401220 */
Handler12::~Handler12() {
}

/* Function start: 0x401290 */
void Handler12::Init(SC_Message* msg) {
    Handler::Init(msg);
    if (msg != 0) {
        sourceAddress = msg->sourceAddress;
    }
}

/* Function start: 0x4012C0 */
int Handler12::ShutDown(SC_Message* msg) {
    return 0;
}

/* Function start: 0x4012D0 */
void Handler12::Update(int param_1, int param_2) {
    if (timer.Update() > 10000) {
        SC_Message_Send(3, targetAddress, targetAddress, sourceAddress, 0x14, 0, 0, 0, 0, 0);
    }
    if (targetAddress == param_2) {
        ShowError("SC_ZBuffer::Update");
    }
}

/* Function start: 0x401330 */
int Handler12::AddMessage(SC_Message* msg) {
    Handler::AddMessage(msg);
    ShowError("SC_ZBuffer::AddMessage");
    return 1;
}

/* Function start: 0x401350 */
int Handler12::Exit(SC_Message* msg) {
    if (msg->targetAddress != targetAddress) {
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
}

/* Function start: 0x401710 */
/* Function start: 0x401790 */
/* Function start: 0x401810 */
void* ZBQueue::Pop()
{
    return LinkedList::RemoveCurrent();
}
