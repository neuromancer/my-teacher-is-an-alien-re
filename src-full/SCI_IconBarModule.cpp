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
extern "C" void SetVideoRes(int, int);
extern "C" void WriteToLog(const char*, ...);
#include "ZBufferManager.h"
#include "Palette.h"
#include "SpriteAction.h"

/* Function start: 0x401000 */
SCI_IconBarModule::SCI_IconBarModule() {
    memset(&field_A8, 0, 0x22 * 4);
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
    Sprite** p;
    int i;
    void* data;

    if (field_E4 != 0) {
        delete (Palette*)field_E4;
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
            delete (T_Hotspot*)*p;
            *p = 0;
        }
        p++;
        i--;
    } while (i != 0);

    LinkedList* list = (LinkedList*)field_128;
    if (list != 0 && list->head != 0) {
        list->current = list->head;
        while (list->head != 0) {
            data = list->RemoveCurrent();
            if (data != 0) {
                delete (SpriteAction*)data;
            }
        }
    }

    {
        Queue* qlist = (Queue*)field_128;
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
    int* msgData;
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

    msgData = (int*)msg;
    if (msgData[5] != 0 && targetRoom != msgData[5]) {
        char* roomName = g_GameState2_0046aa3c->GetState(msgData[5]);
        strcpy(g_StateString_0046aa2c, roomName);
        changed = 1;
        targetRoom = msgData[5];
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
    if (targetRoom != field_B0) {
        changed = 1;

        int prevIdx = g_GameState_0046aa30->FindState("PREVIOUSROOM");
        if (prevIdx < 0 || g_GameState_0046aa30->maxStates - 1 < prevIdx) {
            ShowError("Invalid gamestate %d", prevIdx);
        }
        g_GameState_0046aa30->stateValues[prevIdx] = field_B0;

        int prevInstIdx = g_GameState_0046aa30->FindState("PREVIOUSROOMINSTANCE");
        if (prevInstIdx < 0 || g_GameState_0046aa30->maxStates - 1 < prevInstIdx) {
            ShowError("Invalid gamestate %d", prevInstIdx);
        }
        g_GameState_0046aa30->stateValues[prevInstIdx] = moduleParam;

        int roomInstIdx = g_GameState_0046aa30->FindState("ROOMINSTANCE");
        if (roomInstIdx < 0 || g_GameState_0046aa30->maxStates - 1 < roomInstIdx) {
            ShowError("Invalid gamestate %d", roomInstIdx);
        }
        g_GameState_0046aa30->stateValues[roomInstIdx] = targetRoom;
        field_B0 = targetRoom;

        char roomPath[60];
        sprintf(roomPath, "room%d", targetRoom);
        if (FileExists(roomPath) == 0) {
            // CDData asset pre-cache (FUN_00426330)
        }
    }

    // Check moduleParam change
    int newInstance = msgData[1];
    if (moduleParam != newInstance) {
        moduleParam = newInstance;
        changed = 1;
        int roomInstIdx = g_GameState_0046aa30->FindState("ROOMINSTANCE");
        if (roomInstIdx < 0 || g_GameState_0046aa30->maxStates - 1 < roomInstIdx) {
            ShowError("Invalid gamestate %d", roomInstIdx);
        }
        g_GameState_0046aa30->stateValues[roomInstIdx] = newInstance;
    }

    if (changed != 0 || field_C0 == 0) {
        field_A8 = 0;
        hasBoundaryRect = 0;
        skipActionsCount = 0;
        field_C0 = 1;

        // Cleanup Palette
        if (field_E4 != 0) {
            delete (Palette*)field_E4;
            field_E4 = 0;
        }

        // Cleanup MMPlayer
        if (mmPlayer != 0) {
            delete mmPlayer;
            mmPlayer = 0;
        }

        // Cleanup icons
        int count = 15;
        Sprite** p = icons;
        do {
            if (*p != 0) {
                delete (T_Hotspot*)*p;
                *p = 0;
            }
            p++;
            count--;
        } while (count != 0);

        // Cleanup action list
        if (field_128 != 0) {
            LinkedList* list = (LinkedList*)field_128;
            if (list->head != 0) {
                list->current = list->head;
                while (list->head != 0) {
                    void* data = list->RemoveCurrent();
                    if (data != 0) {
                        delete (SpriteAction*)data;
                    }
                }
            }
            FreeMemory(list);
            field_128 = 0;
        }

        field_BC = 0;
        mode = 0;
        field_C8.x = 0x280;
        field_C8.y = 0x1E0;

        // Parse static scene
        sprintf(key, "[SEARCHSCREEN%d_STATIC]", moduleParam);
        ParseFile(this, g_StateString_0046aa2c, key);

        // Parse period-specific scene if no static override
        if (field_BC == 0) {
            periodIdx = g_GameState_0046aa30->FindState("PERIOD");
            if (periodIdx < 0 || g_GameState_0046aa30->maxStates - 1 < periodIdx) {
                ShowError("Invalid gamestate %d", periodIdx);
            }
            sprintf(key, "[SEARCHSCREEN%d_PERIOD%2.2d]", moduleParam,
                    g_GameState_0046aa30->stateValues[periodIdx]);
            ParseFile(this, g_StateString_0046aa2c, key);
            field_BC = 0;
        }

        // Dispatch queued actions
        if (field_128 != 0) {
            LinkedList* list = (LinkedList*)field_128;
            list->current = list->head;
            if (list->head != 0) {
                do {
                    void* nodeData = 0;
                    if (list->current != 0) {
                        nodeData = list->current->data;
                    }
                    int* actionData = (int*)nodeData;
                    if (actionData[2] == 0 ||
                        (actionData[2] == msgData[2] &&
                         (actionData[3] == 0 ||
                          (actionData[2] == msgData[2] && msgData[3] == actionData[3])))) {
                        EnqueueSpriteAction((void*)actionData[0xD]);
                    }
                    if (list->tail == list->current) break;
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->head != 0);
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

        SetVideoRes(field_C8.x, field_C8.y);

        // ZBufferManager queue cleanup
        if (g_ZBufferManager_0046aa24->m_state != 2) {
            g_ZBufferManager_0046aa24->m_state = 2;

            ZBQueue* queue = g_ZBufferManager_0046aa24->m_queueA0;
            if (queue->head != 0) {
                queue->current = queue->head;
                while (queue->head != 0) {
                    void* item = queue->RemoveCurrent();
                    if (item != 0) {
                        
                        FreeMemory(item);
                    }
                }
            }

            queue = g_ZBufferManager_0046aa24->m_queueA4;
            if (queue->head != 0) {
                queue->current = queue->head;
                while (queue->head != 0) {
                    void* item = queue->RemoveCurrent();
                    if (item != 0) {
                        // DrawEntry cleanup
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
    SendGameMessage(5, field_A8, 0, 0, 0x1b, 0, 0, 0, 0, 0);

    // Apply palette if set
    if (field_E4 != 0) {
        if (g_ZBufferManager_0046aa24->m_palette != 0) {
            WriteToLog("ddouble palette");
        }
        g_ZBufferManager_0046aa24->m_palette = (Palette*)field_E4;
    }
}


/* Function start: 0x401CF0 */
int SCI_IconBarModule::ShutDown(SC_Message* msg) {
    Sprite** pIcon;
    int i;

    if (msg == 0) {
        if (mmPlayer != 0) {
            mmPlayer->StopAll();
        }
    } else {
        if (*(int*)msg == 0x1f) {
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
            ((T_Hotspot*)*pIcon)->StopAll();
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
    Sprite** pIcon;
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
            ((T_Hotspot*)*pIcon)->DoItem(spriteParam);
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

    if (((int*)msg)[0xb] != 0) {
        return 1;
    }

    if (((int*)msg)[9] >= 2) {
        iconIdx = FindClickedIcon(((int*)msg)[7], ((int*)msg)[8]);
        if (iconIdx != -1) {
            ((int*)msg)[5] = iconIdx;
            if (((int*)msg)[9] == 2) {
                ((int*)msg)[4] = 2;
            } else if (((int*)msg)[9] == 3) {
                ((int*)msg)[4] = 3;
            }

            if (g_SelectedItem_0046a6e4 != 0) {
                ((int*)msg)[6] = ((T_Object*)g_SelectedItem_0046a6e4)->itemId;
            } else {
                ((int*)msg)[6] = 0;
            }

            if (skipActionsCount == 0) {
                idx = g_GameState_0046aa30->FindLabel("NUM_ACTIONS");
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                g_GameState_0046aa30->stateValues[idx]++;
            }

            Exit(msg);

            *(int*)msg = 0;
        }

        if (hasBoundaryRect != 0) {
            int mx = ((int*)msg)[7];
            int my = ((int*)msg)[8];
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
    } else if (((int*)msg)[10] >= 2) {
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
    Sprite** pIcon;
    Sprite* icon;
    int inRect;

    i = 0;
    pIcon = icons;
    do {
        icon = *pIcon;
        if (icon != 0) {
            if (x < icon->field_0xa4 || icon->loc_x < x ||
                y < icon->num_logic_conditions || icon->loc_y < y) {
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
        hotspot = ((T_Object*)g_SelectedItem_0046a6e4)->itemId + 0x1d;
        if (g_Mouse_0046aa18->m_sprite == 0) {
            goto done;
        }
        g_Mouse_0046aa18->m_sprite->ResetAnimation(hotspot, 0);
    } else if (iconIdx != -1) {
        Sprite* icon = icons[iconIdx];
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
