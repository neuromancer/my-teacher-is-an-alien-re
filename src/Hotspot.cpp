#include "Hotspot.h"
#include "SC_Question.h"
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
    memset(&sprite, 0, 0x2e * 4);
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
        if (list1 == 0 || list1->DoAll() != 0) {
            state = 2;
        }
        if (state != 2) {
            return 0;
        }
    case 2:
        if (list2 != 0) {
            if (list2->DoAll() != 0) {
                state = 3;
                return 0;
            }
        }
        else {
            state = 3;
            if (dialog != 0) {
                puVar2 = (int*)AllocateMemory(8);
                *puVar2 = param_1;
                puVar2[1] = param_2;
                SC_Message_Send(9, parseFileIndex, 0xb, param_3, 5, dialogParseFileNumber, 0, (int)puVar2, 0, 0);
            }
        }
        return 0;
    case 3:
        if (list3 != 0) {
            if (list3->DoAll() != 0) {
                state = 1;
                return 1;
            }
        }
        else {
            state = 1;
            return 1;
        }
        return 1;
    case 4:
        return 1;
    }
    ShowError("Error in Thotspot.cpp - Update()");
    return 1;
}

/* Function start: 0x409620 */
int T_Hotspot::LBLParse(char* line)
{
    int b, r, t, l;
    char command[32];
    sscanf(line, " %s ", command);

    if (strcmp(command, "RECT") == 0) {
        sscanf(line, " %s %d %d %d %d", command, &l, &t, &r, &b);
        rect.left = l;
        rect.top = t;
        rect.right = r;
        rect.bottom = b;
    } else if (strcmp(command, "DIALOG") == 0) {
        dialog = 1;
    } else if (strcmp(command, "DIALOGPARSEFILENUMBER") == 0) {
        sscanf(line, "%s %d", command, &dialogParseFileNumber);
    } else if (strcmp(command, "PARSEFILEINDEX") == 0) {
        sscanf(line, "%s %d", command, &parseFileIndex);
    } else if (strcmp(command, "SPRITE") == 0) {
        Sprite* spr = new Sprite(NULL);
        sprite = spr;
        Parser::ProcessFile(sprite, this, 0);
    } else if (strcmp(command, "LABEL") == 0) {
        sscanf(line, "%s %s", command, label);
    } else if (strcmp(command, "MOUSE") == 0) {
        sscanf(line, "%s %s", command, label);
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
    memset(&hotspot, 0, 104);
    field_D0 = 1;
    state = 1;
}

/* Function start: 0x40D3A0 */
Hotspot::~Hotspot()
{
    if (hotspot) {
        delete hotspot;
        hotspot = 0;
    }
    if (right_tool) {
        delete right_tool;
        right_tool = 0;
    }
    if (wrong_tool) {
        delete wrong_tool;
        wrong_tool = 0;
    }

    if (pre_message) {
        if (pre_message->m_head) {
            pre_message->m_current = pre_message->m_head;
            do {
                Message* msg = 0;
                QueueNode* node = (QueueNode*)pre_message->m_current;
                if (node) {
                    if (pre_message->m_head == node) pre_message->m_head = (void*)node->next;
                    if (pre_message->m_tail == node) pre_message->m_tail = (void*)node->prev;
                    if (node->prev) ((QueueNode*)node->prev)->next = node->next;
                    if (node->next) ((QueueNode*)node->next)->prev = node->prev;

                    msg = (Message*)pre_message->GetCurrentData();
                    delete node;
                    pre_message->m_current = 0;
                }
                pre_message->m_current = pre_message->m_head;

                if (msg) delete msg;
            } while (pre_message->m_head);
        }
        delete pre_message;
        pre_message = 0;
    }

    if (message) {
        if (message->m_head) {
            message->m_current = message->m_head;
            do {
                Message* msg = 0;
                QueueNode* node = (QueueNode*)message->m_current;
                if (node) {
                    if (message->m_head == node) message->m_head = (void*)node->next;
                    if (message->m_tail == node) message->m_tail = (void*)node->prev;
                    if (node->prev) ((QueueNode*)node->prev)->next = node->next;
                    if (node->next) ((QueueNode*)node->next)->prev = node->prev;

                    msg = (Message*)message->GetCurrentData();
                    delete node;
                    message->m_current = 0;
                }
                message->m_current = message->m_head;

                if (msg) delete msg;
            } while (message->m_head);
        }
        delete message;
        message = 0;
    }
}

/* Function start: 0x40D710 */
int Hotspot::LBLParse(char* line)
{
    char keyword[48];
    sscanf(line, "%s", keyword);
    WriteToMessageLog(line);

    if (strcmp(keyword, "RECT") == 0) {
        int l, t, r, b;
        sscanf(line, " %s %d %d %d %d", keyword, &l, &t, &r, &b);
        rect.left = l;
        rect.top = t;
        rect.right = r;
        rect.bottom = b;
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
        SC_Message* msg = new SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        msg->command = 7;
        Parser::ProcessFile(msg, this, 0);
        
        Queue* q = pre_message;
        if (msg == 0) ShowError("queue fault 0101");
        q->m_current = q->m_head;
        if (q->m_field_0xc == 1 || q->m_field_0xc == 2) {
            if (q->m_head != 0) {
                do {
                    QueueNode* currentPtr = (QueueNode*)q->m_current;
                    if (((SC_Message*)currentPtr->data)->targetAddress < msg->targetAddress) {
                        if (msg == 0) ShowError("queue fault 0102");
                        QueueNode* node = new QueueNode(msg);
                        if (q->m_current == 0) q->m_current = q->m_head;
                        if (q->m_head == 0) {
                            q->m_head = node;
                            q->m_tail = node;
                            q->m_current = node;
                        } else {
                            node->next = (QueueNode*)q->m_current;
                            node->prev = ((QueueNode*)q->m_current)->prev;
                            if (((QueueNode*)q->m_current)->prev == 0) {
                                q->m_head = node;
                            } else {
                                ((QueueNode*)q->m_current)->prev->next = node;
                            }
                            ((QueueNode*)q->m_current)->prev = node;
                        }
                        goto done_premessage;
                    }
                    if (q->m_tail == currentPtr) {
                        if (msg == 0) ShowError("queue fault 0112");
                        QueueNode* node = new QueueNode(msg);
                        if (q->m_current == 0) q->m_current = q->m_tail;
                        if (q->m_head == 0) {
                            q->m_head = node;
                            q->m_tail = node;
                            q->m_current = node;
                        } else {
                            if (q->m_tail == 0 || ((QueueNode*)q->m_tail)->next != 0) ShowError("queue fault 0113");
                            node->next = 0;
                            node->prev = (QueueNode*)q->m_tail;
                            ((QueueNode*)q->m_tail)->next = node;
                            q->m_tail = node;
                        }
                        goto done_premessage;
                    }
                    if (currentPtr != 0) q->m_current = currentPtr->next;
                } while (q->m_current != 0);
            } else {
                q->InsertAtCurrent(msg);
            }
        } else {
            q->InsertAtCurrent(msg);
        }
done_premessage:;
    } else if (strcmp(keyword, "MESSAGE") == 0) {
        if (!message) message = new Queue();
        SC_Message* msg = new SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        msg->command = 7;
        Parser::ProcessFile(msg, this, 0);
        
        Queue* q = message;
        if (msg == 0) ShowError("queue fault 0101");
        q->m_current = q->m_head;
        if (q->m_field_0xc == 1 || q->m_field_0xc == 2) {
            if (q->m_head != 0) {
                do {
                    QueueNode* currentPtr = (QueueNode*)q->m_current;
                    if (((SC_Message*)currentPtr->data)->targetAddress < msg->targetAddress) {
                        if (msg == 0) ShowError("queue fault 0102");
                        QueueNode* node = new QueueNode(msg);
                        if (q->m_current == 0) q->m_current = q->m_head;
                        if (q->m_head == 0) {
                            q->m_head = node;
                            q->m_tail = node;
                            q->m_current = node;
                        } else {
                            node->next = (QueueNode*)q->m_current;
                            node->prev = ((QueueNode*)q->m_current)->prev;
                            if (((QueueNode*)q->m_current)->prev == 0) {
                                q->m_head = node;
                            } else {
                                ((QueueNode*)q->m_current)->prev->next = node;
                            }
                            ((QueueNode*)q->m_current)->prev = node;
                        }
                        goto done_message;
                    }
                    if (q->m_tail == currentPtr) {
                        if (msg == 0) ShowError("queue fault 0112");
                        QueueNode* node = new QueueNode(msg);
                        if (q->m_current == 0) q->m_current = q->m_tail;
                        if (q->m_head == 0) {
                            q->m_head = node;
                            q->m_tail = node;
                            q->m_current = node;
                        } else {
                            if (q->m_tail == 0 || ((QueueNode*)q->m_tail)->next != 0) ShowError("queue fault 0113");
                            node->next = 0;
                            node->prev = (QueueNode*)q->m_tail;
                            ((QueueNode*)q->m_tail)->next = node;
                            q->m_tail = node;
                        }
                        goto done_message;
                    }
                    if (currentPtr != 0) q->m_current = currentPtr->next;
                } while (q->m_current != 0);
            } else {
                q->InsertAtCurrent(msg);
            }
        } else {
            q->InsertAtCurrent(msg);
        }
done_message:;
    } else if (strcmp(keyword, "LABEL") == 0) {
        sscanf(line, "%s %s", keyword, label);
    } else if (strcmp(keyword, "MOUSE") == 0) {
        sscanf(line, "%s %d", keyword, &field_D4);
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
        Message* msg = 0;
        if (q->m_current) {
            msg = (Message*)((QueueNode*)q->m_current)->data;
        }

        TimedEventPool* pool = g_TimedEventPool2_00436988;
        PooledEvent* evt = pool->Create(pool->list.tail, 0);

        ((PooledEvent*)((char*)evt + 8))->CopyFrom((PooledEvent*)msg);

        if (pool->list.tail) {
            ((PooledEvent*)pool->list.tail)->next = evt;
        } else {
            pool->list.head = evt;
        }
        pool->list.tail = evt;

        if (q->m_current == q->m_tail) return;
        if (q->m_current) {
            q->m_current = ((QueueNode*)q->m_current)->next;
        }
    } while (q->m_head != 0);
}



