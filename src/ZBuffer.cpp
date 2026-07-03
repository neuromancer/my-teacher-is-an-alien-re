#include "ZBuffer.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Projectile.h"
#include "RockThrower.h"
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
#include "SC_Question.h"

extern void FreeMemory(void*);

/* Function start: 0x427710 */
ZBuffer::~ZBuffer()
{
    while (ZBuffer::itemCount != 0) {
        int idx = ZBuffer::itemCount - 1;
        ZBuffer::itemCount = idx;
        Projectile* item = ZBuffer::items[idx];
        if (item != 0) {
            delete item;

            ZBuffer::items[ZBuffer::itemCount] = 0;
        }
    }
    if (ZBuffer::items != 0) {
        operator delete(ZBuffer::items);
        ZBuffer::items = 0;
    }
}

/* Function start: 0x44B5A0 */
int ZBuffer::Exit(SC_MessageParser* msg)
{
    return 0;
}

/* Function start: 0x401C80 */
void ZBuffer::CleanUpVBuffer()
{
    VBuffer* vb = m_vbuffer;
    if (vb != 0) {
        delete vb;
        m_vbuffer = 0;
    }
    Animation* ptr = (Animation*)field_0x08;
    if (ptr != 0) {
        delete ptr;
        field_0x08 = 0;
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

/* Function start: 0x4279A0 */
void ZBuffer::ResetItems()
{
    int i = 0;
    if (0 < itemCount) {
        int offset = 0;
        do {
            offset = offset + 4;
            i = i + 1;
            // Original walks a byte offset and re-reads the items field every
            // iteration; only the element access is expressible at field level.
            (*(Projectile**)((char*)items + offset - 4))->active = 0;
        } while (i < itemCount);
    }
}

/* Function start: 0x403520 */
/* Function start: 0x403620 */
/* Function start: 0x42BC50 */
/* Function start: 0x4036A0 */
/* Function start: 0x4035A0 */
void* ZBQueue::Pop()
{
    return LinkedList::RemoveCurrent();
}
