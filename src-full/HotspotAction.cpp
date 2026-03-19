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
#include <string.h>
#include <stdio.h>

extern "C" void ShowError(const char* format, ...);

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

extern "C" extern GameState* DAT_0046aa30;
class MouseControl;
extern MouseControl* DAT_0046aa18;
extern void* DAT_0046aa34;
extern GameState* DAT_0046aa38;
extern int DAT_0046cb90;
extern char* DAT_0046cb94;
extern SpriteAction DAT_00472d90;


/* Function start: 0x41B320 */
HotspotAction::HotspotAction(int id) {
    memset(&field_90, 0, 0x80);
    hotspotId = id;
}

/* Function start: 0x41B3A0 */
HotspotAction::~HotspotAction() {
    void* ptr;
    void* item;
    LinkedList* list;

    ptr = (void*)field_F4;
    if (ptr != 0) {
        delete (MMPlayer*)ptr;
        field_F4 = 0;
    }

    ptr = (void*)field_F8;
    if (ptr != 0) {
        delete (MMPlayer*)ptr;
        field_F8 = 0;
    }

    list = queue100;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                item = list->RemoveCurrent();
                if (item != 0) {
                    delete (SpriteAction*)item;
                }
            }
        }
        FreeMemory(list);
        queue100 = 0;
    }

    list = queueFC;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                item = list->RemoveCurrent();
                if (item != 0) {
                    delete (SpriteAction*)item;
                }
            }
        }
        FreeMemory(list);
        queueFC = 0;
    }

    list = queue104;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                item = list->RemoveCurrent();
                if (item != 0) {
                    delete (SpriteAction*)item;
                }
            }
        }
        FreeMemory(list);
        queue104 = 0;
    }

    list = queue108;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                item = list->RemoveCurrent();
                if (item != 0) {
                    delete (SpriteAction*)item;
                }
            }
        }
        FreeMemory(list);
        queue108 = 0;
    }
}

/* Function start: 0x41B5A0 */
void HotspotAction::Reset() {
    field_A8 = 0;
    state = 0;
    if (field_F4 != 0) {
        field_F4->StopAll();
    }
    if (field_F8 != 0) {
        field_F8->StopAll();
    }
}

/* Function start: 0x41B5E0 */
int HotspotAction::HandleMessage(int* msg) {
    int val;

    if (msg[0] != 0x24) {
        return 0;
    }

    val = msg[4];
    switch (val) {
    case 2:
        val = msg[6];
        field_A8 = val;
        if (field_AC == val) {
            ProcessQueueFC();
            if (field_F4 != 0) {
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
        if (field_F8 != 0) {
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
        if (field_F4 == 0) {
            ShowError("illegal player in hotspot %d", hotspotId);
        }
        if (field_F4->Draw() == 0) {
            state = 0;
            ProcessQueue100();
            Reset();
            return 1;
        }
        break;
    case 3:
        if (field_F8 == 0) {
            ShowError("illegal player in hotspot %d", hotspotId);
        }
        if (field_F8->Draw() == 0) {
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
    int* data;
    ListNode* node;

    if (state != 0 && state != 1) {
        return 0;
    }

    result = 1;
    if (queue108 != 0) {
        queue108->current = queue108->head;
        while (queue108->head != 0) {
            node = queue108->current;
            if (node != 0) {
                data = (int*)node->data;
            } else {
                data = 0;
            }
            if (data[0] != 2) goto notType2;
            result = (DAT_0046aa30)->FUN_00433bb0(data);
            goto afterCheck;
notType2:
            ShowError("illegal message 15");
afterCheck:
            if (result == 0) {
                return 0;
            }
            if (queue108->tail == queue108->current) {
                return 1;
            }
            if (queue108->current != 0) {
                queue108->current = queue108->current->next;
            }
        }
    }
    return 1;
}

/* Function start: 0x41B840 */
void HotspotAction::ProcessQueue100() {
    void* data;

    if (queue100 != 0) {
        queue100->current = queue100->head;
        while (*(int*)queue100 != 0) {
            if (queue100->current == 0) {
                data = 0;
            } else {
                data = queue100->current->data;
            }
            EnqueueSpriteAction(data);
            if (queue100->tail == queue100->current) {
                return;
            }
            if (queue100->current != 0) {
                queue100->current = queue100->current->next;
            }
        }
    }
}

/* Function start: 0x41B8A0 */
void HotspotAction::ProcessQueue104() {
    void* data;

    if (queue104 != 0) {
        queue104->current = queue104->head;
        while (*(int*)queue104 != 0) {
            if (queue104->current == 0) {
                data = 0;
            } else {
                data = queue104->current->data;
            }
            EnqueueSpriteAction(data);
            if (queue104->tail == queue104->current) {
                return;
            }
            if (queue104->current != 0) {
                queue104->current = queue104->current->next;
            }
        }
    }
}

/* Function start: 0x41B900 */
void HotspotAction::ProcessQueueFC() {
    void* data;

    if (queueFC != 0) {
        queueFC->current = queueFC->head;
        while (*(int*)queueFC != 0) {
            if (queueFC->current == 0) {
                data = 0;
            } else {
                data = queueFC->current->data;
            }
            EnqueueSpriteAction(data);
            if (queueFC->tail == queueFC->current) {
                return;
            }
            if (queueFC->current != 0) {
                queueFC->current = queueFC->current->next;
            }
        }
    }
}

/* Function start: 0x41B960 */
/* Function start: 0x41B993 */
int HotspotAction::LBLParse(char* line) {
    char label[64];
    char buf_C0[128];
    char buf_140[128];
    char local_40[16];
    int local_14;
    int local_38;
    int result;
    SpriteAction* sa;
    LinkedList* list;

    buf_C0[0] = 0;
    buf_140[0] = 0;
    label[0] = 0;

    sscanf(line, " %s ", label);

    if (strcmp(label, "HANDLE") == 0) {
        sscanf(line, " %s %d", label, &hotspotId);
    }
    else if (strcmp(label, "ROLLOVER") == 0) {
        sscanf(line, " %s %s", label, buf_C0);
        local_14 = ((GameState*)DAT_0046aa18)->FUN_00432e20(buf_C0);
        field_B0 = local_14;
        ExtractQuotedString(line, field_B4, 0x40);
    }
    else if (strcmp(label, "CORRECT") == 0) {
        sscanf(line, " %s ", label);
        if (field_F4 == 0) {
            field_F4 = new MMPlayer();
        }
        Parser::ProcessFile(field_F4, this, 0);
    }
    else if (strcmp(label, "INCORRECT") == 0) {
        sscanf(line, " %s ", label);
        if (field_F8 == 0) {
            field_F8 = new MMPlayer();
        }
        Parser::ProcessFile(field_F8, this, 0);
    }
    else if (strcmp(label, "INC_ACTIONS") == 0) {
        if (queueFC == 0) {
            queueFC = new LinkedList();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->addressType = 2;
        sa->addressValue = (DAT_0046aa30)->FindState("PERIOD");
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        sa->instruction = 0x11;
        list = queueFC;
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
        if (queue100 == 0) {
            queue100 = new LinkedList();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        ParseSpriteAction(sa, this);
        list = queue100;
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
        if (queue104 == 0) {
            queue104 = new LinkedList();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        ParseSpriteAction(sa, this);
        list = queue104;
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
        if (queueFC == 0) {
            queueFC = new LinkedList();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        ParseSpriteAction(sa, this);
        list = queueFC;
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
        if (queue108 == 0) {
            queue108 = new LinkedList();
        }
        sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sa->fromType = 0x24;
        sa->fromValue = hotspotId;
        ParseSpriteAction(sa, this);
        list = queue108;
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
        if (queue100 == 0) {
            queue100 = new LinkedList();
        }
        sa = new SpriteAction(
            ((GameState*)DAT_0046aa34)->FindState(buf_C0),
            local_14, field_90, field_94, 4, 0, local_14, 0, 0, 0);
        list = queue100;
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
        if (queue100 == 0) {
            queue100 = new LinkedList();
        }
        sa = new SpriteAction();
        memset(sa, 0, sizeof(SpriteAction));
        sa->CopyFrom(&DAT_00472d90);
        list = queue100;
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
        if (queue100 == 0) {
            queue100 = new LinkedList();
        }
        sa = new SpriteAction(4, local_14, field_90, field_94, 2, local_14, 0, 0, 0, 0);
        list = queue100;
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
        if (queue100 == 0) {
            queue100 = new LinkedList();
        }
        sa = new SpriteAction(3, local_14, field_90, field_94, 4, local_14, 0, 0, 0, 0);
        list = queue100;
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
            GameState* gs = DAT_0046aa30;
            int idx = DAT_0046cb90;
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            int val = gs->stateValues[idx];
            char c;
            if (val == -1) {
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                c = DAT_0046cb94[gs->stateValues[idx]];
            } else {
                c = DAT_0046cb94[val];
            }
            sprintf(local_40, "%c%s", (int)c, buf_C0);
        }
        if (queue108 == 0) {
            queue108 = new LinkedList();
        }
        sa = new SpriteAction(2,
            (DAT_0046aa30)->FindState(local_40),
            field_90, field_94,
            DAT_0046aa38->FindState(buf_140),
            local_14, 0, 0, 0, 0);
        list = queue108;
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
        if (queue108 == 0) {
            queue108 = new LinkedList();
        }
        sa = new SpriteAction(2,
            (DAT_0046aa30)->FindState(buf_C0),
            0, 0,
            DAT_0046aa38->FindState(buf_140),
            local_14, 0, 0, 0, 0);
        list = queue108;
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
        sscanf(line, " %s %d", label, &field_AC);
    }
    else if (strcmp(label, "GAMESTATE") == 0) {
        result = sscanf(line, " %s %s %s %d", label, buf_C0, buf_140, &local_14);
        if (queue100 == 0) {
            queue100 = new LinkedList();
        }
        sa = new SpriteAction(2,
            (DAT_0046aa30)->FindState(buf_C0),
            0, 0,
            DAT_0046aa38->FindState(buf_140),
            local_14, 0, 0, 0, 0);
        if ((sa->instruction == 0x11 || sa->instruction == 0x12) && result < 4) {
            sa->extra1 = 1;
        }
        if (sa->instruction == 0x11 && result < 4) {
            ReportUnknownLabel("HotspotAction");
        }
        list = queue100;
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
        if (queue100 == 0) {
            queue100 = new LinkedList();
        }
        sa = new SpriteAction(0x20, local_38, field_90, field_94, 4, local_14, 0, 0, 0, 0);
        list = queue100;
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
        if (queue100 == 0) {
            queue100 = new LinkedList();
        }
        sa = new SpriteAction(2,
            (DAT_0046aa30)->FindState("NUM_ACTIONS"),
            field_90, field_94, 0x11, 0, 0, 0, 0, 0);
        list = queue100;
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
        if (queue100 == 0) {
            queue100 = new LinkedList();
        }
        sa = new SpriteAction(0x1e, local_14, field_90, field_94,
            DAT_0046aa38->FindState(buf_C0),
            0, 0, 0, 0, 0);
        list = queue100;
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
