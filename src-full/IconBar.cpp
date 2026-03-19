#include "IconBar.h"
#include "MouseControl.h"
#include "InvSlotItem.h"
#include "MouseControl.h"
#include <string.h>
#include "LinkedList.h"
#include "MouseControl.h"
#include "Sprite.h"
#include "MouseControl.h"
#include "SpriteAction.h"
#include "MouseControl.h"
#include "Memory.h"
#include "MouseControl.h"
#include "Parser.h"
#include "MouseControl.h"
#include "globals.h"
#include "MouseControl.h"
#include "SC_Question.h"
#include "MouseControl.h"
#include "GameState.h"
#include "MouseControl.h"
#include "DrawEntry.h"
#include "MouseControl.h"
#include "Animation.h"
#include "MouseControl.h"
#include "VBuffer.h"
#include "MouseControl.h"

// Static data
static int g_IconBarLeft;                       // 0x473310
static int g_IconBarTop;                        // 0x473314
static int g_IconBarRight;                      // 0x473318
static int g_IconBarBottom;                     // 0x47331C
static IconBarEntry g_IconBarEntries[6];        // 0x473320 - 0x4733F8
static Sprite* g_IconBarSprite;                 // 0x46af08
static int g_IconBarRefCount;                   // 0x46af0c

// External functions
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" void SetVideoRes(int, int);         // 0x425A90 - in VBuffer.cpp

class InputManager;
extern InputManager* DAT_0046aa08;
extern "C" extern GameState* DAT_0046aa30;
class MouseControl;
extern MouseControl* DAT_0046aa18;
class ZBufferManager;
extern ZBufferManager* DAT_0046aa24;
extern char* DAT_0046aa2c;
extern GameState* DAT_0046aa3c;
extern void* DAT_0046a6e4;
extern int DAT_0046cb90;
extern SpriteAction DAT_00472d58;
extern SpriteAction DAT_00472d20;
extern SpriteAction DAT_00472d90;

// FUN_0044ccf0 is a thiscall Sprite method (4 stack params)


/* Function start: 0x42D340 */
void IconBar::SetIconBarRect() {
    g_IconBarLeft = 0;
    g_IconBarTop = 0x1AB;
    g_IconBarRight = 0x27F;
    g_IconBarBottom = 0x1E0;
}

/* Function start: 0x42D460 */
IconBar::IconBar() {
    handlerId = 0;
}


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
                delete (Sprite*)spr;
                entry->sprite = 0;
            }
            j = 2;
            SpriteAction** pSlot = &entry->slot0;
            do {
                act = *pSlot;
                if (act != 0) {
                    delete (SpriteAction*)act;
                    *pSlot = 0;
                }
                pSlot = pSlot + 1;
                j = j - 1;
            } while (j != 0);
            entry = entry + 1;
        } while (entry < &g_IconBarEntries[6]);
        if (g_IconBarSprite != 0) {
            spr = g_IconBarSprite;
            delete (Sprite*)spr;
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

    CopyCommandData(msg);
    SetVideoRes(0x280, 0x1e0);

    iVar3 = (int)DAT_0046aa24;
    if (*(int*)(iVar3 + 0x98) != 2) {
        *(int*)(iVar3 + 0x98) = 2;

        // Loop 1: cleanup list at offset 0xA0
        piVar1 = *(int**)(iVar3 + 0xa0);
        if (*piVar1 != 0) {
            piVar1[2] = *piVar1;
            while (*piVar1 != 0) {
                int* item = (int*)((LinkedList*)piVar1)->RemoveCurrent();
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
                DrawEntry* item = (DrawEntry*)((LinkedList*)piVar1)->RemoveCurrent();
                if (item != 0) {
                    item->~DrawEntry();
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
                    data = ((LinkedList*)piVar1)->GetCurrentData();
                    if ((void*)piVar1[2] != 0) {
                        delete (ListNode*)piVar1[2];
                        piVar1[2] = 0;
                    }
                    piVar1[2] = *piVar1;
                }
                if (data != 0) {
                    *(int*)data = 0x46102c;
                    ((Rect*)((int)data + 4))->~Rect();
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
    gs = DAT_0046aa30;
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
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
                    ((Sprite*)entry->sprite)->StopAnimationSound();
                }
                entry = entry + 1;
            } while (entry < &g_IconBarEntries[6]);
            if (g_IconBarSprite != 0) {
                g_IconBarSprite->StopAnimationSound();
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
    WriteMessageAddress(msg);

    if (act->lastKey == 0x77) {
        if (g_IconBarEntries[4].field_14 == 0) {
            return 0;
        }
        {
            SpriteAction temp(act->addressType, act->addressValue, act->fromType, act->fromValue,
                             4, 0, 0, 0, 0, 0);
            DAT_00472d58.CopyFrom(&temp);
        }
        SendGameMessage(0x2d, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        return 1;
    }

    {
        int inBounds;
        if (g_IconBarLeft <= act->mousePos.field_0 && g_IconBarRight >= act->mousePos.field_0 &&
            g_IconBarTop <= act->mousePos.field_4 && g_IconBarBottom >= act->mousePos.field_4) {
            inBounds = 1;
        } else {
            inBounds = 0;
        }
        if (inBounds == 0) {
            return 0;
        }
    }

    if (act->button1 < 2) {
        return 1;
    }

    if (DAT_0046a6e4 != 0) {
        SendGameMessage(0x1e, *(int*)((char*)DAT_0046a6e4 + 0x94),
                     handlerId, moduleParam, 0x17, 0, 0, 0, 0, 0);
    }

    buttonIndex = FindClickedEntry((int*)msg);

    if (buttonIndex == -1) {
        return 1;
    }

    if (buttonIndex == 4) {
        {
            SpriteAction temp(act->addressType, act->addressValue, act->fromType, act->fromValue,
                             4, 0, 0, 0, 0, 0);
            DAT_00472d58.CopyFrom(&temp);
        }
        PlayButtonSound(buttonIndex);
        return 1;
    }

    if (buttonIndex == 5) {
        {
            SpriteAction temp(act->addressType, act->addressValue, act->fromType, act->fromValue,
                             4, 0, 0, 0, 0, 0);
            DAT_00472d58.CopyFrom(&temp);
        }
        PlayButtonSound(buttonIndex);
        return 1;
    }

    if (buttonIndex == 0) {
        {
            SpriteAction temp(act->addressType, act->addressValue, act->fromType, act->fromValue,
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
            SpriteAction temp(act->addressType, act->addressValue, act->fromType, act->fromValue,
                             4, result, 0, 0, 0, 0);
            DAT_00472d90.CopyFrom(&temp);
        }
        gs = DAT_0046aa30;
        {
        int idx = gs->FindState("PERIOD");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
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
                SendGameMessage(4, 0x1b86, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
                return 1;
            }
        }
    }

    PlayButtonSound(buttonIndex);
    return 1;
}

/* Function start: 0x42DD30 */ /* 16 bytes, no assembly file extracted */
int IconBar::Exit(SC_Message* msg) {
    return WriteMessageAddress(msg);
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

    ((Sprite*)g_IconBarSprite)->Do(
        g_IconBarSprite->num_states,
        g_IconBarSprite->field_0xb0, 1.0);

    entry = g_IconBarEntries;
    do {
        if (entry->sprite != 0) {
            if (entry == &g_IconBarEntries[2]) {
                gs = DAT_0046aa30;
                if (gs->maxStates - 1 < 0x44) {
                    ShowError("Invalid gamestate %d", 0x44);
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
                    gs = DAT_0046aa30;
                    if (gs->maxStates - 1 < 0x3c) {
                        ShowError("Invalid gamestate %d", 0x3c);
                    }
                    if (gs->stateValues[0x3c] != 1) {
                        ((Sprite*)entry->sprite)->Do(
                            entry->sprite->num_states,
                            entry->sprite->field_0xb0, 1.0);
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
                    ((Sprite*)entry->sprite)->Do(
                        entry->sprite->num_states,
                        entry->sprite->field_0xb0, 1.0);
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
        ShowError("Error in SC_IconBr.cpp: Invalid array index");
    } else {
        i = 2;
        pSlot = &g_IconBarEntries[buttonIndex].slot0;
        do {
            if (*pSlot != 0) {
                EnqueueSpriteAction(*pSlot);
            }
            pSlot = pSlot + 1;
            i = i - 1;
        } while (i != 0);
    }
    SendGameMessage(4, g_IconBarEntries[buttonIndex].field_14,
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

    gs = DAT_0046aa30;
    idx = gs->FindState("PERIOD");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
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
        Parser::ProcessFile(spr, this, (char*)0);
    } else if (strcmp(label, "ICON_SPRITE") == 0) {
        sscanf(param, " %s  %d ", label, &index);
        Sprite* spr = new Sprite(0);
        g_IconBarEntries[index].sprite = spr;
        spr->flags &= ~2;
        ((Sprite*)g_IconBarEntries[index].sprite)->ConfigStates(2);
        ((Sprite*)g_IconBarEntries[index].sprite)->ConfigRange(0, 1, 1, 1);
        ((Sprite*)g_IconBarEntries[index].sprite)->ConfigRange(1, 2, 2, 1);
        g_IconBarEntries[index].sprite->handle = 0x3e9;
        Parser::ProcessFile(g_IconBarEntries[index].sprite, this, (char*)0);
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
        Parser::LBLParse("SC_IconBarModule");
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
    action->addressType = p1;
    action->addressValue = p2;
    action->instruction = p3;
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
            (*pSlot)->fromType = p1;
            (*pSlot)->fromValue = p2;
        }
        pSlot = pSlot + 1;
        i = i - 1;
    } while (i != 0);
}
