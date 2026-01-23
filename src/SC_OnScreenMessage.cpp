#include "SC_OnScreenMessage.h"
#include "Memory.h"
#include "Message.h"
#include "OnScreenMessage.h"

extern "C" {
    void ShowError(const char*, ...);
}


/* Function start: 0x40A2E0 */
SC_OnScreenMessage::SC_OnScreenMessage()
{
    MessageList* list;

    targetAddress = 0xf;

    timer.Reset();

    list = (MessageList*)AllocateMemory(16);
    m_messageList = 0;
    if (list != 0) {
        list->flags = 1;
        list->head = 0;
        list->tail = 0;
        list->current = list->head;
        m_messageList = list;
    }
}

/* Function start: 0x40A3F0 */
void SC_OnScreenMessage::Destroy(int free)
{
    SC_OnScreenMessage::~SC_OnScreenMessage();
    if (free & 1) {
        FreeMemory(this);
    }
}

/* Function start: 0x40A410 */
SC_OnScreenMessage::~SC_OnScreenMessage()
{
    MessageList* list;
    MessageNode* node;
    void* data;

    list = m_messageList;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                node = (MessageNode*)list->current;
                if (node == 0) {
                    data = 0;
                }
                else {
                    if (list->head == node) {
                        list->head = node->next;
                    }
                    if (list->tail == list->current) {
                        list->tail = node->prev;
                    }
                    if (node->prev != 0) {
                        node->prev->next = node->next;
                    }
                    if (node->next != 0) {
                        node->next->prev = node->prev;
                    }
                    data = 0;
                    if (node != 0) {
                        data = node->data;
                        delete node;
                        list->current = 0;
                    }
                    list->current = list->head;
                }
                if (data != 0) {
                    Timer_DecrementCounter();
                    delete (char*)data;
                }
            }
        }
        delete list;
        m_messageList = 0;
    }
}

/* Function start: 0x40A5A0 */
void SC_OnScreenMessage::Copy(SC_OnScreenMessage* other)
{
    CopyCommandData(other);
    if (other) {
        sourceAddress = other->sourceAddress;
    }
}

/* Function start: 0x40A5E0 */
void SC_OnScreenMessage::Update(int param_1, int param_2)
{
    MessageList* list;
    MessageNode* node;
    int iVar1;
    void* this_00;
    int local_1c;
    void* local_18;
    unsigned int uVar6;

    list = m_messageList;
    list->current = list->head;
    local_1c = 0;
    iVar1 = (int)list->head;
    while (iVar1 != 0) {
        this_00 = 0;
        iVar1 = (int)list->current;
        if (iVar1 != 0) {
            this_00 = ((MessageNode*)iVar1)->data;
        }
        if (((OnScreenMessage*)this_00)->Update(local_1c)) {
            list = m_messageList;
            iVar1 = (int)list->current;
            if (iVar1 == 0) {
                local_18 = 0;
            }
            else {
                if (list->head == (void*)iVar1) {
                    list->head = ((MessageNode*)iVar1)->next;
                }
                if (list->tail == list->current) {
                    list->tail = ((MessageNode*)iVar1)->prev;
                }
                if (((MessageNode*)iVar1)->prev != 0) {
                    ((MessageNode*)iVar1)->prev->next = ((MessageNode*)iVar1)->next;
                }
                node = ((MessageNode*)iVar1)->next;
                if (node != 0) {
                    node->prev = ((MessageNode*)iVar1)->prev;
                }
                node = (MessageNode*)iVar1;
                if (node == 0) {
                    local_18 = 0;
                }
                else {
                    local_18 = node->data;
                    delete node;
                    list->current = 0;
                }
                list->current = list->head;
            }
            if (local_18 != 0) {
                delete (OnScreenMessage*)local_18;
            }
        }
        local_1c = local_1c + 1;
        iVar1 = (int)list->head;
        if ((list->tail == list->current) || (10 < local_1c)) break;
        if (list->current != 0) {
            list->current = ((MessageNode*)list->current)->next;
        }
        iVar1 = (int)list->head;
    }
    uVar6 = timer.Update();
    if (60000 < uVar6) {
        SC_Message_Send(3, targetAddress, targetAddress, sourceAddress, 0x14, 0, 0, 0, 0, 0);
    }
    if (targetAddress == param_2) {
        ShowError("SC_OnScreenMessage::Update: m_address == param_2");
    }
}
