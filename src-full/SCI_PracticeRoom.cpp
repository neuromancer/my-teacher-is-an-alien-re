#include "SCI_PracticeRoom.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Sample.h"
#include "LinkedList.h"
#include <string.h>
#include <stdio.h>

extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern "C" void FUN_00454400(void*);
extern "C" void* FUN_00454500(int);
extern "C" void FUN_00413e10(void*, char*, char*, ...);
extern void __cdecl FUN_00425a90(int, int);

extern void __fastcall FUN_004309a0(void*, int, int);
extern void __fastcall FUN_00443990(void*);
extern void __fastcall FUN_0044c740(void*);
extern void __fastcall FUN_0041dc10(void*);
extern void __fastcall FUN_00420d90(void*);
extern void __fastcall FUN_00425490(void*);
extern void __fastcall FUN_00429c10(void*);
extern void* __fastcall FUN_0042bc50(void*);
extern void __fastcall FUN_0042b270(void*);
extern void __fastcall FUN_0042b100(void*);
extern int __fastcall FUN_00443e30(void*);
extern void __fastcall FUN_0044c9d0(void*);
extern void __fastcall FUN_00429df0(void*);
extern void __fastcall FUN_00420f00(void*);
extern void __fastcall FUN_00432da0(void*);
extern void* __fastcall FUN_00425480(void*);
extern void __fastcall FUN_00425550(void*, int, int);
extern int __fastcall FUN_00433ae0(void*, int, char*);
extern int __fastcall FUN_0042a010(void*, int, void*);

extern void __cdecl FUN_00425c50(char*, ...);
extern void __cdecl FUN_00425d70(char*);
extern void __cdecl FUN_00413e70(void*, int, char*);

extern void* __fastcall FUN_0041dbe0(void*);
extern void* __fastcall FUN_004438a0(void*);
extern void* __fastcall FUN_0044c660(void*, int, char*);
extern void* __fastcall FUN_00429b60(void*, int, int, void*);
extern void* __fastcall FUN_00420ce0(void*, int, int);

extern int DAT_0046cb90;
extern "C" extern void* DAT_0046aa30;
extern void* DAT_0046aa18;
extern void* DAT_0046aa24;
extern int DAT_0046ad6c;

class PaletteObj {
public:
    void Load(char* name);
};

class SlimeTable {
    int fields[3];
public:
    SlimeTable();
    ~SlimeTable();
    void Init(int);
    void FUN_004254a0(int);
    void FUN_00425620(int, char*, int);
};

/* Function start: 0x42A8D0 */
SCI_PracticeRoom::SCI_PracticeRoom() {
}

SCI_PracticeRoom::~SCI_PracticeRoom() {
}

/* Function start: 0x42A9F0 */
void SCI_PracticeRoom::Init(SC_Message* msg) {
    FUN_004309a0(this, 0, (int)msg);
    int iVar = *(int*)((int)msg + 4);
    *(int*)((int)this + 0xA8) = 0;
    moduleParam = iVar;
    if (*(int*)((int)msg + 8) == 0x2D) {
        FUN_0042b100(this);
    }
    FUN_00425a90(0x280, 0x1E0);
    SlimeTable* table = new SlimeTable();
    *(SlimeTable**)((int)this + 0xC8) = table;
    void* pvVar = DAT_0046aa30;
    int iGameState = FUN_00433ae0(pvVar, 0, "FINAL_PRACTICEROOM");
    if (iGameState < 0 || *(int*)((int)pvVar + 0x98) - 1 < iGameState) {
        FUN_00425c50("Invalid gamestate %d", iGameState);
    }
    FUN_00413e10(this,
        *(int*)(*(int*)((int)pvVar + 0x90) + iGameState * 4) != 0
            ? "mis\\practice.mis" : "mis\\practice_old.mis",
        (char*)0);
    int* pBC = (int*)((int)this + 0xBC);
    int iCount = 3;
    do {
        int sprite = *pBC;
        if (sprite != 0) {
            *(int*)(sprite + 0x94) = 0;
            void* pSub = *(void**)(sprite + 0x19C);
            if (pSub != 0) {
                ((Sprite*)pSub)->ResetAnimation(0, 0);
            }
        }
        pBC++;
        iCount--;
    } while (iCount != 0);
    int iGS = DAT_0046cb90;
    void* pvVar2 = DAT_0046aa30;
    if (iGS < 0 || *(int*)((int)pvVar2 + 0x98) - 1 < iGS) {
        FUN_00425c50("Invalid gamestate %d", iGS);
    }
    int gsIdx = *(int*)(*(int*)((int)pvVar2 + 0x90) + iGS * 4);
    int selSprite = *(int*)((int)this + gsIdx * 4 + 0xBC);
    if (selSprite != 0) {
        *(int*)(selSprite + 0x94) = 1;
        void* pSub = *(void**)(selSprite + 0x19C);
        if (pSub != 0) {
            ((Sprite*)pSub)->ResetAnimation(1, 0);
        }
    }
    void* palette = *(void**)((int)this + 0xB4);
    if (palette != 0) {
        int* pSlot = (int*)((int)DAT_0046aa24 + 0xA8);
        if (*pSlot != 0) {
            FUN_00425d70("ddouble palette");
        }
        *pSlot = (int)palette;
    }
    void* pSprSub = *(void**)((int)DAT_0046aa18 + 0x94);
    if (pSprSub != 0) {
        ((Sprite*)pSprSub)->ResetAnimation(0, 0);
    }
    FUN_00444d90(5, *(int*)((int)this + 0xCC), handlerId, moduleParam, 0x1B, 0, 0, 0, 0, 0);
    if (*(int*)((int)this + 0x98) == 0x2D) {
        (*(SlimeTable**)((int)this + 0xC8))->Init(DAT_0046ad6c == 0);
        DAT_0046ad6c = 0;
    }
}

/* Function start: 0x42AC20 */
int SCI_PracticeRoom::ShutDown(SC_Message* msg) {
    void* pVar;

    pVar = *(void**)((int)this + 0xAC);
    if (pVar != 0) {
        FUN_00443990(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xAC) = 0;
    }
    pVar = *(void**)((int)this + 0xB0);
    if (pVar != 0) {
        FUN_0044c740(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xB0) = 0;
    }
    int* piVar = *(int**)((int)this + 0xB8);
    if (piVar != 0) {
        if (*piVar != 0) {
            piVar[2] = *piVar;
            while (*piVar != 0) {
                pVar = FUN_0042bc50(piVar);
                if (pVar != 0) {
                    FUN_00429c10(pVar);
                    FUN_00454400(pVar);
                }
            }
        }
        FUN_00454400(piVar);
        *(void**)((int)this + 0xB8) = 0;
    }
    pVar = *(void**)((int)this + 0xB4);
    if (pVar != 0) {
        FUN_0041dc10(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xB4) = 0;
    }
    pVar = *(void**)((int)this + 0xC8);
    if (pVar != 0) {
        FUN_00425490(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xC8) = 0;
    }
    int* pBC = (int*)((int)this + 0xBC);
    int iVar = 3;
    do {
        void* pSprite = (void*)*pBC;
        if (pSprite != 0) {
            FUN_00420d90(pSprite);
            FUN_00454400(pSprite);
            *pBC = 0;
        }
        pBC++;
        iVar--;
    } while (iVar != 0);
    if ((int)msg != 0) {
        if (*(int*)msg == 0x2D) {
            FUN_0042b270(this);
        }
        if ((int)msg != 0) {
            FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        }
    }
    return 0;
}

/* Function start: 0x42AD80 */
void SCI_PracticeRoom::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }
    if (*(int*)((int)this + 0xA8) == 0) {
        void* pVar = *(void**)((int)this + 0xB0);
        if (pVar != 0) {
            *(int*)((int)this + 0xA8) = ((Sprite*)pVar)->Do(
                *(int*)((int)pVar + 0xAC), *(int*)((int)pVar + 0xB0), 1.0);
            if (*(int*)((int)this + 0xA8) == 0) {
                return;
            }
            FUN_0044c9d0(*(void**)((int)this + 0xB0));
            return;
        }
        *(int*)((int)this + 0xA8) = 1;
    }
    void* pAC = *(void**)((int)this + 0xAC);
    if (pAC != 0) {
        FUN_00443e30(pAC);
    }
    int* pList = *(int**)((int)this + 0xB8);
    if (pList != 0) {
        int iHead = *pList;
        pList[2] = iHead;
        int* pList2 = *(int**)((int)this + 0xB8);
        if (*pList2 != 0) {
            do {
                int* pListR = *(int**)((int)this + 0xB8);
                void* pNode = 0;
                int* pCur = (int*)pListR[2];
                if (pCur != 0) {
                    pNode = (void*)pCur[2];
                }
                FUN_00429df0(pNode);
                pListR = *(int**)((int)this + 0xB8);
                int* pCurNode = (int*)pListR[2];
                if (pListR[1] == (int)pCurNode) {
                    break;
                }
                if (pCurNode != 0) {
                    pListR[2] = pCurNode[1];
                }
            } while (**(int**)((int)this + 0xB8) != 0);
        }
    }
    int* pSprites = (int*)((int)this + 0xBC);
    int iCount = 3;
    do {
        if (*pSprites != 0) {
            FUN_00420f00((void*)*pSprites);
        }
        pSprites++;
        iCount--;
    } while (iCount != 0);
    FUN_00432da0(DAT_0046aa18);
}

/* Function start: 0x42AE80 */
int SCI_PracticeRoom::AddMessage(SC_Message* msg) {
    if (*(int*)((int)msg + 0x24) >= 2) {
        int local_14 = 0;
        int* pBC = (int*)((int)this + 0xBC);
        int* local_28 = pBC;
        do {
            int bHit;
            {
                SlimeDim pt;
                pt.field_0 = *(int*)((int)msg + 0x1C);
                pt.field_4 = *(int*)((int)msg + 0x20);
                int sprite = *local_28;
                if (*(int*)(sprite + 0x90) == 0 ||
                    *(int*)(sprite + 0x9C) > pt.field_0 ||
                    *(int*)(sprite + 0xA4) < pt.field_0 ||
                    *(int*)(sprite + 0xA0) > pt.field_4 ||
                    *(int*)(sprite + 0xA8) < pt.field_4) {
                    bHit = 0;
                } else {
                    bHit = 1;
                }
            }
            if (bHit) {
                int iCount = 3;
                do {
                    int sprite = *pBC;
                    *(int*)(sprite + 0x94) = 0;
                    void* pSub = *(void**)(sprite + 0x19C);
                    if (pSub != 0) {
                        ((Sprite*)pSub)->ResetAnimation(0, 0);
                    }
                    pBC++;
                    iCount--;
                } while (iCount != 0);
                int selSprite = *(int*)((int)this + local_14 * 4 + 0xBC);
                *(int*)(selSprite + 0x94) = 1;
                void* pSub = *(void**)(selSprite + 0x19C);
                if (pSub != 0) {
                    ((Sprite*)pSub)->ResetAnimation(1, 0);
                }
                int iGS = DAT_0046cb90;
                void* pvVar = DAT_0046aa30;
                if (iGS < 0 || *(int*)((int)pvVar + 0x98) - 1 < iGS) {
                    FUN_00425c50("Invalid gamestate %d", iGS);
                }
                *(int*)(*(int*)((int)pvVar + 0x90) + iGS * 4) = local_14;
                *(int*)((int)msg + 0x10) = 0;
                goto done;
            }
            local_28++;
            local_14++;
        } while (local_14 < 3);
    }
    if (*(int*)((int)msg + 0x24) == 2) {
        *(int*)((int)msg + 0x10) = 2;
        *(int*)msg = handlerId;
    }
done:
    return 1;
}

/* Function start: 0x42B030 */
int SCI_PracticeRoom::Exit(SC_Message* msg) {
    if (*(int*)msg != handlerId) {
        return 0;
    }
    switch (*(int*)((int)msg + 0x10)) {
    case 2: {
        int* pList = *(int**)((int)this + 0xB8);
        if (pList == 0) {
            break;
        }
        int iHead = *pList;
        pList[2] = iHead;
        int* pList2 = *(int**)((int)this + 0xB8);
        if (*pList2 == 0) {
            break;
        }
        do {
            int* pListR = *(int**)((int)this + 0xB8);
            void* pNode = 0;
            int* pCur = (int*)pListR[2];
            if (pCur != 0) {
                pNode = (void*)pCur[2];
            }
            if (FUN_0042a010(pNode, 0, msg) != 0) {
                break;
            }
            pListR = *(int**)((int)this + 0xB8);
            int* pCurNode = (int*)pListR[2];
            if (pListR[1] == (int)pCurNode) {
                break;
            }
            if (pCurNode != 0) {
                pListR[2] = pCurNode[1];
            }
        } while (**(int**)((int)this + 0xB8) != 0);
        return 1;
    }
    case 7:
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        break;
    default:
        return 0;
    }
    return 1;
}

/* Function start: 0x42B3B0 */
int SCI_PracticeRoom::LBLParse(char* param_1) {
    char local_bc[128];
    char local_3c[32];
    int local_18 = 0;
    int local_1c = 0;

    local_bc[0] = 0;
    local_3c[0] = 0;
    sscanf(param_1, " %s ", local_3c);

    if (strcmp(local_3c, "HANDLE") == 0) {
        sscanf(param_1, "%s %d", local_3c, (int)this + 0x94);
    } else if (strcmp(local_3c, "PALETTE") == 0) {
        sscanf(param_1, "%s %s", local_3c, local_bc);
        void* pal = *(void**)((int)this + 0xB4);
        if (pal != 0) {
            FUN_0041dc10(pal);
            FUN_00454400(pal);
            *(void**)((int)this + 0xB4) = 0;
        }
        void* mem = FUN_00454500(8);
        void* newPal = 0;
        if (mem != 0) {
            newPal = FUN_0041dbe0(mem);
        }
        *(void**)((int)this + 0xB4) = newPal;
        ((PaletteObj*)newPal)->Load(local_bc);
    } else if (strcmp(local_3c, "HOTSPOT") == 0) {
        sscanf(param_1, "%s %d ", local_3c, &local_18);
        if (*(int*)((int)this + 0xB8) == 0) {
            LinkedList* newList = new LinkedList();
            *(LinkedList**)((int)this + 0xB8) = newList;
        }
        void* mem = FUN_00454500(0xC0);
        int* piVar6 = 0;
        if (mem != 0) {
            piVar6 = (int*)FUN_00429b60(mem, 0, local_18, this);
        }
        FUN_00413e70(piVar6, (int)this, 0);
        LinkedList* list = *(LinkedList**)((int)this + 0xB8);
        if (piVar6 == 0) {
            FUN_00425c50("queue fault 0101");
        }
        list->current = list->head;
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                list->InsertNode(piVar6);
            } else {
                do {
                    int cur = (int)list->current;
                    if (*(unsigned int*)(*(int*)(cur + 8) + 0xA4) < *(unsigned int*)((int)piVar6 + 0xA4)) {
                        list->InsertNode(piVar6);
                        break;
                    }
                    if ((int)list->tail == cur) {
                        list->PushNode(piVar6);
                        break;
                    }
                    if (cur != 0) {
                        list->current = (ListNode*)*(int*)(cur + 4);
                    }
                } while (list->current != 0);
            }
        } else {
            list->InsertNode(piVar6);
        }
    } else if (strcmp(local_3c, "CHAR") == 0) {
        sscanf(param_1, "%s %d", local_3c, &local_18);
        void* mem = FUN_00454500(0x1A8);
        void* piVar6 = 0;
        if (mem != 0) {
            piVar6 = FUN_00420ce0(mem, 0, local_18);
        }
        *(void**)((int)this + local_18 * 4 + 0xBC) = piVar6;
        FUN_00413e70(piVar6, (int)this, 0);
    } else if (strcmp(local_3c, "AMBIENT") == 0) {
        if (*(int*)((int)this + 0xAC) == 0) {
            void* mem = FUN_00454500(0xA0);
            void* amb = 0;
            if (mem != 0) {
                amb = FUN_004438a0(mem);
            }
            *(void**)((int)this + 0xAC) = amb;
        }
        FUN_00413e70(*(void**)((int)this + 0xAC), (int)this, 0);
    } else if (strcmp(local_3c, "INTRO") == 0) {
        if (*(int*)((int)this + 0xB0) == 0) {
            void* mem = FUN_00454500(0xF8);
            void* spr = 0;
            if (mem != 0) {
                spr = FUN_0044c660(mem, 0, (char*)0);
            }
            *(void**)((int)this + 0xB0) = spr;
        }
        FUN_00413e70(*(void**)((int)this + 0xB0), (int)this, 0);
    } else if (strcmp(local_3c, "BG_SOUND") == 0) {
        sscanf(param_1, " %s %d ", local_3c, (int)this + 0xCC);
    } else if (strcmp(local_3c, "MAX_SOUNDS") == 0) {
        sscanf(param_1, " %s %d ", local_3c, &local_18);
        (*(SlimeTable**)((int)this + 0xC8))->FUN_004254a0(local_18);
    } else {
        if (strcmp(local_3c, "SOUND") == 0) {
            sscanf(param_1, " %s %d %s %d ", local_3c, &local_18, local_bc, &local_1c);
            if (local_18 >= 0 && local_18 <= *(*(int**)((int)this + 0xC8)) - 1) {
                (*(SlimeTable**)((int)this + 0xC8))->FUN_00425620(local_18, local_bc, local_1c);
                goto lbl_done;
            }
        } else if (strcmp(local_3c, "END") == 0) {
            return 1;
        }
        ReportUnknownLabel("SCI_PracticeRoom");
    }
lbl_done:
    return 0;
}
