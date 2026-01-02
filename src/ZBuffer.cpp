#include "ZBuffer.h"
#include "globals.h"
#include "Message.h"
#include "string.h"
#include <stdlib.h>
#include "VBuffer.h"
#include "Memory.h"
#include "Timer.h"
#include "string.h"

// This is a guess based on usage.
// This is a guess based on usage.
// struct SoundManager {
//     char pad[0x84];
//     Timer timer;
//     int field_98;
//     void* field_9c;
//     void* field_a0;
//     void* field_a4;
// };

// extern SoundManager* g_SoundManager;

/* Function start: 0x4012D0 */
void ZBuffer::Update(int param_1, int param_2)
{
    if (timer.Update() > 10000) {
        SC_Message_Send(3, m_address, m_address, m_from, 0x14, 0, 0, 0, 0, 0);
    }
    if (m_address == param_2) {
        ShowError("SC_ZBuffer::Update");
    }
}

/* Function start: 0x401330 */
void ZBuffer::AddMessage(int param_1)
{
    //VFunc5(param_1);
    ShowError("SC_ZBuffer::AddMessage");
}

/* Function start: 0x401350 */
int ZBuffer::ProcessMessage(void* param_1)
{
    Message* msg = (Message*)param_1;
    if (msg->targetAddress != this->m_address) {
        return 0;
    }
    this->timer.Reset();
    int iVar1 = msg->priority;
    if (iVar1 == 3) {
        *(int*)g_SoundManager |= 2;
        g_SoundManager->timer.Reset();
        return 1;
    }
    if (iVar1 == 0xe) {
        g_SoundManager->m_mode = 2;
        int* piVar2 = (int*)g_SoundManager->m_commandQueue;
        if (*piVar2 != 0) {
            piVar2[2] = *piVar2;
            while (*piVar2 != 0) {
                void* puVar4 = ZBuffer::PopNode(piVar2);
                if (puVar4 != 0) {
                    *(void**)puVar4 = 0;
                    FreeFromGlobalHeap(puVar4);
                }
            }
        }
        piVar2 = (int*)g_SoundManager->m_list3;
        if (*piVar2 != 0) {
            piVar2[2] = *piVar2;
            while (*piVar2 != 0) {
                ZBuffer* pVVar5 = (ZBuffer*)ZBuffer::PopNode_2(piVar2);
                if (pVVar5 != 0) {
                    pVVar5->CleanUpVBuffer();
                    FreeFromGlobalHeap(pVVar5);
                }
            }
        }
        piVar2 = (int*)g_SoundManager->m_list1;
        if (*piVar2 != 0) {
            piVar2[2] = *piVar2;
            while (*piVar2 != 0) {
                void* this_00 = ZBuffer::PopNode_2(piVar2);
                if (this_00 != 0) {
                    FreeFromGlobalHeap(this_00);
                }
            }
        }
    } else {
        if (iVar1 != 0xf) {
            return 0;
        }
        g_SoundManager->m_mode = 1;
        int* piVar2 = (int*)g_SoundManager->m_commandQueue;
        if (*piVar2 != 0) {
            piVar2[2] = *piVar2;
            while (*piVar2 != 0) {
                void* puVar4 = ZBuffer::PopNode(piVar2);
                if (puVar4 != 0) {
                    *(void**)puVar4 = 0;
                    FreeFromGlobalHeap(puVar4);
                }
            }
        }
        piVar2 = (int*)g_SoundManager->m_list3;
        if (*piVar2 != 0) {
            piVar2[2] = *piVar2;
            while (*piVar2 != 0) {
                ZBuffer* pVVar5 = (ZBuffer*)ZBuffer::PopNode_2(piVar2);
                if (pVVar5 != 0) {
                    pVVar5->CleanUpVBuffer();
                    FreeFromGlobalHeap(pVVar5);
                }
            }
        }
        ZBuffer::ClearList((int*)g_SoundManager->m_list1);
    }
    return 1;
}

/* Function start: 0x401500 */
void ZBuffer::CleanUpVBuffer()
{
    VBuffer* vb = this->m_vbuffer;
    if (vb != 0) {
        vb->~VBuffer();
        FreeFromGlobalHeap(vb);
        this->m_vbuffer = 0;
    }
    Parser* ptr = (Parser*)this->m_subObject;
    if (ptr != 0) {
        delete ptr;
        this->m_subObject = 0;
    }
}

/* Function start: 0x4016A0 */
ZBuffer::~ZBuffer()
{
    CleanUpVBuffer();
}

/* Function start: 0x401710 */
void* __fastcall ZBuffer::PopNode(int* param_1)
{
    int* piVar1 = (int*)param_1[2];
    if (piVar1 == 0) {
        return 0;
    }
    if ((int*)*param_1 == piVar1) {
        *param_1 = piVar1[1];
    }
    if ((int*)param_1[1] == piVar1) {
        param_1[1] = *piVar1;
    }
    if (*piVar1 != 0) {
        *(int*)(*piVar1 + 4) = piVar1[1];
    }
    void** puVar2 = (void**)((void**)param_1[2])[1];
    if (puVar2 != 0) {
        *puVar2 = *(void**)param_1[2];
    }
    void** puVar2_2 = (void**)param_1[2];
    void* uVar3 = 0;
    if (puVar2_2 != 0) {
        uVar3 = (void*)puVar2_2[2];
        puVar2_2[2] = 0;
        *puVar2_2 = 0;
        puVar2_2[1] = 0;
        FreeFromGlobalHeap(puVar2_2);
        param_1[2] = 0;
    }
    param_1[2] = *param_1;
    return uVar3;
}

/* Function start: 0x401790 */
void* __fastcall ZBuffer::PopNode_2(int* param_1)
{
    int* piVar1 = (int*)param_1[2];
    if (piVar1 == 0) {
        return 0;
    }
    if ((int*)*param_1 == piVar1) {
        *param_1 = piVar1[1];
    }
    if ((int*)param_1[1] == piVar1) {
        param_1[1] = *piVar1;
    }
    if (*piVar1 != 0) {
        *(int*)(*piVar1 + 4) = piVar1[1];
    }
    void** puVar2 = (void**)((void**)param_1[2])[1];
    if (puVar2 != 0) {
        *puVar2 = *(void**)param_1[2];
    }
    void** puVar2_2 = (void**)param_1[2];
    void* uVar3 = 0;
    if (puVar2_2 != 0) {
        uVar3 = (void*)puVar2_2[2];
        puVar2_2[2] = 0;
        *puVar2_2 = 0;
        puVar2_2[1] = 0;
        FreeFromGlobalHeap(puVar2_2);
        param_1[2] = 0;
    }
    param_1[2] = *param_1;
    return uVar3;
}

/* Function start: 0x401560 */
void __fastcall ZBuffer::ClearList(int* param_1)
{
    if (*param_1 != 0) {
        param_1[2] = *param_1;
        do {
            int* piVar1 = (int*)param_1[2];
            void* local_18;
            if (piVar1 == 0) {
                local_18 = 0;
            } else {
                if ((int*)*param_1 == piVar1) {
                    *param_1 = piVar1[1];
                }
                if ((int*)param_1[1] == piVar1) {
                    param_1[1] = *piVar1;
                }
                if (*piVar1 != 0) {
                    *(int*)(*piVar1 + 4) = piVar1[1];
                }
                void** puVar2 = (void**)((void**)param_1[2])[1];
                if (puVar2 != 0) {
                    *puVar2 = *(void**)param_1[2];
                }
                local_18 = 0;
                void** puVar2_2 = (void**)param_1[2];
                if (puVar2_2 != 0) {
                    local_18 = (void*)puVar2_2[2];
                    puVar2_2[2] = 0;
                    *puVar2_2 = 0;
                    puVar2_2[1] = 0;
                    FreeFromGlobalHeap(puVar2_2);
                    param_1[2] = 0;
                }
                param_1[2] = *param_1;
            }
            if (local_18 != 0) {
                *(void**)local_18 = 0;
                FreeFromGlobalHeap(local_18);
            }
        } while (*param_1 != 0);
    }
}
