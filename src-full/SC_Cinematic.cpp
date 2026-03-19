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

extern "C" extern GameState* DAT_0046aa30;
#define g_GameState_0046aa30 (DAT_0046aa30)
#include "ZBufferManager.h"
extern ZBufferManager* DAT_0046aa24;
extern InputManager* DAT_0046aa08;
class MouseControl;
extern MouseControl* DAT_0046aa18;

extern "C" {
    extern int DAT_0046a6ec;
    extern void* DAT_0046aa14;
    extern char DAT_00473400;
}
extern int g_WaitForInputValue_004373bc;

// Engine list operations



// Engine/Misc
extern "C" void SetVideoRes(int, int);
extern "C" void WriteToLog(const char* format, ...);
extern void BlankScreen();
#include "MouseControl.h"

// File operations
extern "C" char* FormatAssetPath(char*, ...);
extern "C" int FileExists(const char*);

// Palette
#include "Palette.h"

// Screen dimensions
extern "C" int* GetScreenWidth();
extern "C" int* GetScreenHeight();

extern "C" void __stdcall SmackSummary(void*, void*);

/* Function start: 0x4307B0 */
extern "C" void ShowSmackSummary(unsigned int handle)
{
    int summary[19];
    char buffer[1036];

    SmackSummary((void*)handle, summary);

    sprintf(buffer,
        "total time = %lu MS/100 per frame = %lu",
        summary[0], (unsigned int)(summary[1] * 1000) / summary[0]);

    MessageBoxA(0, buffer, "Summary", 0);
}

// FUN_004308c0 = ClearActionList - implemented below
void __fastcall ClearActionList(LinkedList* list);

// SmackWait is linked via smackw32.lib (IAT entry at 0x47655c in original binary)

// Text rendering (font system)
extern "C" void SetFontColor(int index);            // 0x4525EC - sets palette color for text
extern "C" void SetFontPosition(int x, int y);      // 0x4524C2 - sets text cursor position
extern "C" void DrawFontText(char* text, int len);   // 0x45329B - renders text via GDI

// CinematicAction = SoundCommand (vtable 0x461030, sdtor 0x430980)
#include "SoundCommand.h"

extern int g_PeriodStateIdx_0046cb90;
extern char* g_PeriodCharTable_0046cb94;
static char g_CinematicPath_00473cb0[256];

static char g_AnimNameBuf_00473cf0[64];

/* Function start: 0x44E320 */
extern "C" char* MakeAnimName(char* baseName)
{
    int len = strlen(baseName);
    if (len < 4) {
        ShowError("MakeAnimName - invalid base name");
    }
    int num = atoi(baseName + (len - 4));
    if (num == 0) {
        return baseName;
    }
    if (num > 4999) {
        int stateIdx = g_PeriodStateIdx_0046cb90;
        GameState* gs = g_GameState_0046aa30;
        if (stateIdx < 0 || gs->maxStates - 1 < stateIdx) {
            ShowError("Invalid gamestate %d", stateIdx);
        }
        sprintf(g_AnimNameBuf_00473cf0, "%s%c.smk", baseName,
                (int)(char)g_PeriodCharTable_0046cb94[gs->stateValues[stateIdx]]);
        return g_AnimNameBuf_00473cf0;
    }
    sprintf(g_AnimNameBuf_00473cf0, "%s.smk", baseName);
    return g_AnimNameBuf_00473cf0;
}

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
    memset(&palette, 0, 48);
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

    memset(&palette, 0, 12 * 4);
    flags = 0x10;
    savedRenderCtx = *(int*)(DAT_0046a6ec + 0x1c);

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
                        // vtable reset before free (no-op)
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
                        ((ZBuffer*)obj)->CleanUpVBuffer();
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
                        // vtable reset before free (no-op)
                        ((Rect*)((char*)obj + 4))->~Rect();
                        FreeMemory(obj);
                    }
                }
            }

            *(int*)(iVar11 + 0xa8) = 0;
        }
        savedZBState = savedState;
    }

    moduleParam = pmsg[1];
    flags |= pmsg[5];

    if (pmsg[13] != 0) {
        pendingAction = pmsg[13];
        pmsg[13] = 0;
    }

    char* movieName = GetCinematicFilename(moduleParam);
    char* moviePath = FormatAssetPath(movieName);

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
                flags |= 0x100;
                strcpy(&DAT_00473400, moviePath);
            }
        }

        animation = (int)new Animation(moviePath);

        *(int*)(DAT_0046a6ec + 0x1c) = 0;

        startX = pmsg[9];
        startY = pmsg[10];

        if (pmsg[7] != 0) {
            volume = pmsg[7];
        } else {
            volume = 100;
        }

        soundParam = pmsg[8];

        Animation* smk = (Animation*)animation;
        if ((flags & 0x2) != 0 || smk->SetPalette(0, 0x100) == 0) {
            BlankScreen();
        }

        if (flags & 0x10) {
            SendGameMessage(5, 0, handlerId, moduleParam, 0x13, startX, startY, 0, 0, 0);
        }
    }
}

/* Function start: 0x4300D0 */
int SC_Cinematic::ShutDown(SC_Message* msg) {
    if (msg != 0) {
        if (DAT_0046aa24 != 0) {
            int iVar3 = (int)DAT_0046aa24;
            if (*(int*)(iVar3 + 0x98) != savedZBState) {
                *(int*)(iVar3 + 0x98) = savedZBState;

                int* piVar5 = *(int**)(iVar3 + 0xa0);
                if (*piVar5 != 0) {
                    piVar5[2] = *piVar5;
                    while (*piVar5 != 0) {
                        void* obj = ((LinkedList*)piVar5)->RemoveCurrent();
                        if (obj != 0) {
                            // vtable reset before free (no-op)
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
                            ((ZBuffer*)obj)->CleanUpVBuffer();
                            FreeMemory(obj);
                        }
                    }
                }

                int* list3 = *(int**)(iVar3 + 0x9c);
                if (*list3 != 0) {
                    list3[2] = *list3;
                    while (*list3 != 0) {
                        void* obj = ((LinkedList*)list3)->RemoveCurrent();
                        if (obj != 0) {
                            // vtable reset before free (no-op)
                            ((Rect*)((char*)obj + 4))->~Rect();
                            FreeMemory(obj);
                        }
                    }
                }

                *(int*)(iVar3 + 0xa8) = 0;
            }
        }

        *(int*)(DAT_0046a6ec + 0x1c) = savedRenderCtx;

        if (flags & 0x20) {
            SendGameMessage(5, 0, handlerId, moduleParam, 0x13, volume, soundParam, 0, 0, 0);
        }

        if (flags & 0x4) {
            BlankScreen();
        }

        if (animation != 0) {
            GameState* gs = g_GameState_0046aa30;
            int idx = gs->FindState("CINE_SUMMARY");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            if (gs->stateValues[idx] != 0) {
                int handle = (int)((Animation*)animation)->smk;
                ShowSmackSummary(handle);
            }
        }
    }

    if (animation != 0) {
        delete (Animation*)animation;
        animation = 0;
    }

    if (palette != 0) {
        void* pal = (void*)palette;
        delete (Palette*)pal;
        palette = 0;
    }

    if (pendingAction != 0) {
        void* spr = (void*)pendingAction;
        delete (SpriteAction*)spr;
        pendingAction = 0;
    }

    return 0;
}

/* Function start: 0x430350 */
void SC_Cinematic::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }

    if (waitForInput != 0) {
        (DAT_0046aa08)->Refresh(1);

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

        Animation* smk = (Animation*)animation;
        VBuffer* vp = (VBuffer*)DAT_0046aa14;
        vp->CallBlitter(vp->clip_x1, vp->clip_x2, vp->clip_y1, vp->clip_y2, vp->clip_x1, vp->clip_y2, (int)smk->targetBuffer);
        (DAT_0046aa18)->DrawCursor();
        return;
    }

    // SmackWait linked directly via smackw32.lib

    while (1) {
        if ((DAT_0046aa08)->Refresh(1) != 0) {
            EndCinematic();
            return;
        }

        if (!(flags & 0x1)) {
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

        Animation* smk = (Animation*)animation;
        if (SmackWait(smk->smk) == 0) {
            break;
        }
    }

    /* Frame ready - play it */
    {
        Animation* smk = (Animation*)animation;
        int* smkData = (int*)(int)smk->smk;
        if (*(int*)((char*)smkData + 0x68) != 0) {
            smk->UpdatePalette(0, 0x100);
        }
    }

    ((Animation*)animation)->DoFrame();

    {
        Animation* smk = (Animation*)animation;
        VBuffer* surface = (VBuffer*)(int)smk->targetBuffer;
        int* screenH = GetScreenHeight();
        int h = *screenH - 1;
        int* screenW = GetScreenWidth();
        int w = *screenW - 1;

        surface->CallBlitter5(surface->clip_x1, surface->clip_x2, surface->clip_y1, surface->clip_y2, 0, w, 0, h);
    }

    if (flags & 0x100) {
        SetFontColor(0xfa);
        SetFontPosition(0x14, 0x14);
        int len = strlen(&DAT_00473400);
        DrawFontText(&DAT_00473400, len);
    }

    {
        Animation* smk = (Animation*)animation;
        int* smkData = (int*)(int)smk->smk;
        int totalFrames = *(int*)((char*)smkData + 0xc);
        int skipped = *(int*)((char*)smkData + 0x374);
        if (totalFrames - skipped != 1) {
            smk->NextFrame();
            return;
        }
    }

    /* Movie ended - last frame */
    if (flags & 0x200) {
        waitForInput = 1;

        Animation* smk = (Animation*)animation;
        int* smkSurface = (int*)(int)smk->targetBuffer;
        SetVideoRes(smkSurface[5], smkSurface[6]);

        *(int*)(DAT_0046a6ec + 0x1c) = savedRenderCtx;

        if (DAT_0046aa24 != 0) {
            int newState = 1;
            if (savedZBState != 0) {
                newState = savedZBState;
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
                            delete (SoundCommand*)obj;
                        }
                    }
                }

                ClearActionList((LinkedList*)*(void**)(iVar2 + 0xa4));
                ((ZBQueue*)*(void**)(iVar2 + 0x9c))->ClearList();
                *(int*)(iVar2 + 0xa8) = 0;
            }
        }

        Animation* smk2 = (Animation*)animation;
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
    if (pendingAction != 0) {
        EnqueueSpriteAction((void*)pendingAction);
        void* spr = (void*)pendingAction;
        if (spr != 0) {
            delete (SpriteAction*)spr;
            pendingAction = 0;
        }
    } else {
        SendGameMessage(savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }
}

/* Function start: 0x4308C0 */
void __fastcall ClearActionList(LinkedList* list)
{
    if (list->head != 0) {
        list->current = list->head;
        do {
            void* data = list->RemoveCurrent();
            if (data != 0) {
                void* ptr = *(void**)((int)data + 4);
                if (ptr != 0) {
                    delete (VBuffer*)ptr;
                    *(void**)((int)data + 4) = 0;
                }
                Handler* obj = *(Handler**)((int)data + 8);
                if (obj != 0) {
                    delete obj;
                    *(void**)((int)data + 8) = 0;
                }
                FreeMemory(data);
            }
        } while (list->head != 0);
    }
}
