#include "ZBuffer.h"
#include "Message.h"
#include "string.h"
#include <stdlib.h>
#include "VBuffer.h"
#include "Memory.h"
#include "Timer.h"
#include "string.h"

// This is a guess based on usage.
struct SoundManager {
    char pad[0x84];
    Timer timer;
    int field_98;
    void* field_9c;
    void* field_a0;
    void* field_a4;
};

extern SoundManager* g_SoundManager;

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
    if (*(int*)((char*)param_1 + 0x88) != this->m_address) {
        return 0;
    }
    this->timer.Reset();
    int iVar1 = *(int*)((char*)param_1 + 0x98);
    if (iVar1 == 3) {
        *(int*)g_SoundManager |= 2;
        g_SoundManager->timer.Reset();
        return 1;
    }
    if (iVar1 == 0xe) {
        g_SoundManager->field_98 = 2;
        int* piVar2 = (int*)g_SoundManager->field_a0;
        if (*piVar2 != 0) {
            piVar2[2] = *piVar2;
            while (*piVar2 != 0) {
                void* puVar4 = ZBuffer::PopNode(piVar2);
                if (puVar4 != 0) {
                    *(void**)puVar4 = (void*)0x431050;
                    FreeFromGlobalHeap(puVar4);
                }
            }
        }
        piVar2 = (int*)g_SoundManager->field_a4;
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
        piVar2 = (int*)g_SoundManager->field_9c;
        if (*piVar2 != 0) {
            piVar2[2] = *piVar2;
            while (*piVar2 != 0) {
                void* this_00 = ZBuffer::PopNode_2(piVar2);
                if (this_00 != 0) {
                    /* Call scalar deleting destructor via vtable with flag 1 */
                    void (**vtable)(int) = *(void (***)(int))this_00;
                    (*vtable)(1);
                }
            }
        }
    } else {
        if (iVar1 != 0xf) {
            return 0;
        }
        g_SoundManager->field_98 = 1;
        int* piVar2 = (int*)g_SoundManager->field_a0;
        if (*piVar2 != 0) {
            piVar2[2] = *piVar2;
            while (*piVar2 != 0) {
                void* puVar4 = ZBuffer::PopNode(piVar2);
                if (puVar4 != 0) {
                    *(void**)puVar4 = (void*)0x431050;
                    FreeFromGlobalHeap(puVar4);
                }
            }
        }
        piVar2 = (int*)g_SoundManager->field_a4;
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
        ZBuffer::ClearList((int*)g_SoundManager->field_9c);
    }
    return 1;
}

/* Function start: 0x401500 */
void ZBuffer::CleanUpVBuffer()
{
    VBuffer* this_00 = *(VBuffer**)((char*)this + 0x4);
    if (this_00 != 0) {
        this_00->~VBuffer();
        FreeFromGlobalHeap(this_00);
        *(void**)((char*)this + 0x4) = 0;
    }
    if (*(void**)((char*)this + 0x8) != 0) {
        void (**vtable)(int) = *(void (***)(int))((char*)this + 0x8);
        (*vtable)(1);
        *(void**)((char*)this + 0x8) = 0;
    }
}

/* Function start: 0x4016A0 */
ZBuffer::~ZBuffer()
{
    *(void**)this = (void*)0x431058;
}

/* Function start: 0x401710 */
void* ZBuffer::PopNode(int* param_1)
{
    int* piVar1 = (int*)param_1[2];
    if (piVar1 != 0) {
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
        void* uVar3 = 0;
        void** puVar2_2 = (void**)param_1[2];
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
    return 0;
}

/* Function start: 0x401790 */
void* ZBuffer::PopNode_2(int* param_1)
{
    int* piVar1 = (int*)param_1[2];
    if (piVar1 != 0) {
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
        void* uVar3 = 0;
        void** puVar2_2 = (void**)param_1[2];
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
    return 0;
}

/* Function start: 0x401560 */
void ZBuffer::ClearList(int* param_1)
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
                *(void**)local_18 = (void*)0x431058;
                FreeFromGlobalHeap(local_18);
            }
        } while (*param_1 != 0);
    }
}
