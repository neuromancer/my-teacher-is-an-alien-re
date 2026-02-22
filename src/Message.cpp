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
    PooledEvent** pTail;
    TimedEventPool* pPool;
    PooledEvent* pOldTail;
    SC_Message* pEmbedded;
    int count;
    int tmp;
    SC_Message* pSource;
    PooledEvent* pNode;
    PooledEvent** pFreeList;
    int* pPoolSize;
    char* pNewPool;
    PooledEvent* pEntry;
    unsigned int i;

    if (g_TimedEventPool2_00436988 != 0) {
        SC_Message local_dc(targetAddress, sourceAddress, command, data, priority, param1, param2, userPtr, param9, param10);
        pSource = &local_dc;

        pPool = g_TimedEventPool2_00436988;
        pOldTail = g_TimedEventPool2_00436988->list.tail;
        pTail = &g_TimedEventPool2_00436988->list.tail;
        pFreeList = &g_TimedEventPool2_00436988->m_free_list;

        if (*pFreeList == 0) {
            pPoolSize = &g_TimedEventPool2_00436988->m_pool_size;
            pNewPool = new char[g_TimedEventPool2_00436988->m_pool_size * 200 + 4];
            *(PooledEvent**)pNewPool = pPool->m_pool;
            pPool->m_pool = (PooledEvent*)pNewPool;
            count = *pPoolSize;
            pEntry = (PooledEvent*)(pNewPool + count * 200 - 196);
            count--;
            for (; count >= 0; count--) {
                pEntry->next = *pFreeList;
                *pFreeList = pEntry;
                pEntry = (PooledEvent*)((char*)pEntry - 200);
            }
        }
        pNode = *pFreeList;
        *pFreeList = pNode->next;
        pNode->prev = pOldTail;
        pNode->next = 0;
        pPool->m_count = pPool->m_count + 1;

        pEmbedded = (SC_Message*)&pNode->field_0x8;
        memset(pEmbedded, 0, 0xC0);

        count = 0;
        do {
            if (pEmbedded != 0) {
                pEmbedded->SC_Message::SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            }
            pEmbedded = (SC_Message*)((char*)pEmbedded + 0xC0);
            tmp = count;
            count--;
        } while (tmp != 0);

        pEmbedded = (SC_Message*)&pNode->field_0x8;
        pEmbedded->m_subObject = pSource->m_subObject;
        pEmbedded->isProcessingKey = pSource->isProcessingKey;

        i = 0;
        do {
            pEmbedded->currentKey[i] = pSource->currentKey[i];
            i++;
        } while (i < 0x20);

        pEmbedded->lineNumber = pSource->lineNumber;
        {
            int savedPos = pSource->savedFilePos;
            int field3c = pSource->field_0x3c;
            pEmbedded->savedFilePos = savedPos;
            pEmbedded->field_0x3c = field3c;
        }

        i = 0;
        do {
            pEmbedded->filename[i] = pSource->filename[i];
            i++;
        } while (i < 0x40);

        pEmbedded->pFile = pSource->pFile;
        pEmbedded->targetAddress = pSource->targetAddress;
        pEmbedded->sourceAddress = pSource->sourceAddress;
        pEmbedded->command = pSource->command;
        pEmbedded->data = pSource->data;
        pEmbedded->priority = pSource->priority;
        pEmbedded->param1 = pSource->param1;
        pEmbedded->param2 = pSource->param2;
        pEmbedded->clickPos.x = pSource->clickPos.x;
        pEmbedded->clickPos.y = pSource->clickPos.y;
        pEmbedded->mouseX = pSource->mouseX;
        pEmbedded->mouseY = pSource->mouseY;
        pEmbedded->lastKey = pSource->lastKey;
        pEmbedded->time = pSource->time;
        pEmbedded->userPtr = pSource->userPtr;

        if (*pTail == 0) {
            pPool->list.head = pNode;
        }
        else {
            (*pTail)->next = pNode;
        }
        *pTail = pNode;
    }
}

// Message::Message removed - the actual constructor is SC_Message::SC_Message at 0x4198C0
