#include "SCI_IconBarModule.h"
#include "SC_Question.h"
#include "GameState.h"
#include "InputManager.h"
#include "MMPlayer.h"
#include "MouseControl.h"
#include "T_Object.h"
#include "TimeOut.h"
#include "Animation.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

// FUN_004459a0 = T_Hotspot::DoItem — callers updated
#include "Hotspot.h"

extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" int FileExists(const char*);
extern "C" void __cdecl DeleteMatchingFiles(char* pattern, ...);
extern "C" void SetVideoRes(int, int);
extern "C" void WriteToLog(const char*, ...);
#include "ZBufferManager.h"
#include "ZBuffer.h"
#include "SoundCommand.h"
#include "Palette.h"
#include "SpriteAction.h"

/* Function start: 0x401000 */
SCI_IconBarModule::SCI_IconBarModule() {
    memset(&exitTarget, 0, 0x22 * 4);
    boundRect.left = 0;
    boundRect.top = 0;
    boundRect.right = 0;
    boundRect.bottom = 0;
    handlerId = 0x20;
    field_B4 = -1;
    timeout = new TimeOut();
}

/* Function start: 0x4011A0 */
SCI_IconBarModule::~SCI_IconBarModule() {
    T_Hotspot** p;
    int i;
    void* data;

    if (field_E4 != 0) {
        delete field_E4;
        field_E4 = 0;
    }

    if (mmPlayer != 0) {
        delete mmPlayer;
        mmPlayer = 0;
    }

    i = 15;
    p = icons;
    do {
        if (*p != 0) {
            delete *p;
            *p = 0;
        }
        p++;
        i--;
    } while (i != 0);

    if (field_128 != 0 && field_128->head != 0) {
        field_128->current = field_128->head;
        while (field_128->head != 0) {
            data = field_128->RemoveCurrent();
            if (data != 0) {
                delete (SpriteAction*)data;
            }
        }
    }

    {
        Queue* qlist = field_128;
        if (qlist != 0) {
            if (qlist->head != 0) {
                qlist->current = qlist->head;
                while (qlist->head != 0) {
                    data = qlist->Pop();
                    if (data != 0) {
                        delete (SpriteAction*)data;
                    }
                }
            }
            FreeMemory(qlist);
            field_128 = 0;
        }
    }

    if (timeout != 0) {
        delete timeout;
        timeout = 0;
    }
}

/* Function start: 0x4013E0 */
void SCI_IconBarModule::Init(SC_Message* msg) {
    int changed;
    int targetRoom;
    SpriteAction* action;
    int periodIdx;
    char key[28];

    timeout->Start(2000);

    if (g_DetentionFlag_00468764 != 0) {
        SendGameMessage(0x2c, 0, 0, 0, 0x3b, 0, 0, 0, 0, 0);
        timeout->Start(1);
        return;
    }

    changed = 0;
    targetRoom = 0;

    if (*g_StateString_0046aa2c != 0) {
        targetRoom = g_GameState2_0046aa3c->FindState(g_StateString_0046aa2c);
    }

    action = (SpriteAction*)msg;
    if (action->extra1 != 0 && targetRoom != action->extra1) {
        char* roomName = g_GameState2_0046aa3c->GetState(action->extra1);
        strcpy(g_StateString_0046aa2c, roomName);
        changed = 1;
        targetRoom = action->extra1;
    }

    if (targetRoom < 1 || targetRoom > 0x1E) {
        SendGameMessage(0x25, 0, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        g_DetentionFlag_00468764 = 0;
        return;
    }

    // Check PERIOD change
    periodIdx = g_GameState_0046aa30->FindState("PERIOD");
    if (periodIdx < 0 || g_GameState_0046aa30->maxStates - 1 < periodIdx) {
        ShowError("Invalid gamestate %d", periodIdx);
    }
    if (g_GameState_0046aa30->stateValues[periodIdx] != field_B4) {
        changed = 1;
        periodIdx = g_GameState_0046aa30->FindState("PERIOD");
        if (periodIdx < 0 || g_GameState_0046aa30->maxStates - 1 < periodIdx) {
            ShowError("Invalid gamestate %d", periodIdx);
        }
        field_B4 = g_GameState_0046aa30->stateValues[periodIdx];
    }

    // Check room change
    if (targetRoom != currentRoom) {
        changed = 1;

        int prevIdx = g_GameState_0046aa30->FindState("PREVIOUSROOM");
        if (prevIdx < 0 || g_GameState_0046aa30->maxStates - 1 < prevIdx) {
            ShowError("Invalid gamestate %d", prevIdx);
        }
        g_GameState_0046aa30->stateValues[prevIdx] = currentRoom;

        int prevInstIdx = g_GameState_0046aa30->FindState("PREVIOUSROOMINSTANCE");
        if (prevInstIdx < 0 || g_GameState_0046aa30->maxStates - 1 < prevInstIdx) {
            ShowError("Invalid gamestate %d", prevInstIdx);
        }
        g_GameState_0046aa30->stateValues[prevInstIdx] = moduleParam;

        int roomIdx = g_GameState_0046aa30->FindState("ROOM");
        if (roomIdx < 0 || g_GameState_0046aa30->maxStates - 1 < roomIdx) {
            ShowError("Invalid gamestate %d", roomIdx);
        }
        g_GameState_0046aa30->stateValues[roomIdx] = targetRoom;
        currentRoom = targetRoom;

        char roomPath[60];
        sprintf(roomPath, "room%d", targetRoom);
        if (FileExists(roomPath) == 0) {
            DeleteMatchingFiles("%s\\*.smk", roomPath);
            DeleteMatchingFiles("%s\\*.col", roomPath);
            DeleteMatchingFiles("%s\\*.wav", roomPath);
        }
    }

    // Check moduleParam change
    int newInstance = action->addressValue;
    if (moduleParam != newInstance) {
        moduleParam = newInstance;
        changed = 1;
        int roomInstIdx = g_GameState_0046aa30->FindState("ROOMINSTANCE");
        if (roomInstIdx < 0 || g_GameState_0046aa30->maxStates - 1 < roomInstIdx) {
            ShowError("Invalid gamestate %d", roomInstIdx);
        }
        g_GameState_0046aa30->stateValues[roomInstIdx] = newInstance;
    }

    if (changed != 0 || roomInitialized == 0) {
        exitTarget = 0;
        hasBoundaryRect = 0;
        skipActionsCount = 0;
        roomInitialized = 1;

        if (field_E4 != 0) {
            field_E4->~Palette();
            FreeMemory(field_E4);
            field_E4 = 0;
        }

        if (mmPlayer != 0) {
            mmPlayer->~MMPlayer();
            FreeMemory(mmPlayer);
            mmPlayer = 0;
        }

        int count = 15;
        T_Hotspot** p = icons;
        do {
            if (*p != 0) {
                (*p)->~T_Hotspot();
                FreeMemory(*p);
                *p = 0;
            }
            p++;
            count--;
        } while (count != 0);

        if (field_128 != 0) {
            if (field_128->head != 0) {
                field_128->current = field_128->head;
                while (field_128->head != 0) {
                    SpriteAction* data = (SpriteAction*)((Queue*)field_128)->Pop();
                    if (data != 0) {
                        data->~SpriteAction();
                        FreeMemory(data);
                    }
                }
            }
            FreeMemory(field_128);
            field_128 = 0;
        }

        staticSceneFound = 0;
        mode = 0;
        videoDim.x = 0x280;
        videoDim.y = 0x1E0;

        // Parse static scene
        sprintf(key, "[SEARCHSCREEN%d_STATIC]", moduleParam);
        ParseFile(this, g_StateString_0046aa2c, key);

        // Parse period-specific scene if no static override
        if (staticSceneFound == 0) {
            periodIdx = g_GameState_0046aa30->FindState("PERIOD");
            if (periodIdx < 0 || g_GameState_0046aa30->maxStates - 1 < periodIdx) {
                ShowError("Invalid gamestate %d", periodIdx);
            }
            sprintf(key, "[SEARCHSCREEN%d_PERIOD%2.2d]", moduleParam,
                    g_GameState_0046aa30->stateValues[periodIdx]);
            ParseFile(this, g_StateString_0046aa2c, key);
            staticSceneFound = 0;
        }

        // Dispatch queued actions
        if (field_128 != 0) {
            field_128->current = field_128->head;
            if (field_128->head != 0) {
                do {
                    void* nodeData = 0;
                    if (field_128->current != 0) {
                        nodeData = field_128->current->data;
                    }
                    int* actionData = (int*)nodeData;
                    if (actionData[2] == 0 ||
                        (actionData[2] == action->fromType &&
                         (actionData[3] == 0 ||
                          (actionData[2] == action->fromType && action->fromValue == actionData[3])))) {
                        SpriteAction* child = (SpriteAction*)actionData[0xD];
                        if (child != 0) {
                            char buf[128];
                            sprintf(buf, "Init dispatch child: type=%d inst=%d val=%d from=%d\nCondition: data.from=%d msg.from=%d",
                                child->addressType, child->instruction, child->addressValue, child->fromType,
                                actionData[2], action->fromType);
                            MessageBoxA(0, buf, "Init Dispatch", 0);
                        }
                        EnqueueSpriteAction((void*)actionData[0xD]);
                    }
                    if (field_128->tail == field_128->current) break;
                    if (field_128->current != 0) {
                        field_128->current = field_128->current->next;
                    }
                } while (field_128->head != 0);
            }
        }
    }

    // Check character class
    int charIdx = g_PeriodStateIdx_0046cb90;
    if (charIdx < 0 || g_GameState_0046aa30->maxStates - 1 < charIdx) {
        ShowError("Invalid gamestate %d", charIdx);
    }
    char classLabel[8];
    sprintf(classLabel, "%c_CLASS", (int)g_PeriodCharTable_0046cb94[g_GameState_0046aa30->stateValues[charIdx]]);
    int classIdx = g_GameState_0046aa30->FindState(classLabel);
    if (classIdx < 0 || g_GameState_0046aa30->maxStates - 1 < classIdx) {
        ShowError("Invalid gamestate %d", classIdx);
    }
    if (g_GameState_0046aa30->stateValues[classIdx] == targetRoom) {
        int wentIdx = g_GameState_0046aa30->FindState("WENT_TO_CLASS");
        if (wentIdx < 0 || g_GameState_0046aa30->maxStates - 1 < wentIdx) {
            ShowError("Invalid gamestate %d", wentIdx);
        }
        g_GameState_0046aa30->stateValues[wentIdx] = 1;
    }

    // Call IconBar::Init
    IconBar::Init(msg);

    // Setup icon bar entries
    if (mode == 0) {
        int* arr = &g_IconBarState_00473334;
        do {
            *arr = 1;
            arr = arr + 9;
        } while ((unsigned int)arr < (unsigned int)&g_InventoryState_004733e8);
        if (hasBoundaryRect != 0 || (targetRoom > 0x13 && targetRoom < 0x1E)) {
            g_IconBarState_00473334 = 0;
        }
        CopyCommandData(msg);
    } else {
        int* arr = &g_IconBarState_00473334;
        do {
            *arr = 0;
            arr = arr + 9;
        } while ((unsigned int)arr < (unsigned int)&g_InventoryState_004733e8);

        SetVideoRes(videoDim.x, videoDim.y);

        // ZBufferManager queue cleanup
        if (g_ZBufferManager_0046aa24->m_state != 2) {
            g_ZBufferManager_0046aa24->m_state = 2;

            ZBQueue* queue = g_ZBufferManager_0046aa24->m_queueA0;
            if (queue->head != 0) {
                queue->current = queue->head;
                while (queue->head != 0) {
                    void* item = queue->Pop();
                    if (item != 0) {
                        delete (SoundCommand*)item;
                    }
                }
            }

            queue = g_ZBufferManager_0046aa24->m_queueA4;
            if (queue->head != 0) {
                queue->current = queue->head;
                while (queue->head != 0) {
                    void* item = queue->Pop();
                    if (item != 0) {
                        ((ZBuffer*)item)->CleanUpVBuffer();
                        FreeMemory(item);
                    }
                }
            }

            g_ZBufferManager_0046aa24->m_queue9c->ClearList();
            g_ZBufferManager_0046aa24->m_palette = 0;
        }

        if (g_Mouse_0046aa18->m_sprite != 0) {
            g_Mouse_0046aa18->m_sprite->ResetAnimation(0, 0);
        }
    }

    // Send background sound message
    SendGameMessage(5, exitTarget, 0, 0, 0x1b, 0, 0, 0, 0, 0);

    // Apply palette if set
    if (field_E4 != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = field_E4;
    }
}


/* Function start: 0x401CF0 */
int SCI_IconBarModule::ShutDown(SC_Message* msg) {
    T_Hotspot** pIcon;
    int i;

    if (msg == 0) {
        if (mmPlayer != 0) {
            mmPlayer->StopAll();
        }
    } else {
        if (((SpriteAction*)msg)->addressType == 0x1f) {
            goto skip_cursor;
        }
        if (mmPlayer != 0) {
            mmPlayer->StopAll();
        }
    }

skip_cursor:
    pIcon = icons;
    i = 15;
    do {
        if (*pIcon != 0) {
            (*pIcon)->StopAll();
        }
        pIcon = pIcon + 1;
        i = i - 1;
    } while (i != 0);

    if (g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0, 0);
    }

    g_Mouse_0046aa18->DrawCursor();

    if (mode == 0) {
        IconBar::ShutDown(msg);
    }

    return 0;
}

/* Function start: 0x401D80 */
void SCI_IconBarModule::Update(int param1, int param2) {
    T_Hotspot** pIcon;
    int i;
    int spriteParam;

    if (param2 == 0x1f) {
        mmPlayer->Draw();
    }
    if (handlerId != param2) {
        return;
    }
    if (g_DetentionFlag_00468764 != 0) {
        if (timeout->IsTimeOut() != 0) {
            SendGameMessage(0x2c, 0, 0, 0, 0x3b, 0, 0, 0, 0, 0);
            return;
        }
    }
    if (mode == 0) {
        IconBar::Update(param1, param2);
    }
    spriteParam = param1;

    mmPlayer->Draw();
    pIcon = icons;
    i = 15;
    do {
        if (*pIcon != 0) {
            (*pIcon)->DoItem(spriteParam);
        }
        pIcon = pIcon + 1;
        i = i - 1;
    } while (i != 0);

    UpdateCursor();
}

/* Function start: 0x401E40 */
int SCI_IconBarModule::AddMessage(SC_Message* msg) {
    int idx;
    int iconIdx;

    timeout->Start(2000);

    if (mode == 0) {
        if (IconBar::AddMessage(msg) != 0) {
            return 1;
        }
    } else {
        IconBar::CheckButtonClick(msg);
    }

    if (((SpriteAction*)msg)->lastKey != 0) {
        return 1;
    }

    if (((SpriteAction*)msg)->button1 >= 2) {
        iconIdx = FindClickedIcon(((SpriteAction*)msg)->mousePos.x, ((SpriteAction*)msg)->mousePos.y);
        if (iconIdx != -1) {
            ((SpriteAction*)msg)->extra1 = iconIdx;
            if (((SpriteAction*)msg)->button1 == 2) {
                ((SpriteAction*)msg)->instruction = 2;
            } else if (((SpriteAction*)msg)->button1 == 3) {
                ((SpriteAction*)msg)->instruction = 3;
            }

            if (g_SelectedItem_0046a6e4 != 0) {
                ((SpriteAction*)msg)->extra2 = g_SelectedItem_0046a6e4->itemId;
            } else {
                ((SpriteAction*)msg)->extra2 = 0;
            }

            if (skipActionsCount == 0) {
                idx = g_GameState_0046aa30->FindLabel("NUM_ACTIONS");
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                g_GameState_0046aa30->stateValues[idx]++;
            }

            Exit(msg);

            ((SpriteAction*)msg)->addressType = 0;
        }

        if (hasBoundaryRect != 0) {
            int mx = ((SpriteAction*)msg)->mousePos.x;
            int my = ((SpriteAction*)msg)->mousePos.y;
            int inRect;
            if (boundRect.left > mx || boundRect.right < mx || boundRect.top > my || boundRect.bottom < my) {
                inRect = 0;
            } else {
                inRect = 1;
            }
            if (inRect != 0) {
                SendGameMessage(0x2c, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
            }
        }
    } else if (((SpriteAction*)msg)->button2 >= 2) {
        idx = g_GameState_0046aa30->FindLabel("NUM_ACTIONS");
        if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        g_GameState_0046aa30->stateValues[idx]++;
    }

    return 1;
}

/* Function start: 0x4020A0 */
int SCI_IconBarModule::FindClickedIcon(int x, int y) {
    int i;
    T_Hotspot** pIcon;
    T_Hotspot* icon;
    int inRect;

    i = 0;
    pIcon = icons;
    do {
        icon = *pIcon;
        if (icon != 0) {
            if (x < icon->hotspotPos.x || icon->hotspotRight < x ||
                y < icon->hotspotPos.y || icon->hotspotBottom < y) {
                inRect = 0;
            } else {
                inRect = 1;
            }
            if (inRect != 0) {
                return i;
            }
        }
        pIcon = pIcon + 1;
        i = i + 1;
        if (i >= 15) {
            return -1;
        }
    } while (1);
}

/* Function start: 0x402100 */
void SCI_IconBarModule::UpdateCursor() {
    int mouseX;
    int mouseY;
    int iconIdx;
    int hotspot;

    InputState* pMouse = g_InputManager_0046aa08->pMouse;
    mouseY = 0;
    if (pMouse != 0) {
        mouseY = pMouse->y;
    }
    if (pMouse != 0) {
        mouseX = pMouse->x;
    } else {
        mouseX = 0;
    }

    iconIdx = FindClickedIcon(mouseX, mouseY);

    if (g_SelectedItem_0046a6e4 != 0) {
        hotspot = g_SelectedItem_0046a6e4->itemId + 0x1d;
        if (g_Mouse_0046aa18->m_sprite == 0) {
            goto done;
        }
        g_Mouse_0046aa18->m_sprite->ResetAnimation(hotspot, 0);
    } else if (iconIdx != -1) {
        Sprite* icon = (Sprite*)icons[iconIdx];
        int state = 0;
        int frameIdx = icon->handle;
        Animation* anim = *(Animation**)((char*)icon + 0xb4 + frameIdx * 4);
        if (anim != 0) {
            state = *(int*)((char*)anim + 0xb0);
        }
        if (g_Mouse_0046aa18->m_sprite == 0) {
            goto done;
        }
        g_Mouse_0046aa18->m_sprite->ResetAnimation(state, 0);
    } else {
        if (g_Mouse_0046aa18->m_sprite == 0) {
            goto done;
        }
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0, 0);
    }

done:
    g_Mouse_0046aa18->DrawCursor();
}

/* Function start: 0x4021C0 */
void SCI_IconBarModule::Serialize(void* param) {
    int strLen;
    FILE* fp;
    int id;

    strLen = strlen("INVENTORY_INFO") + 1;
    fp = *(FILE**)((char*)param + 0x44);

    if (*(int*)param != 0) {
        /* SAVE */
        id = handlerId;
        fwrite(&id, 4, 1, fp);
        fwrite("INVENTORY_INFO", strLen, 1, fp);
        fwrite(g_StateString_0046aa2c, 0x40, 1, fp);
    } else {
        /* LOAD */
        *g_Buffer_0046aa00 = 0;
        fread(g_Buffer_0046aa00, strLen, 1, fp);

        if (strcmp(g_Buffer_0046aa00, "INVENTORY_INFO") != 0) {
            ShowError("SCI_SearchScreen::Serialize() - Error Loading (Wrong ID '%s')", g_Buffer_0046aa00);
        }
        fread(g_StateString_0046aa2c, 0x40, 1, fp);
    }
}

/* Function start: 0x4022B0 */
int SCI_IconBarModule::LBLParse(char* line) {
    char formatted[128];
    int avail_p4;
    int sound;
    char label[32];
    int rectLeft;
    int rectTop;
    int rectRight;
    int rectBottom;
    int room;
    int hotspotIdx;

    sscanf(line, "%s", label);

    if (strcmp(label, "PALE") == 0) {
        sscanf(line, "%s %s", label, formatted);
        if (field_E4 != 0) {
            field_E4->~Palette();
            FreeMemory(field_E4);
            field_E4 = 0;
        }
        field_E4 = new Palette(formatted);
    } else if (strcmp(label, "AMBIENT") == 0) {
        if (mmPlayer == 0) {
            mmPlayer = new MMPlayer();
        }
        Parser::ProcessFile((Parser*)mmPlayer, this, 0);
    } else if (strcmp(label, "HOTSPOT") == 0) {
        sscanf(line, "%s %d", label, &hotspotIdx);
        if (hotspotIdx >= 15) {
            ShowError("Error in SCIsrcSc.cpp - LBLParse: Too many hotspots");
        }
        if (icons[hotspotIdx] != 0) {
            ShowError("repeat use of hotspot %d");
        }
        T_Hotspot* hs = new T_Hotspot(0);
        icons[hotspotIdx] = hs;
        hs->parentHandlerId = handlerId;
        hs->parentModuleParam = moduleParam;
        hs->hotspotHandle = hotspotIdx;
        Parser::ProcessFile(hs, this, 0);
    } else if (strcmp(label, "DELETE_HOTSPOT") == 0) {
        int ret = sscanf(line, "%s %d", label, &hotspotIdx);
        if (ret < 2) {
            ReportUnknownLabel("SCI_SearchScreen --> DELETE_HOTSPOT");
        }
        T_Hotspot* hs = icons[hotspotIdx];
        if (hs != 0) {
            hs->~T_Hotspot();
            FreeMemory(hs);
            icons[hotspotIdx] = 0;
        }
    } else if (strcmp(label, "DLG_HOTSPOT") == 0) {
        int ret = sscanf(line, "%s %d %d %d %d %d", label, &room, &rectLeft, &rectTop, &rectRight, &rectBottom);
        if (ret < 6) {
            ReportUnknownLabel("SCI_SearchScreen --> DLG_HOTSPOT");
        }
        if (icons[hotspotIdx] != 0) {
            ShowError("repeat use of hotspot %d");
        }
        T_Hotspot* hs = new T_Hotspot(0);
        icons[room] = hs;
        hs->parentHandlerId = handlerId;
        hs->parentModuleParam = moduleParam;
        hs->hotspotHandle = room;
        hs->hotspotPos.x = rectLeft;
        hs->hotspotPos.y = rectTop;
        hs->hotspotRight = rectRight;
        hs->hotspotBottom = rectBottom;
        sprintf(formatted, "DIALOG %d", hotspotIdx);
        hs->LBLParse(formatted);
        if (room >= 15) {
            ShowError("Error in SCIsrcSc.cpp - LBLParse: Too many hotspots");
        }
    } else if (strcmp(label, "AVAILABLE_HOTSPOT") == 0) {
        int ret = sscanf(line, "%s %d %d %d %d %d %d %d %d", label, &hotspotIdx, &rectRight, &rectTop, &rectLeft, &avail_p4, &room, &rectBottom, &sound);
        if (ret < 8) {
            ReportUnknownLabel("SCI_SearchScreen --> AVAILABLE_HOTSPOT");
        }
        if (icons[hotspotIdx] != 0) {
            ShowError("repeat use of hotspot %d");
        }
        T_Hotspot* hs = new T_Hotspot(0);
        icons[hotspotIdx] = hs;
        hs->parentHandlerId = handlerId;
        hs->parentModuleParam = moduleParam;
        hs->hotspotHandle = hotspotIdx;
        hs->hotspotPos.x = rectRight;
        hs->hotspotPos.y = rectTop;
        hs->hotspotRight = rectLeft;
        hs->hotspotBottom = avail_p4;

        int iGS = g_PeriodStateIdx_0046cb90;
        GameState* gs = g_GameState_0046aa30;
        if (iGS < 0 || gs->maxStates - 1 < iGS) {
            ShowError("Invalid gamestate %d", iGS);
        }
        int iGS2 = g_PeriodStateIdx_0046cb90;
        GameState* gs2 = g_GameState_0046aa30;
        int periodVal = gs->stateValues[iGS];
        char ch;
        if (periodVal == -1) {
            if (g_PeriodStateIdx_0046cb90 < 0 || g_GameState_0046aa30->maxStates - 1 < g_PeriodStateIdx_0046cb90) {
                ShowError("Invalid gamestate %d", g_PeriodStateIdx_0046cb90);
            }
            ch = g_PeriodCharTable_0046cb94[gs2->stateValues[iGS2]];
        } else {
            ch = g_PeriodCharTable_0046cb94[periodVal];
        }
        sprintf(formatted, "%cRM%d", (int)ch, room);
        gs = g_GameState_0046aa30;
        int stateIdx = g_GameState_0046aa30->FindLabel(formatted);
        if (stateIdx < 0 || gs->maxStates - 1 < stateIdx) {
            ShowError("Invalid gamestate %d", stateIdx);
        }
        if (gs->stateValues[stateIdx] != 0) {
            sprintf(formatted, "SWITCHROOM %d %d", room, rectBottom);
            hs->LBLParse(formatted);
        } else {
            sprintf(formatted, "PLAYSOUND %d", sound);
            hs->LBLParse(formatted);
        }
    } else if (strcmp(label, "MESSAGE") == 0) {
        if (field_128 == 0) {
            field_128 = new Queue(4);
        }
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        action->fromType = handlerId;
        action->fromValue = moduleParam;
        ParseSpriteAction(action, this);
        field_128->PushNode(action);
    } else if (strcmp(label, "INTRO_HOTSPOT") == 0) {
        sscanf(line, "%s %d", label, &hotspotIdx);
        if (icons[hotspotIdx] != 0) {
            ShowError("repeat use of hotspot %d");
        }
        T_Hotspot* hs = new T_Hotspot(0);
        icons[hotspotIdx] = hs;
        hs->parentHandlerId = handlerId;
        hs->parentModuleParam = moduleParam;
        hs->hotspotHandle = hotspotIdx;
        hs->hotspotPos.x = -1;
        hs->hotspotPos.y = -1;
        hs->hotspotRight = -50;
        hs->hotspotBottom = -50;
        hs->LBLParse("DIALOG 10");

        if (field_128 == 0) {
            field_128 = new Queue(4);
        }
        SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        action->fromType = handlerId;
        action->fromValue = moduleParam;
        ParseSpriteAction(action, this);
        field_128->PushNode(action);
    } else if (strcmp(label, "LATE_HOTSPOT") == 0) {
        sscanf(line, "%s %d", label, &hotspotIdx);
        GameState* gs = g_GameState_0046aa30;
        int wentIdx = g_GameState_0046aa30->FindLabel("WENT_TO_CLASS");
        if (wentIdx < 0 || gs->maxStates - 1 < wentIdx) {
            ShowError("Invalid gamestate %d", wentIdx);
        }
        GameState* gs2 = g_GameState_0046aa30;
        if (gs->stateValues[wentIdx] == 0) {
            int actIdx = g_GameState_0046aa30->FindLabel("NUM_ACTIONS");
            if (actIdx < 0 || gs2->maxStates - 1 < actIdx) {
                ShowError("Invalid gamestate %d", actIdx);
            }
            if (gs2->stateValues[actIdx] > 10) {
                if (icons[hotspotIdx] != 0) {
                    ShowError("repeat use of hotspot %d");
                }
                T_Hotspot* hs = new T_Hotspot(0);
                icons[hotspotIdx] = hs;
                hs->parentHandlerId = handlerId;
                hs->parentModuleParam = moduleParam;
                hs->hotspotHandle = hotspotIdx;
                hs->hotspotPos.x = -1;
                hs->hotspotPos.y = -1;
                hs->hotspotRight = -50;
                hs->hotspotBottom = -50;
                hs->LBLParse("DIALOG 10");

                if (field_128 == 0) {
                    field_128 = new Queue(4);
                }
                SpriteAction* action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                action->fromType = handlerId;
                action->fromValue = moduleParam;
                ParseSpriteAction(action, this);
                field_128->PushNode(action);
            }
        }
    } else if (strcmp(label, "BACKGROUNDSND") == 0) {
        sscanf(line, "%s %d", label, &exitTarget);
    } else if (strcmp(label, "SKIP_PERIOD_PARSE") == 0) {
        staticSceneFound = 1;
    } else if (strcmp(label, "FORCE_PARSE") == 0) {
        roomInitialized = 0;
    } else if (strcmp(label, "NOICONBAR") == 0) {
        mode = 1;
    } else if (strcmp(label, "NOACTIONINC") == 0) {
        skipActionsCount = 1;
    } else if (strcmp(label, "MERCYBUTTON") == 0) {
        hasBoundaryRect = 1;
        Sprite* spr = new Sprite("elements\\study.smk");
        boundRect.left = 0x203;
        boundRect.right = 0x267;
        boundRect.bottom = 0x34;
        boundRect.top = 10;
        spr->loc_x = 0x12;
        spr->loc_y = 10;
        spr->flags |= 0x40;
        spr->priority = 1000;
        if (mmPlayer == 0) {
            mmPlayer = new MMPlayer();
        }
        mmPlayer->AddSprite(spr);
    } else if (strcmp(label, "NO_CHOICEMENU") == 0) {
        hasBoundaryRect = 1;
        boundRect.left = -1;
        boundRect.top = -1;
        boundRect.right = -1;
        boundRect.bottom = -1;
    } else if (strcmp(label, "VIDEO_RES") == 0) {
        sscanf(line, " %s %d %d ", label, &videoDim.x, &videoDim.y);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("SCI_SearchScreen");
    }
    return 0;
}
