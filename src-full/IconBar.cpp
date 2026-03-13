#include "IconBar.h"
#include <string.h>
#include "Sprite.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "Parser.h"
#include "globals.h"
#include "SC_Question.h"
#include "GameState.h"

// Static data
static int g_IconBarLeft;                       // 0x473310
static int g_IconBarTop;                        // 0x473314
static int g_IconBarRight;                      // 0x473318
static int g_IconBarBottom;                     // 0x47331C
static IconBarEntry g_IconBarEntries[6];        // 0x473320 - 0x4733F8
static Sprite* g_IconBarSprite;                 // 0x46af08
static int g_IconBarRefCount;                   // 0x46af0c

// External functions
extern void __fastcall FUN_0044c740(void*);     // Sprite destructor helper
extern void __fastcall FUN_0044c9d0(void*);     // Sprite notify
extern void __fastcall FUN_00444af0(void*);     // SpriteAction destructor
extern void FUN_00444e40(SpriteAction*);        // SpriteAction method (cdecl 1 param)
extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern void __cdecl FUN_00425c50(char*, ...);
extern void __cdecl FUN_00425a90(int, int);
extern void __stdcall FUN_004309a0(int);
extern "C" void FUN_004309c0(void*);
extern void __cdecl FUN_00413e70(void*, int, char*);
extern void __fastcall FUN_00412a50(void*, int, char*);
extern void* __fastcall FUN_00403520(void* list);
extern void* __fastcall FUN_004035a0(void* list);
extern void __fastcall FUN_00401130(void*);
extern void __fastcall FUN_00401c80(void*);
extern void* __fastcall FUN_00404b80(void*);
extern void __fastcall FUN_00404d70(void*, int, int);

extern void* DAT_0046aa08;
extern "C" extern void* DAT_0046aa30;
extern void* DAT_0046aa18;
extern void* DAT_0046aa24;
extern char* DAT_0046aa2c;
extern GameState* DAT_0046aa3c;
extern void* DAT_0046a6e4;
extern int DAT_0046cb90;
extern SpriteAction DAT_00472d58;
extern SpriteAction DAT_00472d20;
extern SpriteAction DAT_00472d90;

// FUN_0044ccf0 is a thiscall Sprite method (4 stack params)
class SpriteRender {
public:
    int RenderAt(int x, int y, int p3, int p4);  // 0x44CCF0
};


// Thiscall Sprite methods
class SpriteSetup {
public:
    void ConfigStates(int p1);              // 0x44D2A0
    void ConfigRange(int, int, int, int);   // 0x44D210
};

/* Function start: 0x42D460 */
IconBar::IconBar() {
    handlerId = 0;
}

// Stub implementations for SCI_Dialog compatibility
void IconBar::InitIconBar(SC_Message* msg) {}
void IconBar::CleanupIconBar(SC_Message* msg) {}
int IconBar::CheckButtonClick(SC_Message* msg) { return 0; }

/* Function start: 0x42D5F0 */
IconBar::~IconBar() {
    Sprite* spr;
    SpriteAction* act;
    int j;

    g_IconBarRefCount = g_IconBarRefCount - 1;
    if (g_IconBarRefCount == 0) {
        IconBarEntry* entry = g_IconBarEntries;
        do {
            spr = entry->sprite;
            if (spr != 0) {
                FUN_0044c740(spr);
                FreeMemory(spr);
                entry->sprite = 0;
            }
            j = 2;
            SpriteAction** pSlot = &entry->slot0;
            do {
                act = *pSlot;
                if (act != 0) {
                    FUN_00444af0(act);
                    FreeMemory(act);
                    *pSlot = 0;
                }
                pSlot = pSlot + 1;
                j = j - 1;
            } while (j != 0);
            entry = entry + 1;
        } while (entry < &g_IconBarEntries[6]);
        if (g_IconBarSprite != 0) {
            spr = g_IconBarSprite;
            FUN_0044c740(spr);
            FreeMemory(spr);
            g_IconBarSprite = 0;
        }
    }
}

/* Function start: 0x42D6E0 */
void IconBar::Init(SC_Message* msg) {
    int iVar3;
    int* piVar1;
    void* data;
    Sprite* spr;
    GameState* gs;
    int idx;

    FUN_004309a0((int)msg);
    FUN_00425a90(0x280, 0x1e0);

    iVar3 = (int)DAT_0046aa24;
    if (*(int*)(iVar3 + 0x98) != 2) {
        *(int*)(iVar3 + 0x98) = 2;

        // Loop 1: cleanup list at offset 0xA0
        piVar1 = *(int**)(iVar3 + 0xa0);
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                int* item = (int*)FUN_00403520(piVar1);
                if (item != 0) {
                    *item = 0x461030;
                    FreeMemory(item);
                }
            }
        }

        // Loop 2: cleanup list at offset 0xA4
        piVar1 = *(int**)(iVar3 + 0xa4);
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                void* item = FUN_004035a0(piVar1);
                if (item != 0) {
                    FUN_00401c80(item);
                    FreeMemory(item);
                }
            }
        }

        // Loop 3: cleanup list at offset 0x9C (inline RemoveCurrent)
        piVar1 = *(int**)(iVar3 + 0x9c);
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                int* current = (int*)piVar1[2];
                if (current == 0) {
                    data = 0;
                } else {
                    if (*piVar1 == (int)current) {
                        *piVar1 = current[1];
                    }
                    if (piVar1[1] == (int)current) {
                        piVar1[1] = *current;
                    }
                    if (*current != 0) {
                        ((int*)*current)[1] = current[1];
                    }
                    if (current[1] != 0) {
                        *(int*)current[1] = *current;
                    }
                    data = FUN_00404b80(piVar1);
                    if ((void*)piVar1[2] != 0) {
                        FUN_00404d70((void*)piVar1[2], 0, 1);
                        piVar1[2] = 0;
                    }
                    piVar1[2] = *piVar1;
                }
                if (data != 0) {
                    *(int*)data = 0x46102c;
                    FUN_00401130((void*)((int)data + 4));
                    FreeMemory(data);
                }
            }
        }

        *(int*)(iVar3 + 0xa8) = 0;
    }

    UpdateAllSlots();

    spr = *(Sprite**)((char*)DAT_0046aa18 + 0x94);
    if (spr != 0) {
        spr->ResetAnimation(0, 0);
    }

    idx = DAT_0046cb90;
    gs = (GameState*)DAT_0046aa30;
    if (idx < 0 || gs->maxStates - 1 < idx) {
        FUN_00425c50("Invalid gamestate %d", idx);
    }
    g_IconBarSprite->ResetAnimation(gs->stateValues[idx] + 1, 0);
}

/* Function start: 0x42D920 */
int IconBar::ShutDown(SC_Message* msg) {
    if (msg != 0) {
        if ((unsigned int)(*(int*)msg - 0x1e) > 9) {
            IconBarEntry* entry = g_IconBarEntries;
            do {
                if (entry->sprite != 0) {
                    FUN_0044c9d0(entry->sprite);
                }
                entry = entry + 1;
            } while (entry < &g_IconBarEntries[6]);
            if (g_IconBarSprite != 0) {
                FUN_0044c9d0(g_IconBarSprite);
            }
        }
    }
    return 0;
}

/* Function start: 0x42D9A0 */
int IconBar::AddMessage(SC_Message* msg) {
    int buttonIndex;
    SpriteAction* act;
    GameState* gs;
    int result;

    act = (SpriteAction*)msg;
    FUN_004309c0(msg);

    if (act->field_2C == 0x77) {
        if (g_IconBarEntries[4].field_14 == 0) {
            return 0;
        }
        {
            SpriteAction temp(act->field_00, act->field_04, act->field_08, act->field_0C,
                             4, 0, 0, 0, 0, 0);
            DAT_00472d58.CopyFrom(&temp);
        }
        FUN_00444d90(0x2d, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        return 1;
    }

    {
        int inBounds;
        if (g_IconBarLeft <= act->dim.field_0 && g_IconBarRight >= act->dim.field_0 &&
            g_IconBarTop <= act->dim.field_4 && g_IconBarBottom >= act->dim.field_4) {
            inBounds = 1;
        } else {
            inBounds = 0;
        }
        if (inBounds == 0) {
            return 0;
        }
    }

    if (act->field_24 < 2) {
        return 1;
    }

    if (DAT_0046a6e4 != 0) {
        FUN_00444d90(0x1e, *(int*)((char*)DAT_0046a6e4 + 0x94),
                     handlerId, moduleParam, 0x17, 0, 0, 0, 0, 0);
    }

    buttonIndex = FindClickedEntry((int*)msg);

    if (buttonIndex == -1) {
        return 1;
    }

    if (buttonIndex == 4) {
        {
            SpriteAction temp(act->field_00, act->field_04, act->field_08, act->field_0C,
                             4, 0, 0, 0, 0, 0);
            DAT_00472d58.CopyFrom(&temp);
        }
        PlayButtonSound(buttonIndex);
        return 1;
    }

    if (buttonIndex == 5) {
        {
            SpriteAction temp(act->field_00, act->field_04, act->field_08, act->field_0C,
                             4, 0, 0, 0, 0, 0);
            DAT_00472d58.CopyFrom(&temp);
        }
        PlayButtonSound(buttonIndex);
        return 1;
    }

    if (buttonIndex == 0) {
        {
            SpriteAction temp(act->field_00, act->field_04, act->field_08, act->field_0C,
                             4, 0, 0, 0, 0, 0);
            DAT_00472d20.CopyFrom(&temp);
        }
        PlayButtonSound(buttonIndex);
        return 1;
    }

    if (buttonIndex == 2) {
        result = 0;
        if (handlerId == 0x20) {
            result = ((GameState*)DAT_0046aa3c)->FindState((char*)DAT_0046aa2c);
        }
        {
            SpriteAction temp(act->field_00, act->field_04, act->field_08, act->field_0C,
                             4, result, 0, 0, 0, 0);
            DAT_00472d90.CopyFrom(&temp);
        }
        gs = (GameState*)DAT_0046aa30;
        {
        int idx = gs->FindState("PERIOD");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            FUN_00425c50("Invalid gamestate %d", idx);
        }
        if (gs->stateValues[idx] == 1) {
            return 1;
        }
        PlayButtonSound(buttonIndex);
        return 1;
        }
    }

    if (buttonIndex == 1) {
        result = ((GameState*)DAT_0046aa3c)->FindState((char*)DAT_0046aa2c);
        if (handlerId == 0x20) {
            if (result == 5 || result == 0x12) {
                FUN_00444d90(4, 0x1b86, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                return 1;
            }
        }
    }

    PlayButtonSound(buttonIndex);
    return 1;
}

/* Function start: 0x42DD40 */
void IconBar::Update(int param1, int param2) {
    IconBarEntry* entry;
    int mouseX;
    int mouseY;
    int* mousePos;
    GameState* gs;

    if (handlerId != param2) {
        return;
    }

    ((SpriteRender*)g_IconBarSprite)->RenderAt(
        g_IconBarSprite->num_states,
        g_IconBarSprite->field_0xb0,
        0, 0x3ff00000);

    entry = g_IconBarEntries;
    do {
        if (entry->sprite != 0) {
            if (entry == &g_IconBarEntries[2]) {
                gs = (GameState*)DAT_0046aa30;
                if (gs->maxStates - 1 < 0x44) {
                    FUN_00425c50("Invalid gamestate %d", 0x44);
                }
                if (*(int*)(*(int*)((char*)gs + 0x90) + 0x110) != 0) {
                    entry->sprite->ResetAnimation(2, 0);
                } else {
                    mousePos = *(int**)((char*)DAT_0046aa08 + 0x1a0);
                    mouseY = 0;
                    if (mousePos != 0) {
                        mouseY = mousePos[1];
                    }
                    if (mousePos != 0) {
                        mouseX = mousePos[0];
                    } else {
                        mouseX = 0;
                    }
                    if (entry->left <= mouseX && entry->right >= mouseX &&
                        entry->top <= mouseY && entry->bottom >= mouseY) {
                        entry->sprite->ResetAnimation(1, 0);
                    } else {
                        entry->sprite->ResetAnimation(0, 0);
                    }
                }
                if (entry->field_14 != 0) {
                    gs = (GameState*)DAT_0046aa30;
                    if (gs->maxStates - 1 < 0x3c) {
                        FUN_00425c50("Invalid gamestate %d", 0x3c);
                    }
                    if (gs->stateValues[0x3c] != 1) {
                        ((SpriteRender*)entry->sprite)->RenderAt(
                            entry->sprite->num_states,
                            entry->sprite->field_0xb0,
                            0, 0x3ff00000);
                    }
                }
            } else {
                mousePos = *(int**)((char*)DAT_0046aa08 + 0x1a0);
                mouseY = 0;
                if (mousePos != 0) {
                    mouseY = mousePos[1];
                }
                if (mousePos != 0) {
                    mouseX = mousePos[0];
                } else {
                    mouseX = 0;
                }
                if (entry->left <= mouseX && entry->right >= mouseX &&
                    entry->top <= mouseY && entry->bottom >= mouseY) {
                    entry->sprite->ResetAnimation(1, 0);
                } else {
                    entry->sprite->ResetAnimation(0, 0);
                }

                if (entry->field_14 != 0) {
                    ((SpriteRender*)entry->sprite)->RenderAt(
                        entry->sprite->num_states,
                        entry->sprite->field_0xb0,
                        0, 0x3ff00000);
                }
            }
        }
        entry = entry + 1;
    } while (entry < &g_IconBarEntries[6]);
}

/* Function start: 0x42DEC0 */
int IconBar::FindClickedEntry(int* param) {
    int i;
    int x;
    int y;
    int found;

    i = 0;
    IconBarEntry* entry = g_IconBarEntries;
    do {
        if (entry->field_14 != 0) {
            x = param[7];
            if (entry->left <= x && entry->right >= x) {
                y = param[8];
                if (entry->top <= y && entry->bottom >= y) {
                    found = 1;
                } else {
                    found = 0;
                }
                if (found != 0) {
                    return i;
                }
            }
        }
        entry = entry + 1;
        i = i + 1;
    } while (entry < &g_IconBarEntries[6]);
    return -1;
}

/* Function start: 0x42DF10 */
void IconBar::PlayButtonSound(int buttonIndex) {
    int i;
    SpriteAction** pSlot;

    if (buttonIndex < 0 || buttonIndex > 6) {
        FUN_00425c50("Error in SC_IconBr.cpp: Invalid array index");
    } else {
        i = 2;
        pSlot = &g_IconBarEntries[buttonIndex].slot0;
        do {
            if (*pSlot != 0) {
                FUN_00444e40(*pSlot);
            }
            pSlot = pSlot + 1;
            i = i - 1;
        } while (i != 0);
    }
    FUN_00444d90(4, g_IconBarEntries[buttonIndex].field_14,
                 handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
}

/* Function start: 0x42DFA0 */
void IconBar::UpdateAllSlots() {
    IconBarEntry* entry;
    GameState* gs;
    int idx;

    entry = g_IconBarEntries;
    do {
        entry->SetSlotProperties(handlerId, moduleParam);
        entry = entry + 1;
    } while (entry < &g_IconBarEntries[6]);

    gs = (GameState*)DAT_0046aa30;
    idx = gs->FindState("PERIOD");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        FUN_00425c50("Invalid gamestate %d", idx);
    }
    *(int*)g_IconBarEntries[0].slot0 = 0x25;
}

/* Function start: 0x42E010 */
int IconBar::LBLParse(char* param) {
    int index;
    int value;
    void* newSpr;
    int p2;
    int p3;
    int p4;
    char label[32];

    index = -1;
    label[0] = 0;
    sscanf(param, " %s ", label);

    if (strcmp(label, "ICONBAR_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        g_IconBarSprite = spr;
        FUN_00413e70(spr, (int)this, (char*)0);
    } else if (strcmp(label, "ICON_SPRITE") == 0) {
        sscanf(param, " %s  %d ", label, &index);
        Sprite* spr = new Sprite(0);
        g_IconBarEntries[index].sprite = spr;
        spr->flags &= ~2;
        ((SpriteSetup*)g_IconBarEntries[index].sprite)->ConfigStates(2);
        ((SpriteSetup*)g_IconBarEntries[index].sprite)->ConfigRange(0, 1, 1, 1);
        ((SpriteSetup*)g_IconBarEntries[index].sprite)->ConfigRange(1, 2, 2, 1);
        g_IconBarEntries[index].sprite->handle = 0x3e9;
        FUN_00413e70(g_IconBarEntries[index].sprite, (int)this, (char*)0);
    } else if (strcmp(label, "ICON_SOUND") == 0) {
        sscanf(param, " %s  %d %d", label, &index, &value);
        g_IconBarEntries[index].field_18 = value;
    } else if (strcmp(label, "RECT") == 0) {
        sscanf(param, " %s  %d %d %d %d %d", label, &index, &value, &p2, &p3, &p4);
        g_IconBarEntries[index].left = value;
        g_IconBarEntries[index].top = p2;
        g_IconBarEntries[index].right = p3;
        g_IconBarEntries[index].bottom = p4;
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        FUN_00412a50(this, 0, "SC_IconBarModule");
    }
    return 0;
}

IconBarEntry::~IconBarEntry() {
}

/* Function start: 0x42E330 */
IconBarEntry::IconBarEntry() {
    left = 0;
    top = 0;
    right = 0;
    bottom = 0;
    memset(this, 0, sizeof(IconBarEntry));
}

/* Function start: 0x42E3A0 */
void IconBarEntry::CreateAction(int p1, int p2, int p3) {
    SpriteAction* action;

    action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    action->field_00 = p1;
    action->field_04 = p2;
    action->field_10 = p3;
    RegisterSlot(action);
}

/* Function start: 0x42E450 */
void IconBarEntry::RegisterSlot(SpriteAction* obj) {
    int i;
    SpriteAction** pSlot;

    i = 0;
    pSlot = &slot0;
    do {
        if (*pSlot == 0) {
            *pSlot = obj;
            return;
        }
        pSlot = pSlot + 1;
        i = i + 1;
    } while (i < 2);
}

/* Function start: 0x42E480 */
void IconBarEntry::SetSlotProperties(int p1, int p2) {
    int i;
    SpriteAction** pSlot;

    i = 2;
    pSlot = &slot0;
    do {
        if (*pSlot != 0) {
            (*pSlot)->field_08 = p1;
            (*pSlot)->field_0C = p2;
        }
        pSlot = pSlot + 1;
        i = i - 1;
    } while (i != 0);
}
