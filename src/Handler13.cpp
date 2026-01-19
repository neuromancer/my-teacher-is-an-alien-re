#include "Handler13.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"

// Data cleanup function for list node data
extern void __fastcall FUN_00401910(void*);

// Timer check/process function
extern int __fastcall FUN_004019a0(void* timerNode);

// TimerNode constructor (returns new TimerNode)
extern void* __fastcall FUN_00401890(void* mem);

// Set timer duration on TimerNode
extern void __fastcall FUN_00401990(void* timerNode, int duration);

// Insert node at current position in list
extern void __fastcall FUN_004024d0(int* list, int node);

// Append node at end of list
extern void __fastcall FUN_004025a0(int* list, int node);

// Pop and return current node from list
extern void* __fastcall FUN_00402680(int* list);

// Node destructor (calls dtor and frees)
extern void __fastcall FUN_00402700(void* node, int shouldDelete);

/* Function start: 0x401B60 */
Handler13::Handler13() {
    // Set handlerId to 13
    handlerId = 13;

    // Call Timer::Reset() on first timer
    timer1.Reset();

    // Allocate and initialize list header structure
    list = new MessageList();
}

/* Function start: 0x401CA0 */
Handler13::~Handler13() {
    MessageList* pList;
    MessageNode* node;
    void* data;

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
                    if (pList->tail == (MessageNode*)pList->current) {
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
                        data = node->data;
                    }
                    if (node != 0) {
                        node->data = 0;
                        node->prev = 0;
                        node->next = 0;
                        FreeMemory(node);
                        pList->current = 0;
                    }
                    pList->current = pList->head;
                }
                // Cleanup data object
                if (data != 0) {
                    FUN_00401910(data);
                    FreeMemory(data);
                }
            }
        }
        FreeMemory(pList);
        list = 0;
    }
}

/* Function start: 0x401DF0 */
void Handler13::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        field_8C = msg->data;
    }
}

/* Function start: 0x401E20 */
void Handler13::Update(SC_Message* msg) {
}

/* Function start: 0x401E30 */
void Handler13::Draw(int param1, int param2) {
    int* piVar1;
    int* puVar2;
    unsigned int uVar3;
    int iVar4;
    int iVar5;
    void* pvVar6;

    uVar3 = timer1.Update();
    if ((uVar3 > 10000) && (*(int*)list == 0)) {
        SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
    }

    timer1.Reset();

    ((int*)list)[2] = *(int*)list;
    iVar4 = *(int*)list;

    while (iVar4 != 0) {
        iVar5 = 0;
        iVar4 = ((int*)list)[2];
        if (iVar4 != 0) {
            iVar5 = *(int*)(iVar4 + 8);
        }
        iVar4 = FUN_004019a0((void*)iVar5);
        if (iVar4 != 0) {
            piVar1 = (int*)list;
            iVar4 = piVar1[2];
            if (iVar4 == 0) {
                pvVar6 = 0;
            } else {
                if (*piVar1 == iVar4) {
                    *piVar1 = *(int*)(iVar4 + 4);
                }
                if (piVar1[1] == piVar1[2]) {
                    piVar1[1] = *(int*)piVar1[2];
                }
                iVar4 = *(int*)piVar1[2];
                if (iVar4 != 0) {
                    *(int*)(iVar4 + 4) = ((int*)piVar1[2])[1];
                }
                puVar2 = (int*)((int*)piVar1[2])[1];
                if (puVar2 != 0) {
                    *puVar2 = *(int*)piVar1[2];
                }
                puVar2 = (int*)piVar1[2];
                pvVar6 = 0;
                if (puVar2 != 0) {
                    pvVar6 = (void*)puVar2[2];
                    puVar2[2] = 0;
                    *puVar2 = 0;
                    puVar2[1] = 0;
                    FreeMemory(puVar2);
                    piVar1[2] = 0;
                }
                piVar1[2] = *piVar1;
            }
            if (pvVar6 != 0) {
                FUN_00401910(pvVar6);
                FreeMemory(pvVar6);
            }
        }
        iVar4 = (int)list;
        iVar5 = *(int*)(iVar4 + 8);
        if (*(int*)(iVar4 + 4) == iVar5) {
            break;
        }
        if (iVar5 != 0) {
            *(int*)(iVar4 + 8) = *(int*)(iVar5 + 4);
        }
        iVar4 = *(int*)list;
    }

    if (handlerId == param2) {
        WriteToMessageLog("SC_Timer::Update");
    }
}

/* Function start: 0x401F90 */
int Handler13::HandleMessage(SC_Message* msg) {
    WriteMessageAddress(msg);
    WriteToMessageLog("SC_Timer::AddMessage");
    return 1;
}

/* Function start: 0x401FB0 */
int Handler13::Exit(SC_Message* msg) {
    int* piVar1;
    int* puVar2;
    int iVar2;
    int iVar5;
    void* pvVar6;
    void* pNode;
    int* pList;

    if (msg->command != handlerId) {
        return 0;
    }

    timer1.Reset();

    switch (msg->priority) {
    case 0xe:
        break;

    case 0xf:
        // Clear entire list
        piVar1 = (int*)list;
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            iVar2 = *piVar1;
            while (iVar2 != 0) {
                iVar2 = piVar1[2];
                if (iVar2 == 0) {
                    pvVar6 = 0;
                } else {
                    if (*piVar1 == iVar2) {
                        *piVar1 = *(int*)(iVar2 + 4);
                    }
                    if (piVar1[1] == piVar1[2]) {
                        piVar1[1] = *(int*)piVar1[2];
                    }
                    iVar2 = *(int*)piVar1[2];
                    if (iVar2 != 0) {
                        *(int*)(iVar2 + 4) = ((int*)piVar1[2])[1];
                    }
                    puVar2 = (int*)((int*)piVar1[2])[1];
                    if (puVar2 != 0) {
                        *puVar2 = *(int*)piVar1[2];
                    }
                    pNode = (void*)piVar1[2];
                    pvVar6 = 0;
                    if (pNode != 0) {
                        pvVar6 = (void*)*(int*)((int)pNode + 8);
                        FUN_00402700(pNode, 1);
                        piVar1[2] = 0;
                    }
                    piVar1[2] = *piVar1;
                }
                if (pvVar6 != 0) {
                    FUN_00401910(pvVar6);
                    FreeMemory(pvVar6);
                }
                iVar2 = *piVar1;
            }
        }
        break;

    case 0x13:
        if (msg->userPtr == 0) {
            msg->Dump(0);
            WriteToMessageLog("SC_Timer::Input");
        } else {
            pNode = 0;
            puVar2 = (int*)AllocateMemory(0x28);
            if (puVar2 != 0) {
                pNode = FUN_00401890(puVar2);
            }
            ((int*)pNode)[3] = msg->field_b8;
            ((int*)pNode)[2] = msg->data;
            ((int*)pNode)[4] = msg->userPtr;
            msg->userPtr = 0;
            FUN_00401990(pNode, msg->param1);
            piVar1 = (int*)list;
            if (pNode == 0) {
                WriteToMessageLog("queue fault 0101");
            }
            piVar1[2] = *piVar1;
            if ((piVar1[3] == 1) || (piVar1[3] == 2)) {
                if (*piVar1 == 0) {
                    FUN_004024d0(piVar1, (int)pNode);
                } else {
                    do {
                        iVar2 = piVar1[2];
                        if ((unsigned int)*(int*)(*(int*)(iVar2 + 8) + 0xc) < (unsigned int)((int*)pNode)[3]) {
                            FUN_004024d0(piVar1, (int)pNode);
                            break;
                        }
                        if (piVar1[1] == iVar2) {
                            FUN_004025a0(piVar1, (int)pNode);
                            break;
                        }
                        if (iVar2 != 0) {
                            piVar1[2] = *(int*)(iVar2 + 4);
                        }
                    } while (piVar1[2] != 0);
                }
            } else {
                FUN_004024d0(piVar1, (int)pNode);
            }
        }
        break;

    case 0x14:
        pNode = 0;
        puVar2 = (int*)AllocateMemory(0x28);
        if (puVar2 != 0) {
            pNode = FUN_00401890(puVar2);
        }
        ((int*)pNode)[2] = msg->data;
        piVar1 = (int*)list;
        if (pNode == 0) {
            WriteToMessageLog("queue fault 0103");
        }
        iVar2 = *piVar1;
        piVar1[2] = iVar2;
        while (iVar2 != 0) {
            iVar2 = piVar1[2];
            iVar5 = 0;
            if (iVar2 != 0) {
                iVar5 = *(int*)(iVar2 + 8);
            }
            if (*(int*)(iVar5 + 0xc) == ((int*)pNode)[3]) {
                pvVar6 = FUN_00402680((int*)list);
                if (pvVar6 != 0) {
                    FUN_00401910(pvVar6);
                    FreeMemory(pvVar6);
                }
                break;
            }
            if (piVar1[1] == iVar2) {
                break;
            }
            if (iVar2 != 0) {
                piVar1[2] = *(int*)(iVar2 + 4);
            }
            iVar2 = piVar1[2];
        }
        break;

    case 0x1b:
        if (*(int*)list == 0) {
            SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
        }
        break;

    default:
        return 0;
    }

    return 1;
}
