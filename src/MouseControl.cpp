#include "MouseControl.h"

extern "C" {
    void* AllocateMemory_Wrapper(int size);
    void FreeFromGlobalHeap(void*);
    void SpriteArray_Cleanup(void*);
    void FUN_0041f471();
}

/* Function start: 0x41F280 */
MouseControl::MouseControl()
{
    vtable = (void**)0x431558;
    field_0x4[30] = 0;
    field_0x4[31] = 0;
    field_0x4[32] = 0;
    field_0x4[33] = 0;
    field_0x4[32] = 1;
    Queue* queue = (Queue*)AllocateMemory_Wrapper(0x10);
    if (queue != 0) {
        queue->m_head = 0;
        queue->m_tail = 0;
        queue->m_current = 0;
        queue->m_field_0xc = 2;
    }
    m_queue = queue;
    field_0x4[30] = 0x54;
}

/* Function start: 0x41F360 */
MouseControl::~MouseControl()
{
    vtable = (void**)0x431558;
    if (m_queue != 0) {
        m_queue->Destroy(1);
        FreeFromGlobalHeap(m_queue);
        m_queue = 0;
    }
    FUN_0041f471();
}
