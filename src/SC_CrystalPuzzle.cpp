#include "SC_CrystalPuzzle.h"
#include "Sprite.h"
#include "Palette.h"
#include "Sample.h"
#include "SpriteAction.h"
#include "globals.h"
#include "string.h"
#include "GameState.h"
#include "ZBufferManager.h"
#include "InputManager.h"
#include "MouseControl.h"
#include "Memory.h"
#include "ZBuffer.h"
#include "SoundCommand.h"
#include "RenderEntry.h"
#include "LinkedList.h"
#include <string.h>
#include <stdio.h>
#include <new.h>

extern "C" void SetVideoRes(int, int);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" void ShowError(const char* format, ...);
extern "C" void WriteToLog(const char* format, ...);
extern "C" char* MakeAudioName(char* baseName);
extern int __stdcall AIL_sample_status(void*);

/* Function start: 0x44EBA0 */
SC_CrystalPuzzle::SC_CrystalPuzzle() {
    memset(&field_A8, 0, 0x46 * 4);

    rect1.left = 0;
    rect1.top = 0;
    handlerId = 0x26;
    needsUpdate = 1;
    rect1.right = 0x197;
    rect1.bottom = 0x1aa;

    rect2.left = 0x1d7;
    rect2.top = 0x3b;
    rect2.right = 0x25c;
    rect2.bottom = 0xbb;

    rect3.left = 0x1de;
    rect3.top = 0xf1;
    rect3.right = 0x25b;
    rect3.bottom = 0x16d;

    rect4.left = 0x73;
    rect4.top = 0x11d;
    rect4.right = 0x12f;
    rect4.bottom = 0x149;

    rect5.left = 0x32;
    rect5.top = 0x14a;
    rect5.right = 0x172;
    rect5.bottom = 0x192;

    palette = new Palette();
    palette->Load("puz_ForceField\\\\Puzztest.col");

    buttons1 = new Sprite("puz_ForceField\\\\buttons1.smk");
    buttons1->loc.x = 0x1dc;
    buttons1->loc.y = 0x147;

    buttons2 = new Sprite("puz_ForceField\\\\buttons2.smk");
    buttons2->loc.x = 0x1dc;
    buttons2->loc.y = 0x11c;

    buttons3 = new Sprite("puz_ForceField\\\\buttons3.smk");
    buttons3->loc.x = 0x1dc;
    buttons3->loc.y = 0xf1;

    paths1 = new Sprite("puz_ForceField\\\\paths1.smk");
    paths1->loc.x = 0x1ee;
    paths1->loc.y = 0x8d;

    paths2 = new Sprite("puz_ForceField\\\\paths2.smk");
    paths2->loc.x = 0x1ee;
    paths2->loc.y = 0x70;

    paths3 = new Sprite("puz_ForceField\\\\paths3.smk");
    paths3->loc.x = 0x1ee;
    paths3->loc.y = 0x53;

    {
        for (int count = 3; count != 0; count--) {
            (&buttons1)[3 - count]->flags &= ~2;
            (&buttons1)[3 - count]->priority = 10;
            (&buttons1)[3 - count]->ConfigStates(3);
            (&buttons1)[3 - count]->ConfigRange(0, 1, 1, 1);
            (&buttons1)[3 - count]->ConfigRange(1, 2, 2, 1);
            (&buttons1)[3 - count]->ConfigRange(2, 3, 3, 1);

            (&paths1)[3 - count]->flags &= ~2;
            (&paths1)[3 - count]->priority = 10;
            (&paths1)[3 - count]->ConfigStates(3);
            (&paths1)[3 - count]->ConfigRange(0, 1, 1, 1);
            (&paths1)[3 - count]->ConfigRange(1, 2, 2, 1);
            (&paths1)[3 - count]->ConfigRange(2, 3, 3, 1);
        }
    }

    puzztest = new Sprite("puz_ForceField\\\\puzztest.smk");
    puzztest->flags &= ~2;
    puzztest->priority = 5;
    puzztest->loc.x = 0;
    puzztest->loc.y = 0;

    litdoors = new Sprite("puz_ForceField\\\\litdoors.smk");
    litdoors->flags &= ~2;
    litdoors->priority = 10;
    litdoors->flags |= 0x40;
    litdoors->ConfigStates(3);
    litdoors->ConfigRange(0, 1, 1, 1);
    litdoors->ConfigRange(1, 2, 2, 1);
    litdoors->ConfigRange(2, 3, 3, 1);

    lowfloor = new Sprite("puz_ForceField\\\\lowfloor.smk");
    lowfloor->loc.x = 0x1d;
    lowfloor->loc.y = 0x150;

    midfloor = new Sprite("puz_ForceField\\\\midfloor.smk");
    midfloor->loc.x = 0x2c;
    midfloor->loc.y = 0x130;

    topfloor = new Sprite("puz_ForceField\\\\topfloor.smk");
    topfloor->loc.x = 0x5a;
    topfloor->loc.y = 0x11c;

    {
        for (int count = 3; count != 0; count--) {
            (&lowfloor)[3 - count]->flags &= ~2;
            (&lowfloor)[3 - count]->priority = 10;
            (&lowfloor)[3 - count]->flags |= 0x40;
            (&lowfloor)[3 - count]->ConfigStates(4);
            (&lowfloor)[3 - count]->ConfigRange(0, 1, 4, 1);
            (&lowfloor)[3 - count]->ConfigRange(1, 5, 8, 1);
            (&lowfloor)[3 - count]->ConfigRange(2, 9, 12, 1);
            (&lowfloor)[3 - count]->ConfigRange(3, 13, 16, 1);
        }
    }

    mapSprite = new Sprite("combats\\\\kidface.smk");
    mapSprite->flags &= ~2;
    mapSprite->priority = 10;
    mapSprite->flags |= 0x40;
    mapSprite->loc.x = 0x1b;
    mapSprite->loc.y = 0x1b3;
    mapSprite->ConfigStates(3);
    {
        int i = 0;
        if (0 < mapSprite->num_states) {
            do {
                i++;
                mapSprite->ranges[i - 1].dim.x = i;
                mapSprite->ranges[i - 1].dim.y = i;
                mapSprite->ranges[i - 1].repeatLimit = 0;
                mapSprite->ranges[i - 1].frameCounter = 0;
            } while (i < mapSprite->num_states);
        }
    }

    {
        int* p = enabledButtons;
        int i;
        for (i = 9; i != 0; i--) {
            *p = 1;
            p++;
        }
    }

    rect6.left = 0;
    rect6.top = 0x1a9;
    rect6.right = 0x280;
    rect6.bottom = 0x1e0;

    rect7.left = 0;
    rect7.top = 0x155;
    rect7.right = 0x1b2;
    rect7.bottom = 0x1e0;
}

/* Function start: 0x44F4D0 */
SC_CrystalPuzzle::~SC_CrystalPuzzle() {
    int i;
    Sprite* spr;

    for (i = 0; i < 3; i++) {
        spr = (&lowfloor)[i];
        if (spr != 0) {
            delete spr;
            (&lowfloor)[i] = 0;
        }

        spr = (&paths1)[i];
        if (spr != 0) {
            delete spr;
            (&paths1)[i] = 0;
        }

        spr = (&buttons1)[i];
        if (spr != 0) {
            delete spr;
            (&buttons1)[i] = 0;
        }
    }

    if (litdoors != 0) {
        delete litdoors;
        litdoors = 0;
    }

    if (puzztest != 0) {
        delete puzztest;
        puzztest = 0;
    }

    if (mapSprite != 0) {
        delete mapSprite;
        mapSprite = 0;
    }

    if (palette != 0) {
        delete palette;
        palette = 0;
    }

    if (field_A8 != 0) {
        delete field_A8;
        field_A8 = 0;
    }
}

/* Function start: 0x44F710 */
void SC_CrystalPuzzle::Init(SC_MessageParser* msg) {
    CopyCommandData(msg);

    SetVideoRes(0x280, 0x1e0);

    ZBufferManager* zbm = g_ZBufferManager_0046aa24;
    if (zbm->m_state != 2) {
        zbm->m_state = 2;

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
                    operator delete(obj);
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
                    operator delete(data);
                }
            }
        }

        zbm->m_palette = 0;
    }

    if (palette != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = palette;
    }

    puzzleFlags = 0;
    if (field_A8 == 0) {
        field_A8 = new SpriteAction(savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }

    int stateIdx = g_PeriodStateIdx_0046cb90;
    GameState* gs = g_GameState_0046aa30;
    if (stateIdx < 0 || gs->maxStates - 1 < stateIdx) {
        ShowError("Invalid gamestate %d", stateIdx);
    }

    int gsVal = gs->stateValues[stateIdx];
    mapSprite->ResetAnimation(gsVal, 0);

    SendGameMessage(5, 0xbc1, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);

    if (enabledButtons[4] != 0) {
        ResetPuzzle(4, 0);
    }
}

/* Function start: 0x44FA20 */
void SC_CrystalPuzzle::ShutDown(SC_MessageParser* msg) {
    if (puzztest != 0) {
        puzztest->StopAnimationSound();
    }

    if (litdoors != 0) {
        litdoors->StopAnimationSound();
    }

    Sprite** p = &paths1;
    int count = 3;
    do {
        p[0]->StopAnimationSound();
        p[3]->StopAnimationSound();
        p[6]->StopAnimationSound();
        p++;
        count--;
    } while (count != 0);

    mapSprite->StopAnimationSound();

    if (sound1 != 0) {
        sound1->Stop();
    }

    Sample* snd = sound1;
    if (snd != 0) {
        snd->Unload();
        FreeMemory(snd);
        sound1 = 0;
    }

    if (field_A8 != 0) {
        delete field_A8;
        field_A8 = 0;
    }

    return;
}

/* Function start: 0x44FAE0 */
int SC_CrystalPuzzle::AddMessage(SC_MessageParser* msg) {
    int* pmsg = (int*)msg;

    if (WriteMessageAddress(msg) != 0) {
        return 1;
    }

    if (pmsg[0xb] == 0x1b) {
        puzzleFlags |= 1;
        SC_CrystalPuzzle::FinalizeExit(msg);
        return 1;
    }

    if (pmsg[9] >= 2) {
        int mouseY;
        int mouseX;
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseY = g_InputManager_0046aa08->pMouse->y;
        } else {
            mouseY = 0;
        }
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        } else {
            mouseX = 0;
        }

        OnClick(mouseX, mouseY);

        int clickX = pmsg[7];
        int clickY = pmsg[8];
        if ((rect6.left <= clickX && clickX <= rect6.right &&
             rect6.top <= clickY && clickY <= rect6.bottom) ||
            (rect7.left <= clickX && clickX <= rect7.right &&
             rect7.top <= clickY && clickY <= rect7.bottom)) {
            puzzleFlags |= 1;
            SC_CrystalPuzzle::FinalizeExit(msg);
            return 1;
        }

        if (floorStates[0] != 0) {
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseY = g_InputManager_0046aa08->pMouse->y;
            } else {
                mouseY = 0;
            }
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseX = g_InputManager_0046aa08->pMouse->x;
            } else {
                mouseX = 0;
            }

            if (g_DoorRects_00473dc8[0].left <= mouseX && mouseX <= g_DoorRects_00473dc8[0].right &&
                mouseY >= g_DoorRects_00473dc8[0].top && mouseY <= g_DoorRects_00473dc8[0].bottom) {

                GameState* gs = g_GameState_0046aa30;
                int idx = gs->FindLabel("LINSEY_ALIEN");
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                if (gs->stateValues[idx] != 0) {
                    field_E8 = 0x21fc;
                } else {
                    puzzleFlags |= 2;
                }

                gs = g_GameState_0046aa30;
                idx = gs->FindLabel("LINSEY_ALIEN");
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                gs->stateValues[idx] = 0;
                SC_CrystalPuzzle::FinalizeExit(msg);
                return 1;
            }
        }

        if (floorStates[1] != 0) {
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseY = g_InputManager_0046aa08->pMouse->y;
            } else {
                mouseY = 0;
            }
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseX = g_InputManager_0046aa08->pMouse->x;
            } else {
                mouseX = 0;
            }

            if (g_DoorRects_00473dc8[1].left <= mouseX && mouseX <= g_DoorRects_00473dc8[1].right &&
                g_DoorRects_00473dc8[1].top <= mouseY && mouseY <= g_DoorRects_00473dc8[1].bottom) {

                GameState* gs = g_GameState_0046aa30;
                int idx = gs->FindLabel("STACY_ALIEN");
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                if (gs->stateValues[idx] != 0) {
                    field_E8 = 0x2200;
                } else {
                    puzzleFlags |= 2;
                }

                gs = g_GameState_0046aa30;
                idx = gs->FindLabel("STACY_ALIEN");
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                gs->stateValues[idx] = 0;
                SC_CrystalPuzzle::FinalizeExit(msg);
                return 1;
            }
        }

        if (floorStates[2] != 0) {
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseY = g_InputManager_0046aa08->pMouse->y;
            } else {
                mouseY = 0;
            }
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseX = g_InputManager_0046aa08->pMouse->x;
            } else {
                mouseX = 0;
            }

            if (g_DoorRects_00473dc8[2].left <= mouseX && mouseX <= g_DoorRects_00473dc8[2].right &&
                g_DoorRects_00473dc8[2].top <= mouseY && mouseY <= g_DoorRects_00473dc8[2].bottom) {
                puzzleFlags |= 2;
                SC_CrystalPuzzle::FinalizeExit(msg);
            }
        }
    }

    return 1;
}

/* Function start: 0x44FE20 */
int SC_CrystalPuzzle::Exit(SC_MessageParser* msg) {
    return handlerId == ((int*)msg)[0];
}

/* Function start: 0x44FE40 */
void SC_CrystalPuzzle::Update(int p1, int p2) {
    if (handlerId != p2) {
        return;
    }

    if (sound1 != 0) {
        Sample* snd = sound1;
        int m_sample = (int)snd->m_sample;
        if (m_sample != 0) {
            int fileSize = *(int*)(m_sample + 0xc);
            if (snd->m_size == fileSize) {
                if (AIL_sample_status((void*)m_sample) == 4) {
                    goto sound_done;
                }
            }
        }

        {
            Sample* snd2 = sound1;
            if (snd2 != 0) {
                snd2->Unload();
                FreeMemory(snd2);
                sound1 = 0;
            }
        }
    }

sound_done:
    if (puzztest != 0) {
        puzztest->Do(puzztest->loc.x, puzztest->loc.y, 1.0);
    }

    if (&buttons1 != 0) {
        DisplayButtons();
    }
    if (&paths1 != 0) {
        DisplayMap();
    }

    if (litdoors != 0) {
        DisplayLitDoors();
    }

    if (field_144 != 0) {
        DisplayThisFloorRow();
    }

    mapSprite->Do(mapSprite->loc.x, mapSprite->loc.y, 1.0);

    int mouseY;
    int mouseX;
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseY = g_InputManager_0046aa08->pMouse->y;
    } else {
        mouseY = 0;
    }
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseX = g_InputManager_0046aa08->pMouse->x;
    } else {
        mouseX = 0;
    }

    if (rect6.left <= mouseX && mouseX <= rect6.right &&
        rect6.top <= mouseY && mouseY <= rect6.bottom) {
        goto set_cursor_10;
    }

    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseY = g_InputManager_0046aa08->pMouse->y;
    } else {
        mouseY = 0;
    }
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseX = g_InputManager_0046aa08->pMouse->x;
    } else {
        mouseX = 0;
    }

    if (rect7.left <= mouseX && mouseX <= rect7.right &&
        rect7.top <= mouseY && mouseY <= rect7.bottom) {
        goto set_cursor_10;
    }

    if (floorStates[0] != 0) {
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseY = g_InputManager_0046aa08->pMouse->y;
        } else {
            mouseY = 0;
        }
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        } else {
            mouseX = 0;
        }

        if (g_DoorRects_00473dc8[0].left <= mouseX && mouseX <= g_DoorRects_00473dc8[0].right &&
            g_DoorRects_00473dc8[0].top <= mouseY && mouseY <= g_DoorRects_00473dc8[0].bottom) {
            goto set_cursor_20;
        }
    }

    if (floorStates[1] != 0) {
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseY = g_InputManager_0046aa08->pMouse->y;
        } else {
            mouseY = 0;
        }
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        } else {
            mouseX = 0;
        }

        if (g_DoorRects_00473dc8[1].left <= mouseX && mouseX <= g_DoorRects_00473dc8[1].right &&
            g_DoorRects_00473dc8[1].top <= mouseY && mouseY <= g_DoorRects_00473dc8[1].bottom) {
            goto set_cursor_20;
        }
    }

    if (floorStates[2] != 0) {
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseY = g_InputManager_0046aa08->pMouse->y;
        } else {
            mouseY = 0;
        }
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        } else {
            mouseX = 0;
        }

        if (g_DoorRects_00473dc8[2].left <= mouseX && mouseX <= g_DoorRects_00473dc8[2].right &&
            g_DoorRects_00473dc8[2].top <= mouseY && mouseY <= g_DoorRects_00473dc8[2].bottom) {
            goto set_cursor_20;
        }
    }

    if (field_144 != 0) {
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseY = g_InputManager_0046aa08->pMouse->y;
        } else {
            mouseY = 0;
        }
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        } else {
            mouseX = 0;
        }

        if (rect2.left <= mouseX && mouseX <= rect2.right &&
            rect2.top <= mouseY && mouseY <= rect2.bottom) {
            {
                Sprite* spr = g_Mouse_0046aa18->m_sprite;
                if (spr != 0) {
                    spr->ResetAnimation(0, 0);
                }
            }
            goto draw_cursor;
        }
    }

    {
        Sprite* spr = g_Mouse_0046aa18->m_sprite;
        if (spr != 0) {
            spr->ResetAnimation(0x15, 0);
        }
    }
    goto draw_cursor;

set_cursor_10:
    {
        Sprite* spr = g_Mouse_0046aa18->m_sprite;
        if (spr != 0) {
            spr->ResetAnimation(0xa, 0);
        }
    }
    goto draw_cursor;

set_cursor_20:
    {
        Sprite* spr = g_Mouse_0046aa18->m_sprite;
        if (spr != 0) {
            spr->ResetAnimation(0x14, 0);
        }
    }

draw_cursor:
    g_Mouse_0046aa18->DrawCursor();
}

/* Function start: 0x450110 */
int SC_CrystalPuzzle::FinalizeExit(SC_MessageParser* msg) {
    SpriteAction* act;
    int idx;
    void* mem;
    GameState* gs;

    if (savedCommand == 0x2b) {
        if (field_E8 != 0) {
            act = field_A8;
            act->addressType = 3;
            act = field_A8;
            act->addressValue = field_E8;
            act = field_A8;
            act->fromType = 0x2b;
            act = field_A8;
            act->fromValue = 1;
        }
    } else if ((puzzleFlags & 1) != 0) {
        act = field_A8;
        act->addressType = 0x20;
        act = field_A8;
        act->addressValue = 2;
        act = field_A8;
        act->extra1 = 0x16;

        gs = g_GameState_0046aa30;
        idx = gs->FindLabel("NUM_ACTIONS");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] += 0xa;
    } else if ((puzzleFlags & 2) != 0) {
        gs = g_GameState_0046aa30;
        idx = gs->FindLabel("ATTEMPTED_RESCUE");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] = 1;

        act = field_A8;
        act->addressType = 0x20;
        act = field_A8;
        act->addressValue = 1;
        act = field_A8;
        act->extra1 = 0x1b;

        gs = g_GameState_0046aa30;
        idx = gs->FindLabel("NUM_ACTIONS");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] += 0x14;
    } else {
        gs = g_GameState_0046aa30;
        idx = gs->FindLabel("ATTEMPTED_RESCUE");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] = 1;

        act = field_A8;
        act->addressType = 3;
        act = field_A8;
        act->addressValue = field_E8;

        act = field_A8;
        SpriteAction* child = act->childAction;
        if (child != 0) {
            delete child;
            act = field_A8;
            act->childAction = 0;
        }

        SpriteAction* newAction = new SpriteAction(0x20, 2, handlerId, moduleParam, 4, 0x18, 0, 0, 0, 0);

        act = field_A8;
        act->childAction = newAction;

        gs = g_GameState_0046aa30;
        idx = gs->FindLabel("NUM_ACTIONS");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] += 0x1e;
    }

    EnqueueSpriteAction(field_A8);

    if (field_A8 != 0) {
        delete field_A8;
        field_A8 = 0;
    }

    return 0;
}

/* Function start: 0x450400 */
void SC_CrystalPuzzle::OnClick(int x, int y) {
    SlimeDim clickPos;
    int inside;

    clickPos.x = x;
    clickPos.y = y;

    if (field_144 == 0) {
        field_144 = 1;
        ResetPuzzle(0, 1);
    } else {
        inside = (rect3.left <= x && x <= rect3.right &&
                  rect3.top <= y && rect3.bottom >= y);
        if (inside) {
            int i = 0;
            GlyphRect* r = g_PuzzleButtonRects_00473d30;
            do {
                inside = (r->left <= clickPos.x && clickPos.x <= r->right &&
                          r->top <= clickPos.y && r->bottom >= clickPos.y);
                if (inside) {
                    OnButtonClick(i);
                }
                r++;
                i++;
            } while (r < &g_PuzzleButtonRects_00473d30[9]);
            CheckSolution();
        } else {
            inside = (rect2.left <= x && x <= rect2.right &&
                      rect2.top <= y && rect2.bottom >= y);
            if (inside) {
                ResetPuzzle(3, 1);
            } else {
                inside = (rect1.left <= x && x <= rect1.right &&
                          rect1.top <= y && rect1.bottom >= y);
                if (inside) {
                    GlyphRect* r = g_DoorRects_00473dc8;
                    do {
                        inside = (r->left <= clickPos.x && clickPos.x <= r->right &&
                                  r->top <= clickPos.y && r->bottom >= clickPos.y);
                        if (inside) {
                            ResetPuzzle(1, 1);
                        }
                        r++;
                    } while (r < &g_DoorRects_00473dc8[3]);

                    inside = (rect4.left <= clickPos.x && clickPos.x <= rect4.right &&
                              rect4.top <= clickPos.y && rect4.bottom >= clickPos.y);
                    if (!inside) {
                        inside = (rect5.left <= clickPos.x && clickPos.x <= rect5.right &&
                                  rect5.top <= clickPos.y && rect5.bottom >= clickPos.y);
                    }
                    if (inside) {
                        ResetPuzzle(0, 1);
                    }
                }
            }
        }
    }
}

/* Function start: 0x450610 */
void SC_CrystalPuzzle::OnButtonClick(int buttonIndex) {
    int prevState = buttonStates[buttonIndex];
    int base = (buttonIndex / 3) * 3;

    ResetPuzzle(base / 3 + 6, 1);

    int* p = &buttonStates[base];
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;

    if (prevState == 0) {
        buttonStates[buttonIndex] = 1;
    }

    needsUpdate = 1;
}

/* Function start: 0x450670 */
int SC_CrystalPuzzle::CheckSolution() {
    int solutionIdx;
    int* solutionPtr;
    int tempStates[9];
    int i;
    int floorIdx;

    int* floorStatesBase = &floorStates[0];
    solutionIdx = 0;
    solutionPtr = g_PuzzleSolutions_0046cc98;
    floorStatesBase[0] = 0;
    floorStatesBase[1] = 0;
    floorStatesBase[2] = 0;

    while (1) {
        for (i = 0; i < 9; i++) {
            tempStates[i] = 0;
        }

        for (i = 0; i < 3; i++) {
            tempStates[solutionPtr[i]] = 1;
        }

        puzzleSolved = 1;
        for (i = 0; i < 9; i++) {
            if (buttonStates[i] != tempStates[i]) {
                puzzleSolved = 0;
            }
        }

        if (puzzleSolved != 0) break;

        solutionPtr += 4;
        solutionIdx++;
        if (solutionPtr >= (int*)0x46cd28) {
            return -1;
        }
    }

    if (solutionIdx >= 2) {
        floorIdx = 2;
        if (solutionIdx < 5) {
            floorIdx = 1;
        }
    } else {
        floorIdx = 0;
    }

    floorStates[floorIdx] = 1;

    int resetIdx = 2;
    if (solutionIdx < 2) {
        resetIdx = 5;
    }

    ResetPuzzle(resetIdx, 1);
    return g_PuzzleResultIdx_0046cca4[solutionIdx * 4];
}

/* Function start: 0x450760 */
void SC_CrystalPuzzle::ResetPuzzle(int idx, int val) {
    char buf[32];
    int sndNum;
    void* mem;
    Sample* newSample;

    if ((idx < 0 || 8 < idx) || idx == 9) {
        ShowError("%d is out of sound array range", idx);
    }

    if (sound1 != 0) {
        sound1->Stop();
        Sample* snd = sound1;
        if (snd != 0) {
            snd->Unload();
            FreeMemory(snd);
            sound1 = 0;
        }
    }

    if (enabledButtons[idx] != 0) {
        if (idx <= 5) {
            sndNum = idx + 0xbb4;
        } else {
            sndNum = idx + 0x139c;
        }

        sprintf(buf, "puz_ForceField\\\\snd%4.4d", sndNum);
        enabledButtons[idx] = val;

        newSample = new Sample();

        sound1 = newSample;
        newSample->Load(MakeAudioName(buf));
        sound1->Play(100, 1);
    }
}

/* Function start: 0x450890 */
void SC_CrystalPuzzle::DisplayButtons() {
    int i;

    for (i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            (&buttons1)[i / 3]->ResetAnimation(i % 3, 0);
            (&buttons1)[i / 3]->Do((&buttons1)[i / 3]->loc.x, (&buttons1)[i / 3]->loc.y, 1.0);
        }
    }
}

/* Function start: 0x4508F0 */
void SC_CrystalPuzzle::DisplayMap() {
    int i;

    if (paths1 == 0 || paths2 == 0 || paths3 == 0) {
        ShowError("Error in SCIpuzz1.cpp - DisplayMap()");
    }

    for (i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            (&paths1)[i / 3]->ResetAnimation(i % 3, 0);
            (&paths1)[i / 3]->Do((&paths1)[i / 3]->loc.x, (&paths1)[i / 3]->loc.y, 1.0);
        }
    }
}

/* Function start: 0x450980 */
void SC_CrystalPuzzle::DisplayLitDoors() {
    int i;
    int locX;

    for (i = 0; i < 3; i++) {
        if (floorStates[i] != 0) {
            litdoors->ResetAnimation(i, 0);

            locX = 0;
            if (i != 0) {
                if ((unsigned)(i - 1) < 1) {
                    locX = 0x92;
                } else {
                    locX = 0x138;
                }
            }

            int locY;
            if ((unsigned)(i - 1) < 1) {
                locY = 0x6e;
            } else {
                locY = 0x60;
            }

            litdoors->Do(locX, locY, 1.0);
        }
    }
}

/* Function start: 0x4509F0 */
void SC_CrystalPuzzle::DisplayThisFloorRow() {
    int floorVals[3];
    int i;

    floorVals[0] = -1;
    floorVals[1] = -1;
    floorVals[2] = -1;

    if (lowfloor == 0 || midfloor == 0 || topfloor == 0) {
        return;
    }

    for (i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            if (i >= 0 && i <= 2) {
                floorVals[0] = i;
            } else if (i >= 3 && i <= 5) {
                floorVals[1] = i % 3;
            } else if (i >= 6 && i <= 8) {
                floorVals[2] = i % 3;
            }
        }
    }

    if (needsUpdate != 0) {
        needsUpdate = 0;
        for (i = 0; i < 3; i++) {
            (&lowfloor)[i]->flags |= 0x20;

            if (floorVals[i] < -1 || floorVals[i] > 2) {
                ShowError("Error in SCIpuzz1.cpp - DisplayThisFloorRow:  Invalid case value");
            }

            int state;
            if (floorVals[i] == -1) {
                state = 3;
            } else {
                state = floorVals[i];
            }

            (&lowfloor)[i]->ResetAnimation(state, 0);
        }
    }

    for (i = 3; i != 0; i--) {
        Sprite* spr = (&lowfloor)[3 - i];
        spr->Do(spr->loc.x, spr->loc.y, 1.0);
    }
}

/* address unknown — 0x450B10 is EngineB ctor */
void SC_CrystalPuzzle::PlayPuzzleSound(int idx, int loop) {
}
