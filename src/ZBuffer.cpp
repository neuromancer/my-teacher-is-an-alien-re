#include "ZBuffer.h"
#include "Message.h"
#include "string.h"
#include <stdlib.h>
#include "VBuffer.h"

#include "Timer.h"

extern "C" {
    void* SC__ZBuffer__PopNode(void*);
    void FreeFromGlobalHeap(void*);
    void* SC__ZBuffer__PopNode_2(void*);
    void VBuffer_VBuffer_Owner___VBuffer_Owner(void*);
    void* SC__ZBuffer__PopNode_3(void*);
    void SC__ZBuffer___ZBuffer(void*, int);
    void SC__ZBuffer__ClearList(void*);
    void ShowError(const char*, ...);
    void VBuffer___VBuffer(void*);
}

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
        SC_Message_Send(3, field_0x88, field_0x88, field_0x8c, 0x14, 0, 0, 0, 0, 0);
    }
    if (field_0x88 == param_2) {
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
    if (*(int*)((char*)param_1 + 0x88) != this->field_0x88) {
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
                void* puVar4 = SC__ZBuffer__PopNode(piVar2);
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
                void* pVVar5 = SC__ZBuffer__PopNode_2(piVar2);
                if (pVVar5 != 0) {
                    VBuffer_VBuffer_Owner___VBuffer_Owner(pVVar5);
                    FreeFromGlobalHeap(pVVar5);
                }
            }
        }
        piVar2 = (int*)g_SoundManager->field_9c;
        if (*piVar2 != 0) {
            piVar2[2] = *piVar2;
            if (*piVar2 != 0) {
                do {
                    void* this_00 = SC__ZBuffer__PopNode_3(piVar2);
                    if (this_00 != 0) {
                        SC__ZBuffer___ZBuffer(this_00, 1);
                    }
                } while (*piVar2 != 0);
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
                void* puVar4 = SC__ZBuffer__PopNode(piVar2);
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
                void* pVVar5 = SC__ZBuffer__PopNode_2(piVar2);
                if (pVVar5 != 0) {
                    VBuffer_VBuffer_Owner___VBuffer_Owner(pVVar5);
                    FreeFromGlobalHeap(pVVar5);
                }
            }
        }
        SC__ZBuffer__ClearList(g_SoundManager->field_9c);
    }
    return 1;
}

/* Function start: 0x401500 */
void ZBuffer::CleanUpVBuffer()
{
    void* this_00 = *(void**)((char*)this + 0x4);
    if (this_00 != 0) {
        VBuffer___VBuffer(this_00);
        FreeFromGlobalHeap(this_00);
        *(void**)((char*)this + 0x4) = 0;
    }
    if (*(void**)((char*)this + 0x8) != 0) {
        void (**vtable)(int) = *(void (***)(int))((char*)this + 0x8);
        (*vtable)(1);
        *(void**)((char*)this + 0x8) = 0;
    }
}
