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
#include "ZBuffer.h"
#include "SoundCommand.h"
#include "MouseControl.h"
#include "DrawEntry.h"
#include "VBuffer.h"

// extern functions (single-param __fastcall == thiscall with 0 stack params)

// FUN_00421020 = T_MenuButton::ProcessSpriteActions - implemented in T_MenuHotspot.cpp
// FUN_00421880 = Timer::Timer — implemented in Timer.cpp
// FUN_004218b0 = Timer::~Timer — implemented in Timer.cpp
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
// FUN_00413e70 = Parser::ProcessFile in Parser.cpp
extern "C" void WriteToLog(const char* format, ...);



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
        delete palette;
        palette = 0;
    }

    if (background != 0) {
        delete background;
        background = 0;
    }

    ptr = okayButton;
    if (ptr != 0) {
        delete (T_MenuHotspot*)ptr;
        okayButton = 0;
    }

    ptr = cancelButton;
    if (ptr != 0) {
        delete (T_MenuHotspot*)ptr;
        cancelButton = 0;
    }

    if (menuSound != 0) {
        menuSound->Unload();
        delete menuSound;
        menuSound = 0;
    }

    for (i = 3; i != 0; i--) {
        ptr = characters[3 - i];
        if (ptr != 0) {
            delete (T_MenuHotspot*)ptr;
            characters[3 - i] = 0;
        }
    }

    for (i = 9; i != 0; i--) {
        ptr = options[9 - i];
        if (ptr != 0) {
            delete (T_MenuHotspot*)ptr;
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

    if (g_SchoolMenuActive_0046a190 != 0) {
        return;
    }
    g_SchoolMenuActive_0046a190 = 1;

    IconBar::InitIconBar(msg);

    int* arr = &g_IconBarState_00473334;
    do {
        *arr = 1;
        arr = arr + 9;
    } while ((unsigned int)arr < (unsigned int)&g_InventoryState_004733e8);
    g_IconBarState_00473334 = 0;
    g_SchoolMenuField1_00473358 = 0;

    gs = g_GameState_0046aa30;
    periodIdx = gs->FindState("PERIOD");
    gs->ValidateIndex(periodIdx);
    periodVal = gs->stateValues[periodIdx];
    if (periodVal == 1) {
        g_SchoolMenuField2_0047337c = 0;
    } else if (periodVal > 1) {
        g_SchoolMenuField2_0047337c = 1;
    }

    ParseFile(this, "mis\\skool.mis", "[DURINGSCHOOLMENU]");
    ParseFile(this, "mis\\skool.mis", "[ROOMAVALIBLITY_PERIOD%d]", periodVal);

    if (palette != 0) {
        ZBufferManager* zbm = g_ZBufferManager_0046aa24;
        if (zbm->m_palette != 0) {
            WriteToLog("ddouble palette");
        }
        zbm->m_palette = palette;
    }

    // Clean up existing sound
    ptr = menuSound;
    if (ptr != 0) {
        menuSound->Unload();
        delete menuSound;
        menuSound = 0;
    }

    // Allocate new sound object
    menuSound = new Sample();
    selectedOption = -1;

    // Send initial message
    SendGameMessage(5, 0x455, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);

    // Reset character sprites
    i = 0;
    charSprite = characters;
    do {
        T_MenuHotspot* chr = *charSprite;
        if (chr != 0) {
            chr->bounds.left = 0;
            Sprite* animCtrl = chr->cursor;
            if (animCtrl != 0) {
                animCtrl->ResetAnimation(0, 0);
            }
        }
        charSprite++;
        i++;
    } while (i < 3);

    // Select current character
    gs = g_GameState_0046aa30;
    int charIdx = g_PeriodStateIdx_0046cb90;
    gs->ValidateIndex(charIdx);
    int* stateVals = gs->stateValues;
    int selChar = stateVals[charIdx];
    if (characters[selChar] != 0 && characters[i] != 0) {
        gs = g_GameState_0046aa30;
        charIdx = g_PeriodStateIdx_0046cb90;
        gs->ValidateIndex(charIdx);
        stateVals = gs->stateValues;
        T_MenuHotspot* chrPtr = characters[stateVals[charIdx]];
        chrPtr->bounds.left = 1;
        Sprite* animCtrl = chrPtr->cursor;
        if (animCtrl != 0) {
            animCtrl->ResetAnimation(1, 0);
        }
    }

    // Set cursor
    charIdx = g_PeriodStateIdx_0046cb90;
    gs = g_GameState_0046aa30;
    gs->ValidateIndex(charIdx);
    stateVals = gs->stateValues;
    g_SchoolMenuSprite_0046af08->ResetAnimation(stateVals[charIdx] + 1, 0);

    SetupOptions();

    // Check AWARE_TEACHER state
    gs = g_GameState_0046aa30;
    int awareIdx = gs->FindState("AWARE_TEACHER");
    gs->ValidateIndex(awareIdx);
    stateVals = gs->stateValues;
    if (stateVals[awareIdx] == 0) {
        // Check TEACHER_ALIEN state
        gs = g_GameState_0046aa30;
        int alienIdx = gs->FindState("TEACHER_ALIEN");
        gs->ValidateIndex(alienIdx);
        stateVals = gs->stateValues;
        if (stateVals[alienIdx] == 1) {
            // Check 2_NOTALIEN
            gs = g_GameState_0046aa30;
            int na2Idx = gs->FindState("2_NOTALIEN");
            gs->ValidateIndex(na2Idx);
            stateVals = gs->stateValues;
            if (stateVals[na2Idx] != 0) {
                // Check 3_NOTALIEN
                gs = g_GameState_0046aa30;
                int na3Idx = gs->FindState("3_NOTALIEN");
                gs->ValidateIndex(na3Idx);
                stateVals = gs->stateValues;
                if (stateVals[na3Idx] != 0) {
                    // Set AWARE_TEACHER = 1
                    gs = g_GameState_0046aa30;
                    awareIdx = gs->FindState("AWARE_TEACHER");
                    gs->ValidateIndex(awareIdx);
                    stateVals = gs->stateValues;
                    stateVals[awareIdx] = 1;
                    SendGameMessage(4, 0x1e87, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                }
            }
        } else if (stateVals[alienIdx] == 2) {
            // Check 1_NOTALIEN
            gs = g_GameState_0046aa30;
            int na1Idx = gs->FindState("1_NOTALIEN");
            gs->ValidateIndex(na1Idx);
            stateVals = gs->stateValues;
            if (stateVals[na1Idx] != 0) {
                gs = g_GameState_0046aa30;
                int na3Idx = gs->FindState("3_NOTALIEN");
                gs->ValidateIndex(na3Idx);
                stateVals = gs->stateValues;
                if (stateVals[na3Idx] != 0) {
                    gs = g_GameState_0046aa30;
                    awareIdx = gs->FindState("AWARE_TEACHER");
                    gs->ValidateIndex(awareIdx);
                    stateVals = gs->stateValues;
                    stateVals[awareIdx] = 2;
                    SendGameMessage(4, 0x1e89, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                }
            }
        } else if (stateVals[alienIdx] == 3) {
            // Check 1_NOTALIEN with bounds check
            gs = g_GameState_0046aa30;
            int na1Idx = gs->FindState("1_NOTALIEN");
            if (na1Idx < 0 || na1Idx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", na1Idx);
            }
            stateVals = gs->stateValues;
            if (stateVals[na1Idx] != 0) {
                // Check 2_NOTALIEN with bounds check
                gs = g_GameState_0046aa30;
                int na2Idx = gs->FindState("2_NOTALIEN");
                if (na2Idx < 0 || na2Idx > gs->maxStates - 1) {
                    ShowError("Invalid gamestate %d", na2Idx);
                }
                stateVals = gs->stateValues;
                if (stateVals[na2Idx] != 0) {
                    gs = g_GameState_0046aa30;
                    awareIdx = gs->FindState("AWARE_TEACHER");
                    gs->ValidateIndex(awareIdx);
                    stateVals = gs->stateValues;
                    stateVals[awareIdx] = 3;
                    SendGameMessage(4, 0x1e88, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                }
            }
        }
    }

    // Send background sound message
    SendGameMessage(5, bgSoundId, 0, 0, 0x1b, 0, 0, 0, 0, 0);

    // Set PREVIOUSROOM state
    gs = g_GameState_0046aa30;
    int prevIdx = gs->FindState("PREVIOUSROOM");
    gs->ValidateIndex(prevIdx);
    stateVals = gs->stateValues;
    stateVals[prevIdx] = 0x62;

    // Check P_IN_DETENTION
    gs = g_GameState_0046aa30;
    int pDetIdx = gs->FindState("P_IN_DETENTION");
    if (pDetIdx < 0 || pDetIdx > gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", pDetIdx);
    }
    stateVals = gs->stateValues;
    if (stateVals[pDetIdx] != 0) {
        // Check S_IN_DETENTION
        gs = g_GameState_0046aa30;
        int sDetIdx = gs->FindState("S_IN_DETENTION");
        if (sDetIdx < 0 || sDetIdx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", sDetIdx);
        }
        stateVals = gs->stateValues;
        if (stateVals[sDetIdx] != 0) {
            // Check D_IN_DETENTION
            gs = g_GameState_0046aa30;
            int dDetIdx = gs->FindState("D_IN_DETENTION");
            if (dDetIdx < 0 || dDetIdx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", dDetIdx);
            }
            stateVals = gs->stateValues;
            if (stateVals[dDetIdx] != 0) {
                // All 3 in detention - send detention message
                SendGameMessage(3, 0x1202, 0x2d, 1, 4, 0, 0, 0, 0, 0);
                // Create and push empty SpriteAction
                SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                g_PendingAction_00472d58.CopyFrom(&action);

                // Clear MUST_SAVEGAME
                gs = g_GameState_0046aa30;
                int saveIdx = gs->FindState("MUST_SAVEGAME");
                gs->ValidateIndex(saveIdx);
                stateVals = gs->stateValues;
                stateVals[saveIdx] = 0;
            }
        }
    }
}

/* Function start: 0x41E950 */
int SCI_SchoolMenu::ShutDown(SC_Message* msg) {
    ZBufferManager* zbm;
    ZBQueue* queue;
    void* item;
    int i;

    if (g_SchoolMenuActive_0046a190 == 0) {
        goto done;
    }
    g_SchoolMenuActive_0046a190 = 0;

    zbm = g_ZBufferManager_0046aa24;

    queue = zbm->m_queueA0;
    if (queue->head != 0) {
        queue->current = queue->head;
        while (queue->head != 0) {
            item = queue->Pop();
            if (item != 0) {
                delete (SoundCommand*)item;
            }
        }
    }

    queue = zbm->m_queueA4;
    if (queue->head != 0) {
        queue->current = queue->head;
        while (queue->head != 0) {
            item = queue->Pop();
            if (item != 0) {
                ((ZBuffer*)item)->CleanUpVBuffer();
                FreeMemory(item);
            }
        }
    }

    queue = zbm->m_queue9c;
    if (queue->head != 0) {
        queue->current = queue->head;
        while (queue->head != 0) {
            item = queue->Pop();
            if (item != 0) {
                ((RenderEntry*)item)->RenderEntry::~RenderEntry();
                FreeMemory(item);
            }
        }
    }

    zbm->m_palette = 0;

    if (palette != 0) {
        palette->~Palette();
        FreeMemory(palette);
        palette = 0;
    }

    if (background != 0) {
        background->~MMPlayer();
        FreeMemory(background);
        background = 0;
    }

    if (okayButton != 0) {
        okayButton->~T_MenuHotspot();
        FreeMemory(okayButton);
        okayButton = 0;
    }

    if (cancelButton != 0) {
        cancelButton->~T_MenuHotspot();
        FreeMemory(cancelButton);
        cancelButton = 0;
    }

    if (menuSound != 0) {
        menuSound->Unload();
        FreeMemory(menuSound);
        menuSound = 0;
    }

    {
        T_MenuHotspot** base = characters;
        i = 3;
        do {
            T_MenuHotspot* ptr = *base;
            if (ptr != 0) {
                ptr->~T_MenuHotspot();
                FreeMemory(ptr);
                *base = 0;
            }
            base++;
            i--;
        } while (i != 0);
    }

    {
        T_MenuHotspot** base = options;
        i = 9;
        do {
            T_MenuHotspot* ptr = *base;
            if (ptr != 0) {
                ptr->~T_MenuHotspot();
                FreeMemory(ptr);
                *base = 0;
            }
            base++;
            i--;
        } while (i != 0);
    }

    IconBar::CleanupIconBar(msg);

done:
    return 0;
}

/* Function start: 0x41EBD0 */
void SCI_SchoolMenu::Update(int param1, int param2) {
    T_MenuHotspot** charPtr;
    int i;

    if (handlerId != param2) {
        return;
    }
    if (g_SchoolMenuActive_0046a190 == 0) {
        return;
    }

    IconBar::Update(param1, param2);

    if (background != 0) {
        background->Draw();
    }

    if (cancelButton != 0) {
        cancelButton->Update();
    }

    if (okayButton != 0 && selectedOption > -1 && IsCharacterActive() != 0) {
        T_MenuHotspot* ok = okayButton;
        ok->bounds.left = 1;
        Sprite* spr = ok->cursor;
        if (spr != 0) {
            spr->ResetAnimation(1, 0);
        }
    } else {
        T_MenuHotspot* ok = okayButton;
        ok->bounds.left = 0;
        Sprite* spr = ok->cursor;
        if (spr != 0) {
            spr->ResetAnimation(0, 0);
        }
    }

    if (okayButton != 0) {
        okayButton->Update();
    }

    charPtr = characters;
    i = 3;
    do {
        if (*charPtr != 0) {
            (*charPtr)->Update();
        }
        charPtr++;
        i--;
    } while (i != 0);

    charPtr = options;
    i = 9;
    do {
        if (*charPtr != 0) {
            (*charPtr)->Update();
        }
        charPtr++;
        i--;
    } while (i != 0);

    g_Mouse_0046aa18->DrawCursor();
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
    EnqueueSpriteAction(&g_IconBarAction_00472d20);
}

/* Function start: 0x41F6B0 */
int SCI_SchoolMenu::IsCharacterActive() {
    int i = 0;
    T_MenuHotspot** p = characters;
    do {
        T_MenuHotspot* chr = *p;
        if (chr != 0) {
            int state = chr->bounds.left;
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

    gs = g_GameState_0046aa30;
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
                opt->bounds.left = 0;
                Sprite* animCtrl = opt->cursor;
                if (animCtrl != 0) {
                    animCtrl->ResetAnimation(0, 0);
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
        charIdx = g_PeriodStateIdx_0046cb90;
        gs = g_GameState_0046aa30;
        if (charIdx < 0 || charIdx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", charIdx);
        }
        stateVals = gs->stateValues;
        charVal = stateVals[charIdx];

        char ch;
        if (charVal == -1) {
            // Use default character
            charIdx = g_PeriodStateIdx_0046cb90;
            gs = g_GameState_0046aa30;
            if (charIdx < 0 || charIdx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", charIdx);
            }
            stateVals = gs->stateValues;
            ch = g_PeriodCharTable_0046cb94[stateVals[charIdx]];
        } else {
            ch = g_PeriodCharTable_0046cb94[charVal];
        }

        i++;
        sprintf(buf, "%cRM%d", (int)ch, i);

        if (*optPtr == 0) {
            goto next_option;
        }

        gs = g_GameState_0046aa30;
        stIdx = gs->FindState(buf);
        if (stIdx < 0 || stIdx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", stIdx);
        }

        int stVal;
        T_MenuHotspot* opt;
        opt = *optPtr;
        stateVals = gs->stateValues;
        stVal = stateVals[stIdx];
        opt->sprite = (Sprite*)stVal;

        gs = g_GameState_0046aa30;
        stIdx = gs->FindState(buf);
        if (stIdx < 0 || stIdx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", stIdx);
        }
        stateVals = gs->stateValues;

        if (stateVals[stIdx] == 0) {
            opt->bounds.left = -1;
            Sprite* animCtrl = opt->cursor;
            if (animCtrl != 0) {
                animCtrl->ResetAnimation(-1, 0);
            }
        } else {
            opt->bounds.left = 0;
            Sprite* animCtrl = opt->cursor;
            if (animCtrl != 0) {
                animCtrl->ResetAnimation(0, 0);
            }
        }

next_option:
        optPtr++;
    } while (i < 9);
}

/* Function start: 0x41F900 */
void SCI_SchoolMenu::OnProcessStart() {
    int charIdx;
    int i;
    char buf[16];

    for (charIdx = 0; charIdx < 3; charIdx++) {
        for (i = 1; i <= 0x12; i++) {
            char ch;
            if (charIdx == -1) {
                GameState* gs = g_GameState_0046aa30;
                int periodIdx = g_PeriodStateIdx_0046cb90;
                if (periodIdx < 0 || periodIdx > gs->maxStates - 1) {
                    ShowError("Invalid gamestate %d", periodIdx);
                }
                ch = g_PeriodCharTable_0046cb94[gs->stateValues[periodIdx]];
            } else {
                ch = g_PeriodCharTable_0046cb94[charIdx];
            }

            sprintf(buf, "%cRM%d", (int)ch, i);

            GameState* gs = g_GameState_0046aa30;
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
    T_MenuHotspot** charBase;
    int i;
    T_MenuHotspot** selPtr;
    GameState* gs;
    int* stateVals;
    int* msgMouse;
    SpriteAction* action;
    T_MenuHotspot** charPtr;
    int hitChar;
    char buf[28];
    char capBuf[28];
    int hitResult;
    T_MenuHotspot** optBase;
    T_MenuHotspot** optPtr;
    int hitOpt;
    char nextChar;

    action = (SpriteAction*)msg;
    if (IconBar::CheckButtonClick(msg) != 0) goto done;
    if (action->button1 < 2) goto done;

    // Check NUM_ACTIONS state
    gs = g_GameState_0046aa30;
    i = gs->FindState("NUM_ACTIONS");
    gs->ValidateIndex(i);
    stateVals = gs->stateValues;
    if (stateVals[i] >= 1) goto check_options;

    charBase = characters;
    charPtr = characters;
    i = 0;
    msgMouse = &action->mousePos.x;

char_loop:
    {
        SlimeDim mouseCoords = *(SlimeDim*)msgMouse;
        T_MenuHotspot* chr = *charPtr;
        if (chr->sprite != 0) {
            if (chr->bounds.right > mouseCoords.x ||
                chr->activeRight < mouseCoords.x ||
                chr->bounds.bottom > mouseCoords.y ||
                chr->activeBottom < mouseCoords.y) {
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
            if (((GlyphRect*)&opt->bounds.right)->HitTest(optCoords.x, optCoords.y)) {
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
            gs = g_GameState_0046aa30;
            int charIdx = g_PeriodStateIdx_0046cb90;
            gs->ValidateIndex(charIdx);
            stateVals = gs->stateValues;
            ch = g_PeriodCharTable_0046cb94[stateVals[charIdx]];
        } else {
            ch = g_PeriodCharTable_0046cb94[i];
        }

        sprintf(buf, "%c_IN_DETENTION", (int)ch);
        gs = g_GameState_0046aa30;
        hitResult = gs->FindState(buf);
        gs->ValidateIndex(hitResult);
        stateVals = gs->stateValues;
        if (stateVals[hitResult] != 0) goto check_options;

        if (i == -1) {
            gs = g_GameState_0046aa30;
            int charIdx = g_PeriodStateIdx_0046cb90;
            gs->ValidateIndex(charIdx);
            stateVals = gs->stateValues;
            ch = g_PeriodCharTable_0046cb94[stateVals[charIdx]];
        } else {
            ch = g_PeriodCharTable_0046cb94[i];
        }
        sprintf(buf, "%c_DETENTION_SUSPENSION", (int)ch);
        gs = g_GameState_0046aa30;
        hitResult = gs->FindState(buf);
        gs->ValidateIndex(hitResult);
        stateVals = gs->stateValues;
        if (stateVals[hitResult] > 2) goto check_options;

        if (i == -1) {
            gs = g_GameState_0046aa30;
            int charIdx = g_PeriodStateIdx_0046cb90;
            gs->ValidateIndex(charIdx);
            stateVals = gs->stateValues;
            ch = g_PeriodCharTable_0046cb94[stateVals[charIdx]];
        } else {
            ch = g_PeriodCharTable_0046cb94[i];
        }
        sprintf(buf, "%c_CAPTURED", (int)ch);
        gs = g_GameState_0046aa30;
        hitResult = gs->FindState(buf);
        gs->ValidateIndex(hitResult);
        stateVals = gs->stateValues;
        if (stateVals[hitResult] != 0) goto check_options;

        // Select this character - reset all
        {
            T_MenuHotspot** basePtr = charBase;
            int count = 3;
            do {
                T_MenuHotspot* chr = *basePtr;
                chr->bounds.left = 0;
                Sprite* animCtrl = chr->cursor;
                if (animCtrl != 0) {
                    animCtrl->ResetAnimation(0, 0);
                }
                basePtr++;
                count--;
            } while (count != 0);
        }

        selPtr = &characters[i];
        {
            T_MenuHotspot* selChr = *selPtr;
            selChr->bounds.left = 1;
            Sprite* animCtrl = selChr->cursor;
            if (animCtrl != 0) {
                animCtrl->ResetAnimation(1, 0);
            }
        }

        // Update global character selection
        gs = g_GameState_0046aa30;
        hitResult = g_PeriodStateIdx_0046cb90;
        gs->ValidateIndex(hitResult);
        stateVals = gs->stateValues;
        stateVals[hitResult] = i;

        hitResult = g_PeriodStateIdx_0046cb90;
        gs = g_GameState_0046aa30;
        gs->ValidateIndex(hitResult);
        stateVals = gs->stateValues;
        g_SchoolMenuSprite_0046af08->ResetAnimation(stateVals[hitResult] + 1, 0);

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
            gs = g_GameState_0046aa30;
            int periodIdx2 = g_PeriodStateIdx_0046cb90;
            int pIdx = gs->FindState("PERIOD");
            gs->ValidateIndex(pIdx);
            stateVals = gs->stateValues;
            int period = stateVals[pIdx];
            int offset = (g_GameState_0046aa30)->GetStateValue(periodIdx2);
            int fullIdx = period * 3 + offset;
            char* sndFile = GetSoundFilename(selSpr->soundEntries[fullIdx - 3]);
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
                opt->bounds.left = 0;
                Sprite* animCtrl = opt->cursor;
                if (animCtrl != 0) {
                    animCtrl->ResetAnimation(0, 0);
                }
            }
            baseOpts++;
            count--;
        } while (count != 0);
    }

    selPtr = &options[i];
    {
        T_MenuHotspot* selOpt = *selPtr;
        selOpt->bounds.left = 1;
        Sprite* animCtrl = selOpt->cursor;
        if (animCtrl != 0) {
            animCtrl->ResetAnimation(1, 0);
        }
    }

    // Load option sound
    {
        T_MenuHotspot* selOpt2 = *selPtr;
        gs = g_GameState_0046aa30;
        int charStateIdx = g_PeriodStateIdx_0046cb90;
        int pIdx = gs->FindState("PERIOD");
        gs->ValidateIndex(pIdx);
        stateVals = gs->stateValues;
        int period = stateVals[pIdx];
        int offset = (g_GameState_0046aa30)->GetStateValue(charStateIdx);
        int fullIdx = period * 3 + offset;
        char* sndFile = GetSoundFilename(selOpt2->soundEntries[fullIdx - 3]);
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
        int* goMouse = &action->mousePos.x;
        goCoords.x = goMouse[0];
        goCoords.y = goMouse[1];
        T_MenuHotspot* goSpr = okayButton;
        if (goSpr->sprite != 0) {
            if (((GlyphRect*)&goSpr->bounds.right)->HitTest(goCoords.x, goCoords.y)) {
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
        gs = g_GameState_0046aa30;
        i = gs->FindState("NUM_ACTIONS");
        gs->ValidateIndex(i);
        stateVals = gs->stateValues;
        stateVals[i]++;

        // Execute option action
        options[selectedOption]->ProcessSpriteActions();

        // Determine next character
        gs = g_GameState_0046aa30;
        i = gs->FindState("PERIOD");
        nextChar = 0;
        gs->ValidateIndex(i);
        stateVals = gs->stateValues;
        int period = stateVals[i];

        if (period == 0x12) {
            gs = g_GameState_0046aa30;
            int charIdx = g_PeriodStateIdx_0046cb90;
            gs->ValidateIndex(charIdx);
            stateVals = gs->stateValues;
            if (g_PeriodCharTable_0046cb94[stateVals[charIdx]] == 'D') {
                nextChar = 'P';
            } else {
                gs = g_GameState_0046aa30;
                charIdx = g_PeriodStateIdx_0046cb90;
                gs->ValidateIndex(charIdx);
                stateVals = gs->stateValues;
                if (g_PeriodCharTable_0046cb94[stateVals[charIdx]] == 'S') {
                    nextChar = 'D';
                } else {
                    gs = g_GameState_0046aa30;
                    charIdx = g_PeriodStateIdx_0046cb90;
                    gs->ValidateIndex(charIdx);
                    stateVals = gs->stateValues;
                    if (g_PeriodCharTable_0046cb94[stateVals[charIdx]] == 'P') {
                        nextChar = 'S';
                    }
                }
            }
        } else if (period == 0x13) {
            gs = g_GameState_0046aa30;
            i = gs->FindState("D_CAPTURED");
            gs->ValidateIndex(i);
            stateVals = gs->stateValues;
            if (stateVals[i] == 0) {
                gs = g_GameState_0046aa30;
                int charIdx = g_PeriodStateIdx_0046cb90;
                gs->ValidateIndex(charIdx);
                stateVals = gs->stateValues;
                if (g_PeriodCharTable_0046cb94[stateVals[charIdx]] != 'D') {
                    nextChar = 'D';
                    goto set_captured;
                }
            }

            gs = g_GameState_0046aa30;
            i = gs->FindState("S_CAPTURED");
            gs->ValidateIndex(i);
            stateVals = gs->stateValues;
            if (stateVals[i] == 0) {
                gs = g_GameState_0046aa30;
                int charIdx = g_PeriodStateIdx_0046cb90;
                gs->ValidateIndex(charIdx);
                stateVals = gs->stateValues;
                if (g_PeriodCharTable_0046cb94[stateVals[charIdx]] != 'S') {
                    nextChar = 'S';
                    goto set_captured;
                }
            }

            gs = g_GameState_0046aa30;
            i = gs->FindState("P_CAPTURED");
            gs->ValidateIndex(i);
            stateVals = gs->stateValues;
            if (stateVals[i] == 0) {
                gs = g_GameState_0046aa30;
                int charIdx = g_PeriodStateIdx_0046cb90;
                gs->ValidateIndex(charIdx);
                stateVals = gs->stateValues;
                if (g_PeriodCharTable_0046cb94[stateVals[charIdx]] != 'P') {
                    nextChar = 'P';
                }
            }
        }

set_captured:
        if (nextChar != 0) {
            sprintf(capBuf, "%c_CAPTURED", (int)nextChar);
            gs = g_GameState_0046aa30;
            i = gs->FindState(capBuf);
            gs->ValidateIndex(i);
            stateVals = gs->stateValues;
            stateVals[i] = 1;

            sprintf(capBuf, "%c_IN_DETENTION", (int)nextChar);
            gs = g_GameState_0046aa30;
            i = gs->FindState(capBuf);
            gs->ValidateIndex(i);
            stateVals = gs->stateValues;
            stateVals[i] = 1;
        }
    }

check_back:
    if (cancelButton == 0) goto done;
    {
        SlimeDim backCoords;
        backCoords.x = ((SpriteAction*)msg)->mousePos.x;
        backCoords.y = ((SpriteAction*)msg)->mousePos.y;
        T_MenuHotspot* backSpr = cancelButton;
        if (backSpr->sprite != 0) {
            if (((GlyphRect*)&backSpr->bounds.right)->HitTest(backCoords.x, backCoords.y)) {
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
    int count;
    int period;
    char label[32];
    int val3;
    int val2;
    int val1;
    char buf6[32];
    char buf5[32];
    char buf4[32];
    char bufs[96];
    char filename[64];
    GameState* gs;
    int* stateVals;
    int idx;

    sscanf(line, "%s", label);

    if (strcmp(label, "BACK") == 0) {
        background = new MMPlayer();
        Parser::ProcessFile((Parser*)background, this, 0);
        return 0;
    }

    if (strcmp(label, "AVAILABILITY") == 0) {
        count = sscanf(line, "%s %d %s %s %s", label, &period, bufs, bufs + 0x20, bufs + 0x40);
        if (count < 3 || period > 0x14 || period < 1) {
            ReportUnknownLabel("SCI_SchoolMenu");
        }
        gs = g_GameState_0046aa30;
        idx = gs->FindState("PERIOD");
        if (idx < 0 || idx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateVals = gs->stateValues;
        if (stateVals[idx] == period) {
            count = count - 2;
            if (count > 0) {
                char* p = bufs;
                do {
                    gs = g_GameState_0046aa30;
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
        sscanf(line, "%s %s", label, filename);
        palette = new Palette();
        palette->Load(filename);
        return 0;
    }

    if (strcmp(label, "BGSND") == 0) {
        sscanf(line, "%s %d", label, &bgSoundId);
        return 0;
    }

    if (strcmp(label, "CHAR") == 0) {
        sscanf(line, "%s %d", label, &period);
        T_MenuHotspot* hotspot = new T_MenuHotspot(period);
        characters[period] = hotspot;
        Parser::ProcessFile((Parser*)hotspot, this, 0);
        return 0;
    }

    if (strstr(label, "OPTION") != 0) {
        sscanf(line, "%s %d", label, &period);
        T_MenuHotspot* hotspot = new T_MenuHotspot(period);
        options[period] = hotspot;
        Parser::ProcessFile((Parser*)hotspot, this, 0);
        return 0;
    }

    if (strcmp(label, "OKAY") == 0) {
        okayButton = new T_MenuHotspot(0);
        Parser::ProcessFile((Parser*)okayButton, this, 0);
        return 0;
    }

    if (strcmp(label, "CANCEL") == 0) {
        cancelButton = new T_MenuHotspot(0);
        Parser::ProcessFile((Parser*)cancelButton, this, 0);

        // Check if NUM_ACTIONS is 0 - if so, destroy back button
        gs = g_GameState_0046aa30;
        idx = gs->FindState("NUM_ACTIONS");
        if (idx < 0 || idx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateVals = gs->stateValues;
        if (stateVals[idx] == 0 && cancelButton != 0) {
            delete cancelButton;
            cancelButton = 0;
        }
        return 0;
    }

    if (strstr(label, "CLASS") != 0) {
        count = sscanf(line, "%s %d %s %d %s %d %s %d", label, &period, buf4, &val1, buf5, &val2, buf6, &val3);
        if (count < 4 || period > 0x14 || period < 1) {
            ReportUnknownLabel("SCI_SchoolMenu");
        }
        gs = g_GameState_0046aa30;
        idx = gs->FindState("PERIOD");
        if (idx < 0 || idx > gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", idx);
        }
        stateVals = gs->stateValues;
        if (stateVals[idx] == period) {
            gs = g_GameState_0046aa30;
            idx = gs->FindState(buf4);
            if (idx < 0 || idx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", idx);
            }
            stateVals = gs->stateValues;
            stateVals[idx] = val1;

            gs = g_GameState_0046aa30;
            idx = gs->FindState(buf5);
            if (idx < 0 || idx > gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", idx);
            }
            stateVals = gs->stateValues;
            stateVals[idx] = val2;

            gs = g_GameState_0046aa30;
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


