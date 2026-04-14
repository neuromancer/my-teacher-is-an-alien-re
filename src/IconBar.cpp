#include "IconBar.h"
#include "MouseControl.h"
#include "InputManager.h"
#include "InvSlotItem.h"
#include <string.h>
#include "LinkedList.h"
#include "Sprite.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "Parser.h"
#include "globals.h"
#include "SC_Question.h"
#include "GameState.h"
#include "DrawEntry.h"
#include "Animation.h"
#include "VBuffer.h"
#include "ZBufferManager.h"
#include "RenderEntry.h"
#include "SoundCommand.h"
#include "ZBuffer.h"

// CRT init: original binary calls SetIconBarRect via _initterm table (0x468030)
static void __cdecl _iconbar_crt_init(void) { IconBar::SetIconBarRect(); }
typedef void (__cdecl *_PVFV)(void);
#pragma data_seg(".CRT$XCU")
static _PVFV _init_iconbar = _iconbar_crt_init;
#pragma data_seg()

// External functions
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" void SetVideoRes(int, int);         // 0x425A90 - in VBuffer.cpp

// FUN_0044ccf0 is a thiscall Sprite method (4 stack params)


/* Function start: 0x42D340 */
void IconBar::SetIconBarRect() {
    g_IconBarLeft_00473310 = 0;
    g_IconBarTop_00473314 = 0x1AB;
    g_IconBarRight_00473318 = 0x27F;
    g_IconBarBottom_0047331c = 0x1E0;
}

/* Function start: 0x42D460 */
IconBar::IconBar() {
    if (g_SchoolMenuSprite_0046af08 == 0) {
        ParseFile(this, "mis\\Iconbar.mis", "[ICONBAR]");
        g_IconBarEntries_00473320[0].CreateAction(0x25, 1, 4);
        g_IconBarEntries_00473320[1].CreateAction(0x1E, 1, 4);
        g_IconBarEntries_00473320[2].CreateAction(1, 0x10, 4);
        g_IconBarEntries_00473320[2].CreateAction(0x20, 1, 4);
        g_IconBarEntries_00473320[3].CreateAction(0x27, 1, 4);
        g_IconBarEntries_00473320[4].CreateAction(0x2D, 1, 4);
        SpriteAction* exitAction = new SpriteAction(0x2F, 0, 0x2D, 1, 4, 0x2C, 2, 0, 0, 0);
        g_IconBarEntries_00473320[5].RegisterSlot(exitAction);
        g_InventoryState_004733e8 = 1;
    }
    handlerId = 0;
    g_IconBarRefCount_0046af0c++;
}


/* Function start: 0x42D5F0 */
IconBar::~IconBar() {
    Sprite* spr;
    SpriteAction* act;
    int j;

    g_IconBarRefCount_0046af0c = g_IconBarRefCount_0046af0c - 1;
    if (g_IconBarRefCount_0046af0c == 0) {
        IconBarEntry* entry = g_IconBarEntries_00473320;
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
        } while (entry < &g_IconBarEntries_00473320[6]);
        if (g_SchoolMenuSprite_0046af08 != 0) {
            spr = g_SchoolMenuSprite_0046af08;
            delete (Sprite*)spr;
            g_SchoolMenuSprite_0046af08 = 0;
        }
    }
}

/* Function start: 0x42D6E0 */
void IconBar::Init(SC_MessageParser* msg) {
    int iVar3;
    int* piVar1;
    void* data;
    Sprite* spr;
    GameState* gs;
    int idx;

    CopyCommandData(msg);
    SetVideoRes(0x280, 0x1e0);

    {
        ZBufferManager* zbm = g_ZBufferManager_0046aa24;
        if (zbm->m_state != 2) {
            zbm->m_state = 2;

            // Cleanup sound command queue
            ZBQueue* q0 = zbm->m_queueA0;
            if (q0->head != 0) {
                q0->current = q0->head;
                while (q0->head != 0) {
                    void* item = q0->Pop();
                    if (item != 0) {
                        delete (SoundCommand*)item;
                    }
                }
            }

            // Cleanup ZBuffer queue
            ZBQueue* q4 = zbm->m_queueA4;
            if (q4->head != 0) {
                q4->current = q4->head;
                while (q4->head != 0) {
                    void* item = q4->Pop();
                    if (item != 0) {
                        ((ZBuffer*)item)->CleanUpVBuffer();
                        FreeMemory(item);
                    }
                }
            }

            // Cleanup render queue (RenderEntry items)
            {
                ZBQueue* q9c = zbm->m_queue9c;
                if (q9c->head != 0) {
                    q9c->current = q9c->head;
                    while (q9c->head != 0) {
                        ListNode* node = q9c->current;
                        void* data;
                        if (node == 0) {
                            data = 0;
                        } else {
                            if (q9c->head == node) {
                                q9c->head = node->next;
                            }
                            if (q9c->tail == q9c->current) {
                                q9c->tail = q9c->current->prev;
                            }
                            node = q9c->current;
                            if (node->prev != 0) {
                                node->prev->next = node->next;
                            }
                            node = q9c->current;
                            if (node->next != 0) {
                                node->next->prev = node->prev;
                            }
                            data = q9c->GetCurrentData();
                            if (q9c->current != 0) {
                                delete q9c->current;
                                q9c->current = 0;
                            }
                            q9c->current = q9c->head;
                        }
                        if (data != 0) {
                            ((RenderEntry*)data)->RenderEntry::~RenderEntry();
                            FreeMemory(data);
                        }
                    }
                }
            }

            zbm->m_palette = 0;
        }
    }

    UpdateAllSlots();

    spr = g_Mouse_0046aa18->m_sprite;
    if (spr != 0) {
        spr->ResetAnimation(0, 0);
    }

    idx = g_PeriodStateIdx_0046cb90;
    gs = g_GameState_0046aa30;
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    g_SchoolMenuSprite_0046af08->ResetAnimation(gs->stateValues[idx] + 1, 0);
}

/* Function start: 0x42D920 */
int IconBar::ShutDown(SC_MessageParser* msg) {
    if (msg != 0) {
        if ((unsigned int)(*(int*)msg - 0x1e) > 9) {
            IconBarEntry* entry = g_IconBarEntries_00473320;
            do {
                if (entry->sprite != 0) {
                    entry->sprite->StopAnimationSound();
                }
                entry = entry + 1;
            } while (entry < &g_IconBarEntries_00473320[6]);
            if (g_SchoolMenuSprite_0046af08 != 0) {
                g_SchoolMenuSprite_0046af08->StopAnimationSound();
            }
        }
    }
    return 0;
}

/* Function start: 0x42D9A0 */
int IconBar::AddMessage(SC_MessageParser* msg) {
    int buttonIndex;
    SpriteAction* act;
    GameState* gs;
    int result;

    act = (SpriteAction*)msg;
    WriteMessageAddress(msg);

    if (act->lastKey == 0x77) {
        if (g_IconBarEntries_00473320[4].field_14 == 0) {
            return 0;
        }
        {
            SpriteAction temp(act->addressType, act->addressValue, act->fromType, act->fromValue,
                             4, 0, 0, 0, 0, 0);
            g_PendingAction_00472d58.CopyFrom(&temp);
        }
        SendGameMessage(0x2d, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        return 1;
    }

    {
        int inBounds;
        if (g_IconBarLeft_00473310 <= act->mousePos.x && g_IconBarRight_00473318 >= act->mousePos.x &&
            g_IconBarTop_00473314 <= act->mousePos.y && g_IconBarBottom_0047331c >= act->mousePos.y) {
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

    if (g_SelectedItem_0046a6e4 != 0) {
        SendGameMessage(0x1e, ((Handler*)g_SelectedItem_0046a6e4)->moduleParam,
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
            g_PendingAction_00472d58.CopyFrom(&temp);
        }
        PlayButtonSound(buttonIndex);
        return 1;
    }

    if (buttonIndex == 5) {
        {
            SpriteAction temp(act->addressType, act->addressValue, act->fromType, act->fromValue,
                             4, 0, 0, 0, 0, 0);
            g_PendingAction_00472d58.CopyFrom(&temp);
        }
        PlayButtonSound(buttonIndex);
        return 1;
    }

    if (buttonIndex == 0) {
        {
            SpriteAction temp(act->addressType, act->addressValue, act->fromType, act->fromValue,
                             4, 0, 0, 0, 0, 0);
            g_IconBarAction_00472d20.CopyFrom(&temp);
        }
        PlayButtonSound(buttonIndex);
        return 1;
    }

    if (buttonIndex == 2) {
        result = 0;
        if (handlerId == 0x20) {
            result = ((GameState*)g_GameState2_0046aa3c)->FindState((char*)g_StateString_0046aa2c);
        }
        {
            SpriteAction temp(act->addressType, act->addressValue, act->fromType, act->fromValue,
                             4, result, 0, 0, 0, 0);
            g_HotspotAction_00472d90.CopyFrom(&temp);
        }
        gs = g_GameState_0046aa30;
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
        result = ((GameState*)g_GameState2_0046aa3c)->FindState((char*)g_StateString_0046aa2c);
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

/* Function start: 0x42DD30 */
int IconBar::Exit(SC_MessageParser* msg) {
    return 0;
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

    g_SchoolMenuSprite_0046af08->Do(
        g_SchoolMenuSprite_0046af08->loc_x,
        g_SchoolMenuSprite_0046af08->loc_y, 1.0);

    entry = g_IconBarEntries_00473320;
    do {
        if (entry->sprite != 0) {
            if (entry == &g_IconBarEntries_00473320[2]) {
                gs = g_GameState_0046aa30;
                if (gs->maxStates - 1 < 0x44) {
                    ShowError("Invalid gamestate %d", 0x44);
                }
                if (gs->stateValues[0x44] != 0) {
                    entry->sprite->ResetAnimation(2, 0);
                } else {
                    mousePos = (int*)g_InputManager_0046aa08->pMouse;
                    mouseY = 0;
                    if (mousePos != 0) {
                        mouseY = mousePos[1];
                    }
                    if (mousePos != 0) {
                        mouseX = mousePos[0];
                    } else {
                        mouseX = 0;
                    }
                    if (entry->bounds.left <= mouseX && entry->bounds.right >= mouseX &&
                        entry->bounds.top <= mouseY && entry->bounds.bottom >= mouseY) {
                        entry->sprite->ResetAnimation(1, 0);
                    } else {
                        entry->sprite->ResetAnimation(0, 0);
                    }
                }
                if (entry->field_14 != 0) {
                    gs = g_GameState_0046aa30;
                    if (gs->maxStates - 1 < 0x3c) {
                        ShowError("Invalid gamestate %d", 0x3c);
                    }
                    if (gs->stateValues[0x3c] != 1) {
                        entry->sprite->Do(
                            entry->sprite->loc_x,
                            entry->sprite->loc_y, 1.0);
                    }
                }
            } else {
                mousePos = (int*)g_InputManager_0046aa08->pMouse;
                mouseY = 0;
                if (mousePos != 0) {
                    mouseY = mousePos[1];
                }
                if (mousePos != 0) {
                    mouseX = mousePos[0];
                } else {
                    mouseX = 0;
                }
                if (entry->bounds.left <= mouseX && entry->bounds.right >= mouseX &&
                    entry->bounds.top <= mouseY && entry->bounds.bottom >= mouseY) {
                    entry->sprite->ResetAnimation(1, 0);
                } else {
                    entry->sprite->ResetAnimation(0, 0);
                }

                if (entry->field_14 != 0) {
                    entry->sprite->Do(
                        entry->sprite->loc_x,
                        entry->sprite->loc_y, 1.0);
                }
            }
        }
        entry = entry + 1;
    } while (entry < &g_IconBarEntries_00473320[6]);
}

/* Function start: 0x42DEC0 */
int IconBar::FindClickedEntry(int* param) {
    int i;
    int x;
    int y;
    int found;

    i = 0;
    IconBarEntry* entry = g_IconBarEntries_00473320;
    do {
        if (entry->field_14 != 0) {
            x = param[7];
            if (entry->bounds.left <= x && entry->bounds.right >= x) {
                y = param[8];
                if (entry->bounds.top <= y && entry->bounds.bottom >= y) {
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
    } while (entry < &g_IconBarEntries_00473320[6]);
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
        pSlot = &g_IconBarEntries_00473320[buttonIndex].slot0;
        do {
            if (*pSlot != 0) {
                EnqueueSpriteAction(*pSlot);
            }
            pSlot = pSlot + 1;
            i = i - 1;
        } while (i != 0);
    }
    SendGameMessage(4, g_IconBarEntries_00473320[buttonIndex].field_18,
                 handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
}

/* Function start: 0x42DFA0 */
void IconBar::UpdateAllSlots() {
    IconBarEntry* entry;
    GameState* gs;
    int idx;

    entry = g_IconBarEntries_00473320;
    do {
        entry->SetSlotProperties(handlerId, moduleParam);
        entry = entry + 1;
    } while (entry < &g_IconBarEntries_00473320[6]);

    gs = g_GameState_0046aa30;
    idx = gs->FindState("PERIOD");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    g_IconBarEntries_00473320[0].slot0->addressType = 0x25;
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
        g_SchoolMenuSprite_0046af08 = spr;
        Parser::ProcessFile(spr, this, (char*)0);
    } else if (strcmp(label, "ICON_SPRITE") == 0) {
        sscanf(param, " %s  %d ", label, &index);
        Sprite* spr = new Sprite(0);
        g_IconBarEntries_00473320[index].sprite = spr;
        spr->flags &= ~2;
        g_IconBarEntries_00473320[index].sprite->ConfigStates(2);
        g_IconBarEntries_00473320[index].sprite->ConfigRange(0, 1, 1, 1);
        g_IconBarEntries_00473320[index].sprite->ConfigRange(1, 2, 2, 1);
        g_IconBarEntries_00473320[index].sprite->priority = 0x3e9;
        Parser::ProcessFile(g_IconBarEntries_00473320[index].sprite, this, (char*)0);
    } else if (strcmp(label, "ICON_SOUND") == 0) {
        sscanf(param, " %s  %d %d", label, &index, &value);
        g_IconBarEntries_00473320[index].field_18 = value;
    } else if (strcmp(label, "RECT") == 0) {
        sscanf(param, " %s  %d %d %d %d %d", label, &index, &value, &p2, &p3, &p4);
        g_IconBarEntries_00473320[index].bounds.left = value;
        g_IconBarEntries_00473320[index].bounds.top = p2;
        g_IconBarEntries_00473320[index].bounds.right = p3;
        g_IconBarEntries_00473320[index].bounds.bottom = p4;
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_IconBarModule");
    }
    return 0;
}

/* Function start: 0x42E330 */
IconBarEntry::IconBarEntry() {
    memset(this, 0, 9 * 4);
}

// IconBarEntry::~IconBarEntry defined in IconBarEntry.cpp (separate TU forces SEH in ctor)

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
