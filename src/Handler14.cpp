#include "Handler14.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"
#include "SC_OnScreenMessage.h"
#include "SC_Question.h"
#include "SoundItem.h"

/* Function start: 0x40B7E0 */
Handler14::Handler14() {
    // Set handlerId to 14
    handlerId = 14;

    // Call Timer::Reset() on timer
    timer.Reset();

    // Allocate and initialize list header structure
    list = new MessageList();
}

/* Function start: 0x40B910 */
Handler14::~Handler14() {
    MessageList* pList;
    MessageNode* node;
    SoundItem* data;

    pList = list;
    if (pList != 0) {
        if (pList->head != 0) {
            pList->current = pList->head;
            while (pList->head != 0) {
                node = (MessageNode*)pList->current;
                if (node == 0) {
                    data = 0;
                } else {
                    // Unlink node from head
                    if (pList->head == node) {
                        pList->head = node->next;
                    }
                    // Unlink node from tail
                    if (pList->tail == pList->current) {
                        pList->tail = ((MessageNode*)pList->current)->prev;
                    }
                    // Update next node's prev pointer
                    node = (MessageNode*)pList->current;
                    if (node->prev != 0) {
                        node->prev->next = node->next;
                    }
                    // Update prev node's next pointer
                    node = (MessageNode*)pList->current;
                    if (node->next != 0) {
                        node->next->prev = node->prev;
                    }
                    // Get data and free node
                    node = (MessageNode*)pList->current;
                    data = 0;
                    if (node != 0) {
                        data = (SoundItem*)node->data;
                        node->data = 0;
                        node->prev = 0;
                        node->next = 0;
                        FreeMemory(node);
                        pList->current = 0;
                    }
                    pList->current = pList->head;
                }
                // Cleanup data object (SoundItem)
                if (data != 0) {
                    delete data;
                }
            }
        }
        FreeMemory(pList);
        list = 0;
    }
}

/* Function start: 0x40BAD0 */
void Handler14::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        field_8C = msg->data;
    }
}

/* Function start: 0x40BB10 */
void Handler14::Draw(int param1, int param2) {
    MessageList* pList;
    MessageNode* node;
    unsigned int uVar3;
    SoundItem* soundItem;
    SoundItem* data;
    int iVar4;

    uVar3 = timer.Update();
    if ((uVar3 > 60000) && (list->head == 0)) {
        SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
    }

    pList = list;
    if (pList != 0) {
        pList->current = pList->head;
        iVar4 = (int)pList->head;

        while (iVar4 != 0) {
            soundItem = 0;
            node = (MessageNode*)pList->current;
            if (node != 0) {
                soundItem = (SoundItem*)node->data;
            }

            iVar4 = soundItem->IsFinished();
            if (iVar4 != 0) {
                node = (MessageNode*)pList->current;
                if (node == 0) {
                    data = 0;
                } else {
                    // Unlink node from head
                    if (pList->head == node) {
                        pList->head = node->next;
                    }
                    // Unlink node from tail
                    if (pList->tail == pList->current) {
                        pList->tail = ((MessageNode*)pList->current)->prev;
                    }
                    // Update next node's prev pointer
                    node = (MessageNode*)pList->current;
                    if (node->prev != 0) {
                        node->prev->next = node->next;
                    }
                    // Update prev node's next pointer
                    node = (MessageNode*)pList->current;
                    if (node->next != 0) {
                        node->next->prev = node->prev;
                    }
                    // Get data and free node
                    node = (MessageNode*)pList->current;
                    data = 0;
                    if (node != 0) {
                        data = (SoundItem*)node->data;
                        node->data = 0;
                        node->prev = 0;
                        node->next = 0;
                        FreeMemory(node);
                        pList->current = 0;
                    }
                    pList->current = pList->head;
                }
                // Cleanup data object (SoundItem)
                if (data != 0) {
                    delete data;
                }
            }

            node = (MessageNode*)pList->current;
            if (pList->tail == node) {
                break;
            }
            if (node != 0) {
                pList->current = node->next;
            }
            iVar4 = (int)pList->head;
        }
    }

    if (handlerId == param2) {
        WriteToMessageLog("SC_Sound::Update");
    }
}

/* Function start: 0x40BD10 */
int Handler14::HandleMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    WriteToMessageLog("SC_Sound::AddMessage");
    return 1;
}

int Handler14::Update(SC_Message* msg) {
    return 0;
}

/* Function start: 0x40BD30 */
int Handler14::Exit(SC_Message* msg) {
    MessageList* pList;
    MessageNode* node;
    SoundItem* eventData;
    SoundItem* data;
    void* pvVar7;
    int iVar1;

    if (msg->targetAddress != handlerId) {
        return 0;
    }

    timer.Reset();

    switch (msg->priority) {
    case 3:
        pvVar7 = FindOrCreateSound(msg->data);
        ((SoundItem*)pvVar7)->Start();
        break;

    case 0xf:
        // Clear entire list
        pList = list;
        if (pList->head != 0) {
            pList->current = pList->head;
            while (pList->head != 0) {
                data = (SoundItem*)pList->PopCurrent();
                if (data != 0) {
                    delete data;
                }
            }
        }
        break;

    case 0x10:
        pvVar7 = FindOrCreateSound(msg->data);
        ((SoundItem*)pvVar7)->AdjustVolume(10);
        break;

    case 0x11:
        pvVar7 = FindOrCreateSound(msg->data);
        ((SoundItem*)pvVar7)->AdjustVolume(-10);
        break;

    case 0x12:
        pvVar7 = FindOrCreateSound(msg->data);
        ((SoundItem*)pvVar7)->SetVolume(msg->param1);
        break;

    case 0x13:
        FindOrCreateSound(msg->data);
        break;

    case 0x14:
        // Remove sound by ID
        pList = list;
        pList->current = pList->head;
        iVar1 = (int)pList->head;
        while (iVar1 != 0) {
            node = (MessageNode*)pList->current;
            data = 0;
            if (node != 0) {
                data = (SoundItem*)node->data;
            } else {
                data = (SoundItem*)0x18;  // Null pointer access pattern
            }
            if (data->soundId == msg->data) {
                node = (MessageNode*)pList->current;
                if (node == 0) {
                    eventData = 0;
                } else {
                    // Unlink node from head
                    if (pList->head == node) {
                        pList->head = node->next;
                    }
                    // Unlink node from tail
                    if (pList->tail == pList->current) {
                        pList->tail = ((MessageNode*)pList->current)->prev;
                    }
                    // Update next node's prev pointer
                    node = (MessageNode*)pList->current;
                    if (node->prev != 0) {
                        node->prev->next = node->next;
                    }
                    // Update prev node's next pointer
                    node = (MessageNode*)pList->current;
                    if (node->next != 0) {
                        node->next->prev = node->prev;
                    }
                    eventData = (SoundItem*)pList->GetCurrentData();
                    if (pList->current != 0) {
                        ((MessageNode*)pList->current)->Destroy(1);
                        pList->current = 0;
                    }
                    pList->current = pList->head;
                }
                // Cleanup data object
                if (eventData != 0) {
                    delete eventData;
                }
            }
            node = (MessageNode*)pList->current;
            if (pList->tail == node) {
                break;
            }
            if (node != 0) {
                pList->current = node->next;
            }
            iVar1 = (int)pList->head;
        }
        break;

    case 0x1a:
        pvVar7 = FindOrCreateSound(msg->data);
        ((SoundItem*)pvVar7)->Resume();
        break;

    case 0x1b:
        SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
        break;

    default:
        return 0;
    }

    return 1;
}

/* Function start: 0x40C0E0 */
SoundItem* Handler14::FindOrCreateSound(int soundId)
{
    MessageList* pList;
    MessageNode* currNode;
    SoundItem* foundItem;
    void* allocResult;
    SoundItem* newItem;
    MessageNode* newNode;
    int currData;

    pList = Handler14::list;
    currData = (int)pList->head;
    if (currData != 0) {
        pList->current = pList->head;
        currData = (int)pList->head;
        while (currData != 0) {
            currNode = (MessageNode*)pList->current;
            currData = 0x18;
            if (currNode != 0) {
                currData = ((SoundItem*)currNode->data)->soundId;
            }
            if (soundId == currData) {
                currNode = (MessageNode*)pList->current;
                if (currNode == 0) {
                    foundItem = 0;
                } else {
                    foundItem = (SoundItem*)currNode->data;
                }
                return foundItem;
            }
            if (pList->tail == pList->current) {
                break;
            }
            if (currNode != 0) {
                pList->current = currNode->next;
            }
            currData = (int)pList->head;
        }
    }

    // Not found - allocate new SoundItem
    allocResult = operator new(0x20);
    newItem = 0;
    if (allocResult != 0) {
        newItem = (SoundItem*)allocResult;
        newItem->SoundItem::SoundItem(soundId);
    }

    if (newItem == 0) {
        ShowError("queue fault 0101");
    }

    pList->current = pList->head;

    // Check list type for insertion method
    if (pList->flags == 1 || pList->flags == 2) {
        if (pList->head == 0) {
            if (newItem == 0) {
                ShowError("queue fault 0102");
            }

            allocResult = operator new(0xc);
            newNode = 0;
            if (allocResult != 0) {
                newNode = ((MessageNode*)allocResult)->Init(newItem);
            }

            if (pList->current == 0) {
                pList->current = pList->head;
            }

            if (pList->head == 0) {
                pList->head = newNode;
                pList->tail = newNode;
                pList->current = newNode;
            } else {
                newNode->prev = (MessageNode*)pList->current;
                newNode->next = ((MessageNode*)pList->current)->next;
                if (((MessageNode*)pList->current)->next == 0) {
                    pList->head = newNode;
                    ((MessageNode*)pList->current)->next = newNode;
                } else {
                    ((MessageNode*)pList->current)->next->prev = newNode;
                    ((MessageNode*)pList->current)->next = newNode;
                }
            }
        } else {
            // Priority insertion loop
            while (pList->current != 0) {
                currNode = (MessageNode*)pList->current;
                if (((SoundItem*)currNode->data)->soundId < newItem->soundId) {
                    if (newItem == 0) {
                        ShowError("queue fault 0102");
                    }

                    allocResult = operator new(0xc);
                    newNode = 0;
                    if (allocResult != 0) {
                        ((MessageNode*)allocResult)->data = newItem;
                        ((MessageNode*)allocResult)->prev = 0;
                        ((MessageNode*)allocResult)->next = 0;
                        newNode = (MessageNode*)allocResult;
                    }

                    if (pList->current == 0) {
                        pList->current = pList->head;
                    }

                    if (pList->head == 0) {
                        pList->head = newNode;
                        pList->tail = newNode;
                        pList->current = newNode;
                    } else {
                        newNode->prev = (MessageNode*)pList->current;
                        newNode->next = ((MessageNode*)pList->current)->next;
                        if (((MessageNode*)pList->current)->next == 0) {
                            pList->head = newNode;
                            ((MessageNode*)pList->current)->next = newNode;
                        } else {
                            ((MessageNode*)pList->current)->next->prev = newNode;
                            ((MessageNode*)pList->current)->next = newNode;
                        }
                    }
                    return newItem;
                }

                if (pList->tail == pList->current) {
                    if (newItem == 0) {
                        ShowError("queue fault 0112");
                    }

                    allocResult = operator new(0xc);
                    newNode = 0;
                    if (allocResult != 0) {
                        ((MessageNode*)allocResult)->data = newItem;
                        ((MessageNode*)allocResult)->prev = 0;
                        ((MessageNode*)allocResult)->next = 0;
                        newNode = (MessageNode*)allocResult;
                    }

                    if (pList->current == 0) {
                        pList->current = pList->tail;
                    }

                    if (pList->head == 0) {
                        pList->head = newNode;
                        pList->tail = newNode;
                        pList->current = newNode;
                    } else {
                        if (pList->tail == 0 || ((MessageNode*)pList->tail)->prev != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->prev = 0;
                        newNode->next = (MessageNode*)pList->tail;
                        ((MessageNode*)pList->tail)->prev = newNode;
                        pList->tail = newNode;
                    }
                    return newItem;
                }

                if (currNode != 0) {
                    pList->current = currNode->next;
                }
            }
        }
    } else {
        pList->InsertBeforeCurrent(newItem);
    }

    return newItem;
}
