#include "SC_OnScreenMessage.h"
#include "string.h"

extern "C" {
    extern int OnScreenMessage_Update(void*, int);
    extern void SC_OnScreenMessage_AddMessage();
    extern void FreeFromGlobalHeap(void*);
    extern void SC_Message_Send(int, int, int, int, int, int, int, int, int, int);
    //extern void Timer_DecrementCounter();
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
                        FreeFromGlobalHeap(puVar3);
                        piVar2->m_current = 0;
                    }
                    piVar2->m_current = piVar2->m_head;
                }
                if (local_18 != (void*)0x0) {
                    SC_OnScreenMessage_AddMessage();
                    FreeFromGlobalHeap(local_18);
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
        SC_Message_Send(3, field_0x88, field_0x88, field_0x8c, 0x14, 0, 0, 0, 0, 0);
    }
    if (field_0x88 == param_2) {
        ShowError("SC_OnScreenMessage::Update: field_0x88 == param_2");
    }
}