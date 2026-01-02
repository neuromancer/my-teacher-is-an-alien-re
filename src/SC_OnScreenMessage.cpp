#include "SC_OnScreenMessage.h"
#include "string.h"
#include "Memory.h"
#include "Message.h"

extern "C" {
    extern int OnScreenMessage_Update(void*, int);
    extern void SC_OnScreenMessage_AddMessage();
    // SC_Message_Send is C++ from Message.h
    //extern void Timer_DecrementCounter();
}


extern "C" {
    extern void Timer_DecrementCounter_wrapper();
    extern void Timer_DecrementCounter_wrapper_2();
    extern void GameState_dtor_wrapper_4();
}

/* Function start: 0x40A5A0 */
void SC_OnScreenMessage::Copy(SC_OnScreenMessage* other)
{
    this->CopyCommandData(other);
    if (other) {
        this->sourceAddress = other->sourceAddress;
    }
}

/* Function start: 0x40A2E0 */
SC_OnScreenMessage::SC_OnScreenMessage()
{
    int* p = &this->targetAddress;
    for (int i = 0; i < 6; i++) {
        *p++ = 0;
    }

    this->targetAddress = 0xf;

    timer.Reset();

    this->messageQueue.m_head = AllocateMemory(16);
    if (this->messageQueue.m_head) {
        QueueNode* node = (QueueNode*)this->messageQueue.m_head;
        node->next = 0;
        node->prev = 0;
        node->data = 0;
    }
}

/* Function start: 0x40A3F0 */
void SC_OnScreenMessage::Destroy(int free)
{
    this->Free();
    if (free) {
        FreeMemory(this);
    }
}

/* Function start: 0x40A410 */
void SC_OnScreenMessage::Free()
{
    Queue* queue = &this->messageQueue;

    try {
        queue->m_current = queue->m_head;
        while (queue->m_current) {
            void* data = ((QueueNode*)queue->m_current)->data;

            if (queue->m_head == queue->m_current) {
                queue->m_head = ((QueueNode*)queue->m_current)->next;
            }

            if (queue->m_tail == queue->m_current) {
                queue->m_tail = ((QueueNode*)queue->m_current)->prev;
            }

            if (((QueueNode*)queue->m_current)->prev) {
                ((QueueNode*)((QueueNode*)queue->m_current)->prev)->next = ((QueueNode*)queue->m_current)->next;
            }

            if (((QueueNode*)queue->m_current)->next) {
                ((QueueNode*)((QueueNode*)queue->m_current)->next)->prev = ((QueueNode*)queue->m_current)->prev;
            }

            if (data) {
                Timer_DecrementCounter_wrapper();
                FreeFromGlobalHeap(data);
            }

            FreeFromGlobalHeap(queue->m_current);
            queue->m_current = queue->m_head;
        }
    } catch (...) {
        // The original code has an empty catch block
    }

    if (queue->m_head) {
        FreeFromGlobalHeap(queue->m_head);
        queue->m_head = 0;
    }

    Timer_DecrementCounter_wrapper_2();
    GameState_dtor_wrapper_4();
}

/* Function start: 0x40A5E0 */
void SC_OnScreenMessage::Update(int param_1, int param_2)
{
    int iVar1;
    Queue* piVar2;
    QueueNode* puVar3;
    int iVar4;
    int bVar5;
    unsigned int uVar6;
    void* this_00;
    int local_1c;
    void* local_18;

    try {
        piVar2 = &this->messageQueue;
        piVar2->m_current = piVar2->m_head;
        local_1c = 0;
        iVar1 = (int)piVar2->m_head;
        while (iVar1 != 0) {
            this_00 = (void*)0x0;
            iVar1 = (int)piVar2->m_current;
            if (iVar1 != 0) {
                this_00 = ((QueueNode*)iVar1)->data;
            }
            bVar5 = OnScreenMessage_Update(this_00, local_1c);
            if (bVar5) {
                piVar2 = &this->messageQueue;
                iVar1 = (int)piVar2->m_current;
                if (iVar1 == 0) {
                    local_18 = (void*)0x0;
                }
                else {
                    if (piVar2->m_head == (void*)iVar1) {
                        piVar2->m_head = ((QueueNode*)iVar1)->next;
                    }
                    if (piVar2->m_tail == piVar2->m_current) {
                        piVar2->m_tail = ((QueueNode*)iVar1)->prev;
                    }
                    iVar4 = (int)((QueueNode*)iVar1)->prev;
                    if (iVar4 != 0) {
                        ((QueueNode*)iVar4)->next = ((QueueNode*)iVar1)->next;
                    }
                    puVar3 = ((QueueNode*)iVar1)->next;
                    if (puVar3 != (QueueNode*)0x0) {
                        puVar3->prev = ((QueueNode*)iVar1)->prev;
                    }
                    puVar3 = (QueueNode*)iVar1;
                    if (puVar3 == (QueueNode*)0x0) {
                        local_18 = (void*)0x0;
                    }
                    else {
                        local_18 = puVar3->data;
                        puVar3->data = 0;
                        puVar3->next = 0;
                        puVar3->prev = 0;
                        FreeMemory(puVar3);
                        piVar2->m_current = 0;
                    }
                    piVar2->m_current = piVar2->m_head;
                }
                if (local_18 != (void*)0x0) {
                    SC_OnScreenMessage_AddMessage();
                    FreeMemory(local_18);
                }
            }
            local_1c = local_1c + 1;
            iVar1 = (int)piVar2->m_head;
            iVar4 = (int)piVar2->m_current;
            if ((piVar2->m_tail == (void*)iVar4) || (10 < local_1c)) break;
            if (iVar4 != 0) {
                piVar2->m_current = ((QueueNode*)iVar4)->next;
            }
            iVar1 = (int)piVar2->m_head;
        }
    }
    catch (...) {
    }
    uVar6 = timer.Update();
    if (60000 < uVar6) {
        SC_Message_Send(3, targetAddress, targetAddress, sourceAddress, 0x14, 0, 0, 0, 0, 0);
    }
    if (targetAddress == param_2) {
        ShowError("SC_OnScreenMessage::Update: m_address == param_2");
    }
}
