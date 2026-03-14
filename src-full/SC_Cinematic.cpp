#include "SC_Cinematic.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"
#include "Animation.h"
#include "GameLoop.h"
#include "VBuffer.h"
#include "GameState.h"
#include "InputManager.h"
#include "Memory.h"
#include "Timer.h"
#include "LinkedList.h"
#include "ZBuffer.h"
#include <string.h>
#include <smack.h>

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" char* GetCinematicFilename(int id);

extern "C" extern void* DAT_0046aa30;
#define g_GameState_0046aa30 ((GameState*)DAT_0046aa30)
extern void* DAT_0046aa24;
extern void* DAT_0046aa08;
extern void* DAT_0046aa18;

extern "C" {
    extern int DAT_0046a6ec;
    extern void* DAT_0046aa14;
    extern char DAT_00473400;
}
extern int g_WaitForInputValue_004373bc;

// Engine list operations
extern void* __fastcall FUN_00403620(void* list);
extern void __fastcall FUN_00401c80(void* obj);


// Engine/Misc
extern void __cdecl FUN_00425a90(int width, int height);
extern "C" void WriteToLog(const char* format, ...);
extern void BlankScreen();
extern void __fastcall FUN_00432da0(void* self);

// File operations
extern "C" void* FUN_004260f0(char* name);
extern "C" int FileExists(const char*);

// Palette
#include "Palette.h"

// Message operations
extern void __cdecl FUN_00444e40(void* msg);

// Screen dimensions
extern "C" int* GetScreenWidth();
extern "C" int* GetScreenHeight();

// Summary
extern "C" void FUN_004307b0(int handle);

// List cleanup
extern void __fastcall FUN_004308c0(void* list);

// SmackWait is linked via smackw32.lib (IAT entry at 0x47655c in original binary)

// Text rendering (font system)
extern "C" void SetFontColor(int index);            // 0x4525EC - sets palette color for text
extern "C" void SetFontPosition(int x, int y);      // 0x4524C2 - sets text cursor position
extern "C" void DrawFontText(char* text, int len);   // 0x45329B - renders text via GDI

// CinematicAction - objects in list1 of DAT_0046aa24
struct CinematicAction {
    void Execute(int flag);  // 0x430980
};
void CinematicAction::Execute(int) {}

extern int g_PeriodStateIdx_0046cb90;
extern char* g_PeriodCharTable_0046cb94;
static char g_CinematicPath_00473cb0[256];

/* Function start: 0x44E3E0 */
extern "C" char* GetCinematicFilename(int param_1)
{
    if (param_1 >= 5000) {
        int stateIdx = g_PeriodStateIdx_0046cb90;
        GameState* gs = g_GameState_0046aa30;
        if (stateIdx < 0 || gs->maxStates - 1 < stateIdx) {
            ShowError("Invalid gamestate %d", stateIdx);
        }
        char suffix = g_PeriodCharTable_0046cb94[gs->stateValues[stateIdx]];
        sprintf(g_CinematicPath_00473cb0, "cine\\cin%4.4d%c.smk", param_1, (int)suffix);
        return g_CinematicPath_00473cb0;
    }
    sprintf(g_CinematicPath_00473cb0, "cine\\cin%4.4d.smk", param_1);
    return g_CinematicPath_00473cb0;
}

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
                    void* obj = ((LinkedList*)piVar5)->RemoveCurrent();
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
                    void* obj = ((LinkedList*)piVar6)->RemoveCurrent();
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
                        ((InvSlotItem*)((char*)obj + 4))->~InvSlotItem();
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

    char* movieName = GetCinematicFilename(moduleParam);
    char* moviePath = (char*)FUN_004260f0(movieName);

    if (FileExists(moviePath) == 0) {
        WriteToLog("missing cinematic %s", moviePath);
        EndCinematic();
    } else {
        if (g_GameState_0046aa30 != 0) {
            GameState* gs = g_GameState_0046aa30;
            if (gs->maxStates - 1 < 4) {
                ShowError("Invalid gamestate %d", 4);
            }
            if (gs->stateValues[4] != 0) {
                field_B8 |= 0x100;
                strcpy(&DAT_00473400, moviePath);
            }
        }

        field_AC = (int)new Animation(moviePath);

        *(int*)(DAT_0046a6ec + 0x1c) = 0;

        field_BC = pmsg[9];
        field_C0 = pmsg[10];

        if (pmsg[7] != 0) {
            field_C4 = pmsg[7];
        } else {
            field_C4 = 100;
        }

        field_C8 = pmsg[8];

        Animation* smk = (Animation*)field_AC;
        if ((field_B8 & 0x2) != 0 || smk->SetPalette(0, 0x100) == 0) {
            BlankScreen();
        }

        if (field_B8 & 0x10) {
            SendGameMessage(5, 0, handlerId, moduleParam, 0x13, field_BC, field_C0, 0, 0, 0);
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
                        void* obj = ((LinkedList*)piVar5)->RemoveCurrent();
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
                        void* obj = ((LinkedList*)piVar6)->RemoveCurrent();
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
                            ((InvSlotItem*)((char*)obj + 4))->~InvSlotItem();
                            FreeMemory(obj);
                        }
                    }
                }

                *(int*)(iVar3 + 0xa8) = 0;
            }
        }

        *(int*)(DAT_0046a6ec + 0x1c) = field_B4;

        if (field_B8 & 0x20) {
            SendGameMessage(5, 0, handlerId, moduleParam, 0x13, field_C4, field_C8, 0, 0, 0);
        }

        if (field_B8 & 0x4) {
            BlankScreen();
        }

        if (field_AC != 0) {
            GameState* gs = g_GameState_0046aa30;
            int idx = gs->FindState("CINE_SUMMARY");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            if (gs->stateValues[idx] != 0) {
                int handle = (int)((Animation*)field_AC)->smk;
                FUN_004307b0(handle);
            }
        }
    }

    if (field_AC != 0) {
        delete (Animation*)field_AC;
        field_AC = 0;
    }

    if (field_A8 != 0) {
        void* pal = (void*)field_A8;
        ((Palette*)pal)->~Palette();
        FreeMemory(pal);
        field_A8 = 0;
    }

    if (field_D0 != 0) {
        void* spr = (void*)field_D0;
        delete (SpriteAction*)spr;
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
        ((InputManager*)DAT_0046aa08)->Refresh(1);

        int* mousePtr = *(int**)((char*)DAT_0046aa08 + 0x1a0);
        int hasInput;
        if (mousePtr != 0 && (mousePtr[4] >= 1 || mousePtr[5] >= 1 || g_WaitForInputValue_004373bc != 0)) {
            hasInput = 1;
        } else {
            hasInput = 0;
        }

        if (hasInput) {
            EndCinematic();
        }

        Animation* smk = (Animation*)field_AC;
        VBuffer* vp = (VBuffer*)DAT_0046aa14;
        vp->CallBlitter(vp->clip_x1, vp->clip_x2, vp->clip_y1, vp->clip_y2, vp->clip_x1, vp->clip_y2, (int)smk->targetBuffer);
        FUN_00432da0(DAT_0046aa18);
        return;
    }

    // SmackWait linked directly via smackw32.lib

    while (1) {
        if (((InputManager*)DAT_0046aa08)->Refresh(1) != 0) {
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
            if (g_WaitForInputValue_004373bc != 0) {
                int key = WaitForInput();
                hasInput = (key == 0x1b);
            }
            if (hasInput) {
                EndCinematic();
                return;
            }
        }

        Animation* smk = (Animation*)field_AC;
        if (SmackWait(smk->smk) == 0) {
            break;
        }
    }

    /* Frame ready - play it */
    {
        Animation* smk = (Animation*)field_AC;
        int* smkData = (int*)(int)smk->smk;
        if (*(int*)((char*)smkData + 0x68) != 0) {
            smk->UpdatePalette(0, 0x100);
        }
    }

    ((Animation*)field_AC)->DoFrame();

    {
        Animation* smk = (Animation*)field_AC;
        VBuffer* surface = (VBuffer*)(int)smk->targetBuffer;
        int* screenH = GetScreenHeight();
        int h = *screenH - 1;
        int* screenW = GetScreenWidth();
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
        Animation* smk = (Animation*)field_AC;
        int* smkData = (int*)(int)smk->smk;
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

        Animation* smk = (Animation*)field_AC;
        int* smkSurface = (int*)(int)smk->targetBuffer;
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
                        void* obj = ((LinkedList*)list1)->RemoveCurrent();
                        if (obj != 0) {
                            ((CinematicAction*)obj)->Execute(1);
                        }
                    }
                }

                FUN_004308c0(*(void**)(iVar2 + 0xa4));
                ((ZBQueue*)*(void**)(iVar2 + 0x9c))->ClearList();
                *(int*)(iVar2 + 0xa8) = 0;
            }
        }

        Animation* smk2 = (Animation*)field_AC;
        VBuffer* vp = (VBuffer*)DAT_0046aa14;
        vp->CallBlitter(vp->clip_x1, vp->clip_x2, vp->clip_y1, vp->clip_y2, vp->clip_x1, vp->clip_y2, (int)smk2->targetBuffer);
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
            delete (SpriteAction*)spr;
            field_D0 = 0;
        }
    } else {
        SendGameMessage(savedMsgData, savedCommand, moduleParam, handlerId, 4, 0, 0, 0, 0, 0);
    }
}
