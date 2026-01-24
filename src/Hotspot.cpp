#include "Hotspot.h"
#include "string.h"
#include "Message.h"
#include "Memory.h"
#include "Sprite.h"
#include "MouseControl.h"
#include "TimedEvent.h"
#include "globals.h"
#include "string.h"
#include <stdio.h>
#include <string.h>

// Helper for Miles Sound System
typedef void* HSAMPLE;
extern "C" int __stdcall AIL_sample_status(HSAMPLE);


/* Function start: 0x409230 */
T_Hotspot::T_Hotspot()
{
    int* ptr;
    int i;

    field_13C = 0;
    rect_x = 0;
    rect_y = 0;
    rect_w = 0;
    rect_h = 0;

    ptr = (int*)&sprite;
    for (i = 0x2e; i != 0; i--) {
        *ptr = 0;
        ptr++;
    }
    enabled = 1;
    state = 1;
    dialogParseFileNumber = -1;
    parseFileIndex = -1;
}

/* Function start: 0x4092E0 */
T_Hotspot::~T_Hotspot()
{
    Sprite* spr;
    MouseControl* mc;

    spr = sprite;
    if (spr != 0) {
        delete spr;
        sprite = 0;
    }

    mc = list1;
    if (mc != 0) {
        delete mc;
        list1 = 0;
    }

    mc = list2;
    if (mc != 0) {
        delete mc;
        list2 = 0;
    }

    mc = list3;
    if (mc != 0) {
        delete mc;
        list3 = 0;
    }
}

// Wrapper for SCI_AfterSchoolMenu compatibility (SEH handler often needs a simpler entry)
void __fastcall FUN_004092e0(void* obj)
{
    if (obj) {
        ((T_Hotspot*)obj)->~T_Hotspot();
    }
}

/* Function start: 0x409400 */
unsigned char T_Hotspot::Do()
{
    if (sprite != 0) {
        return sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
    }
    ShowError("Missing sprite in Hotspot Do()");
    return 0;
}

/* Function start: 0x409440 */
int T_Hotspot::SetState(int newState)
{
    if (sprite != 0) {
        sprite->SetState2(newState);
        return 1;
    }
    ShowError("Error in T_Hotspot::Set_State");
    return 0;
}

/* Function start: 0x409470 */
int T_Hotspot::GetState()
{
    if (sprite == 0) {
        ShowError("Error in T_Hotspot::Get_State");
        return 0;
    }
    return sprite->current_state;
}

/* Function start: 0x4094A0 */
void T_Hotspot::Exit()
{
    if (sprite != 0) {
        sprite->StopAnimationSound();
    }
    if (list1 != 0) {
        list1->StopAll();
    }
    if (list2 != 0) {
        list2->StopAll();
    }
    if (list3 != 0) {
        list3->StopAll();
    }
}

/* Function start: 0x4094F0 */
int T_Hotspot::Update(int param_1, int param_2, int param_3)
{
    int iVar1;
    int* puVar2;

    switch (state) {
    case 1:
        if ((list1 == 0) || (iVar1 = list1->DoAll(), iVar1 != 0)) {
            state = 2;
        }
        if (state != 2) {
            return 0;
        }
    case 2:
        if (list2 == 0) {
            state = 3;
            if (dialog != 0) {
                puVar2 = (int*)AllocateMemory(8);
                *puVar2 = param_1;
                puVar2[1] = param_2;
                SC_Message_Send(9, parseFileIndex, 0xb, param_3, 5, dialogParseFileNumber, 0, (int)puVar2, 0, 0);
            }
        }
        else {
            iVar1 = list2->DoAll();
            if (iVar1 != 0) {
                state = 3;
                return 0;
            }
        }
        return 0;
    case 3:
        if (list3 == 0) {
            state = 1;
        }
        else {
            iVar1 = list3->DoAll();
            if (iVar1 != 0) {
                state = 1;
                return 1;
            }
        }
        return 1;
    case 4:
        return 1;
    }
    ShowError("Error in Thotspot.cpp - Update()");
    return 0;
}

/* Function start: 0x409620 */
int T_Hotspot::LBLParse(char* line)
{
    char command[32];
    sscanf(line, " %s ", command);

    if (strcmp(command, "RECT") == 0) {
        sscanf(line, "%*s %d %d %d %d", &rect_x, &rect_y, &rect_w, &rect_h);
    } else if (strcmp(command, "DIALOG") == 0) {
        dialog = 1;
    } else if (strcmp(command, "DIALOGPARSEFILENUMBER") == 0) {
        sscanf(line, "%*s %d", &dialogParseFileNumber);
    } else if (strcmp(command, "PARSEFILEINDEX") == 0) {
        sscanf(line, "%*s %d", &parseFileIndex);
    } else if (strcmp(command, "SPRITE") == 0) {
        Sprite* newSprite = (Sprite*)AllocateMemory(0xd8);
        if (newSprite) {
            newSprite->Sprite::Sprite(NULL);
        }
        sprite = newSprite;
        Parser::ProcessFile(sprite, this, 0);
    } else if (strcmp(command, "LABEL") == 0) {
        sscanf(line, "%*s %s", label);
    } else if (strcmp(command, "MOUSE") == 0) {
        sscanf(line, "%*s %s", mouse);
    } else if (strcmp(command, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("T_Hotspot");
    }
    return 0;
}

/* Function start: 0x40D300 */
Hotspot::Hotspot()
{
    Parser::Parser();
    rect.left = 0;
    rect.top = 0;
    rect.right = 0;
    rect.bottom = 0;
    memset(&hotspot, 0, 104);
    field_D0 = 1;
    state = 1;
}

/* Function start: 0x40D3A0 */
Hotspot::~Hotspot()
{
    if (hotspot) delete hotspot;
    if (right_tool) delete right_tool;
    if (wrong_tool) delete wrong_tool;

    if (pre_message) {
        pre_message->m_current = pre_message->m_head;
        while (pre_message->m_head) {
            QueueNode* node = (QueueNode*)pre_message->m_current;

            if (pre_message->m_head == node) pre_message->m_head = (void*)node->next;
            if (pre_message->m_tail == node) pre_message->m_tail = (void*)node->prev;
            if (node->prev) node->prev->next = node->next;
            if (node->next) node->next->prev = node->prev;

            Message* msg = (Message*)node->data;
            delete node;
            if (msg) delete msg;

            pre_message->m_current = pre_message->m_head;
        }
        FreeMemory(pre_message);
        pre_message = 0;
    }

    if (message) {
        message->m_current = message->m_head;
        while (message->m_head) {
            QueueNode* node = (QueueNode*)message->m_current;

            if (message->m_head == node) message->m_head = (void*)node->next;
            if (message->m_tail == node) message->m_tail = (void*)node->prev;
            if (node->prev) node->prev->next = node->next;
            if (node->next) node->next->prev = node->prev;

            Message* msg = (Message*)node->data;
            delete node;
            if (msg) delete msg;

            message->m_current = message->m_head;
        }
        FreeMemory(message);
        message = 0;
    }
}

/* Function start: 0x40D710 */
int Hotspot::LBLParse(char* line)
{
    char keyword[48];
    sscanf(line, "%s", keyword);

    if (strcmp(keyword, "RECT") == 0) {
        sscanf(line, " %s %d %d %d %d", keyword, &rect.left, &rect.top, &rect.right, &rect.bottom);
    } else if (strcmp(keyword, "HOTSBEGIN") == 0) {
        hotspot = new MouseControl();
        Parser::ProcessFile(hotspot, this, 0);
    } else if (strcmp(keyword, "RIGHT_TOOL") == 0) {
        right_tool = new MouseControl();
        Parser::ProcessFile(right_tool, this, 0);
    } else if (strcmp(keyword, "WRONG_TOOL") == 0) {
        wrong_tool = new MouseControl();
        Parser::ProcessFile(wrong_tool, this, 0);
    } else if (strcmp(keyword, "PREMESSAGE") == 0) {
        if (!pre_message) pre_message = new Queue();
        Message* msg = new Message(); // (0, 0, 0, 0, 0, 0, 0, 0, 0);
        msg->command = 7;
        Parser::ProcessFile((Parser*)msg, this, 0);
        pre_message->Push(msg);
    } else if (strcmp(keyword, "MESSAGE") == 0) {
        if (!message) message = new Queue();
        Message* msg = new Message(); //(0, 0, 0, 0, 0, 0, 0, 0, 0);
        msg->command = 7;
        Parser::ProcessFile((Parser*)msg, this, 0);
        message->Push(msg);
    } else if (strcmp(keyword, "LABEL") == 0) {
        sscanf(line, "%s %s", keyword, label);
    } else if (strcmp(keyword, "MOUSE") == 0) {
        sscanf(line, "%s %d", keyword, &state);
    } else if (strcmp(keyword, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("T_Hotspot");
    }
    return 0;
}

/* Function start: 0x40D610 */
unsigned char Hotspot::Do()
{
    switch (state) {
    case 1:
        QueueEvents(pre_message);
        state = 2;
    case 2:
        if (hotspot == 0 || hotspot->DoAll() == 0) {
            state = 4;
        }
        if (state != 4) return 0;
    case 4:
        QueueEvents(message);
        WriteToMessageLog("\n\n\nJUST SENT MESSAGES\n\n\n");
        return 1;
    }

    ShowError("Error in Thotspot.cpp - Update()");
    return 1;
}

/* Function start: 0x40D6A0 */
void Hotspot::QueueEvents(Queue* q)
{
    if (!q) return;
    q->m_current = q->m_head;
    if (!q->m_head) return;

    do {
        Message* msg = (Message*)((QueueNode*)q->m_current)->data;
        TimedEventPool* pool = g_TimedEventPool2_00436988;
        PooledEvent* evt = pool->Create(pool->list.tail, 0);
        if (evt) {
            //((Message*)((char*)evt + 8))->CopyFrom(msg);
            
            if (pool->list.tail) {
                *(void**)pool->list.tail = evt;
            } else {
                pool->list.head = evt;
            }
            pool->list.tail = evt;
        }

        if (q->m_current == q->m_tail) break;
        q->m_current = ((QueueNode*)q->m_current)->next;
    } while (q->m_head);
}



