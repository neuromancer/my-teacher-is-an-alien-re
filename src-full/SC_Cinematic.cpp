#include "SC_Cinematic.h"
#include "SmkPlayer.h"
#include "GameLoop.h"
#include "VBuffer.h"
#include "GameState.h"
#include "InputManager.h"
#include "Memory.h"
#include "Timer.h"
#include "LinkedList.h"
#include <string.h>
#include <smack.h>

extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern "C" char* FUN_0044e3e0(int handle);

extern GameState* g_GameState_0046aa30;
extern void* DAT_0046aa24;
extern void* DAT_0046aa08;
extern void* DAT_0046aa18;

extern "C" {
    extern int DAT_0046a6ec;
    extern void* DAT_0046aa14;
    extern int DAT_0046ac04;
    extern char DAT_00473400;
}

// Engine list operations
extern void* __fastcall FUN_00403520(void* list);
extern void* __fastcall FUN_004035a0(void* list);
extern void* __fastcall FUN_00403620(void* list);
extern void __fastcall FUN_00401c80(void* obj);

// Embedded object destructor (16-byte object at +4 in 3rd list objects)
extern void __fastcall FUN_00401130(void* obj);

// Engine/Misc
extern void __cdecl FUN_00425a90(int width, int height);
extern void __cdecl FUN_00425d70(char* msg, ...);
extern void __cdecl FUN_00425c50(char* msg, ...);
extern "C" void FUN_00425f10();
extern void __fastcall FUN_00432da0(void* self);

// File operations
extern "C" void* FUN_004260f0(char* name);
extern "C" int FUN_00425fa0(void* path);

// Palette
extern void __fastcall FUN_0041dc10(void* palette);

// Message operations
extern void __fastcall FUN_00444af0(void* msg);
extern void __cdecl FUN_00444e40(void* msg);

// Screen dimensions (0x4205E0, 0x4205F0 - return ptrs to render target w/h)
extern "C" int* FUN_004205f0();
extern "C" int* FUN_004205e0();

// Summary
extern "C" void FUN_004307b0(int handle);

// List cleanup
extern void __fastcall FUN_004308c0(void* list);
extern void __fastcall FUN_004145f0(void* list);

// SmackWait function pointer (IAT entry at 0x47655c)
extern "C" extern int (__stdcall *DAT_0047655c)(int);

// Text rendering (font system)
extern "C" void SetFontColor(int index);            // 0x4525EC - sets palette color for text
extern "C" void SetFontPosition(int x, int y);      // 0x4524C2 - sets text cursor position
extern "C" void DrawFontText(char* text, int len);   // 0x45329B - renders text via GDI

// CinematicAction - objects in list1 of DAT_0046aa24
struct CinematicAction {
    void Execute(int flag);  // 0x430980
};

/* Function start: 0x42FBD0 */
SC_Cinematic::SC_Cinematic() {
    memset(&field_A8, 0, 48);
    handlerId = 3;
}

/* Function start: 0x42FC90 */
SC_Cinematic::~SC_Cinematic() {
    ShutDown(0);
}

/* Function start: 0x42FCF0 */
void SC_Cinematic::Init(SC_Message* msg) {
    int* pmsg = (int*)msg;

    CopyCommandData(msg);

    memset(&field_A8, 0, 12 * 4);
    field_B8 = 0x10;
    field_B4 = *(int*)(DAT_0046a6ec + 0x1c);

    int iVar11 = (int)DAT_0046aa24;
    if (iVar11 != 0) {
        int savedState = *(int*)(iVar11 + 0x98);
        if (savedState != 0) {
            *(int*)(iVar11 + 0x98) = 0;

            int* piVar5 = *(int**)(iVar11 + 0xa0);
            if (*piVar5 != 0) {
                piVar5[2] = *piVar5;
                while (*piVar5 != 0) {
                    void* obj = FUN_00403520(piVar5);
                    if (obj != 0) {
                        *(int*)obj = 0x461030;
                        FreeMemory(obj);
                    }
                }
            }

            int* piVar6 = *(int**)(iVar11 + 0xa4);
            if (*piVar6 != 0) {
                piVar6[2] = *piVar6;
                while (*piVar6 != 0) {
                    void* obj = FUN_004035a0(piVar6);
                    if (obj != 0) {
                        FUN_00401c80(obj);
                        FreeMemory(obj);
                    }
                }
            }

            LinkedList* list3 = *(LinkedList**)(iVar11 + 0x9c);
            if (list3->head != 0) {
                list3->current = list3->head;
                while (list3->head != 0) {
                    void* obj = list3->RemoveCurrent();
                    if (obj != 0) {
                        *(int*)obj = 0x46102c;
                        FUN_00401130((char*)obj + 4);
                        FreeMemory(obj);
                    }
                }
            }

            *(int*)(iVar11 + 0xa8) = 0;
        }
        field_B0 = savedState;
    }

    moduleParam = pmsg[1];
    field_B8 |= pmsg[5];

    if (pmsg[13] != 0) {
        field_D0 = pmsg[13];
        pmsg[13] = 0;
    }

    char* movieName = FUN_0044e3e0(moduleParam);
    char* moviePath = (char*)FUN_004260f0(movieName);

    if (FUN_00425fa0(moviePath) == 0) {
        FUN_00425d70("missing cinematic %s", moviePath);
        EndCinematic();
    } else {
        if (g_GameState_0046aa30 != 0) {
            GameState* gs = g_GameState_0046aa30;
            if (gs->maxStates - 1 < 4) {
                FUN_00425c50("Invalid gamestate %d", 4);
            }
            if (gs->stateValues[4] != 0) {
                field_B8 |= 0x100;
                strcpy(&DAT_00473400, moviePath);
            }
        }

        field_AC = (int)new SmkPlayer(moviePath);

        *(int*)(DAT_0046a6ec + 0x1c) = 0;

        field_BC = pmsg[9];
        field_C0 = pmsg[10];

        if (pmsg[7] != 0) {
            field_C4 = pmsg[7];
        } else {
            field_C4 = 100;
        }

        field_C8 = pmsg[8];

        SmkPlayer* smk = (SmkPlayer*)field_AC;
        if ((field_B8 & 0x2) != 0 || smk->Open(0, 0x100) == 0) {
            FUN_00425f10();
        }

        if (field_B8 & 0x10) {
            FUN_00444d90(5, 0, handlerId, moduleParam, 0x13, field_BC, field_C0, 0, 0, 0);
        }
    }
}

/* Function start: 0x4300D0 */
int SC_Cinematic::ShutDown(SC_Message* msg) {
    if (msg != 0) {
        if (DAT_0046aa24 != 0) {
            int iVar3 = (int)DAT_0046aa24;
            if (*(int*)(iVar3 + 0x98) != field_B0) {
                *(int*)(iVar3 + 0x98) = field_B0;

                int* piVar5 = *(int**)(iVar3 + 0xa0);
                if (*piVar5 != 0) {
                    piVar5[2] = *piVar5;
                    while (*piVar5 != 0) {
                        void* obj = FUN_00403520(piVar5);
                        if (obj != 0) {
                            *(int*)obj = 0x461030;
                            FreeMemory(obj);
                        }
                    }
                }

                int* piVar6 = *(int**)(iVar3 + 0xa4);
                if (*piVar6 != 0) {
                    piVar6[2] = *piVar6;
                    while (*piVar6 != 0) {
                        void* obj = FUN_004035a0(piVar6);
                        if (obj != 0) {
                            FUN_00401c80(obj);
                            FreeMemory(obj);
                        }
                    }
                }

                int* list3 = *(int**)(iVar3 + 0x9c);
                if (*list3 != 0) {
                    list3[2] = *list3;
                    while (*list3 != 0) {
                        void* obj = FUN_00403620(list3);
                        if (obj != 0) {
                            *(int*)obj = 0x46102c;
                            FUN_00401130((char*)obj + 4);
                            FreeMemory(obj);
                        }
                    }
                }

                *(int*)(iVar3 + 0xa8) = 0;
            }
        }

        *(int*)(DAT_0046a6ec + 0x1c) = field_B4;

        if (field_B8 & 0x20) {
            FUN_00444d90(5, 0, handlerId, moduleParam, 0x13, field_C4, field_C8, 0, 0, 0);
        }

        if (field_B8 & 0x4) {
            FUN_00425f10();
        }

        if (field_AC != 0) {
            GameState* gs = g_GameState_0046aa30;
            int idx = gs->FindState("CINE_SUMMARY");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                FUN_00425c50("Invalid gamestate %d", idx);
            }
            if (gs->stateValues[idx] != 0) {
                int handle = *(int*)(field_AC + 0xc);
                FUN_004307b0(handle);
            }
        }
    }

    if (field_AC != 0) {
        delete (SmkPlayer*)field_AC;
        field_AC = 0;
    }

    if (field_A8 != 0) {
        void* pal = (void*)field_A8;
        FUN_0041dc10(pal);
        FreeMemory(pal);
        field_A8 = 0;
    }

    if (field_D0 != 0) {
        void* spr = (void*)field_D0;
        FUN_00444af0(spr);
        FreeMemory(spr);
        field_D0 = 0;
    }

    return 0;
}

/* Function start: 0x430350 */
void SC_Cinematic::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }

    if (field_CC != 0) {
        ((GameLoop*)DAT_0046aa08)->ProcessEvents(1);

        int* mousePtr = *(int**)((char*)DAT_0046aa08 + 0x1a0);
        int hasInput;
        if (mousePtr != 0 && (mousePtr[4] >= 1 || mousePtr[5] >= 1 || DAT_0046ac04 != 0)) {
            hasInput = 1;
        } else {
            hasInput = 0;
        }

        if (hasInput) {
            EndCinematic();
        }

        SmkPlayer* smk = (SmkPlayer*)field_AC;
        VBuffer* vp = (VBuffer*)DAT_0046aa14;
        vp->CallBlitter(vp->clip_x1, vp->clip_x2, vp->clip_y1, vp->clip_y2, vp->clip_x1, vp->clip_y2, smk->field_18);
        FUN_00432da0(DAT_0046aa18);
        return;
    }

    int (__stdcall *smkWait)(int) = DAT_0047655c;

    while (1) {
        if (((GameLoop*)DAT_0046aa08)->ProcessEvents(1) != 0) {
            EndCinematic();
            return;
        }

        if (!(field_B8 & 0x1)) {
            int* mousePtr = *(int**)((char*)DAT_0046aa08 + 0x1a0);
            int hasInput;
            if (mousePtr != 0) {
                if (mousePtr[5] >= 2) {
                    hasInput = 1;
                } else {
                    hasInput = 0;
                }
            } else {
                hasInput = 0;
            }

            if (hasInput) {
                EndCinematic();
                return;
            }

            hasInput = 0;
            if (DAT_0046ac04 != 0) {
                int key = WaitForInput();
                hasInput = (key == 0x1b);
            }
            if (hasInput) {
                EndCinematic();
                return;
            }
        }

        SmkPlayer* smk = (SmkPlayer*)field_AC;
        if (smkWait(smk->field_0C) == 0) {
            break;
        }
    }

    /* Frame ready - play it */
    {
        SmkPlayer* smk = (SmkPlayer*)field_AC;
        int* smkData = (int*)smk->field_0C;
        if (*(int*)((char*)smkData + 0x68) != 0) {
            smk->SetVolume(0, 0x100);
        }
    }

    ((SmkPlayer*)field_AC)->Render();

    {
        SmkPlayer* smk = (SmkPlayer*)field_AC;
        VBuffer* surface = (VBuffer*)smk->field_18;
        int* screenH = FUN_004205f0();
        int h = *screenH - 1;
        int* screenW = FUN_004205e0();
        int w = *screenW - 1;

        surface->CallBlitter5(surface->clip_x1, surface->clip_x2, surface->clip_y1, surface->clip_y2, 0, w, 0, h);
    }

    if (field_B8 & 0x100) {
        SetFontColor(0xfa);
        SetFontPosition(0x14, 0x14);
        int len = strlen(&DAT_00473400);
        DrawFontText(&DAT_00473400, len);
    }

    {
        SmkPlayer* smk = (SmkPlayer*)field_AC;
        int* smkData = (int*)smk->field_0C;
        int totalFrames = *(int*)((char*)smkData + 0xc);
        int skipped = *(int*)((char*)smkData + 0x374);
        if (totalFrames - skipped != 1) {
            smk->NextFrame();
            return;
        }
    }

    /* Movie ended - last frame */
    if (field_B8 & 0x200) {
        field_CC = 1;

        SmkPlayer* smk = (SmkPlayer*)field_AC;
        int* smkSurface = (int*)smk->field_18;
        FUN_00425a90(smkSurface[5], smkSurface[6]);

        *(int*)(DAT_0046a6ec + 0x1c) = field_B4;

        if (DAT_0046aa24 != 0) {
            int newState = 1;
            if (field_B0 != 0) {
                newState = field_B0;
            }
            int iVar2 = (int)DAT_0046aa24;
            int* statePtr = (int*)(iVar2 + 0x98);
            if (*statePtr != newState) {
                *statePtr = newState;

                int* list1 = *(int**)(iVar2 + 0xa0);
                if (*list1 != 0) {
                    list1[2] = *list1;
                    while (*list1 != 0) {
                        void* obj = FUN_00403520(list1);
                        if (obj != 0) {
                            ((CinematicAction*)obj)->Execute(1);
                        }
                    }
                }

                FUN_004308c0(*(void**)(iVar2 + 0xa4));
                FUN_004145f0(*(void**)(iVar2 + 0x9c));
                *(int*)(iVar2 + 0xa8) = 0;
            }
        }

        SmkPlayer* smk2 = (SmkPlayer*)field_AC;
        VBuffer* vp = (VBuffer*)DAT_0046aa14;
        vp->CallBlitter(vp->clip_x1, vp->clip_x2, vp->clip_y1, vp->clip_y2, vp->clip_x1, vp->clip_y2, smk2->field_18);
    } else {
        Timer timer;
        timer.Wait(0x96);
        EndCinematic();
    }
}

/* Function start: 0x4306A0 */
int SC_Cinematic::AddMessage(SC_Message* msg) {
    int* pmsg = (int*)msg;
    WriteMessageAddress(msg);

    if (pmsg[11] == 0x1b || pmsg[10] > 1) {
        pmsg[4] = 0;
        EndCinematic();
    }

    return 1;
}

/* Function start: 0x430730 */
void SC_Cinematic::EndCinematic() {
    if (field_D0 != 0) {
        FUN_00444e40((void*)field_D0);
        void* spr = (void*)field_D0;
        if (spr != 0) {
            FUN_00444af0(spr);
            FreeMemory(spr);
            field_D0 = 0;
        }
    } else {
        FUN_00444d90(4, 0, handlerId, moduleParam, 0, 0, savedCommand, savedMsgData, 0, 0);
    }
}
