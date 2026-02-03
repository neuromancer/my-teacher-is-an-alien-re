#include "Hotspot.h"
#include "SC_Question.h"
#include "string.h"
#include "Message.h"
#include "Memory.h"
#include "Sprite.h"
#include "MMPlayer.h"
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
    MMPlayer* mc;

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
        if (list1 == 0 || list1->Draw() != 0) {
            state = 2;
        }
        if (state != 2) {
            return 0;
        }
    case 2:
        if (list2 != 0) {
            if (list2->Draw() != 0) {
                state = 3;
                return 0;
            }
        }
        else {
            state = 3;
            if (dialog != 0) {
                puVar2 = (int*)new char[8];
                *puVar2 = param_1;
                puVar2[1] = param_2;
                SC_Message_Send(9, parseFileIndex, 0xb, param_3, 5, dialogParseFileNumber, 0, (int)puVar2, 0, 0);
            }
        }
        return 0;
    case 3:
        if (list3 != 0) {
            if (list3->Draw() != 0) {
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

	Queue* q = pre_message;
	if (q) {
		if (q->head) {
			q->current = q->head;
			while (q->head) {
				QueueNode* node = (QueueNode*)q->current;
				SC_Message* msg;
				if (!node) {
					msg = 0;
				} else {
					if (q->head == node) q->head = node->next;
					if (q->tail == node) q->tail = node->prev;
					if (node->prev) node->prev->next = node->next;
					if (node->next) node->next->prev = node->prev;

					msg = (SC_Message*)q->GetCurrentData();

					if (q->current) {
						delete (QueueNode*)q->current;
						q->current = 0;
					}
					q->current = q->head;
				}
				if (msg) {
					msg->~SC_Message();
					delete (char*)msg;
				}
			}
		}
		delete (char*)q;
		pre_message = 0;
	}

	q = message;
	if (q) {
		if (q->head) {
			q->current = q->head;
			while (q->head) {
				QueueNode* node = (QueueNode*)q->current;
				SC_Message* msg;
				if (!node) {
					msg = 0;
				} else {
					if (q->head == node) q->head = node->next;
					if (q->tail == node) q->tail = node->prev;
					if (node->prev) node->prev->next = node->next;
					if (node->next) node->next->prev = node->prev;

					msg = (SC_Message*)q->GetCurrentData();

					if (q->current) {
						delete (QueueNode*)q->current;
						q->current = 0;
					}
					q->current = q->head;
				}
				if (msg) {
					msg->~SC_Message();
					delete (char*)msg;
				}
			}
		}
		delete (char*)q;
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
        hotspot = new MMPlayer();
        Parser::ProcessFile(hotspot, this, 0);
    } else if (strcmp(keyword, "RIGHT_TOOL") == 0) {
        right_tool = new MMPlayer();
        Parser::ProcessFile(right_tool, this, 0);
    } else if (strcmp(keyword, "WRONG_TOOL") == 0) {
        wrong_tool = new MMPlayer();
        Parser::ProcessFile(wrong_tool, this, 0);
    } else if (strcmp(keyword, "PREMESSAGE") == 0) {
        if (!pre_message) pre_message = new Queue();
        SC_Message* msg = new SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        msg->command = 7;
        Parser::ProcessFile(msg, this, 0);

        Queue* q = pre_message;
        q->ResetForSortedAdd(msg);
        if (q->type == 1 || q->type == 2) {
            if (q->head != 0) {
                do {
                    QueueNode* currentPtr = (QueueNode*)q->current;
                    if (((SC_Message*)currentPtr->data)->targetAddress < msg->targetAddress) {
                        q->Insert(msg);
                        goto done_premessage;
                    }
                    if (q->tail == currentPtr) {
                        q->Push(msg);
                        goto done_premessage;
                    }
                    if (currentPtr != 0) q->current = currentPtr->next;
                } while (q->current != 0);
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
        q->ResetForSortedAdd(msg);
        if (q->type == 1 || q->type == 2) {
            if (q->head != 0) {
                do {
                    QueueNode* currentPtr = (QueueNode*)q->current;
                    if (((SC_Message*)currentPtr->data)->targetAddress < msg->targetAddress) {
                        q->Insert(msg);
                        goto done_message;
                    }
                    if (q->tail == currentPtr) {
                        q->Push(msg);
                        goto done_message;
                    }
                    if (currentPtr != 0) q->current = currentPtr->next;
                } while (q->current != 0);
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
        if (hotspot == 0 || hotspot->Draw() == 0) {
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
    q->current = q->head;
    if (!q->head) return;

    do {
        Message* msg = 0;
        if (q->current) {
            msg = (Message*)((QueueNode*)q->current)->data;
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

        if (q->current == q->tail) return;
        if (q->current) {
            q->current = ((QueueNode*)q->current)->next;
        }
    } while (q->head != 0);
}



