#include "SC_Sound.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"
#include "SC_OnScreenMessage.h"
#include "SC_Question.h"
#include "SoundItem.h"

/* Function start: 0x40B7E0 */
SC_Sound::SC_Sound() {
    // Set handlerId to 14
    handlerId = 14;

    // Call Timer::Reset() on timer
    timer.Reset();

    // Allocate and initialize list header structure
    list = new MessageList();
}

/* Function start: 0x40B910 */
SC_Sound::~SC_Sound() {
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                MessageNode* node = (MessageNode*)list->current;
                SoundItem* data = 0;

                if (node != 0) {
                    if (list->head == node) {
                        list->head = node->next;
                    }
                    if (list->tail == node) {
                        list->tail = node->prev;
                    }
                    if (node->prev != 0) {
                        node->prev->next = node->next;
                    }
                    if (node->next != 0) {
                        node->next->prev = node->prev;
                    }

                    data = (SoundItem*)node->data;
                    node->data = 0;
                    node->prev = 0;
                    node->next = 0;
                    FreeMemory(node);
                    list->current = 0;
                }

                list->current = list->head;

                if (data != 0) {
                    data->SoundItem::~SoundItem();
                    FreeMemory(data);
                }
            }
        }
        FreeMemory(list);
        list = 0;
    }
}

/* Function start: 0x40BAD0 */
void SC_Sound::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        field_8C = msg->sourceAddress;
    }
}

/* Function start: 0x40BB10 */
void SC_Sound::Update(int param1, int param2) {
    if (timer.Update() > 60000) {
        if (list->head == 0) {
            SC_Message_Send(3, handlerId, handlerId, field_8C, 20, 0, 0, 0, 0, 0);
        }
    }

    if (list != 0) {
        list->current = list->head;
        if (list->head != 0) {
            do {
                SoundItem* soundItem = (SoundItem*)list->GetCurrentData();

                if (soundItem->IsFinished()) {
                    MessageNode* node = (MessageNode*)list->current;
                    SoundItem* data = 0;

                    if (node != 0) {
                        if (list->head == node) {
                            list->head = node->next;
                        }
                        if (list->tail == node) {
                            list->tail = node->prev;
                        }
                        if (node->prev != 0) {
                            node->prev->next = node->next;
                        }
                        if (node->next != 0) {
                            node->next->prev = node->prev;
                        }

                        data = (SoundItem*)node->data;
                        node->Destroy(1);
                        list->current = 0;
                    }

                    list->current = list->head;

                    if (data != 0) {
                        data->SoundItem::~SoundItem();
                        FreeMemory(data);
                    }
                }

                if (list->tail == list->current) {
                    break;
                }

                if (list->current != 0) {
                    list->current = ((MessageNode*)list->current)->next;
                }
            } while (list->head != 0);
        }
    }

    if (handlerId == param2) {
        WriteToMessageLog("SC_Sound::Update");
    }
}

/* Function start: 0x40BD10 */
int SC_Sound::AddMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    WriteToMessageLog("SC_Sound::AddMessage");
    return 1;
}

int SC_Sound::ShutDown(SC_Message* msg) {
    return 0;
}

/* Function start: 0x40BD30 */
int SC_Sound::Exit(SC_Message* msg) {
    if (msg->targetAddress != handlerId) {
        return 0;
    }

    timer.Reset();

    switch (msg->priority) {
    case 3:
    {
        SoundItem* item = FindOrCreateSound(msg->sourceAddress);
        item->Start();
        break;
    }

    case 15:
    {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                SoundItem* data = (SoundItem*)list->PopCurrent();
                if (data != 0) {
                    data->SoundItem::~SoundItem();
                    FreeMemory(data);
                }
            }
        }
        break;
    }

    case 16:
    {
        SoundItem* item = FindOrCreateSound(msg->sourceAddress);
        item->AdjustVolume(10);
        break;
    }

    case 17:
    {
        SoundItem* item = FindOrCreateSound(msg->sourceAddress);
        item->AdjustVolume(-10);
        break;
    }

    case 18:
    {
        SoundItem* item = FindOrCreateSound(msg->sourceAddress);
        item->SetVolume(msg->param1);
        break;
    }

    case 19:
        FindOrCreateSound(msg->sourceAddress);
        break;

    case 20:
    {
        list->current = list->head;
        if (list->head != 0) {
            do {
                MessageNode* node = (MessageNode*)list->current;
                SoundItem* data = (SoundItem*)node->data;

                if (data->soundId == msg->sourceAddress) {
                    SoundItem* eventData = (SoundItem*)list->GetCurrentData();

                    if (list->current != 0) {
                        if (list->head == node) {
                            list->head = node->next;
                        }
                        if (list->tail == node) {
                            list->tail = node->prev;
                        }
                        if (node->prev != 0) {
                            node->prev->next = node->next;
                        }
                        if (node->next != 0) {
                            node->next->prev = node->prev;
                        }

                        node->Destroy(1);
                        list->current = 0;
                    }

                    list->current = list->head;

                    if (eventData != 0) {
                        eventData->SoundItem::~SoundItem();
                        FreeMemory(eventData);
                    }
                }

                if (list->tail == list->current) {
                    break;
                }

                if (list->current != 0) {
                    list->current = ((MessageNode*)list->current)->next;
                }
            } while (list->head != 0);
        }
        break;
    }

    case 26:
    {
        SoundItem* item = FindOrCreateSound(msg->sourceAddress);
        item->Resume();
        break;
    }

    case 27:
        SC_Message_Send(3, handlerId, handlerId, field_8C, 20, 0, 0, 0, 0, 0);
        break;

    default:
        return 0;
    }

    return 1;
}

/* Function start: 0x40C0E0 */
SoundItem* SC_Sound::FindOrCreateSound(int soundId)
{
    MessageList* pList;
    MessageNode* currNode;
    SoundItem* foundItem;
    void* allocResult;
    SoundItem* newItem;
    MessageNode* newNode;

    pList = SC_Sound::list;
    if (pList->head != 0) {
        pList->current = pList->head;
        while (pList->head != 0) {
            currNode = (MessageNode*)pList->current;
            if (currNode == 0) {
                if (soundId == *(int*)0x18) {
                    goto found;
                }
            } else {
                if (soundId == ((SoundItem*)currNode->data)->soundId) {
found:
                    currNode = (MessageNode*)pList->current;
                    if (currNode == 0) {
                        foundItem = 0;
                    } else {
                        foundItem = (SoundItem*)currNode->data;
                    }
                    return foundItem;
                }
            }
            if (pList->tail == pList->current) {
                break;
            }
            if (currNode != 0) {
                pList->current = currNode->next;
            }
        }
    }

    // Not found - allocate new SoundItem
    newItem = new SoundItem(soundId);

    pList = SC_Sound::list;
    if (newItem == 0) {
        ShowError("queue fault 0101");
    }

    pList->current = pList->head;

    // Check list type for insertion method
    if (pList->flags != 1 && pList->flags != 2) {
        pList->InsertBeforeCurrent(newItem);
        return newItem;
    }

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
        return newItem;
    }

    // Priority insertion loop
    do {
        currNode = (MessageNode*)pList->current;
        if ((unsigned int)((SoundItem*)currNode->data)->soundId < (unsigned int)newItem->soundId) {
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
                if (pList->tail == 0 || ((MessageNode*)pList->tail)->next != 0) {
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
    } while (pList->current != 0);

    return newItem;
}
