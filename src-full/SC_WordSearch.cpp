#include "SC_WordSearch.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Sample.h"
#include <string.h>
#include <stdio.h>

extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern "C" void FUN_00454400(void*);
extern "C" void* FUN_00454500(int);
extern "C" void FUN_00413e10(void*, char*, char*, ...);

extern void __fastcall FUN_004218c0(void*);
extern void __fastcall FUN_0044c740(void*);
extern void __fastcall FUN_0041dc10(void*);
extern void __fastcall FUN_00424ee0(void*);
extern void __fastcall FUN_004309a0(void*, int, int);
extern int __fastcall FUN_00433ae0(void*, int, char*);

extern void __cdecl FUN_00444e40(void*);

extern void __cdecl FUN_00425c50(char*, ...);

extern unsigned int __fastcall FUN_004218e0(void*);
extern void __fastcall FUN_00432da0(void* self);
extern void __fastcall FUN_00411180(void*, int, int);
extern void __fastcall FUN_00404350(void*, int, int, int, int, int, int, int, int);
extern void __fastcall FUN_00404230(void*, int, char*, int, int, int, int);

extern char* DAT_0046bacc;
extern int DAT_0046cb90;
extern "C" extern void* DAT_0046aa30;
extern void* DAT_0046aa18;
extern void* DAT_0046aa08;
extern void* DAT_0046aa24;
extern "C" extern void* DAT_0046aa14;

/* Function start: 0x435800 */
SC_WordSearch::SC_WordSearch() {
}

SC_WordSearch::~SC_WordSearch() {
}

/* Function start: 0x435BE0 */
void SC_WordSearch::Init(SC_Message* msg) {
    FUN_004309a0(this, 0, (int)msg);
    if (msg != 0) {
        *(int*)((int)this + 0x94) = *(int*)((int)msg + 4);
        SC_WordSearch::FUN_4368F0();
    }
    FUN_00413e10(this, DAT_0046bacc, (char*)0);
    if (*(int*)((int)this + 0x830) == 0) {
        *(void**)((int)this + 0x830) = new SpriteAction(
            *(int*)((int)this + 0x98),
            *(int*)((int)this + 0x9c),
            *(int*)((int)this + 0x90),
            *(int*)((int)this + 0x94),
            4, 0, 0, 0, 0, 0
        );
    }
    *(int*)((int)this + 0x81c) = 0;
    int iVar2 = DAT_0046cb90;
    void* pvVar4 = DAT_0046aa30;
    if (iVar2 < 0 || *(int*)((int)pvVar4 + 0x98) - 1 < iVar2) {
        FUN_00425c50("Invalid gamestate %d", iVar2);
    }
    ((Sprite*)*(void**)((int)this + 0x82c))->ResetAnimation(
        *(int*)(*(int*)((int)pvVar4 + 0x90) + iVar2 * 4), 0);
    FUN_004218c0((void*)((int)this + 0xC0));
    if (*(void**)((int)this + 0xB8) != 0) {
        ((Sample*)*(void**)((int)this + 0xB8))->Play(100, 1);
    }
    if (*(void**)((int)this + 0xBC) != 0) {
        ((Sample*)*(void**)((int)this + 0xBC))->Play(100, 1);
    }
}

/* Function start: 0x435D40 */
int SC_WordSearch::ShutDown(SC_Message* msg) {
    void* pVar;

    pVar = *(void**)((int)this + 0xAC);
    if (pVar != 0) {
        FUN_0044c740(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xAC) = 0;
    }
    pVar = *(void**)((int)this + 0xA8);
    if (pVar != 0) {
        FUN_0041dc10(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xA8) = 0;
    }
    pVar = *(void**)((int)this + 0xB8);
    if (pVar != 0) {
        FUN_00424ee0(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xB8) = 0;
    }
    pVar = *(void**)((int)this + 0xB0);
    if (pVar != 0) {
        FUN_00424ee0(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xB0) = 0;
    }
    pVar = *(void**)((int)this + 0xB4);
    if (pVar != 0) {
        FUN_00424ee0(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xB4) = 0;
    }
    pVar = *(void**)((int)this + 0xBC);
    if (pVar != 0) {
        FUN_00424ee0(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0xBC) = 0;
    }
    pVar = *(void**)((int)this + 0x830);
    if (pVar != 0) {
        delete (SpriteAction*)pVar;
        *(void**)((int)this + 0x830) = 0;
    }
    pVar = *(void**)((int)this + 0x820);
    if (pVar != 0) {
        FUN_0044c740(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0x820) = 0;
    }
    pVar = *(void**)((int)this + 0x824);
    if (pVar != 0) {
        FUN_0044c740(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0x824) = 0;
    }
    pVar = *(void**)((int)this + 0x828);
    if (pVar != 0) {
        FUN_0044c740(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0x828) = 0;
    }
    pVar = *(void**)((int)this + 0x82C);
    if (pVar != 0) {
        FUN_0044c740(pVar);
        FUN_00454400(pVar);
        *(void**)((int)this + 0x82C) = 0;
    }
    if ((int)msg != 0) {
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    return 0;
}

/* Function start: 0x435F10 */
void SC_WordSearch::Update(int param1, int param2) {
    Sprite* spr;
    void* pvVar10;
    int* piVar1;
    int iVar5, iVar9;
    int iVar8;

    spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
    if (spr != 0) {
        spr->ResetAnimation(0, 0);
    }

    unsigned int uVar4 = FUN_004218e0((void*)((int)this + 0xC0));
    if (uVar4 > 60000 && param2 != handlerId) {
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    if (handlerId != param2) {
        return;
    }

    if (*(int*)((int)this + 0x714) != 0) {
        pvVar10 = *(void**)((int)this + 0x828);
        ((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0);
        pvVar10 = *(void**)((int)this + 0x82c);
        ((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0);
        FUN_00432da0(DAT_0046aa18);
        piVar1 = *(int**)((int)DAT_0046aa08 + 0x1a0);
        iVar9 = 0;
        if (piVar1 != 0) {
            iVar9 = piVar1[1];
        }
        if (piVar1 != 0) {
            iVar5 = *piVar1;
        } else {
            iVar5 = 0;
        }
        if (iVar5 < *(int*)((int)this + 0x704)) return;
        if (*(int*)((int)this + 0x70c) < iVar5) return;
        if (iVar9 < *(int*)((int)this + 0x708)) return;
        if (*(int*)((int)this + 0x710) < iVar9) return;
        spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
        if (spr != 0) {
            spr->ResetAnimation(0xf, 0);
        }
        FUN_00404350(DAT_0046aa24, 0,
            *(int*)((int)this + 0x704), *(int*)((int)this + 0x708),
            *(int*)((int)this + 0x70c), *(int*)((int)this + 0x710),
            10000, 0xfc, 2);
        return;
    }

    pvVar10 = *(void**)((int)this + 0xac);
    if (pvVar10 != 0) {
        ((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0);
    } else {
        FUN_00411180(DAT_0046aa14, 0, 0);
    }

    int* piVar13 = (int*)((int)this + 0x4d4);
    char* pcVar12 = (char*)((int)this + 0xd4);
    int local_4 = 0x20;
    int* piVar11 = piVar13;
    do {
        if (*pcVar12 != '\0') {
            iVar9 = 0;
            piVar1 = *(int**)((int)DAT_0046aa08 + 0x1a0);
            if (piVar1 != 0) {
                iVar9 = piVar1[1];
            }
            if (piVar1 != 0) {
                iVar5 = *piVar1;
            } else {
                iVar5 = 0;
            }
            if (*piVar11 > iVar5 || piVar11[2] < iVar5 || piVar11[1] > iVar9 ||
               (iVar5 = piVar11[3], iVar5 < iVar9)) {
                iVar8 = 0xFC;
                iVar5 = piVar11[3];
            } else {
                iVar8 = 0xFD;
            }
            FUN_00404230(DAT_0046aa24, 0, pcVar12, *piVar11 + 10, iVar5, 10000, iVar8);
        }
        piVar11 = piVar11 + 4;
        pcVar12 = pcVar12 + 0x20;
        local_4 = local_4 - 1;
    } while (local_4 != 0);

    int* puVar2 = *(int**)((int)DAT_0046aa08 + 0x1a0);
    if (puVar2 == 0 || (uVar4 = *puVar2, (int)uVar4 < 0x22)) {
        spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
        if (spr != 0) {
            spr->ResetAnimation(10, 0);
        }
    } else {
        if (0x23f < (int)uVar4) {
            spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
            if (spr != 0) {
                spr->ResetAnimation(0xb, 0);
            }
            goto sprite_section;
        }
        {
        unsigned int uVar7 = 0;
        if (puVar2 != 0) {
            uVar7 = puVar2[1];
        }
        unsigned int uVar6 = ((int)(puVar2 == 0) - 1) & uVar4;

        if (*(int*)((int)this + 0x6d4) > (int)uVar6 ||
            *(int*)((int)this + 0x6dc) < (int)uVar6 ||
            *(int*)((int)this + 0x6d8) > (int)uVar7 ||
            *(int*)((int)this + 0x6e0) < (int)uVar7) {

            uVar7 = 0;
            if (puVar2 != 0) {
                uVar7 = puVar2[1];
            }
            uVar6 = ((int)(puVar2 == 0) - 1) & uVar4;

            if (*(int*)((int)this + 0x6e4) > (int)uVar6 ||
                *(int*)((int)this + 0x6ec) < (int)uVar6 ||
                *(int*)((int)this + 0x6e8) > (int)uVar7 ||
                *(int*)((int)this + 0x6f0) < (int)uVar7) {

                uVar7 = 0;
                if (puVar2 != 0) {
                    uVar7 = puVar2[1];
                }
                uVar4 = ((int)(puVar2 == 0) - 1) & uVar4;

                if (*(int*)((int)this + 0x6f4) > (int)uVar4 ||
                    *(int*)((int)this + 0x6fc) < (int)uVar4 ||
                    *(int*)((int)this + 0x6f8) > (int)uVar7 ||
                    *(int*)((int)this + 0x700) < (int)uVar7) {
                    pcVar12 = (char*)((int)this + 0xd4);
                    iVar9 = 0x20;
                    do {
                        if (*pcVar12 != '\0') {
                            iVar5 = 0;
                            piVar11 = *(int**)((int)DAT_0046aa08 + 0x1a0);
                            if (piVar11 != 0) {
                                iVar5 = piVar11[1];
                            }
                            if (piVar11 != 0) {
                                iVar8 = *piVar11;
                            } else {
                                iVar8 = 0;
                            }
                            if (*piVar13 <= iVar8 && iVar8 <= piVar13[2] &&
                                piVar13[1] <= iVar5 &&
                                iVar5 <= piVar13[3] &&
                                *(Sprite**)((int)DAT_0046aa18 + 0x94) != 0) {
                                (*(Sprite**)((int)DAT_0046aa18 + 0x94))->ResetAnimation(0xf, 0);
                            }
                        }
                        piVar13 = piVar13 + 4;
                        pcVar12 = pcVar12 + 0x20;
                        iVar9 = iVar9 - 1;
                    } while (iVar9 != 0);
                } else {
                    spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
                    if (spr != 0) {
                        spr->ResetAnimation(0xf, 0);
                    }
                    FUN_00404350(DAT_0046aa24, 0,
                        *(int*)((int)this + 0x6f4), *(int*)((int)this + 0x6f8),
                        *(int*)((int)this + 0x6fc), *(int*)((int)this + 0x700),
                        10000, 0xfc, 2);
                }
            } else {
                spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
                if (spr != 0) {
                    spr->ResetAnimation(0xf, 0);
                }
                FUN_00404350(DAT_0046aa24, 0,
                    *(int*)((int)this + 0x6e4), *(int*)((int)this + 0x6e8),
                    *(int*)((int)this + 0x6ec), *(int*)((int)this + 0x6f0),
                    10000, 0xfc, 2);
            }
        } else {
            spr = *(Sprite**)((int)DAT_0046aa18 + 0x94);
            if (spr != 0) {
                spr->ResetAnimation(0xf, 0);
            }
            FUN_00404350(DAT_0046aa24, 0,
                *(int*)((int)this + 0x6d4), *(int*)((int)this + 0x6d8),
                *(int*)((int)this + 0x6dc), *(int*)((int)this + 0x6e0),
                10000, 0xfc, 2);
        }
        }
    }

    sprite_section:
    pvVar10 = *(void**)((int)this + 0x820);
    if (((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0) != 0) {
        if (*(int*)(*(int*)((int)this + 0x820) + 0x98) == 0) {
            ((Sprite*)*(void**)((int)this + 0x820))->ResetAnimation(-1, 0);
            *(int*)((int)this + 0x81c) = *(int*)((int)this + 0x81c) | 1;
            SC_WordSearch::FUN_436790();
        }
    }
    pvVar10 = *(void**)((int)this + 0x824);
    if (((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0) != 0) {
        if (*(int*)(*(int*)((int)this + 0x824) + 0x98) == 0) {
            SC_WordSearch::FUN_4368F0();
            ((Sprite*)*(void**)((int)this + 0x824))->ResetAnimation(-1, 0);
        }
    }
    FUN_00404230(DAT_0046aa24, 0, (char*)((int)this + 0x718), 0x49, 0xdc, 10000, 0);
    FUN_00432da0(DAT_0046aa18);
    pvVar10 = *(void**)((int)this + 0x82c);
    ((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0);
}

/* Function start: 0x436790 */
void SC_WordSearch::FUN_436790() {
    void* pvVar2;
    int uVar3;

    if (*(int*)((int)this + 0x98) != 0x2B) {
        pvVar2 = DAT_0046aa30;
        if (*(unsigned char*)((int)this + 0x81c) & 1) {
            uVar3 = FUN_00433ae0(pvVar2, 0, "PUZ_WAHOO_WON");
            if (uVar3 < 0 || *(int*)((int)pvVar2 + 0x98) - 1 < uVar3) {
                FUN_00425c50("Invalid gamestate %d", uVar3);
            }
            *(int*)(*(int*)((int)pvVar2 + 0x90) + uVar3 * 4) = 1;
            *(int*)(*(int*)((int)this + 0x830)) = 0x20;
            *(int*)(*(int*)((int)this + 0x830) + 4) = 1;
            *(int*)(*(int*)((int)this + 0x830) + 0x14) = 0x13;
            FUN_00444d90(4, 0x1BA8, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
        } else {
            uVar3 = FUN_00433ae0(pvVar2, 0, "PERIOD");
            if (uVar3 < 0 || *(int*)((int)pvVar2 + 0x98) - 1 < uVar3) {
                FUN_00425c50("Invalid gamestate %d", uVar3);
            }
            if (*(int*)(*(int*)((int)pvVar2 + 0x90) + uVar3 * 4) % 5 == 0) {
                *(int*)(*(int*)((int)this + 0x830)) = 0x2C;
                *(int*)(*(int*)((int)this + 0x830) + 4) = 1;
            } else {
                *(int*)(*(int*)((int)this + 0x830)) = 0x20;
                *(int*)(*(int*)((int)this + 0x830) + 4) = 2;
                *(int*)(*(int*)((int)this + 0x830) + 0x14) = 8;
            }
        }
    }
    FUN_00444e40(*(void**)((int)this + 0x830));
    void* pVar = *(void**)((int)this + 0x830);
    if (pVar != 0) {
        delete (SpriteAction*)pVar;
        *(void**)((int)this + 0x830) = 0;
    }
}

/* Function start: 0x4368F0 */
void SC_WordSearch::FUN_4368F0() {
    int iVar2 = DAT_0046cb90;
    void* pvVar3 = DAT_0046aa30;
    if (iVar2 < 0 || *(int*)((int)pvVar3 + 0x98) - 1 < iVar2) {
        FUN_00425c50("Invalid gamestate %d", iVar2);
    }
    int gsVal = *(int*)(*(int*)((int)pvVar3 + 0x90) + iVar2 * 4);
    if (gsVal == 0) {
        memset((char*)((int)this + 0xD4), 0, 0x400);
        strcpy((char*)((int)this + 0xD4), "Encounter");
        strcpy((char*)((int)this + 0xF4), "Eye");
        strcpy((char*)((int)this + 0x114), "Spaceship");
        strcpy((char*)((int)this + 0x134), "Wand");
        strcpy((char*)((int)this + 0x154), "Roswell");
        strcpy((char*)((int)this + 0x174), "Two");
        strcpy((char*)((int)this + 0x194), "Abduction");
        strcpy((char*)((int)this + 0x1B4), "Believe");
        strcpy((char*)((int)this + 0x1D4), "Sighting");
        strcpy((char*)((int)this + 0x1F4), "Cover-up");
    } else if (gsVal == 1) {
        memset((char*)((int)this + 0xD4), 0, 0x400);
        strcpy((char*)((int)this + 0xD4), "Encounter");
        strcpy((char*)((int)this + 0xF4), "Eye");
        strcpy((char*)((int)this + 0x114), "Spaceship");
        strcpy((char*)((int)this + 0x134), "Wand");
        strcpy((char*)((int)this + 0x154), "Area 51");
        strcpy((char*)((int)this + 0x174), "Roswell");
        strcpy((char*)((int)this + 0x194), "Information");
        strcpy((char*)((int)this + 0x1B4), "Barca-Lounger");
        strcpy((char*)((int)this + 0x1D4), "Two");
        strcpy((char*)((int)this + 0x1F4), "UFO");
        strcpy((char*)((int)this + 0x214), "Grey-men");
        strcpy((char*)((int)this + 0x234), "Abduction");
        strcpy((char*)((int)this + 0x254), "Sighting");
        strcpy((char*)((int)this + 0x274), "Believe");
        strcpy((char*)((int)this + 0x294), "Cover-up");
    } else if (gsVal == 2) {
        memset((char*)((int)this + 0xD4), 0, 0x400);
        strcpy((char*)((int)this + 0xD4), "Encounter");
        strcpy((char*)((int)this + 0xF4), "Eye");
        strcpy((char*)((int)this + 0x114), "Spaceship");
        strcpy((char*)((int)this + 0x134), "Area 51");
        strcpy((char*)((int)this + 0x154), "Wand");
        strcpy((char*)((int)this + 0x174), "Roswell");
        strcpy((char*)((int)this + 0x194), "Information");
        strcpy((char*)((int)this + 0x1B4), "Barca-Lounger");
        strcpy((char*)((int)this + 0x1D4), "Two");
        strcpy((char*)((int)this + 0x1F4), "Cow");
        strcpy((char*)((int)this + 0x214), "UFO");
        strcpy((char*)((int)this + 0x234), "Grey-men");
        strcpy((char*)((int)this + 0x254), "Abduction");
        strcpy((char*)((int)this + 0x274), "Believe");
        strcpy((char*)((int)this + 0x294), "Bogie");
        strcpy((char*)((int)this + 0x2B4), "Sighting");
        strcpy((char*)((int)this + 0x2D4), "Conspiracy");
        strcpy((char*)((int)this + 0x2F4), "Probe");
        strcpy((char*)((int)this + 0x314), "Nuts");
        strcpy((char*)((int)this + 0x334), "Cover-up");
    }
    *(char*)((int)this + 0x718) = 0;
    *(int*)((int)this + 0x818) = 0;
}

/* Function start: 0x437080 */
int SC_WordSearch::Exit(SC_Message* msg) {
    if (*(int*)msg != handlerId) {
        return 0;
    }
    FUN_004218c0((void*)((int)this + 0xC0));
    int iVar = *(int*)((int)msg + 0x10);
    if (iVar != 0) {
        if (iVar != 7) {
            return 0;
        }
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    return 1;
}
