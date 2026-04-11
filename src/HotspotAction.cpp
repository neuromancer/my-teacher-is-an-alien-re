#include "HotspotAction.h"
#include "MouseControl.h"
#include "GameState.h"
#include "MouseControl.h"
#include "SpriteAction.h"
#include "MouseControl.h"
#include "Queue.h"
#include "MouseControl.h"
#include "MMPlayer.h"
#include "MouseControl.h"
#include "Memory.h"
#include "MouseControl.h"
#include "SC_Question.h"
#include "MouseControl.h"
#include "string.h"
#include "MouseControl.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>

extern "C" void ShowError(const char* format, ...);

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);



/* Function start: 0x41B320 */
HotspotAction::HotspotAction(int id) {
    memset(&parentHandlerId, 0, 0x80);
    hotspotId = id;
}

/* Function start: 0x41B3A0 */
HotspotAction::~HotspotAction() {
    SpriteAction* item;
    Queue* list;

    if (correctPlayer != 0) {
        delete correctPlayer;
        correctPlayer = 0;
    }

    if (incorrectPlayer != 0) {
        delete incorrectPlayer;
        incorrectPlayer = 0;
    }

    list = messagesQueue;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                item = (SpriteAction*)list->Pop();
                if (item != 0) {
                    delete item;
                }
            }
        }
        FreeMemory(list);
        messagesQueue = 0;
    }

    list = actionsQueue;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                item = (SpriteAction*)list->Pop();
                if (item != 0) {
                    delete item;
                }
            }
        }
        FreeMemory(list);
        actionsQueue = 0;
    }

    list = incorrectQueue;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                item = (SpriteAction*)list->Pop();
                if (item != 0) {
                    delete item;
                }
            }
        }
        FreeMemory(list);
        incorrectQueue = 0;
    }

    list = conditionsQueue;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                item = (SpriteAction*)list->Pop();
                if (item != 0) {
                    delete item;
                }
            }
        }
        FreeMemory(list);
        conditionsQueue = 0;
    }
}

/* Function start: 0x41B5A0 */
void HotspotAction::Reset() {
    currentSpriteId = 0;
    state = 0;
    if (correctPlayer != 0) {
        correctPlayer->StopAll();
    }
    if (incorrectPlayer != 0) {
        incorrectPlayer->StopAll();
    }
}

/* Function start: 0x41B5E0 */
int HotspotAction::HandleMessage(int* msg) {
    int val;
    SpriteAction* action = (SpriteAction*)msg;

    if (action->addressType != 0x24) {
        return 0;
    }

    val = action->instruction;
    switch (val) {
    case 2:
        val = action->extra2;
        currentSpriteId = val;
        if (checkObjectId == val) {
            ProcessQueueFC();
            if (correctPlayer != 0) {
                state = 2;
                return 1;
            }
            state = 0;
            ProcessQueue100();
            return 1;
        }
        if (val != 0) {
            SendGameMessage(0x1e, val, 0x1e, 0, 6, 0, 0, 0, 0, 0);
            state = 0;
            return 1;
        }
        ProcessQueue104();
        if (incorrectPlayer != 0) {
            state = 3;
            return 1;
        }
        state = 0;
        return 1;
    case 6:
        state = 0;
    case 3:
        return 1;
    }
    return 0;
}

/* Function start: 0x41B6E0 */
int HotspotAction::Update(int param) {
    switch (state) {
    case 2:
        if (correctPlayer == 0) {
            ShowError("illegal player in hotspot %d", hotspotId);
        }
        if (correctPlayer->Draw() == 0) {
            state = 0;
            ProcessQueue100();
            Reset();
            return 1;
        }
        break;
    case 3:
        if (incorrectPlayer == 0) {
            ShowError("illegal player in hotspot %d", hotspotId);
        }
        if (incorrectPlayer->Draw() == 0) {
            state = 0;
            Reset();
        }
        break;
    }
    return 1;
}

/* Function start: 0x41B790 */
int HotspotAction::CheckConditions() {
    int result;
    SpriteAction* data;
    ListNode* node;

    if (state != 0 && state != 1) {
        { FILE* _f=fopen("debug.log","a"); if(_f){fprintf(_f,"CheckCond: state=%d FAIL (not 0 or 1)\n",state);fclose(_f);} }
        return 0;
    }

    result = 1;
    if (conditionsQueue != 0) {
        conditionsQueue->current = conditionsQueue->head;
        while (conditionsQueue->head != 0) {
            node = conditionsQueue->current;
            if (node != 0) {
                data = (SpriteAction*)node->data;
            } else {
                data = 0;
            }
            if (data->addressType != 2) goto notType2;
            result = (g_GameState_0046aa30)->CheckCondition((int*)data);
            goto afterCheck;
notType2:
            ShowError("illegal message 15");
afterCheck:
            if (result == 0) {
                { FILE* _f=fopen("debug.log","a"); if(_f){fprintf(_f,"CheckCond: condition FAIL addr=%d val=%d inst=%d\n",data->addressType,data->addressValue,data->instruction);fclose(_f);} }
                return 0;
            }
            if (conditionsQueue->tail == conditionsQueue->current) {
                return 1;
            }
            if (conditionsQueue->current != 0) {
                conditionsQueue->current = conditionsQueue->current->next;
            }
        }
    }
    return 1;
}

/* Function start: 0x41B840 */
void HotspotAction::ProcessQueue100() {
    void* data;

    if (messagesQueue != 0) {
        messagesQueue->current = messagesQueue->head;
        while (*(int*)messagesQueue != 0) {
            if (messagesQueue->current == 0) {
                data = 0;
            } else {
                data = messagesQueue->current->data;
            }
            EnqueueSpriteAction(data);
            if (messagesQueue->tail == messagesQueue->current) {
                return;
            }
            if (messagesQueue->current != 0) {
                messagesQueue->current = messagesQueue->current->next;
            }
        }
    }
}

/* Function start: 0x41B8A0 */
void HotspotAction::ProcessQueue104() {
    void* data;

    if (incorrectQueue != 0) {
        incorrectQueue->current = incorrectQueue->head;
        while (*(int*)incorrectQueue != 0) {
            if (incorrectQueue->current == 0) {
                data = 0;
            } else {
                data = incorrectQueue->current->data;
            }
            EnqueueSpriteAction(data);
            if (incorrectQueue->tail == incorrectQueue->current) {
                return;
            }
            if (incorrectQueue->current != 0) {
                incorrectQueue->current = incorrectQueue->current->next;
            }
        }
    }
}

/* Function start: 0x41B900 */
void HotspotAction::ProcessQueueFC() {
    void* data;

    if (actionsQueue != 0) {
        actionsQueue->current = actionsQueue->head;
        while (*(int*)actionsQueue != 0) {
            if (actionsQueue->current == 0) {
                data = 0;
            } else {
                data = actionsQueue->current->data;
            }
            EnqueueSpriteAction(data);
            if (actionsQueue->tail == actionsQueue->current) {
                return;
            }
            if (actionsQueue->current != 0) {
                actionsQueue->current = actionsQueue->current->next;
            }
        }
    }
}

/* Function start: 0x41B993 */
int HotspotAction::LBLParse(char* line) { // prologue at 0x41B960
    char label[64];
    char buf_C0[128];
    char buf_140[128];
    char local_40[16];
    int local_14;
    int local_38;
    int result;
    SpriteAction* sa;
    Queue* list;

    buf_C0[0] = 0;
    buf_140[0] = 0;
    label[0] = 0;

    sscanf(line, " %s ", label);

    if (strcmp(label, "HANDLE") == 0) {
        sscanf(line, " %s %d", label, &hotspotId);
    }
    else if (strcmp(label, "ROLLOVER") == 0) {
        sscanf(line, " %s %s", label, buf_C0);
        local_14 = ((GameState*)g_Mouse_0046aa18)->FindStateByName(buf_C0);
        rolloverStateIdx = local_14;
        ExtractQuotedString(line, rolloverText, 0x40);
    }
    else if (strcmp(label, "CORRECT") == 0) {
        sscanf(line, " %s ", label);
        if (correctPlayer == 0) {
            correctPlayer = new MMPlayer();
        }
        Parser::ProcessFile(correctPlayer, this, 0);
    }
    else if (strcmp(label, "INCORRECT") == 0) {
        sscanf(line, " %s ", label);
        if (incorrectPlayer == 0) {
            incorrectPlayer = new MMPlayer();
        }
        Parser::ProcessFile(incorrectPlayer, this, 0);
    }
    else if (strcmp(label, "INC_ACTIONS") == 0) {
        if (actionsQueue == 0) {
            actionsQueue = new Queue();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->addressType = 2;
        sa->addressValue = (g_GameState_0046aa30)->FindState("PERIOD");
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        sa->instruction = 0x11;
        list = actionsQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "MESSAGE") == 0) {
        if (messagesQueue == 0) {
            messagesQueue = new Queue();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        ParseSpriteAction(sa, this);
        list = messagesQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "INCORRECTMESSAGE") == 0) {
        if (incorrectQueue == 0) {
            incorrectQueue = new Queue();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        ParseSpriteAction(sa, this);
        list = incorrectQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "ACTIONS") == 0) {
        if (actionsQueue == 0) {
            actionsQueue = new Queue();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        ParseSpriteAction(sa, this);
        list = actionsQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "CHECKMSG") == 0) {
        if (conditionsQueue == 0) {
            conditionsQueue = new Queue();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        ParseSpriteAction(sa, this);
        list = conditionsQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "SWITCHFOCUS") == 0) {
        sscanf(line, " %s %s %d", label, buf_C0, &local_14);
        if (messagesQueue == 0) {
            messagesQueue = new Queue();
        }
        sa = new SpriteAction(
            g_StringTable_0046aa34->FindState(buf_C0),
            local_14, parentHandlerId, parentModuleParam, 4, 0, local_14, 0, 0, 0);
        list = messagesQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "SWITCHPREVIOUSROOM") == 0) {
        if (messagesQueue == 0) {
            messagesQueue = new Queue();
        }
        sa = new SpriteAction();
        memset(sa, 0, sizeof(SpriteAction));
        sa->CopyFrom(&g_HotspotAction_00472d90);
        list = messagesQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "PLAYSOUND") == 0) {
        sscanf(line, " %s %d", label, &local_14);
        if (messagesQueue == 0) {
            messagesQueue = new Queue();
        }
        sa = new SpriteAction(4, local_14, parentHandlerId, parentModuleParam, 2, local_14, 0, 0, 0, 0);
        list = messagesQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "PLAYCINEMATIC") == 0) {
        sscanf(line, " %s %d", label, &local_14);
        if (messagesQueue == 0) {
            messagesQueue = new Queue();
        }
        sa = new SpriteAction(3, local_14, parentHandlerId, parentModuleParam, 4, local_14, 0, 0, 0, 0);
        list = messagesQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "CHECKAVAILIABLE") == 0) {
        result = sscanf(line, " %s %s %s %d", label, buf_C0, buf_140, &local_14);
        if (result < 4) {
            local_14 = 0;
        }
        {
            GameState* gs = g_GameState_0046aa30;
            int idx = g_PeriodStateIdx_0046cb90;
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            int val = gs->stateValues[idx];
            char c;
            if (val == -1) {
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                c = g_PeriodCharTable_0046cb94[gs->stateValues[idx]];
            } else {
                c = g_PeriodCharTable_0046cb94[val];
            }
            sprintf(local_40, "%c%s", (int)c, buf_C0);
        }
        if (conditionsQueue == 0) {
            conditionsQueue = new Queue();
        }
        sa = new SpriteAction(2,
            (g_GameState_0046aa30)->FindState(local_40),
            parentHandlerId, parentModuleParam,
            g_StringState_0046aa38->FindState(buf_140),
            local_14, 0, 0, 0, 0);
        list = conditionsQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "CHECKGAMESTATE") == 0) {
        result = sscanf(line, " %s %s %s %d", label, buf_C0, buf_140, &local_14);
        if (result < 4) {
            local_14 = 0;
        }
        if (conditionsQueue == 0) {
            conditionsQueue = new Queue();
        }
        sa = new SpriteAction(2,
            (g_GameState_0046aa30)->FindState(buf_C0),
            0, 0,
            g_StringState_0046aa38->FindState(buf_140),
            local_14, 0, 0, 0, 0);
        list = conditionsQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "CHECKOBJECT") == 0) {
        sscanf(line, " %s %d", label, &checkObjectId);
    }
    else if (strcmp(label, "GAMESTATE") == 0) {
        result = sscanf(line, " %s %s %s %d", label, buf_C0, buf_140, &local_14);
        if (messagesQueue == 0) {
            messagesQueue = new Queue();
        }
        sa = new SpriteAction(2,
            (g_GameState_0046aa30)->FindState(buf_C0),
            0, 0,
            g_StringState_0046aa38->FindState(buf_140),
            local_14, 0, 0, 0, 0);
        if ((sa->instruction == 0x11 || sa->instruction == 0x12) && result < 4) {
            sa->extra1 = 1;
        }
        if (sa->instruction == 0x11 && result < 4) {
            ReportUnknownLabel("HotspotAction");
        }
        list = messagesQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "SWITCHROOM") == 0) {
        result = sscanf(line, " %s %d %d", label, &local_14, &local_38);
        if (result != 3) {
            ShowError("Error in ThotsLvl.cpp %s in parameter", line);
        }
        if (messagesQueue == 0) {
            messagesQueue = new Queue();
        }
        sa = new SpriteAction(0x20, local_38, parentHandlerId, parentModuleParam, 4, local_14, 0, 0, 0, 0);
        list = messagesQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        list->PushNode(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "ACTION") == 0) {
        if (messagesQueue == 0) {
            messagesQueue = new Queue();
        }
        sa = new SpriteAction(2,
            (g_GameState_0046aa30)->FindState("NUM_ACTIONS"),
            parentHandlerId, parentModuleParam, 0x11, 0, 0, 0, 0, 0);
        list = messagesQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        ((Queue*)list)->Push(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "OBJECT") == 0) {
        sscanf(line, " %s %d %s", label, &local_14, buf_C0);
        if (messagesQueue == 0) {
            messagesQueue = new Queue();
        }
        sa = new SpriteAction(0x1e, local_14, parentHandlerId, parentModuleParam,
            g_StringState_0046aa38->FindState(buf_C0),
            0, 0, 0, 0, 0);
        list = messagesQueue;
        list->ResetForSortedAdd(sa);
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent(sa);
            } else {
                do {
                    if (*(int*)list->current->data < sa->addressType) {
                        ((Queue*)list)->InsertAtCurrent(sa);
                        break;
                    }
                    if (list->tail == list->current) {
                        ((Queue*)list)->Push(sa);
                        break;
                    }
                    if (list->current != 0) {
                        list->current = list->current->next;
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent(sa);
        }
    }
    else if (strcmp(label, "END") == 0) {
        return 1;
    }
    else {
        ReportUnknownLabel("HotspotAction");
    }

    return 0;
}
