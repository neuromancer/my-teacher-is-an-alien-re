#include "SCI_PracticeRoom.h"
#include "globals.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Palette.h"
#include "Sample.h"
#include "LinkedList.h"
#include "MMPlayer.h"
#include "GameState.h"
#include "Memory.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <new.h>

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
// FUN_00413e10 = ParseFile in Parser.h
extern "C" void SetVideoRes(int, int);



// FUN_00429c10 = SC_PRHotSpot::~SC_PRHotSpot — callers updated
// FUN_0042bc50 = LinkedList::RemoveCurrent COMDAT — callers updated to use RemoveCurrent()

static char* g_PracticeSavePath = "cfg\\practice.sav";
extern "C" FILE* __cdecl OpenSaveFile(char* path, char* mode); // 0x426050
void SavePracticeState();   // 0x42B100
void LoadPracticeState();   // 0x42B270
// FUN_00429df0 = SC_PRHotSpot::Update — callers updated
// FUN_00420f00 = T_MenuHotspot::Update — callers updated
#include "SC_PRHotSpot.h"
#include "T_MenuHotspot.h"
#include "MouseControl.h"

extern "C" void WriteToLog(const char* format, ...);
// FUN_00413e70 = Parser::ProcessFile in Parser.cpp

// FUN_00429b60 = SC_PRHotSpot ctor — callers updated to use new
// FUN_00420ce0 = T_MenuHotspot(int) ctor — callers updated to use new

/* Function start: 0x42B0F0 */
void __fastcall PracticeRoomNotify(void*) {}  // empty no-op, called on SC_PRHotSpot for hotspots 0x14-0x16



#include "SlimeTable.h"

/* Function start: 0x42A8D0 */
SCI_PracticeRoom::SCI_PracticeRoom() {
    memset(&introPlayed, 0, 10 * 4);
    handlerId = 0x2B;
}

SCI_PracticeRoom::~SCI_PracticeRoom() {
}

/* Function start: 0x42A9F0 */
void SCI_PracticeRoom::Init(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    CopyCommandData(msg);
    moduleParam = action->addressValue;
    introPlayed = 0;
    if (action->fromType == 0x2D) {
        SavePracticeState();
    }
    SetVideoRes(0x280, 0x1E0);
    slimeTable = new SlimeTable();
    GameState* gs = g_GameState_0046aa30;
    int iGameState = gs->FindLabel("FINAL_PRACTICEROOM");
    if (iGameState < 0 || gs->maxStates - 1 < iGameState) {
        ShowError("Invalid gamestate %d", iGameState);
    }
    ParseFile(this,
        gs->stateValues[iGameState] != 0
            ? "mis\\practice.mis" : "mis\\practice_old.mis",
        (char*)0);
    T_MenuHotspot** pBC = periodSprites;
    int iCount = 3;
    do {
        T_MenuHotspot* spr = *pBC;
        if (spr != 0) {
            spr->state = 0;
            if (spr->cursor != 0) {
                spr->cursor->ResetAnimation(0, 0);
            }
        }
        pBC++;
        iCount--;
    } while (iCount != 0);
    int iGS = g_PeriodStateIdx_0046cb90;
    gs = g_GameState_0046aa30;
    if (iGS < 0 || gs->maxStates - 1 < iGS) {
        ShowError("Invalid gamestate %d", iGS);
    }
    int gsIdx = gs->stateValues[iGS];
    T_MenuHotspot* selSprite = periodSprites[gsIdx];
    if (selSprite != 0) {
        selSprite->state = 1;
        if (selSprite->cursor != 0) {
            selSprite->cursor->ResetAnimation(1, 0);
        }
    }
    {
        Palette* pal = palette;
        if (pal != 0) {
            Palette** pDest = &g_ZBufferManager_0046aa24->m_palette;
            if (*pDest != 0) {
                WriteToLog("ddouble palette");
            }
            *pDest = pal;
        }
    }
    Sprite* mouseSpr = g_Mouse_0046aa18->m_sprite;
    if (mouseSpr != 0) {
        mouseSpr->ResetAnimation(0, 0);
    }
    SendGameMessage(5, bgSoundHandle, handlerId, moduleParam, 0x1B, 0, 0, 0, 0, 0);
    if (savedCommand == 0x2D) {
        slimeTable->Play(g_SlimeTableInit_0046ad6c == 0);
        g_SlimeTableInit_0046ad6c = 0;
    }
}

/* Function start: 0x42AC20 */
void SCI_PracticeRoom::ShutDown(SC_MessageParser* msg) {
    if (ambient != 0) {
        delete ambient;
        ambient = 0;
    }
    if (introSprite != 0) {
        delete introSprite;
        introSprite = 0;
    }
    {
        LinkedList* list = hotspotList;
        if (list != 0) {
            if (list->head != 0) {
                list->current = list->head;
                while (list->head != 0) {
                    void* data = ((EventList*)list)->RemoveCurrent();
                    if (data != 0) {
                        ((SC_PRHotSpot*)data)->~SC_PRHotSpot();
                        operator delete(data);
                    }
                }
            }
            operator delete(list);
            hotspotList = 0;
        }
    }
    if (palette != 0) {
        delete palette;
        palette = 0;
    }
    if (slimeTable != 0) {
        delete slimeTable;
        slimeTable = 0;
    }
    T_MenuHotspot** pBC = &periodSprites[0];
    int iVar = 3;
    do {
        T_MenuHotspot* spr = *pBC;
        if (spr != 0) {
            spr->~T_MenuHotspot();
            operator delete(spr);
            *pBC = 0;
        }
        pBC++;
        iVar--;
    } while (iVar != 0);
    if (msg != 0) {
        if (((SpriteAction*)msg)->addressType == 0x2D) {
            LoadPracticeState();
        }
        if (msg != 0) {
            SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        }
    }
    return;
}

/* Function start: 0x42AD80 */
void SCI_PracticeRoom::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }
    if (introPlayed == 0) {
        if (introSprite != 0) {
            introPlayed = introSprite->Do(introSprite->loc_x, introSprite->loc_y, 1.0);
            if (introPlayed == 0) {
                return;
            }
            introSprite->StopAnimationSound();
            return;
        }
        introPlayed = 1;
    }
    if (ambient != 0) {
        ((MMPlayer*)ambient)->Draw();
    }
    if (hotspotList != 0) {
        hotspotList->current = hotspotList->head;
        if (hotspotList->head != 0) {
            do {
                SC_PRHotSpot* hs = 0;
                ListNode* node = hotspotList->current;
                if (node != 0) {
                    hs = (SC_PRHotSpot*)node->data;
                }
                hs->Update();
                if (hotspotList->tail == hotspotList->current) break;
                if (hotspotList->current != 0) {
                    hotspotList->current = hotspotList->current->next;
                }
            } while (hotspotList->head != 0);
        }
    }
    T_MenuHotspot** pSprites = periodSprites;
    int iCount = 3;
    do {
        if (*pSprites != 0) {
            (*pSprites)->Update();
        }
        pSprites++;
        iCount--;
    } while (iCount != 0);
    (g_Mouse_0046aa18)->DrawCursor();
}

/* Function start: 0x42AE80 */
int SCI_PracticeRoom::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;

    if (action->button1 >= 2) {
        int local_14 = 0;
        T_MenuHotspot** pBC = periodSprites;
        T_MenuHotspot** local_28 = pBC;
        SlimeDim* pMouse = &action->mousePos;
        do {
            int bHit;
            {
                SlimeDim pt;
                pt.x = pMouse->x;
                pt.y = pMouse->y;
                T_MenuHotspot* hs = *local_28;
                if (hs->sprite == 0 ||
                    hs->bounds.left > pt.x ||
                    hs->bounds.right < pt.x ||
                    hs->bounds.top > pt.y ||
                    hs->bounds.bottom < pt.y) {
                    bHit = 0;
                } else {
                    bHit = 1;
                }
            }
            if (bHit) {
                T_MenuHotspot** resetPtr = pBC;
                int iCount = 3;
                do {
                    T_MenuHotspot* spr = *resetPtr;
                    spr->state = 0;
                    if (spr->cursor != 0) {
                        spr->cursor->ResetAnimation(0, 0);
                    }
                    resetPtr++;
                    iCount--;
                } while (iCount != 0);
                T_MenuHotspot* sel = periodSprites[local_14];
                sel->state = 1;
                if (sel->cursor != 0) {
                    sel->cursor->ResetAnimation(1, 0);
                }
                int iGS = g_PeriodStateIdx_0046cb90;
                GameState* gs = g_GameState_0046aa30;
                if (iGS < 0 || gs->maxStates - 1 < iGS) {
                    ShowError("Invalid gamestate %d", iGS);
                }
                gs->stateValues[iGS] = local_14;
                action->instruction = 0;
                goto done;
            }
            local_28++;
            local_14++;
        } while (local_14 < 3);
    }
    if (action->button1 == 2) {
        action->instruction = 2;
        action->addressType = handlerId;
    }
done:
    return 1;
}

/* Function start: 0x42B030 */
int SCI_PracticeRoom::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    switch (action->instruction) {
    case 2: {
        if (hotspotList == 0) break;
        hotspotList->current = hotspotList->head;
        if (hotspotList->head == 0) break;
        do {
            SC_PRHotSpot* hs = 0;
            ListNode* node = hotspotList->current;
            if (node != 0) {
                hs = (SC_PRHotSpot*)node->data;
            }
            if (hs->CheckCollision(msg) != 0) {
                break;
            }
            if (hotspotList->tail == hotspotList->current) break;
            if (hotspotList->current != 0) {
                hotspotList->current = hotspotList->current->next;
            }
        } while (hotspotList->head != 0);
        return 1;
    }
    case 7:
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        break;
    default:
        return 0;
    }
    return 1;
}

#include "FileArchive.h"

/* Function start: 0x42B100 */
void SavePracticeState() {
    FileArchive* ar = new FileArchive(g_PracticeSavePath);

    if (ar->fp != 0) {
        fclose(ar->fp);
        ar->fp = 0;
    }

    ar->fp = OpenSaveFile(ar->filename, "w");
    if (ar->fp == 0) {
        ShowError("FileArchive::Open() - Unable to open file '%s' for mode '%s'",
            ar->filename, "w");
    }

    ar->mode = 1;
    g_GameState_0046aa30->Serialize(ar);

    if (ar->fp != 0) {
        fclose(ar->fp);
        ar->fp = 0;
    }

    delete ar;

    GameState* gs = g_GameState_0046aa30;
    int gsIdx = gs->FindLabel("IN_PRACTICEROOM");
    if (gsIdx < 0 || gs->maxStates - 1 < gsIdx) {
        ShowError("Invalid gamestate %d", gsIdx);
    }
    gs->stateValues[gsIdx] = 1;
}

/* Function start: 0x42B270 */
void LoadPracticeState() {
    FileArchive* ar = new FileArchive(g_PracticeSavePath);

    if (ar->fp != 0) {
        fclose(ar->fp);
        ar->fp = 0;
    }

    ar->fp = OpenSaveFile(ar->filename, "r");
    if (ar->fp == 0) {
        ShowError("FileArchive::Open() - Unable to open file '%s' for mode '%s'",
            ar->filename, "r");
    }

    ar->mode = 0;
    g_GameState_0046aa30->Serialize(ar);

    if (ar->fp != 0) {
        fclose(ar->fp);
        ar->fp = 0;
    }

    delete ar;
}

/* Function start: 0x42B3B0 */
int SCI_PracticeRoom::LBLParse(char* param_1) {
    char local_bc[128];
    char local_3c[32];
    int local_18 = 0;
    int local_1c = 0;

    local_bc[0] = 0;
    local_3c[0] = 0;
    sscanf(param_1, " %s ", local_3c);

    if (strcmp(local_3c, "HANDLE") == 0) {
        sscanf(param_1, "%s %d", local_3c, &moduleParam);
    } else if (strcmp(local_3c, "PALETTE") == 0) {
        sscanf(param_1, "%s %s", local_3c, local_bc);
        if (palette != 0) {
            delete palette;
            palette = 0;
        }
        void* mem = operator new(8);
        Palette* newPal = 0;
        if (mem != 0) {
            newPal = InitPalette((Palette*)mem);
        }
        palette = newPal;
        newPal->Load(local_bc);
    } else if (strcmp(local_3c, "HOTSPOT") == 0) {
        sscanf(param_1, "%s %d ", local_3c, &local_18);
        if (hotspotList == 0) {
            hotspotList = new LinkedList();
        }
        SC_PRHotSpot* piVar6 = new SC_PRHotSpot(local_18, (int)this);
        Parser::ProcessFile((Parser*)piVar6, this, 0);
        LinkedList* list = hotspotList;
        if (piVar6 == 0) {
            ShowError("queue fault 0101");
        }
        list->current = list->head;
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                list->InsertNode(piVar6);
            } else {
                do {
                    ListNode* cur = list->current;
                    if (((SC_PRHotSpot*)cur->data)->hotspotId < piVar6->hotspotId) {
                        list->InsertNode(piVar6);
                        break;
                    }
                    if (list->tail == cur) {
                        list->PushNode(piVar6);
                        break;
                    }
                    if (cur != 0) {
                        list->current = cur->next;
                    }
                } while (list->current != 0);
            }
        } else {
            list->InsertNode(piVar6);
        }
    } else if (strcmp(local_3c, "CHAR") == 0) {
        sscanf(param_1, "%s %d", local_3c, &local_18);
        T_MenuHotspot* piVar6 = new T_MenuHotspot(local_18);
        periodSprites[local_18] = piVar6;
        Parser::ProcessFile((Parser*)piVar6, this, 0);
    } else if (strcmp(local_3c, "AMBIENT") == 0) {
        if (ambient == 0) {
            ambient = new MMPlayer();
        }
        Parser::ProcessFile((Parser*)ambient, this, 0);
    } else if (strcmp(local_3c, "INTRO") == 0) {
        if (introSprite == 0) {
            introSprite = new Sprite((char*)0);
        }
        Parser::ProcessFile((Parser*)introSprite, this, 0);
    } else if (strcmp(local_3c, "BG_SOUND") == 0) {
        sscanf(param_1, " %s %d ", local_3c, &bgSoundHandle);
    } else if (strcmp(local_3c, "MAX_SOUNDS") == 0) {
        sscanf(param_1, " %s %d ", local_3c, &local_18);
        slimeTable->Allocate(local_18);
    } else {
        if (strcmp(local_3c, "SOUND") == 0) {
            sscanf(param_1, " %s %d %s %d ", local_3c, &local_18, local_bc, &local_1c);
            if (local_18 >= 0 && local_18 <= *(int*)slimeTable - 1) {
                slimeTable->LoadEntry(local_18, local_bc, local_1c);
                goto lbl_done;
            }
        } else if (strcmp(local_3c, "END") == 0) {
            return 1;
        }
        ReportUnknownLabel("SCI_PracticeRoom");
    }
lbl_done:
    return 0;
}
