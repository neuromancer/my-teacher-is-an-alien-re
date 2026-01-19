#include "Handler14.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"
#include "SC_OnScreenMessage.h"
#include "SC_Question.h"
#include "Sample.h"

// SoundItem class - represents a sound being played
// Size: 0x20 bytes
// vtable: 0x431238
// Layout:
//   0x00: vtable pointer
//   0x04-0x17: Timer (size 0x14)
//   0x18: soundId
//   0x1C: Sample* soundPtr
class SoundItem {
public:
    void* vtable;           // 0x00
    Timer timer;            // 0x04 - 0x17
    int soundId;            // 0x18
    Sample* soundPtr;       // 0x1C
};

// External functions
extern void __fastcall FUN_0040b5d0(void* item, int soundId);  // SoundItem constructor
extern int __fastcall FUN_0040b700(void* item);                 // Check if sound finished
extern void __fastcall FUN_0040b750(int item);                  // Resume sound
extern void __fastcall FUN_0040b770(int item);                  // Start/play sound
extern void FUN_0040b790(void* item, int volume);              // Adjust volume
extern void FUN_0040b7c0(void* item, int volume);              // Set volume
extern void* FUN_0040c0e0(void* handler, int soundId);         // Find or create sound item
extern void* __fastcall FUN_0040c500(int* list);               // Pop from list
extern void* __fastcall FUN_0040c0d0(int list);                // Get current node data
extern void FUN_0040c580(void* node, int flag);                // Node destructor
extern void FUN_0040c430(int* list, int data);                 // Insert into list
// FUN_0041e470 is Sample::Unload() - use soundPtr->Unload() instead
extern int _AIL_sample_status_4(int sample);                   // AIL sample status
extern int _AIL_sample_volume_4(int sample);                   // AIL sample volume
extern void FUN_0041e580(void* sound, int volume, int param);  // Set sound volume
extern void FUN_0041e6d0(void* sound, int volume, int param);  // Set sound playback

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
                    data->vtable = (void*)0x431238;
                    if (data->soundPtr != 0) {
                        data->soundPtr->Unload();
                        FreeMemory(data->soundPtr);
                        data->soundPtr = 0;
                    }
                    FreeMemory(data);
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

/* Function start: 0x40BD10 */
int Handler14::HandleMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    WriteToMessageLog("SC_Sound::AddMessage");
    return 1;
}

/* Function start: 0x40BB00 */
int Handler14::Update(SC_Message* msg) {
    return 0;
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

            iVar4 = FUN_0040b700(soundItem);
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
                    data->vtable = (void*)0x431238;
                    if (data->soundPtr != 0) {
                        data->soundPtr->Unload();
                        FreeMemory(data->soundPtr);
                        data->soundPtr = 0;
                    }
                    FreeMemory(data);
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

/* Function start: 0x40BD30 */
int Handler14::Exit(SC_Message* msg) {
    MessageList* pList;
    MessageNode* node;
    SoundItem* eventData;
    SoundItem* data;
    void* pvVar7;
    int iVar1;

    if (msg->command != handlerId) {
        return 0;
    }

    timer.Reset();

    switch (msg->priority) {
    case 3:
        pvVar7 = FUN_0040c0e0(this, msg->data);
        FUN_0040b770((int)pvVar7);
        break;

    case 0xf:
        // Clear entire list
        pList = list;
        if (pList->head != 0) {
            pList->current = pList->head;
            while (pList->head != 0) {
                data = (SoundItem*)FUN_0040c500((int*)pList);
                if (data != 0) {
                    data->vtable = (void*)0x431238;
                    if (data->soundPtr != 0) {
                        data->soundPtr->Unload();
                        FreeMemory(data->soundPtr);
                        data->soundPtr = 0;
                    }
                    FreeMemory(data);
                }
            }
        }
        break;

    case 0x10:
        pvVar7 = FUN_0040c0e0(this, msg->data);
        FUN_0040b790(pvVar7, 10);
        break;

    case 0x11:
        pvVar7 = FUN_0040c0e0(this, msg->data);
        FUN_0040b790(pvVar7, -10);
        break;

    case 0x12:
        pvVar7 = FUN_0040c0e0(this, msg->data);
        FUN_0040b7c0(pvVar7, msg->param1);
        break;

    case 0x13:
        FUN_0040c0e0(this, msg->data);
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
                    eventData = (SoundItem*)FUN_0040c0d0((int)pList);
                    if (pList->current != 0) {
                        FUN_0040c580(pList->current, 1);
                        pList->current = 0;
                    }
                    pList->current = pList->head;
                }
                // Cleanup data object
                if (eventData != 0) {
                    eventData->vtable = (void*)0x431238;
                    if (eventData->soundPtr != 0) {
                        eventData->soundPtr->Unload();
                        FreeMemory(eventData->soundPtr);
                        eventData->soundPtr = 0;
                    }
                    FreeMemory(eventData);
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
        pvVar7 = FUN_0040c0e0(this, msg->data);
        FUN_0040b750((int)pvVar7);
        break;

    case 0x1b:
        SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
        break;

    default:
        return 0;
    }

    return 1;
}
