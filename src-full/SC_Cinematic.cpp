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

extern "C" extern GameState* g_GameState_0046aa30;
#define g_GameState_0046aa30 (g_GameState_0046aa30)
#include "ZBufferManager.h"
extern ZBufferManager* g_ZBufferManager_0046aa24;
extern InputManager* g_InputManager_0046aa08;
class MouseControl;
#include "GameEngine.h"
extern MouseControl* g_Mouse_0046aa18;

extern "C" {
    extern int g_GameEngine_0046a6ec;
    extern VBuffer* DAT_0046aa14;
    extern char g_CinematicDebugStr_00473400;
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
    SpriteAction* action = (SpriteAction*)msg;

    CopyCommandData(msg);

    memset(&palette, 0, 12 * 4);
    flags = 0x10;
    savedRenderCtx = ((GameEngine*)g_GameEngine_0046a6ec)->m_frameTime;

    ZBufferManager* zbm = g_ZBufferManager_0046aa24;
    if (zbm != 0) {
        int savedState = zbm->m_state;
        if (savedState != 0) {
            zbm->m_state = 0;

            ZBQueue* q1 = zbm->m_queueA0;
            if (q1->head != 0) {
                q1->current = q1->head;
                while (q1->head != 0) {
                    void* obj = ((LinkedList*)q1)->RemoveCurrent();
                    if (obj != 0) {
                        FreeMemory(obj);
                    }
                }
            }

            ZBQueue* q2 = zbm->m_queueA4;
            if (q2->head != 0) {
                q2->current = q2->head;
                while (q2->head != 0) {
                    void* obj = ((LinkedList*)q2)->RemoveCurrent();
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
                    void* obj = ((LinkedList*)q3)->RemoveCurrent();
                    if (obj != 0) {
                        ((Rect*)((char*)obj + 4))->~Rect();
                        FreeMemory(obj);
                    }
                }
            }

            zbm->m_palette = 0;
        }
        savedZBState = savedState;
    }

    moduleParam = action->addressValue;
    flags |= action->extra1;

    if ((int)action->childAction != 0) {
        pendingAction = (int)action->childAction;
        action->childAction = 0;
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
                strcpy(&g_CinematicDebugStr_00473400, moviePath);
            }
        }

        animation = (int)new Animation(moviePath);

        ((GameEngine*)g_GameEngine_0046a6ec)->m_frameTime = 0;

        startX = action->button1;
        startY = action->button2;

        if (action->mousePos.field_0 != 0) {
            volume = action->mousePos.field_0;
        } else {
            volume = 100;
        }

        soundParam = action->mousePos.field_4;

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
        if (g_ZBufferManager_0046aa24 != 0) {
            ZBufferManager* zbm = g_ZBufferManager_0046aa24;
            if (zbm->m_state != savedZBState) {
                zbm->m_state = savedZBState;

                ZBQueue* q1 = zbm->m_queueA0;
                if (q1->head != 0) {
                    q1->current = q1->head;
                    while (q1->head != 0) {
                        void* obj = ((LinkedList*)q1)->RemoveCurrent();
                        if (obj != 0) {
                            FreeMemory(obj);
                        }
                    }
                }

                ZBQueue* q2 = zbm->m_queueA4;
                if (q2->head != 0) {
                    q2->current = q2->head;
                    while (q2->head != 0) {
                        void* obj = ((LinkedList*)q2)->RemoveCurrent();
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
                        void* obj = ((LinkedList*)q3)->RemoveCurrent();
                        if (obj != 0) {
                            ((Rect*)((char*)obj + 4))->~Rect();
                            FreeMemory(obj);
                        }
                    }
                }

                zbm->m_palette = 0;
            }
        }

        ((GameEngine*)g_GameEngine_0046a6ec)->m_frameTime = savedRenderCtx;

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
        (g_InputManager_0046aa08)->Refresh(1);

        InputState* mouse = g_InputManager_0046aa08->pMouse;
        int hasInput;
        if (mouse != 0 && (mouse->ext1 >= 1 || mouse->ext2 >= 1 || g_WaitForInputValue_004373bc != 0)) {
            hasInput = 1;
        } else {
            hasInput = 0;
        }

        if (hasInput) {
            EndCinematic();
        }

        Animation* smk = (Animation*)animation;
        VBuffer* vp = DAT_0046aa14;
        vp->CallBlitter(vp->clip_x1, vp->clip_x2, vp->clip_y1, vp->clip_y2, vp->clip_x1, vp->clip_y2, (int)smk->targetBuffer);
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
        Animation* anim = (Animation*)animation;
        if (anim->smk->NewPalette != 0) {
            anim->UpdatePalette(0, 0x100);
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
        int len = strlen(&g_CinematicDebugStr_00473400);
        DrawFontText(&g_CinematicDebugStr_00473400, len);
    }

    {
        Animation* anim2 = (Animation*)animation;
        int totalFrames = anim2->smk->Frames;
        int currentFrame = anim2->smk->FrameNum;
        if (totalFrames - currentFrame != 1) {
            anim2->NextFrame();
            return;
        }
    }

    /* Movie ended - last frame */
    if (flags & 0x200) {
        waitForInput = 1;

        Animation* anim3 = (Animation*)animation;
        VBuffer* tb = anim3->targetBuffer;
        SetVideoRes(tb->width, tb->height);

        ((GameEngine*)g_GameEngine_0046a6ec)->m_frameTime = savedRenderCtx;

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
                        void* obj = ((LinkedList*)q1)->RemoveCurrent();
                        if (obj != 0) {
                            delete (SoundCommand*)obj;
                        }
                    }
                }

                ClearActionList((LinkedList*)zbm->m_queueA4);
                zbm->m_queue9c->ClearList();
                zbm->m_palette = 0;
            }
        }

        Animation* anim4 = (Animation*)animation;
        VBuffer* vp = DAT_0046aa14;
        vp->CallBlitter(vp->clip_x1, vp->clip_x2, vp->clip_y1, vp->clip_y2, vp->clip_x1, vp->clip_y2, (int)anim4->targetBuffer);
    } else {
        Timer timer;
        timer.Wait(0x96);
        EndCinematic();
    }
}

/* Function start: 0x4306A0 */
int SC_Cinematic::AddMessage(SC_Message* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    WriteMessageAddress(msg);

    if (action->lastKey == 0x1b || action->button2 > 1) {
        action->instruction = 0;
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
