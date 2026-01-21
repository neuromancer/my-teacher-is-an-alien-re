#include "SC_Question.h"
#include "Message.h"
#include <string.h>
#include "Memory.h"
#include "TimedEvent.h"

extern "C" {
    extern TimedEventPool* g_TimedEventPool2_00436988;
}

/* Function start: 0x41A150 */
void SC_Message_Send(int targetAddress, int sourceAddress, int command, int data,
    int priority, int param1, int param2, int userPtr,
    int param9, int param10)
{
    SC_Message local_dc(targetAddress, sourceAddress, command, data, priority, param1, param2, userPtr, param9, param10);
    PooledEvent** pTail;
    TimedEventPool* pPool;
    PooledEvent* pOldTail;
    PooledEvent* pMsg;
    int count;
    int tmp;
    SC_Message* pSource;
    PooledEvent* pNode;
    PooledEvent** pFreeList;
    int* pPoolSize;
    int* pNewPool;
    PooledEvent* pEntry;
    unsigned int i;

    if (g_TimedEventPool2_00436988 != 0) {
        pSource = &local_dc;

        pPool = g_TimedEventPool2_00436988;
        pOldTail = g_TimedEventPool2_00436988->list.tail;
        pTail = &g_TimedEventPool2_00436988->list.tail;
        pFreeList = &g_TimedEventPool2_00436988->m_free_list;

        if (*pFreeList == 0) {
            pPoolSize = &g_TimedEventPool2_00436988->m_pool_size;
            pNewPool = (int*)AllocateMemory(g_TimedEventPool2_00436988->m_pool_size * 200 + 4);
            *pNewPool = (int)pPool->m_pool;
            pPool->m_pool = (PooledEvent*)pNewPool;
            count = *pPoolSize;
            pEntry = (PooledEvent*)((char*)pNewPool + count * 200 - 196);
            count--;
            for (; -1 < count; count--) {
                *(PooledEvent**)pEntry = *pFreeList;
                *pFreeList = pEntry;
                pEntry = (PooledEvent*)((char*)pEntry - 200);
            }
        }
        pNode = *pFreeList;
        *pFreeList = (PooledEvent*)pNode->next;
        pNode->prev = pOldTail;
        pNode->next = 0;
        pPool->m_count = pPool->m_count + 1;

        pMsg = (PooledEvent*)&pNode->field_0x8;
        memset(pMsg, 0, 0x30 * sizeof(int));

        count = 0;
        do {
            if (pMsg != 0) {
                ((SC_Message*)pMsg)->SC_Message::SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            }
            pMsg = (PooledEvent*)((int*)pMsg + 0x30);
            tmp = count;
            count--;
        } while (tmp != 0);

        pNode->field_0x8 = (int)pSource->m_subObject;
        pNode->m_duration = pSource->isProcessingKey;

        i = 0;
        do {
            ((char*)pNode)[i + 0x18] = pSource->currentKey[i];
            i++;
        } while (i < 0x20);

        pNode->field_0x38 = pSource->lineNumber;
        tmp = pSource->field_0x3c;
        *(int*)pNode->m_data_0x40 = pSource->savedFilePos;
        *(int*)(pNode->m_data_0x40 + 4) = tmp;

        i = 0;
        do {
            ((char*)pNode)[i + 0x48] = pSource->filename[i];
            i++;
        } while (i < 0x40);

        pNode->targetAddress = (int)pSource->pFile;
        pNode->command = pSource->targetAddress;
        pNode->data = pSource->sourceAddress;
        pNode->priority = pSource->command;
        pNode->param1 = pSource->data;
        pNode->param2 = pSource->priority;
        pNode->clickX = pSource->param1;
        pNode->clickY = pSource->param2;
        pNode->mouseX = pSource->clickX;
        pNode->mouseY = pSource->clickY;
        pNode->field_0xb4 = pSource->mouseX;
        pNode->field_0xb8 = pSource->mouseY;
        pNode->userPtr = pSource->field_b4;
        pNode->field_0xc0 = pSource->field_b8;
        pNode->field_0xc4 = pSource->userPtr;

        if (*pTail == 0) {
            pPool->list.head = pNode;
        }
        else {
            *(PooledEvent**)(*pTail) = pNode;
        }
        *pTail = pNode;
    }
}

// Message::Message removed - the actual constructor is SC_Message::SC_Message at 0x4198C0
