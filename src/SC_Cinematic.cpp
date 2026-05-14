#include "SC_Cinematic.h"
#include "stubs.h"
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
#include "RenderEntry.h"
#include "DrawEntry.h"
#include "globals.h"
#include <string.h>
#include <smack.h>

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" char* GetCinematicFilename(int id);

#define g_GameState_0046aa30 (g_GameState_0046aa30)
#include "ZBufferManager.h"
class MouseControl;
#include "GameEngine.h"

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
    unsigned int summary[21];
    char buffer[1036];

    SmackSummary((void*)handle, summary);

    unsigned int fps = (summary[3] * 100) / summary[0];
    unsigned int ms100 = (summary[3] * 100) / summary[1];
    unsigned int decompPct = (summary[8] * 100) / summary[0];
    unsigned int blitPct = (summary[6] * 1000) / summary[0];

    sprintf(buffer,
        "total time = %lu \nMS*100 per frame (100000/MS100PerFrame=Frames/Sec) = %lu "
        "\nTime to open and prepare for decompression = %lu "
        "\nTotal Frames displayed = %lu "
        "\nTotal number of skipped frames = %lu "
        "\nTotal number of sound skips = %lu "
        "\nTotal time spent blitting = %lu "
        "\nTotal time spent reading  = %lu "
        "\nTotal time spent decompressing = %lu "
        "\nTotal time spent reading in background = %lu "
        "\nTotal io speed (bytes/second) = %lu "
        "\nSlowest single frame time = %lu "
        "\nSecond slowest single frame time = %lu "
        "\nSlowest single frame number = %lu "
        "\nSecond slowest single frame number = %lu "
        "\nAverage size of the frame = %lu "
        "\nHighest 1 sec data rate = %lu "
        "\nHighest 1 sec data rate starting frame = %lu "
        "\nHighest amount of memory allocated = %lu "
        "\nTotal extra memory allocated = %lu "
        "\nHighest extra memory actually used = %lu "
        "\nfps: %i\t\t\t"
        "\ndecomp %%: %i\t\t"
        "\nblit %%: %i\t\t"
        "\nread %%: %i\t\t\n",
        summary[0], summary[1], summary[2], summary[3],
        summary[4], summary[5], summary[6], summary[7],
        summary[8], summary[9], summary[10], summary[11],
        summary[12], summary[13], summary[14], summary[15],
        summary[16], summary[17], summary[18], summary[19],
        summary[20],
        fps, decompPct, blitPct, ms100);

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

static char g_CinematicPath_00473cb0[256];

static char g_AnimNameBuf_00473cf0[64];

/* Function start: 0x44E320 */
extern "C" char* MakeSoundName_Cine(char* baseName)
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
void SC_Cinematic::Init(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;

    CopyCommandData(msg);

    memset(&palette, 0, 12 * 4);
    flags = 0x10;
    savedRenderCtx = g_GameEngine_0046a6ec->m_frameTime;

    if (g_ZBufferManager_0046aa24 != 0) {
        ZBufferManager* zbm = g_ZBufferManager_0046aa24;
        int savedState = zbm->m_state;
        if (savedState != 0) {
            zbm->m_state = 0;

            ZBQueue* q1 = zbm->m_queueA0;
            if (q1->head != 0) {
                q1->current = q1->head;
                while (q1->head != 0) {
                    void* obj = q1->Pop();
                    if (obj != 0) {
                        delete (SoundCommand*)obj;
                    }
                }
            }

            ZBQueue* q2 = zbm->m_queueA4;
            if (q2->head != 0) {
                q2->current = q2->head;
                while (q2->head != 0) {
                    void* obj = q2->Pop();
                    if (obj != 0) {
                        ((ZBuffer*)obj)->CleanUpVBuffer();
                        FreeMemory(obj);
                    }
                }
            }

            ZBQueue* q3 = zbm->m_queue9c;
            if (q3->head != 0) {
                q3->current = q3->head;
                while (q3->head != 0) {
                    ListNode* node = q3->current;
                    void* data;
                    if (node == 0) {
                        data = 0;
                    } else {
                        if (q3->head == node) {
                            q3->head = node->next;
                        }
                        if (q3->tail == q3->current) {
                            q3->tail = q3->current->prev;
                        }
                        node = q3->current;
                        if (node->prev != 0) {
                            node->prev->next = node->next;
                        }
                        node = q3->current;
                        if (node->next != 0) {
                            node->next->prev = node->prev;
                        }
                        data = q3->GetCurrentData();
                        if (q3->current != 0) {
                            delete q3->current;
                            q3->current = 0;
                        }
                        q3->current = q3->head;
                    }
                    if (data != 0) {
                        ((RenderEntry*)data)->RenderEntry::~RenderEntry();
                        FreeMemory(data);
                    }
                }
            }
            zbm->m_palette = 0;
        }
        savedZBState = savedState;
    }

    moduleParam = action->addressValue;
    flags |= action->extra1;

    if (action->childAction != 0) {
        pendingAction = action->childAction;
        action->childAction = 0;
    }

    char* movieName = GetCinematicFilename(moduleParam);
    char* moviePath = FormatAssetPath(movieName);

    if (FileExists(moviePath) != 0) {
        if (g_GameState_0046aa30 != 0) {
            GameState* gs = g_GameState_0046aa30;
            if (gs->maxStates - 1 < 4) {
                ShowError("Invalid gamestate %d", 4);
            }
            if (gs->stateValues[4] != 0) {
                flags |= 0x100;
                strcpy(&g_CinematicDebugStr_00473400, moviePath);
            }
        }

        animation = new Animation(moviePath);

        g_GameEngine_0046a6ec->m_frameTime = 0;

        startX = action->button1;
        startY = action->button2;

        if (action->mousePos.x != 0) {
            volume = action->mousePos.x;
        } else {
            volume = 100;
        }

        soundParam = action->mousePos.y;

        if ((flags & 0x2) != 0 || animation->SetPalette(0, 0x100) == 0) {
            BlankScreen();
        }

        if (flags & 0x10) {
            SendGameMessage(5, 0, handlerId, moduleParam, 0x13, startX, startY, 0, 0, 0);
        }
    } else {
        WriteToLog("missing cinematic %s", moviePath);
        EndCinematic();
    }
}

/* Function start: 0x4300D0 */
int SC_Cinematic::ShutDown(SC_MessageParser* msg) {
    if (msg != 0) {
        if (g_ZBufferManager_0046aa24 != 0) {
            ZBufferManager* zbm = g_ZBufferManager_0046aa24;
            if (zbm->m_state != savedZBState) {
                zbm->m_state = savedZBState;

                ZBQueue* q1 = zbm->m_queueA0;
                if (q1->head != 0) {
                    q1->current = q1->head;
                    while (q1->head != 0) {
                        void* obj = q1->Pop();
                        if (obj != 0) {
                            delete (SoundCommand*)obj;
                        }
                    }
                }

                ZBQueue* q2 = zbm->m_queueA4;
                if (q2->head != 0) {
                    q2->current = q2->head;
                    while (q2->head != 0) {
                        void* obj = q2->Pop();
                        if (obj != 0) {
                            ((ZBuffer*)obj)->CleanUpVBuffer();
                            FreeMemory(obj);
                        }
                    }
                }

                ZBQueue* q3 = zbm->m_queue9c;
                if (q3->head != 0) {
                    q3->current = q3->head;
                    while (q3->head != 0) {
                        RenderEntry* entry = (RenderEntry*)q3->Pop();
                        if (entry != 0) {
                            entry->RenderEntry::~RenderEntry();
                            FreeMemory(entry);
                        }
                    }
                }
                zbm->m_palette = 0;
            }
        }

        g_GameEngine_0046a6ec->m_frameTime = savedRenderCtx;

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
                int handle = (int)animation->smk;
                ShowSmackSummary(handle);
            }
        }
    }

    if (animation != 0) {
        delete animation;
        animation = 0;
    }

    if (palette != 0) {
        delete palette;
        palette = 0;
    }

    if (pendingAction != 0) {
        delete pendingAction;
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
        (g_InputManager_0046aa08)->Refresh(1);

        InputState* mouse = g_InputManager_0046aa08->pMouse;
        int hasInput;
        if (mouse != 0 && (mouse->ext1 >= 1 || mouse->ext2 >= 1 || g_WaitForInputValue_0046ac04 != 0)) {
            hasInput = 1;
        } else {
            hasInput = 0;
        }

        if (hasInput) {
            EndCinematic();
        }

        VBuffer* vp = g_BackBuffer_0046aa14;
        vp->CallBlitter(vp->clip_y1, vp->clip_y2, vp->clip_x1, vp->clip_x2, vp->clip_y1, vp->clip_x2, (int)animation->targetBuffer);
        (g_Mouse_0046aa18)->DrawCursor();
        return;
    }

    // SmackWait linked directly via smackw32.lib

    while (1) {
        if ((g_InputManager_0046aa08)->Refresh(1) != 0) {
            EndCinematic();
            return;
        }

        if (!(flags & 0x1)) {
            InputState* mouse = g_InputManager_0046aa08->pMouse;
            int hasInput;
            if (mouse != 0) {
                if (mouse->ext2 >= 2) {
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
            if (g_WaitForInputValue_0046ac04 != 0) {
                int key = WaitForInput();
                hasInput = (key == 0x1b);
            }
            if (hasInput) {
                EndCinematic();
                return;
            }
        }

        if (SmackWait(animation->smk) == 0) {
            break;
        }
    }

    /* Frame ready - play it */
    if (animation->smk->NewPalette != 0) {
        animation->UpdatePalette(0, 0x100);
    }

    animation->DoFrame();

    {
        VBuffer* surface = animation->targetBuffer;
        int* screenH = GetScreenHeight();
        int h = *screenH - 1;
        int* screenW = GetScreenWidth();
        int w = *screenW - 1;

        surface->CallBlitter5(surface->clip_x1, surface->clip_x2, surface->clip_y1, surface->clip_y2, 0, w, 0, h);
    }

    if (flags & 0x100) {
        SetFontColor(0xfa);
        SetFontPosition(0x14, 0x14);
        int len = strlen(&g_CinematicDebugStr_00473400);
        DrawFontText(&g_CinematicDebugStr_00473400, len);
    }

    {
        int totalFrames = animation->smk->Frames;
        int currentFrame = animation->smk->FrameNum;
        if (totalFrames - currentFrame != 1) {
            animation->NextFrame();
            return;
        }
    }

    /* Movie ended - last frame */
    if (flags & 0x200) {
        waitForInput = 1;

        VBuffer* tb = animation->targetBuffer;
        SetVideoRes(tb->width, tb->height);

        g_GameEngine_0046a6ec->m_frameTime = savedRenderCtx;

        if (g_ZBufferManager_0046aa24 != 0) {
            int newState = 1;
            if (savedZBState != 0) {
                newState = savedZBState;
            }
            ZBufferManager* zbm = g_ZBufferManager_0046aa24;
            if (zbm->m_state != newState) {
                zbm->m_state = newState;

                ZBQueue* q1 = zbm->m_queueA0;
                if (q1->head != 0) {
                    q1->current = q1->head;
                    while (q1->head != 0) {
                        SoundCommand* obj = (SoundCommand*)q1->Pop();
                        if (obj != 0) {
                            delete obj;
                        }
                    }
                }

                ClearActionList((LinkedList*)zbm->m_queueA4);
                zbm->m_queue9c->ClearList();
                zbm->m_palette = 0;
            }
        }

        VBuffer* vp = g_BackBuffer_0046aa14;
        vp->CallBlitter(vp->clip_y1, vp->clip_y2, vp->clip_x1, vp->clip_x2, vp->clip_y1, vp->clip_x2, (int)animation->targetBuffer);
    } else {
        Timer timer;
        timer.Wait(0x96);
        EndCinematic();
    }
}

/* Function start: 0x4306A0 */
int SC_Cinematic::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    WriteMessageAddress(msg);

    if (action->lastKey == 0x1b || action->button2 > 1) {
        action->instruction = 0;
        EndCinematic();
    }

    return 1;
}

/* Function start: 0x4306E0 */
int SC_Cinematic::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    if (action->instruction != 7) {
        return 0;
    }
    SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    return 1;
}

/* Function start: 0x430730 */
void SC_Cinematic::EndCinematic() {
    if (pendingAction != 0) {
        EnqueueSpriteAction(pendingAction);
        if (pendingAction != 0) {
            delete pendingAction;
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
            DrawEntry* entry = (DrawEntry*)list->RemoveCurrent();
            if (entry != 0) {
                VBuffer* vb = entry->m_videoBuffer;
                if (vb != 0) {
                    delete vb;
                    entry->m_videoBuffer = 0;
                }
                RenderEntry* obj = (RenderEntry*)entry->m_childObject;
                if (obj != 0) {
                    delete obj;
                    entry->m_childObject = 0;
                }
                FreeMemory(entry);
            }
        } while (list->head != 0);
    }
}
