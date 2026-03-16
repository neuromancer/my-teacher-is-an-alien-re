#include "SCI_SchoolMenu.h"
#include "SpriteAction.h"
#include "LinkedList.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <new.h>
#include "GameState.h"
#include "globals.h"
#include "Sprite.h"
#include "Sample.h"
#include "Timer.h"
#include "Palette.h"
#include "RenderEntry.h"
#include "MMPlayer.h"
#include "main.h"

// extern globals - C linkage (matching stubs.cpp)
extern "C" {
    extern GameState* DAT_0046aa30;
    extern int DAT_0046a190;
    extern void* DAT_0046af08;
}
// extern globals - C++ linkage (matching stubs.cpp)
extern SpriteAction DAT_00472d20;
class ZBufferManager;
extern ZBufferManager* DAT_0046aa24;
extern SpriteAction DAT_00472d58;
extern int DAT_0046cb90;
extern char* DAT_0046cb94;
extern int DAT_00473334;
extern int DAT_00473358;
extern int DAT_004733e8;
extern int DAT_0047337c;

// extern functions (single-param __fastcall == thiscall with 0 stack params)
extern void __fastcall FUN_00420d90(void*);
// FUN_00421020 = ProcessSpriteActions - implemented below
void __fastcall ProcessSpriteActions(void* obj);
extern void __fastcall FUN_00421880(void*);
extern void __fastcall FUN_004218b0(void*);
// FUN_0041dbe0 = InitPalette (Palette.h)

// T_MenuHotspot is the 0x1A8-byte menu button widget
#include "T_MenuHotspot.h"

// extern "C" functions (matching stubs.cpp)
extern "C" {
    // FUN_00413e10 = ParseFile in Parser.h
    int FileExists(const char*);
    void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
    char* GetSoundFilename(int handle);
}
// C++ linkage functions (matching stubs.cpp)
extern void* __cdecl FUN_00444a40(void*, int, int, int, int, int, int, int, int, int, int);
// FUN_00413e70 = Parser::ProcessFile in Parser.cpp
extern "C" void WriteToLog(const char* format, ...);

extern void* __fastcall FUN_00403620(void*);
extern void __fastcall FUN_00401c80(void*);

/* Function start: 0x41E030 */
SCI_SchoolMenu::SCI_SchoolMenu() {
    memset(&palette, 0, 0x50);
    handlerId = 0x25;
    moduleParam = 1;
}

/* Function start: 0x41E0E0 */
SCI_SchoolMenu::~SCI_SchoolMenu() {
    void* ptr;
    int i;

    if (palette != 0) {
        palette->~Palette();
        free(palette);
        palette = 0;
    }

    if (background != 0) {
        background->~MMPlayer();
        free(background);
        background = 0;
    }

    ptr = okayButton;
    if (ptr != 0) {
        FUN_00420d90(ptr);
        free(ptr);
        okayButton = 0;
    }

    ptr = cancelButton;
    if (ptr != 0) {
        FUN_00420d90(ptr);
        free(ptr);
        cancelButton = 0;
    }

    if (menuSound != 0) {
        menuSound->Unload();
        free(menuSound);
        menuSound = 0;
    }

    for (i = 3; i != 0; i--) {
        ptr = characters[3 - i];
        if (ptr != 0) {
            FUN_00420d90(ptr);
            free(ptr);
            characters[3 - i] = 0;
        }
    }

    for (i = 9; i != 0; i--) {
        ptr = options[9 - i];
        if (ptr != 0) {
            FUN_00420d90(ptr);
            free(ptr);
            options[9 - i] = 0;
        }
    }
}

/* Function start: 0x41E270 */
void SCI_SchoolMenu::Init(SC_Message* msg) {
    int periodIdx;
    int periodVal;
    T_MenuHotspot** charSprite;
    void* ptr;
    GameState* gs;
    int i;

    if (DAT_0046a190 != 0) {
        return;
    }
    DAT_0046a190 = 1;

    IconBar::InitIconBar(msg);

    int* arr = &DAT_00473334;
    do {
        *arr = 1;
        arr = arr + 9;
    } while ((unsigned int)arr < (unsigned int)&DAT_004733e8);
    DAT_00473334 = 0;
    DAT_00473358 = 0;

    gs = DAT_0046aa30;
    periodIdx = gs->FindState("PERIOD");
    gs->FUN_00409f20(periodIdx);
    periodVal = gs->stateValues[periodIdx];
    if (periodVal == 1) {
        DAT_0047337c = 0;
    } else if (periodVal > 1) {
        DAT_0047337c = 1;
    }

    ParseFile(this, "mis\\skool.mis", "[DURINGSCHOOLMENU]");
    ParseFile(this, "mis\\skool.mis", "[ROOMAVALIBLITY_PERIOD%d]", periodVal);

    if (palette != 0) {
        ZBufferManager* zbm = DAT_0046aa24;
        if (zbm->m_palette != 0) {
            WriteToLog("ddouble palette");
        }
        zbm->m_palette = palette;
    }

    // Clean up existing sound
    ptr = menuSound;
    if (ptr != 0) {
        menuSound->Unload();
        free(ptr);
        menuSound = 0;
    }

    // Allocate new sound object
    void* sndMem = malloc(0x10);
    void* sndObj = 0;
    if (sndMem != 0) {
        new (sndMem) Sample();
        sndObj = sndMem;
    }
    menuSound = (Sample*)sndObj;
    selectedOption = -1;

    // Send initial message
    SendGameMessage(5, 0x455, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);

    // Reset character sprites
    i = 0;
    charSprite = characters;
    do {
        T_MenuHotspot* chr = *charSprite;
        if (chr != 0) {
            chr->field_94 = 0;
            Sprite* animCtrl = chr->cursor;
            if (animCtrl != 0) {
                ((Sprite*)animCtrl)->ResetAnimation(0, 0);
            }
        }
        charSprite++;
        i++;
    } while (i < 3);

    // Select current character
    gs = DAT_0046aa30;
    int charIdx = DAT_0046cb90;
    gs->FUN_00409f20(charIdx);
    int* stateVals = (int*)gs->stateValues;
    stateVals = (int*)*stateVals;
    int selChar = stateVals[charIdx];
    if (characters[selChar] != 0 && characters[i] != 0) {
        gs = DAT_0046aa30;
        charIdx = DAT_0046cb90;
        gs->FUN_00409f20(charIdx);
        stateVals = (int*)gs->stateValues;
        stateVals = (int*)*stateVals;
        T_MenuHotspot* chrPtr = characters[stateVals[charIdx]];
        chrPtr->field_94 = 1;
        Sprite* animCtrl = chrPtr->cursor;
        if (animCtrl != 0) {
            ((Sprite*)animCtrl)->ResetAnimation(1, 0);
        }
    }

    // Set cursor
    charIdx = DAT_0046cb90;
    gs = DAT_0046aa30;
    gs->FUN_00409f20(charIdx);
    stateVals = (int*)gs->stateValues;
    stateVals = (int*)*stateVals;
    ((Sprite*)DAT_0046af08)->ResetAnimation(stateVals[charIdx] + 1, 0);

    SetupOptions();

    // Check AWARE_TEACHER state
    gs = DAT_0046aa30;
    int awareIdx = gs->FindState("AWARE_TEACHER");
    gs->FUN_00409f20(awareIdx);
    stateVals = (int*)gs->stateValues;
    stateVals = (int*)*stateVals;
    if (stateVals[awareIdx] == 0) {
        // Check TEACHER_ALIEN state
        gs = DAT_0046aa30;
        int alienIdx = gs->FindState("TEACHER_ALIEN");
        gs->FUN_00409f20(alienIdx);
        stateVals = (int*)gs->stateValues;
        stateVals = (int*)*stateVals;
        if (stateVals[alienIdx] == 1) {
            // Check 2_NOTALIEN
            gs = DAT_0046aa30;
            int na2Idx = gs->FindState("2_NOTALIEN");
            gs->FUN_00409f20(na2Idx);
            stateVals = (int*)gs->stateValues;
            stateVals = (int*)*stateVals;
            if (stateVals[na2Idx] != 0) {
                // Check 3_NOTALIEN
                gs = DAT_0046aa30;
                int na3Idx = gs->FindState("3_NOTALIEN");
                gs->FUN_00409f20(na3Idx);
                stateVals = (int*)gs->stateValues;
                stateVals = (int*)*stateVals;
                if (stateVals[na3Idx] != 0) {
                    // Set AWARE_TEACHER = 1
                    gs = DAT_0046aa30;
                    awareIdx = gs->FindState("AWARE_TEACHER");
                    gs->FUN_00409f20(awareIdx);
                    stateVals = (int*)gs->stateValues;
                    stateVals = (int*)*stateVals;
                    stateVals[awareIdx] = 1;
                    SendGameMessage(4, 0x1e87, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                }
            }
        } else if (stateVals[alienIdx] == 2) {
            // Check 1_NOTALIEN
            gs = DAT_0046aa30;
            int na1Idx = gs->FindState("1_NOTALIEN");
            gs->FUN_00409f20(na1Idx);
            stateVals = (int*)gs->stateValues;
            stateVals = (int*)*stateVals;
            if (stateVals[na1Idx] != 0) {
                gs = DAT_0046aa30;
                int na3Idx = gs->FindState("3_NOTALIEN");
                gs->FUN_00409f20(na3Idx);
                stateVals = (int*)gs->stateValues;
                stateVals = (int*)*stateVals;
                if (stateVals[na3Idx] != 0) {
                    gs = DAT_0046aa30;
                    awareIdx = gs->FindState("AWARE_TEACHER");
                    gs->FUN_00409f20(awareIdx);
                    stateVals = (int*)gs->stateValues;
                    stateVals = (int*)*stateVals;
                    stateVals[awareIdx] = 2;
                    SendGameMessage(4, 0x1e89, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                }
            }
        } else if (stateVals[alienIdx] == 3) {
            // Check 1_NOTALIEN with bounds check
            gs = DAT_0046aa30;
            int na1Idx = gs->FindState("1_NOTALIEN");
            if (na1Idx < 0 || na1Idx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", na1Idx);
            }
            stateVals = (int*)gs->stateValues;
            stateVals = (int*)*stateVals;
            if (stateVals[na1Idx] != 0) {
                // Check 2_NOTALIEN with bounds check
                gs = DAT_0046aa30;
                int na2Idx = gs->FindState("2_NOTALIEN");
                if (na2Idx < 0 || na2Idx > gs->maxStates - 1) {
                    ShowError("Invalid gamestate %d", na2Idx);
                }
                stateVals = (int*)gs->stateValues;
                stateVals = (int*)*stateVals;
                if (stateVals[na2Idx] != 0) {
                    gs = DAT_0046aa30;
                    awareIdx = gs->FindState("AWARE_TEACHER");
                    gs->FUN_00409f20(awareIdx);
                    stateVals = (int*)gs->stateValues;
                    stateVals = (int*)*stateVals;
                    stateVals[awareIdx] = 3;
                    SendGameMessage(4, 0x1e88, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                }
            }
        }
    }

    // Send background sound message
    SendGameMessage(5, bgSoundId, 0, 0, 0x1b, 0, 0, 0, 0, 0);

    // Set PREVIOUSROOM state
    gs = DAT_0046aa30;
    int prevIdx = gs->FindState("PREVIOUSROOM");
    gs->FUN_00409f20(prevIdx);
    stateVals = (int*)gs->stateValues;
    stateVals = (int*)*stateVals;
    stateVals[prevIdx] = 0x62;

    // Check P_IN_DETENTION
    gs = DAT_0046aa30;
    int pDetIdx = gs->FindState("P_IN_DETENTION");
    if (pDetIdx < 0 || pDetIdx > gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", pDetIdx);
    }
    stateVals = (int*)gs->stateValues;
    stateVals = (int*)*stateVals;
    if (stateVals[pDetIdx] != 0) {
        // Check S_IN_DETENTION
        gs = DAT_0046aa30;
        int sDetIdx = gs->FindState("S_IN_DETENTION");
        if (sDetIdx < 0 || sDetIdx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", sDetIdx);
        }
        stateVals = (int*)gs->stateValues;
        stateVals = (int*)*stateVals;
        if (stateVals[sDetIdx] != 0) {
            // Check D_IN_DETENTION
            gs = DAT_0046aa30;
            int dDetIdx = gs->FindState("D_IN_DETENTION");
            if (dDetIdx < 0 || dDetIdx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", dDetIdx);
            }
            stateVals = (int*)gs->stateValues;
            stateVals = (int*)*stateVals;
            if (stateVals[dDetIdx] != 0) {
                // All 3 in detention - send detention message
                SendGameMessage(3, 0x1202, 0x2d, 1, 4, 0, 0, 0, 0, 0);
                // Create and push empty SpriteAction
                SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                DAT_00472d58.CopyFrom(&action);

                // Clear MUST_SAVEGAME
                gs = DAT_0046aa30;
                int saveIdx = gs->FindState("MUST_SAVEGAME");
                gs->FUN_00409f20(saveIdx);
                stateVals = (int*)gs->stateValues;
                stateVals = (int*)*stateVals;
                stateVals[saveIdx] = 0;
            }
        }
    }
}

/* Function start: 0x41E950 */
int SCI_SchoolMenu::ShutDown(SC_Message* msg) {
    void* ptr;
    T_MenuHotspot** base;
    int count;

    if (DAT_0046a190 == 0) {
        return 0;
    }
    DAT_0046a190 = 0;

    // Clean up ZBufferManager queues
    ZBufferManager* zbm = DAT_0046aa24;
    ZBQueue* queue;

    queue = zbm->m_queueA0;
    if (queue->head != 0) {
        queue->current = queue->head;
        while (queue->head != 0) {
            void* item = ((LinkedList*)queue)->RemoveCurrent();
            if (item != 0) {
                *(int*)item = 0x461030;
                free(item);
            }
        }
    }

    queue = zbm->m_queueA4;
    if (queue->head != 0) {
        queue->current = queue->head;
        while (queue->head != 0) {
            void* item = ((LinkedList*)queue)->RemoveCurrent();
            if (item != 0) {
                FUN_00401c80(item);
                free(item);
            }
        }
    }

    ZBQueue* queue9c = zbm->m_queue9c;
    if (queue9c->head != 0) {
        queue9c->current = queue9c->head;
        while (queue9c->head != 0) {
            RenderEntry* item = (RenderEntry*)FUN_00403620((void*)queue9c);
            if (item != 0) {
                item->RenderEntry::~RenderEntry();
                free(item);
            }
        }
    }

    zbm->m_palette = 0;

    // Clean up own fields
    ptr = palette;
    if (ptr != 0) {
        (palette)->~Palette();
        free(ptr);
        palette = 0;
    }

    ptr = background;
    if (ptr != 0) {
        ((MMPlayer*)ptr)->~MMPlayer();
        free(ptr);
        background = 0;
    }

    ptr = okayButton;
    if (ptr != 0) {
        FUN_00420d90(ptr);
        free(ptr);
        okayButton = 0;
    }

    ptr = cancelButton;
    if (ptr != 0) {
        FUN_00420d90(ptr);
        free(ptr);
        cancelButton = 0;
    }

    ptr = menuSound;
    if (ptr != 0) {
        menuSound->Unload();
        free(ptr);
        menuSound = 0;
    }

    base = characters;
    count = 3;
    do {
        ptr = (void*)*base;
        if (ptr != 0) {
            FUN_00420d90(ptr);
            free(ptr);
            *base = 0;
        }
        base++;
        count--;
    } while (count != 0);

    base = options;
    count = 9;
    do {
        ptr = (void*)*base;
        if (ptr != 0) {
            FUN_00420d90(ptr);
            free(ptr);
            *base = 0;
        }
        base++;
        count--;
    } while (count != 0);

    IconBar::CleanupIconBar(msg);
    return 0;
}

/* Function start: 0x41F610 */
int GlyphRect::HitTest(int x, int y) {
    if (left > x || right < x || top > y || bottom < y) {
        return 0;
    }
    return 1;
}

/* Function start: 0x41F640 */
int GameState::GetStateValue(int index) {
    if (index < 0 || index > maxStates - 1) {
        ShowError("Invalid gamestate %d", index);
    }
    return stateValues[index];
}

/* Function start: 0x41F6A0 */
void SCI_SchoolMenu::PlayMenuSound() {
    EnqueueSpriteAction((void*)&DAT_00472d20);
}

/* Function start: 0x41F6B0 */
int SCI_SchoolMenu::IsCharacterActive() {
    int i = 0;
    T_MenuHotspot** p = characters;
    do {
        T_MenuHotspot* chr = *p;
        if (chr != 0) {
            int state = chr->field_94;
            if (state == 1 || state == 2) {
                return 1;
            }
        }
        p++;
        i++;
    } while (i < 3);
    return 0;
}

/* Function start: 0x41F6E0 */
void SCI_SchoolMenu::SetupOptions() {
    GameState* gs;
    int* stateVals;
    int periodIdx;
    int periodVal;
    int charIdx;
    int charVal;
    int i;
    int stIdx;
    char buf[8];

    gs = DAT_0046aa30;
    periodIdx = gs->FindState("PERIOD");
    if (periodIdx < 0 || periodIdx > gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", periodIdx);
    }
    stateVals = gs->stateValues;
    periodVal = stateVals[periodIdx];

    if (periodVal == 5 || periodVal == 10 || periodVal == 15 || periodVal == 20) {
        // Free period - hide all options
        T_MenuHotspot** p = options;
        int count = 9;
        do {
            T_MenuHotspot* opt = *p;
            if (opt != 0) {
                opt->field_94 = 0;
                Sprite* animCtrl = opt->cursor;
                if (animCtrl != 0) {
                    ((Sprite*)animCtrl)->ResetAnimation(0, 0);
                }
            }
            p++;
            count--;
        } while (count != 0);
        return;
    }

    i = 0;
    T_MenuHotspot** optPtr = options;
    do {
        charIdx = DAT_0046cb90;
        gs = DAT_0046aa30;
        if (charIdx < 0 || charIdx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", charIdx);
        }
        stateVals = gs->stateValues;
        charVal = stateVals[charIdx];

        char ch;
        if (charVal == -1) {
            // Use default character
            charIdx = DAT_0046cb90;
            gs = DAT_0046aa30;
            if (charIdx < 0 || charIdx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", charIdx);
            }
            stateVals = gs->stateValues;
            ch = DAT_0046cb94[stateVals[charIdx]];
        } else {
            ch = DAT_0046cb94[charVal];
        }

        i++;
        sprintf(buf, "%cRM%d", (int)ch, i);

        if (*optPtr == 0) {
            goto next_option;
        }

        gs = DAT_0046aa30;
        stIdx = gs->FindState(buf);
        if (stIdx < 0 || stIdx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", stIdx);
        }

        int stVal;
        T_MenuHotspot* opt;
        opt = *optPtr;
        stateVals = gs->stateValues;
        stVal = stateVals[stIdx];

        gs = DAT_0046aa30;
        stIdx = gs->FindState(buf);
        if (stIdx < 0 || stIdx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", stIdx);
        }
        stateVals = gs->stateValues;

        if (stateVals[stIdx] == 0) {
            opt->field_94 = -1;
            Sprite* animCtrl = opt->cursor;
            if (animCtrl != 0) {
                ((Sprite*)animCtrl)->ResetAnimation(-1, 0);
            }
        } else {
            opt->field_94 = 0;
            Sprite* animCtrl = opt->cursor;
            if (animCtrl != 0) {
                ((Sprite*)animCtrl)->ResetAnimation(0, 0);
            }
        }

next_option:
        optPtr++;
    } while (i < 9);
}

/* Function start: 0x41F900 */
void InitAllSchedule() {
    int charIdx;
    int i;
    char buf[16];

    for (charIdx = 0; charIdx < 3; charIdx++) {
        for (i = 1; i <= 0x12; i++) {
            char ch;
            if (charIdx == -1) {
                GameState* gs = DAT_0046aa30;
                int periodIdx = DAT_0046cb90;
                if (periodIdx < 0 || periodIdx > gs->maxStates - 1) {
                    ShowError("Invalid gamestate %d", periodIdx);
                }
                ch = DAT_0046cb94[gs->stateValues[periodIdx]];
            } else {
                ch = DAT_0046cb94[charIdx];
            }

            sprintf(buf, "%cRM%d", (int)ch, i);

            GameState* gs = DAT_0046aa30;
            int stIdx = gs->FindState(buf);
            if (stIdx < 0 || stIdx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", stIdx);
            }
            gs->stateValues[stIdx] = 1;
        }
    }
}

/* Function start: 0x41ECE0 */
int SCI_SchoolMenu::AddMessage(SC_Message* msg) {
    SpriteAction* action;
    GameState* gs;
    int* stateVals;
    int i;
    int hitResult;
    T_MenuHotspot** charBase;
    T_MenuHotspot** charPtr;
    int* msgMouse;
    int hitChar;
    char buf[28];
    T_MenuHotspot** optBase;
    T_MenuHotspot** optPtr;
    int hitOpt;
    T_MenuHotspot** selPtr;
    char capBuf[28];
    char nextChar;

    action = (SpriteAction*)msg;
    if (IconBar::CheckButtonClick(msg) != 0) goto done;
    if (action->button1 < 2) goto done;

    // Check NUM_ACTIONS state
    gs = DAT_0046aa30;
    i = gs->FindState("NUM_ACTIONS");
    gs->FUN_00409f20(i);
    stateVals = gs->stateValues;
    if (stateVals[i] >= 1) goto check_options;

    charBase = characters;
    charPtr = characters;
    i = 0;
    msgMouse = &action->mousePos.field_0;

char_loop:
    {
        SlimeDim mouseCoords = *(SlimeDim*)msgMouse;
        T_MenuHotspot* chr = *charPtr;
        if (chr->sprite != 0) {
            if (chr->bounds.left > mouseCoords.field_0 ||
                chr->bounds.right < mouseCoords.field_0 ||
                chr->bounds.top > mouseCoords.field_4 ||
                chr->bounds.bottom < mouseCoords.field_4) {
                hitChar = 0;
            } else {
                hitChar = 1;
            }
        } else {
            hitChar = 0;
        }
    }
    if (hitChar != 0) goto char_click;
    charPtr++;
    i++;
    if (i < 3) goto char_loop;

check_options:
    if (IsCharacterActive() == 0) goto check_go;

    i = 0;
    optBase = options;
    optPtr = options;

opt_loop:
    {
        T_MenuHotspot* opt = *optPtr;
        if (opt == 0) goto next_opt;
        {
            SlimeDim optCoords = action->mousePos;
            if (opt->sprite == 0) goto opt_miss;
            if (opt->bounds.HitTest(optCoords.field_0, optCoords.field_4)) {
                hitOpt = 1;
                goto opt_done;
            }
opt_miss:
            hitOpt = 0;
opt_done:
            ;
        }
        if (hitOpt != 0) goto opt_click;
    }
    goto next_opt;

char_click:
    // Character was clicked
    {
        char ch;
        if (i == -1) {
            gs = DAT_0046aa30;
            int charIdx = DAT_0046cb90;
            gs->FUN_00409f20(charIdx);
            stateVals = gs->stateValues;
            ch = DAT_0046cb94[stateVals[charIdx]];
        } else {
            ch = DAT_0046cb94[i];
        }

        sprintf(buf, "%c_IN_DETENTION", (int)ch);
        gs = DAT_0046aa30;
        hitResult = gs->FindState(buf);
        gs->FUN_00409f20(hitResult);
        stateVals = gs->stateValues;
        if (stateVals[hitResult] != 0) goto check_options;

        if (i == -1) {
            gs = DAT_0046aa30;
            int charIdx = DAT_0046cb90;
            gs->FUN_00409f20(charIdx);
            stateVals = gs->stateValues;
            ch = DAT_0046cb94[stateVals[charIdx]];
        } else {
            ch = DAT_0046cb94[i];
        }
        sprintf(buf, "%c_DETENTION_SUSPENSION", (int)ch);
        gs = DAT_0046aa30;
        hitResult = gs->FindState(buf);
        gs->FUN_00409f20(hitResult);
        stateVals = gs->stateValues;
        if (stateVals[hitResult] > 2) goto check_options;

        if (i == -1) {
            gs = DAT_0046aa30;
            int charIdx = DAT_0046cb90;
            gs->FUN_00409f20(charIdx);
            stateVals = gs->stateValues;
            ch = DAT_0046cb94[stateVals[charIdx]];
        } else {
            ch = DAT_0046cb94[i];
        }
        sprintf(buf, "%c_CAPTURED", (int)ch);
        gs = DAT_0046aa30;
        hitResult = gs->FindState(buf);
        gs->FUN_00409f20(hitResult);
        stateVals = gs->stateValues;
        if (stateVals[hitResult] != 0) goto check_options;

        // Select this character - reset all
        {
            T_MenuHotspot** basePtr = charBase;
            int count = 3;
            do {
                T_MenuHotspot* chr = *basePtr;
                chr->field_94 = 0;
                Sprite* animCtrl = chr->cursor;
                if (animCtrl != 0) {
                    ((Sprite*)animCtrl)->ResetAnimation(0, 0);
                }
                basePtr++;
                count--;
            } while (count != 0);
        }

        selPtr = &characters[i];
        {
            T_MenuHotspot* selChr = *selPtr;
            selChr->field_94 = 1;
            Sprite* animCtrl = selChr->cursor;
            if (animCtrl != 0) {
                ((Sprite*)animCtrl)->ResetAnimation(1, 0);
            }
        }

        // Update global character selection
        gs = DAT_0046aa30;
        hitResult = DAT_0046cb90;
        gs->FUN_00409f20(hitResult);
        stateVals = gs->stateValues;
        stateVals[hitResult] = i;

        hitResult = DAT_0046cb90;
        gs = DAT_0046aa30;
        gs->FUN_00409f20(hitResult);
        stateVals = gs->stateValues;
        ((Sprite*)DAT_0046af08)->ResetAnimation(stateVals[hitResult] + 1, 0);

        SetupOptions();
        selectedOption = -1;

        // Play sound
        menuSound->~Sample();
        {
            Timer timedEvt;
            timedEvt.Wait(0x96);
        }

        // Load character-specific sound
        {
            T_MenuHotspot* selSpr = *selPtr;
            gs = DAT_0046aa30;
            int periodIdx2 = DAT_0046cb90;
            int pIdx = gs->FindState("PERIOD");
            gs->FUN_00409f20(pIdx);
            stateVals = gs->stateValues;
            int period = stateVals[pIdx];
            int offset = (DAT_0046aa30)->GetStateValue(periodIdx2);
            int fullIdx = period * 3 + offset;
            char* sndFile = GetSoundFilename(((int*)selSpr)[0x28 + fullIdx]);
            menuSound->Load(sndFile);
            menuSound->Play(100, 1);
        }

    }
    goto check_options;

next_opt:
    optPtr++;
    i++;
    if (i < 9) goto opt_loop;
    goto check_go;

opt_click:
    // Option was clicked - reset all
    {
        T_MenuHotspot** baseOpts = optBase;
        int count = 9;
        do {
            T_MenuHotspot* opt = *baseOpts;
            if (opt != 0) {
                opt->field_94 = 0;
                Sprite* animCtrl = opt->cursor;
                if (animCtrl != 0) {
                    ((Sprite*)animCtrl)->ResetAnimation(0, 0);
                }
            }
            baseOpts++;
            count--;
        } while (count != 0);
    }

    selPtr = &options[i];
    {
        T_MenuHotspot* selOpt = *selPtr;
        selOpt->field_94 = 1;
        Sprite* animCtrl = selOpt->cursor;
        if (animCtrl != 0) {
            ((Sprite*)animCtrl)->ResetAnimation(1, 0);
        }
    }

    // Load option sound
    {
        T_MenuHotspot* selOpt2 = *selPtr;
        gs = DAT_0046aa30;
        int charStateIdx = DAT_0046cb90;
        int pIdx = gs->FindState("PERIOD");
        gs->FUN_00409f20(pIdx);
        stateVals = gs->stateValues;
        int period = stateVals[pIdx];
        int offset = (DAT_0046aa30)->GetStateValue(charStateIdx);
        int fullIdx = period * 3 + offset;
        char* sndFile = GetSoundFilename(((int*)selOpt2)[0x28 + fullIdx]);
        menuSound->Load(sndFile);
        menuSound->Play(100, 1);
    }
    selectedOption = i;
    goto check_go;

check_go:
    if (selectedOption <= -1) goto check_back;
    if (IsCharacterActive() == 0) goto check_back;

    // Check go button hit
    {
        SlimeDim goCoords;
        int* goMouse = &action->mousePos.field_0;
        goCoords.field_0 = goMouse[0];
        goCoords.field_4 = goMouse[1];
        T_MenuHotspot* goSpr = okayButton;
        if (goSpr->sprite != 0) {
            if (goSpr->bounds.HitTest(goCoords.field_0, goCoords.field_4)) {
                hitResult = 1;
            } else {
                hitResult = 0;
            }
        } else {
            hitResult = 0;
        }
    }
    if (hitResult != 0) {
        // Go button clicked - increment NUM_ACTIONS
        gs = DAT_0046aa30;
        i = gs->FindState("NUM_ACTIONS");
        gs->FUN_00409f20(i);
        stateVals = gs->stateValues;
        stateVals[i]++;

        // Execute option action
        ProcessSpriteActions((void*)options[selectedOption]);

        // Determine next character
        gs = DAT_0046aa30;
        i = gs->FindState("PERIOD");
        nextChar = 0;
        gs->FUN_00409f20(i);
        stateVals = gs->stateValues;
        int period = stateVals[i];

        if (period == 0x12) {
            gs = DAT_0046aa30;
            int charIdx = DAT_0046cb90;
            gs->FUN_00409f20(charIdx);
            stateVals = gs->stateValues;
            if (DAT_0046cb94[stateVals[charIdx]] == 'D') {
                nextChar = 'P';
            } else {
                gs = DAT_0046aa30;
                charIdx = DAT_0046cb90;
                gs->FUN_00409f20(charIdx);
                stateVals = gs->stateValues;
                if (DAT_0046cb94[stateVals[charIdx]] == 'S') {
                    nextChar = 'D';
                } else {
                    gs = DAT_0046aa30;
                    charIdx = DAT_0046cb90;
                    gs->FUN_00409f20(charIdx);
                    stateVals = gs->stateValues;
                    if (DAT_0046cb94[stateVals[charIdx]] == 'P') {
                        nextChar = 'S';
                    }
                }
            }
        } else if (period == 0x13) {
            gs = DAT_0046aa30;
            i = gs->FindState("D_CAPTURED");
            gs->FUN_00409f20(i);
            stateVals = gs->stateValues;
            if (stateVals[i] == 0) {
                gs = DAT_0046aa30;
                int charIdx = DAT_0046cb90;
                gs->FUN_00409f20(charIdx);
                stateVals = gs->stateValues;
                if (DAT_0046cb94[stateVals[charIdx]] != 'D') {
                    nextChar = 'D';
                    goto set_captured;
                }
            }

            gs = DAT_0046aa30;
            i = gs->FindState("S_CAPTURED");
            gs->FUN_00409f20(i);
            stateVals = gs->stateValues;
            if (stateVals[i] == 0) {
                gs = DAT_0046aa30;
                int charIdx = DAT_0046cb90;
                gs->FUN_00409f20(charIdx);
                stateVals = gs->stateValues;
                if (DAT_0046cb94[stateVals[charIdx]] != 'S') {
                    nextChar = 'S';
                    goto set_captured;
                }
            }

            gs = DAT_0046aa30;
            i = gs->FindState("P_CAPTURED");
            gs->FUN_00409f20(i);
            stateVals = gs->stateValues;
            if (stateVals[i] == 0) {
                gs = DAT_0046aa30;
                int charIdx = DAT_0046cb90;
                gs->FUN_00409f20(charIdx);
                stateVals = gs->stateValues;
                if (DAT_0046cb94[stateVals[charIdx]] != 'P') {
                    nextChar = 'P';
                }
            }
        }

set_captured:
        if (nextChar != 0) {
            sprintf(capBuf, "%c_CAPTURED", (int)nextChar);
            gs = DAT_0046aa30;
            i = gs->FindState(capBuf);
            gs->FUN_00409f20(i);
            stateVals = gs->stateValues;
            stateVals[i] = 1;

            sprintf(capBuf, "%c_IN_DETENTION", (int)nextChar);
            gs = DAT_0046aa30;
            i = gs->FindState(capBuf);
            gs->FUN_00409f20(i);
            stateVals = gs->stateValues;
            stateVals[i] = 1;
        }
    }

check_back:
    if (cancelButton == 0) goto done;
    {
        SlimeDim backCoords;
        backCoords.field_0 = ((int*)msg)[7];
        backCoords.field_4 = ((int*)msg)[8];
        T_MenuHotspot* backSpr = cancelButton;
        if (backSpr->sprite != 0) {
            if (backSpr->bounds.HitTest(backCoords.field_0, backCoords.field_4)) {
                hitResult = 1;
            } else {
                hitResult = 0;
            }
        } else {
            hitResult = 0;
        }
    }
    if (hitResult != 0) {
        PlayMenuSound();
    }

done:
    return 1;
}

/* Function start: 0x41F9D0 */
int SCI_SchoolMenu::LBLParse(char* line) {
    char label[32];
    char buf1[32];
    char buf2[32];
    char buf3[32];
    char buf4[32];
    char buf5[32];
    char buf6[28];
    int period;
    int val1;
    int val2;
    int val3;
    int count;
    GameState* gs;
    int* stateVals;
    int idx;

    sscanf(line, "%s", label);

    if (strcmp(label, "BACK") == 0) {
        void* mem = malloc(0xa0);
        void* obj = 0;
        if (mem != 0) {
            new (mem) MMPlayer();
            obj = mem;
        }
        background = (MMPlayer*)obj;
        Parser::ProcessFile((Parser*)obj, this, 0);
        return 0;
    }

    if (strcmp(label, "AVAILABILITY") == 0) {
        count = sscanf(line, "%s %d %s %s %s", label, &period, buf1, buf2, buf3);
        if (count < 3 || period > 0x14 || period < 1) {
            ReportUnknownLabel("SCI_SchoolMenu");
        }
        gs = DAT_0046aa30;
        idx = gs->FindState("PERIOD");
        if (idx < 0 || idx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateVals = gs->stateValues;
        if (stateVals[idx] == period) {
            count = count - 2;
            if (count > 0) {
                char* p = buf1;
                do {
                    gs = DAT_0046aa30;
                    idx = gs->FindState(p);
                    if (idx < 0 || idx > gs->maxStates - 1) {
                        ShowError("Invalid gamestate %d", idx);
                    }
                    stateVals = gs->stateValues;
                    p = p + 0x20;
                    count--;
                    stateVals[idx] = 0;
                } while (count != 0);
            }
        }
        return 0;
    }

    if (strcmp(label, "PALE") == 0) {
        char filename[64];
        sscanf(line, "%s %s", label, filename);
        void* mem = malloc(8);
        void* obj = 0;
        if (mem != 0) {
            InitPalette((Palette*)mem);
            obj = mem;
        }
        palette = (Palette*)obj;
        palette->Load(filename);
        return 0;
    }

    if (strcmp(label, "BGSND") == 0) {
        sscanf(line, "%s %d", label, &bgSoundId);
        return 0;
    }

    if (strcmp(label, "CHAR") == 0) {
        sscanf(line, "%s %d", label, &period);
        void* mem = malloc(0x1a8);
        void* obj = 0;
        if (mem != 0) {
            new (mem) T_MenuHotspot(period);
            obj = mem;
        }
        characters[period] = (T_MenuHotspot*)obj;
        Parser::ProcessFile((Parser*)obj, this, 0);
        return 0;
    }

    if (strstr(label, "OPTION") != 0) {
        sscanf(line, "%s %d", label, &period);
        void* mem = malloc(0x1a8);
        void* obj = 0;
        if (mem != 0) {
            new (mem) T_MenuHotspot(period);
            obj = mem;
        }
        options[period] = (T_MenuHotspot*)obj;
        Parser::ProcessFile((Parser*)obj, this, 0);
        return 0;
    }

    if (strcmp(label, "OKAY") == 0) {
        void* mem = malloc(0x1a8);
        void* obj = 0;
        if (mem != 0) {
            new (mem) T_MenuHotspot(0);
            obj = mem;
        }
        okayButton = (T_MenuHotspot*)obj;
        Parser::ProcessFile((Parser*)obj, this, 0);
        return 0;
    }

    if (strcmp(label, "CANCEL") == 0) {
        void* mem = malloc(0x1a8);
        void* obj = 0;
        if (mem != 0) {
            new (mem) T_MenuHotspot(0);
            obj = mem;
        }
        cancelButton = (T_MenuHotspot*)obj;
        Parser::ProcessFile((Parser*)obj, this, 0);

        // Check if NUM_ACTIONS is 0 - if so, destroy back button
        gs = DAT_0046aa30;
        idx = gs->FindState("NUM_ACTIONS");
        if (idx < 0 || idx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateVals = gs->stateValues;
        if (stateVals[idx] == 0 && cancelButton != 0) {
            FUN_00420d90(cancelButton);
            free(cancelButton);
            cancelButton = 0;
        }
        return 0;
    }

    if (strstr(label, "CLASS") != 0) {
        count = sscanf(line, "%s %d %s %d %s %d %s %d", label, &period, buf4, &val1, buf5, &val2, buf6, &val3);
        if (count < 4 || period > 0x14 || period < 1) {
            ReportUnknownLabel("SCI_SchoolMenu");
        }
        gs = DAT_0046aa30;
        idx = gs->FindState("PERIOD");
        if (idx < 0 || idx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateVals = gs->stateValues;
        if (stateVals[idx] == period) {
            gs = DAT_0046aa30;
            idx = gs->FindState(buf4);
            if (idx < 0 || idx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", idx);
            }
            stateVals = gs->stateValues;
            stateVals[idx] = val1;

            gs = DAT_0046aa30;
            idx = gs->FindState(buf5);
            if (idx < 0 || idx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", idx);
            }
            stateVals = gs->stateValues;
            stateVals[idx] = val2;

            gs = DAT_0046aa30;
            idx = gs->FindState(buf6);
            if (idx < 0 || idx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", idx);
            }
            stateVals = gs->stateValues;
            stateVals[idx] = val3;
        }
        return 0;
    }

    if (strcmp(label, "END") == 0) {
        return 1;
    }

    ReportUnknownLabel("SCI_SchoolMenu");
    return 0;
}


/* Function start: 0x421020 */
void __fastcall ProcessSpriteActions(void* obj)
{
    int param_1 = (int)obj;
    int* list = *(int**)(param_1 + 0x1a0);
    if (list != 0) {
        list[2] = *list;
        if (**(int**)(param_1 + 0x1a0) == 0) return;
        do {
            int cur = *(int*)(*(int*)(param_1 + 0x1a0) + 8);
            void* data;
            if (cur == 0) {
                data = 0;
            } else {
                data = *(void**)(cur + 8);
            }
            EnqueueSpriteAction(data);
            int listPtr = *(int*)(param_1 + 0x1a0);
            int curNode = *(int*)(listPtr + 8);
            if (*(int*)(listPtr + 4) == curNode) {
                return;
            }
            if (curNode != 0) {
                *(int*)(listPtr + 8) = *(int*)(curNode + 4);
            }
        } while (**(int**)(param_1 + 0x1a0) != 0);
    }
}
