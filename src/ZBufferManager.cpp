#include "ZBufferManager.h"
#include "VBuffer.h"
#include "Timer.h"
#include "ZBuffer.h"
#include "Palette.h"
#include "Memory.h"
#include "Animation.h"
#include "GlyphRect.h"
#include "string.h"
#include <string.h>

// External functions - still need proper identification
extern "C" {
    int __fastcall FUN_0041ebb0(void*);           // Check/validate pointer
    void __fastcall FUN_004189a0(void*, int);     // Node cleanup/destructor
    void FUN_0041c94c();                          // SEH unwinding thunk
}

extern void* __fastcall FUN_0041bb10(int* param);
extern void __fastcall FUN_0041cce0(void* node, int flag);
extern void* __fastcall FUN_00401810(int* param);

extern VBuffer* g_WorkBuffer_00436974;

ZBufferManager::~ZBufferManager()
{
    Cleanup();
}

/* Function start: 0x41B760 */
ZBufferManager::ZBufferManager() {
    ZBQueue* puVar1;
    ZBQueue* puVar2;
    
    WriteToMessageLogIfEnabled(L"\"declaring ZBuffer\"");
    memset(this, 0, 0x2b * 4);
    
    puVar1 = (ZBQueue*)operator new(sizeof(ZBQueue));
    puVar2 = 0;
    if (puVar1 != 0) {
        puVar1->type = 2;
        puVar1->head = 0;
        puVar1->tail = 0;
        puVar1->current = puVar1->head;
        puVar2 = puVar1;
    }
    m_queueA0 = puVar2;
    
    puVar1 = (ZBQueue*)operator new(sizeof(ZBQueue));
    puVar2 = 0;
    if (puVar1 != 0) {
        puVar1->type = 0;
        puVar1->head = 0;
        puVar1->tail = 0;
        puVar1->current = puVar1->head;
        puVar2 = puVar1;
    }
    m_queueA4 = puVar2;
    
    puVar1 = (ZBQueue*)operator new(sizeof(ZBQueue));
    puVar2 = 0;
    if (puVar1 != 0) {
        puVar1->type = 1;
        puVar1->head = 0;
        puVar1->tail = 0;
        puVar1->current = puVar1->head;
        puVar2 = puVar1;
    }
    m_queue9c = puVar2;
    
    m_state = 2;
}

/* Function start: 0x41B8E0 */
void ZBufferManager::Cleanup() {
    int* piVar1;
    int iVar3;
    unsigned int* puVar4;
    void* pvVar5;
    int* local_14;
    unsigned int* local_18;
    unsigned int* local_1c;
    
    // First: drain queue at 0xA0 with special handling
    piVar1 = (int*)this->m_queueA0;
    iVar3 = *piVar1;
    while (iVar3 != 0) {
        piVar1 = (int*)this->m_queueA0;
        int queueType = piVar1[3];
        if (queueType == 1 || queueType == 4) {
            iVar3 = *piVar1;
        } else if (queueType == 2 || queueType == 0) {
            iVar3 = piVar1[1];
        } else {
            ShowError("\"bad queue type %lu\"", queueType);
        }
        piVar1[2] = iVar3;
        
        iVar3 = piVar1[2];
        if (iVar3 != 0) {
            if (*piVar1 == iVar3) {
                *piVar1 = *(int*)(iVar3 + 4);
            }
            if (piVar1[1] == piVar1[2]) {
                piVar1[1] = *(int*)piVar1[2];
            }
            iVar3 = *(int*)piVar1[2];
            if (iVar3 != 0) {
                *(int*)(iVar3 + 4) = ((int*)piVar1[2])[1];
            }
            int* pNode = (int*)((int*)piVar1[2])[1];
            if (pNode != 0) {
                *pNode = *(int*)piVar1[2];
            }
            FUN_0041bb10(piVar1);
            void* nodePtr = (void*)piVar1[2];
            if (nodePtr != 0) {
                FUN_0041cce0(nodePtr, 1);
                piVar1[2] = 0;
            }
            piVar1[2] = *piVar1;
        }
        piVar1 = (int*)this->m_queueA0;
        iVar3 = *piVar1;
    }
    
    // Second: drain queue at 0xA0 (again), freeing remaining items
    piVar1 = (int*)this->m_queueA0;
    if (piVar1 != 0) {
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                puVar4 = (unsigned int*)ZBuffer::PopNode(piVar1);
                if (puVar4 != 0) {
                    *puVar4 = 0x431050;
                    FreeMemory(puVar4);
                }
            }
        }
        FreeMemory(piVar1);
        this->m_queueA0 = 0;
    }
    
    // Third: drain queue at 0xA4
    piVar1 = (int*)this->m_queueA4;
    if (piVar1 != 0) {
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                pvVar5 = ZBuffer::PopNode_2(piVar1);
                if (pvVar5 != 0) {
                    ((ZBuffer*)pvVar5)->CleanUpVBuffer();
                    FreeMemory(pvVar5);
                }
            }
        }
        FreeMemory(piVar1);
        this->m_queueA4 = 0;
    }
    
    // Fourth: drain queue at 0x9C
    local_14 = (int*)this->m_queue9c;
    if (local_14 != 0) {
        if (*local_14 != 0) {
            local_14[2] = *local_14;
            while (*local_14 != 0) {
                local_18 = (unsigned int*)FUN_00401810(local_14);
                if (local_18 != 0) {
                    local_1c = local_18;
                    *local_18 = 0x431058;
                    try {
                        ((GlyphRect*)(local_1c + 1))->~GlyphRect();
                    } catch (...) {
                    }
                    FreeMemory(local_18);
                }
            }
        }
        FreeMemory(local_14);
        this->m_queue9c = 0;
    }
}

/* Function start: 0x41C5A0 */
void ZBufferManager::ProcessRenderQueues()
{
    unsigned int* pThis = (unsigned int*)this;
    int* piVar1;
    int iVar3;
    void* pvVar4;
    unsigned int uVar5;
    unsigned int* puVar6;
    unsigned int* puVar7;
    unsigned int* local_14;  // saved this pointer / [EBP-0x10]
    int local_height;        // [EBP-0x14] 
    int local_width;         // [EBP-0x18]
    int local_1c;            // [EBP-0x1c]
    int local_20;            // [EBP-0x20]
    
    pThis[1] = 0;
    local_14 = pThis;
    
    if (pThis[0x26] == 2) {
        // State type 2 processing
        if ((void*)pThis[0x2a] != 0) {
            iVar3 = FUN_0041ebb0((void*)pThis[0x2a]);
            if (iVar3 == 0) {
                BlankScreen();
                ((Palette*)local_14[0x2a])->SetPalette(0, 0x100);
            }
            local_14[0x2a] = 0;
        }
        
        local_height = g_WorkBuffer_00436974->height - 1;
        local_width = g_WorkBuffer_00436974->width - 1;
        local_20 = 0;
        local_1c = 0;
        
        try {
            iVar3 = *(int*)local_14[0x28];
            while (iVar3 != 0) {
                puVar7 = 0;
                piVar1 = (int*)local_14[0x28];
                int queueType = piVar1[3];
                if (queueType == 1 || queueType == 4) {
                    iVar3 = *piVar1;
                    piVar1[2] = iVar3;
                } else if (queueType == 2 || queueType == 0) {
                    iVar3 = piVar1[1];
                    piVar1[2] = iVar3;
                } else {
                    ShowError("\"bad queue type %lu\"", queueType);
                }
                
                if (piVar1[2] != 0) {
                    puVar7 = (unsigned int*)ZBuffer::PopNode(piVar1);
                }
                
                if (puVar7 != 0) {
                    // Virtual call through vtable [puVar7[0]], passing &local_20
                    (*(void (**)(unsigned int*, int*))*puVar7)(puVar7, &local_20);
                    
                    if (puVar7 != 0) {
                        *puVar7 = 0x431050;  // vtable for destructor
                        FreeMemory(puVar7);
                    }
                }
                
                iVar3 = *(int*)local_14[0x28];
            }
        } catch (...) {
        }
        
        // Process queue at 0xa4
        piVar1 = (int*)local_14[0x29];
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                pvVar4 = ZBuffer::PopNode_2(piVar1);
                if (pvVar4 != 0) {
                    ((ZBuffer*)pvVar4)->CleanUpVBuffer();
                    FreeMemory(pvVar4);
                }
            }
        }
        
        if ((*local_14 & 2) != 0) {
            uVar5 = ((Timer*)(local_14 + 0x21))->Update();
            if (uVar5 > 2000) {
                *local_14 = *local_14 & 0xfffffffd;
            }
        }
    }
    else if (pThis[0x26] == 3) {
        // State type 3 processing
        if ((void*)pThis[0x2a] != 0) {
            iVar3 = FUN_0041ebb0((void*)pThis[0x2a]);
            if (iVar3 == 0) {
                BlankScreen();
                ((Palette*)local_14[0x2a])->SetPalette(0, 0x100);
            }
            local_14[0x2a] = 0;
        }
        
        iVar3 = *(int*)local_14[0x27];
        while (iVar3 != 0) {
            puVar7 = 0;
            piVar1 = (int*)local_14[0x27];
            int queueType = piVar1[3];
            if (queueType == 1 || queueType == 4) {
                iVar3 = *piVar1;
                piVar1[2] = iVar3;
            } else if (queueType == 2 || queueType == 0) {
                iVar3 = piVar1[1];
                piVar1[2] = iVar3;
            } else {
                ShowError("\"bad queue type %lu\"", queueType);
            }
            
            iVar3 = piVar1[2];
            if (iVar3 != 0) {
                // Unlink node from double linked list
                if (*piVar1 == iVar3) {
                    *piVar1 = *(int*)(iVar3 + 4);
                }
                if (piVar1[1] == piVar1[2]) {
                    piVar1[1] = *(int*)piVar1[2];
                }
                iVar3 = *(int*)piVar1[2];
                if (iVar3 != 0) {
                    *(int*)(iVar3 + 4) = ((int*)piVar1[2])[1];
                }
                puVar6 = (unsigned int*)((unsigned int*)piVar1[2])[1];
                if (puVar6 != 0) {
                    *puVar6 = *(unsigned int*)piVar1[2];
                }
                
                void* nodePtr = (void*)piVar1[2];
                if (nodePtr == 0) {
                    puVar7 = 0;
                } else {
                    puVar7 = *(unsigned int**)((char*)nodePtr + 8);
                    FUN_004189a0(nodePtr, 1);
                    piVar1[2] = 0;
                }
                piVar1[2] = *piVar1;
                local_height = (int)puVar7;
            }
            
            // Check queue at 0xa0
            piVar1 = (int*)local_14[0x28];
            if (piVar1 == 0 || *piVar1 == 0) {
                break;
            }
            piVar1[2] = *piVar1;
            
            if (*(int*)local_14[0x28] != 0) {
                unsigned int* iterPtr = puVar7 + 1;
                do {
                    puVar6 = 0;
                    if (*(int*)(local_14[0x28] + 8) != 0) {
                        puVar6 = *(unsigned int**)(*(int*)(local_14[0x28] + 8) + 8);
                    }
                    // Virtual call
                    (*(void (**)(unsigned int*, unsigned int*))*puVar6)(puVar6, iterPtr);
                    
                    unsigned int queuePtr = local_14[0x28];
                    iVar3 = *(int*)(queuePtr + 8);
                    if (*(int*)(queuePtr + 4) == iVar3) {
                        break;
                    }
                    if (iVar3 != 0) {
                        *(unsigned int*)(queuePtr + 8) = *(unsigned int*)(iVar3 + 4);
                    }
                } while (*(int*)local_14[0x28] != 0);
            }
            
            if (puVar7 != 0) {
                *puVar7 = 0x431058;  // vtable for destructor
                FUN_0041c94c();
                FreeMemory(puVar7);
            }
            
            iVar3 = *(int*)local_14[0x27];
        }
        
        // Process queue at 0xa4
        piVar1 = (int*)local_14[0x29];
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                pvVar4 = ZBuffer::PopNode_2(piVar1);
                if (pvVar4 != 0) {
                    ((ZBuffer*)pvVar4)->CleanUpVBuffer();
                    FreeMemory(pvVar4);
                }
            }
        }
        
        if ((*local_14 & 2) != 0) {
            uVar5 = ((Timer*)(local_14 + 0x21))->Update();
            if (uVar5 > 2000) {
                *local_14 = *local_14 & 0xfffffffd;
            }
        }
    }
}
