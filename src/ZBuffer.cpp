#include "ZBuffer.h"
#include "globals.h"
#include "Message.h"

char s_MustDefine[] = "Parser:: Must define an LBLParse func";
char s_hIam[] = "hIam %d";

#include "string.h"
#include <stdlib.h>
#include "VBuffer.h"
#include "Memory.h"
#include "Timer.h"
#include "string.h"

// ZBQueue is defined in ZBufferManager.h
#include "ZBufferManager.h"

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
                void* data = ZBuffer::PopNode(queue);
                if (data != 0) {
                    *(void**)data = 0;
                    FreeFromGlobalHeap(data);
                }
            }
        }
        queue = g_ZBufferManager_0043698c->m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                ZBuffer* zb = (ZBuffer*)ZBuffer::PopNode_2(queue);
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
                void* data = ZBuffer::PopNode_2(queue);
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
                void* data = ZBuffer::PopNode(queue);
                if (data != 0) {
                    *(void**)data = 0;
                    FreeFromGlobalHeap(data);
                }
            }
        }
        queue = g_ZBufferManager_0043698c->m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                ZBuffer* zb = (ZBuffer*)ZBuffer::PopNode_2(queue);
                if (zb != 0) {
                    zb->CleanUpVBuffer();
                    FreeFromGlobalHeap(zb);
                }
            }
        }
        ZBuffer::ClearList(g_ZBufferManager_0043698c->m_queue9c);
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
    Parser* ptr = (Parser*)m_subObject;
    if (ptr != 0) {
        delete ptr;
        m_subObject = 0;
    }
}

/* Function start: 0x401560 */
void ZBuffer::ClearList(ZBQueue* queue)
{
    if (queue->head != 0) {
        queue->current = queue->head;
        do {
            ZBQueueNode* node = queue->current;
            void* data;
            if (node == 0) {
                data = 0;
            } else {
                if (queue->head == node) {
                    queue->head = node->prev;
                }
                if (queue->tail == node) {
                    queue->tail = node->next;
                }
                if (node->next != 0) {
                    node->next->prev = node->prev;
                }
                ZBQueueNode* prevNode = queue->current->prev;
                if (prevNode != 0) {
                    prevNode->next = queue->current->next;
                }
                data = 0;
                ZBQueueNode* currentNode = queue->current;
                if (currentNode != 0) {
                    data = currentNode->data;
                    delete currentNode;
                    queue->current = 0;
                }
                queue->current = queue->head;
            }
            if (data != 0) {
                *(void**)data = 0;
                FreeFromGlobalHeap(data);
            }
        } while (queue->head != 0);
    }
}

/* Function start: 0x4016A0 */
ZBuffer::~ZBuffer()
{
    CleanUpVBuffer();
}

/* Function start: 0x401710 */
void* ZBuffer::PopNode(ZBQueue* queue)
{
    ZBQueueNode* node = queue->current;
    if (node == 0) {
        return 0;
    }
    if (queue->head == node) {
        queue->head = node->prev;
    }
    if (queue->tail == node) {
        queue->tail = node->next;
    }
    if (node->next != 0) {
        node->next->prev = node->prev;
    }
    ZBQueueNode* prevNode = queue->current->prev;
    if (prevNode != 0) {
        prevNode->next = queue->current->next;
    }
    ZBQueueNode* currentNode = queue->current;
    void* data = 0;
    if (currentNode != 0) {
        data = currentNode->data;
        delete currentNode;
        queue->current = 0;
    }
    queue->current = queue->head;
    return data;
}

/* Function start: 0x401790 */
void* ZBuffer::PopNode_2(ZBQueue* queue)
{
    ZBQueueNode* node = queue->current;
    if (node == 0) {
        return 0;
    }
    if (queue->head == node) {
        queue->head = node->prev;
    }
    if (queue->tail == node) {
        queue->tail = node->next;
    }
    if (node->next != 0) {
        node->next->prev = node->prev;
    }
    ZBQueueNode* prevNode = queue->current->prev;
    if (prevNode != 0) {
        prevNode->next = queue->current->next;
    }
    ZBQueueNode* currentNode = queue->current;
    void* data = 0;
    if (currentNode != 0) {
        data = currentNode->data;
        delete currentNode;
        queue->current = 0;
    }
    queue->current = queue->head;
    return data;
}

/* Function start: 0x401810 */
void* ZBQueue::PopNode()
{
    ZBQueueNode* node;
    ZBQueueNode* prevNode;
    void* data;

    node = current;
    if (node == 0) {
        return 0;
    }
    if (head == node) {
        head = node->prev;
    }
    if (tail == node) {
        tail = node->next;
    }
    if (node->next != 0) {
        node->next->prev = node->prev;
    }
    prevNode = current->prev;
    if (prevNode != 0) {
        prevNode->next = current->next;
    }
    prevNode = current;
    data = 0;
    if (prevNode != 0) {
        data = prevNode->data;
    }
    if (prevNode != 0) {
        delete prevNode;
        current = 0;
    }
    current = head;
    return data;
}
