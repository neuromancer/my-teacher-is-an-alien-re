#include "SC_Question.h"
#include "Memory.h"
#include "MouseControl.h"
#include "SpriteList.h"
#include "FlagArray.h"
#include "TimedEvent.h"
#include "GameState.h"
#include "globals.h"
#include "string.h"
#include "ZBufferManager.h"
#include <stdio.h>
#include <string.h>



/* Function start: 0x4066D0 */
SC_Question::SC_Question(int id)
{
    // Zero the SC_Question-specific fields (0x26 dwords = 152 bytes starting at offset 0x88)
    memset(&mouseControl, 0, 0x98);
    
    // Set question ID
    questionId = id;
    
    state = 0;
    messageQueue = new Queue();
    
    // Parse the question file
    ParseFile(this, "mis\\quest001.mis", "[QUESTION%5.5d]", questionId);
    
    // Check if question was already answered
    if (g_Manager_00435a84->GetFlag(questionId, 1) != 0) {
        state = 2;
    }
}

/* Function start: 0x4067E0 */
SC_Question::~SC_Question()
{
    QueueNode* current;
    void* msgData;
    Queue* queue;
    
    // Clean up mouseControl at offset 0x88
    delete mouseControl;
    
    // Clean up messageQueue at offset 0x8c
    queue = messageQueue;
    if (queue != 0) {
        if (queue->m_head != 0) {
            queue->m_current = queue->m_head;
            while (queue->m_head != 0) {
                current = (QueueNode*)queue->m_current;
                if (current == 0) {
                    msgData = 0;
                } else {
                    // Unlink the node
                    if (queue->m_head == current) {
                        queue->m_head = current->next;
                    }
                    if (queue->m_tail == current) {
                        queue->m_tail = current->next;
                    }
                    if (current->next != 0) {
                        current->next->prev = current->prev;
                    }
                    if (current->prev != 0) {
                        current->prev->next = current->next;
                    }
                    
                    msgData = 0;
                    if (current != 0) {
                        msgData = current->data;
                        delete current;
                    }
                    queue->m_current = queue->m_head;
                }
                
                // Call destructor and free message data
                delete (SC_Message*)msgData;
            }
        }
        delete queue;
    }
    // Parser destructor is called automatically
}

/* Function start: 0x406930 */
void SC_Question::Update(int x, int y)
{
    switch (state) {
    case 0:
        g_ZBufferManager_0043698c->ShowSubtitle(label, x + 10, y + 23, 10000, 8);
        break;
    case 1:
        if (mouseControl != 0) {
            if (((SpriteList*)mouseControl)->DoAll() == 0) {
                Finalize();
            }
        }
        break;
    case 2:
        break;
    default:
        ShowError("illegal state");
        break;
    }
}

/* Function start: 0x4069B0 */
void SC_Question::Finalize()
{
    Queue* queue;
    void* msgData;
    int queueType;
    TimedEvent* event;
    TimedEventPool* pool;
    QueueNode* current;
    
    // Mark question as answered in flag array
    g_Manager_00435a84->SetFlag(questionId, 1);
    
    state = 2;
    if (messageQueue->m_head == 0) {
        return;
    }
    
    do {
        queue = messageQueue;
        msgData = 0;
        queueType = queue->m_field_0xc;
        
        if (queueType == 1 || queueType == 4) {
            queue->m_current = queue->m_head;
        }
        else if (queueType == 2 || queueType == 0) {
            queue->m_current = queue->m_tail;
        }
        else {
            ShowError("bad queue type %lu", queueType);
        }
        
        // Pop the current item from queue
        current = (QueueNode*)queue->m_current;
        if (current != 0) {
            if (queue->m_head == current) {
                queue->m_head = current->next;
            }
            if (queue->m_tail == current) {
                queue->m_tail = current->prev;
            }
            if (current->next != 0) {
                current->next->prev = current->prev;
            }
            if (current->prev != 0) {
                current->prev->next = current->next;
            }
            
            if (current != 0) {
                msgData = current->data;
            }
            else {
                msgData = 0;
            }
            
            if (current != 0) {
                current->data = 0;
                current->prev = 0;
                current->next = 0;
                FreeMemory(current);
                queue->m_current = 0;
            }
            queue->m_current = queue->m_head;
        }
        
        // Create timed event in pool and add message to it
        pool = g_TimedEventPool2_00436988;
        event = pool->Create((void*)pool->list.tail, 0);
        ((TimedEvent*)((int*)event + 2))->CopyFrom((TimedEvent*)msgData);
        
        // Link event to pool tail
        if (pool->list.tail == 0) {
            pool->list.head = event;
        }
        else {
            *(TimedEvent**)pool->list.tail = event;
        }
        pool->list.tail = event;
        
        // Destroy the original message if not null
        if (msgData != 0) {
            ((SC_Message*)msgData)->~SC_Message();
            FreeMemory(msgData);
        }
    } while (messageQueue->m_head != 0);
}

/* Function start: 0x4199A0 */
SC_Message::~SC_Message()
{
}

/* Function start: 0x4198C0 */
SC_Message::SC_Message(int targetAddress, int sourceAddress, int command, int data, int priority, int param1, int param2, int userPtr, int clickX, int clickY)
{
    this->targetAddress = targetAddress;
    this->sourceAddress = sourceAddress;
    this->command = command;
    this->data = data;
    this->priority = priority;
    this->param1 = param1;
    this->param2 = param2;
    this->userPtr = userPtr;
    this->clickX = clickX;
    this->clickY = clickY;
}

/* Function start: 0x419FD0 */
void SC_Message::Dump(int unused)
{
    int targetAddr;
    int srcAddr;
    const char* str1;
    const char* str2;
    
    // Log ADDRESS field
    // EDI = targetAddress, EAX = sourceAddress
    targetAddr = targetAddress;
    srcAddr = sourceAddress;
    if (targetAddr == 5) {
        str1 = g_GameState_00436998->GetState(srcAddr);
        str2 = g_GameState3_0043699c->GetState(targetAddr);
        WriteToMessageLog("\t\t\tADDRESS\t\t%s  %s", str2, str1);
    } else {
        str1 = g_GameState3_0043699c->GetState(targetAddr);
        WriteToMessageLog("\t\t\tADDRESS\t\t%s  %d", str1, srcAddr);
    }
    
    // Log FROM field
    // EDI = command, EAX = data
    targetAddr = command;
    srcAddr = data;
    if (targetAddr == 5) {
        str1 = g_GameState_00436998->GetState(srcAddr);
        str2 = g_GameState3_0043699c->GetState(targetAddr);
        WriteToMessageLog("\t\t\tFROM\t\t%s  %s", str2, str1);
    } else {
        str1 = g_GameState3_0043699c->GetState(targetAddr);
        WriteToMessageLog("\t\t\tFROM\t\t%s  %d", str1, srcAddr);
    }
    
    // Log INSTRUCTION field
    str1 = g_GameState4_004369a0->GetState(priority);
    WriteToMessageLog("\t\t\tINSTRUCTION\t%s", str1);
    
    // Log MOUSE field (clickX=0xa4, clickY=0xa8)
    WriteToMessageLog("\t\t\tMOUSE\t\t%d\t%d", clickX, clickY);
    
    // Log optional fields
    if (mouseX != 0) {
        WriteToMessageLog("\t\t\tBUTTON1\t\t%d", mouseX);
    }
    if (mouseY != 0) {
        WriteToMessageLog("\t\t\tBUTTON2\t\t%d", mouseY);
    }
    if (field_b4 != 0) {
        WriteToMessageLog("\t\t\tLASTKEY\t\t%d", field_b4);
    }
    if (field_b8 != 0) {
        WriteToMessageLog("\t\t\tTIME\t\t%lu", field_b8);
    }
    if (param1 != 0) {
        WriteToMessageLog("\t\t\tEXTRA1\t\t%lu", param1);
    }
    if (param2 != 0) {
        WriteToMessageLog("\t\t\tEXTRA2\t\t%lu", param2);
    }
    
    WriteToMessageLog("\t\tEND\t\t//message");
}

/* Function start: 0x406AF0 */
int SC_Question::LBLParse(char* param_1)
{
    void* mem;
    char local_34[32];
    
    sscanf(param_1, "%s", local_34);
    
    if (strcmp(local_34, "OVERLAYS") == 0) {
        MouseControl* mc = new MouseControl();
        mouseControl = mc;
        Parser::ProcessFile(mc, this, 0);
    }
    else if (strcmp(local_34, "LABEL") == 0) {
        ExtractQuotedString(param_1, label, 0x80);
    }
    else if (strcmp(local_34, "MESSAGE") == 0) {
        SC_Message* msg = new SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        Parser::ProcessFile(msg, this, 0);
        
        Queue* queue = messageQueue;
        if (msg == 0) {
            ShowError("queue fault 0101");
        }
        
        queue->m_current = queue->m_head;
        
        if (queue->m_field_0xc == 1 || queue->m_field_0xc == 2) {
            if (queue->m_head != 0) {
                do {
                    void* currentPtr = queue->m_current;
                    if (((SC_Message*)((QueueNode*)currentPtr)->data)->targetAddress < msg->targetAddress) {
                        if (msg == 0) {
                            ShowError("queue fault 0102");
                        }
                        
                        mem = AllocateMemory(0xc);
                        QueueNode* node = 0;
                        try {
                            if (mem != 0) {
                                node = (QueueNode*)mem;
                                node->data = msg;
                                node->prev = 0;
                                node->next = 0;
                            }
                        } catch (...) {
                        }
                        
                        if (queue->m_current == 0) {
                            queue->m_current = queue->m_head;
                        }
                        
                        if (queue->m_head == 0) {
                            queue->m_head = node;
                            queue->m_tail = node;
                            queue->m_current = node;
                        }
                        else {
                            node->next = (QueueNode*)queue->m_current;
                            node->prev = ((QueueNode*)queue->m_current)->prev;
                            if (((QueueNode*)queue->m_current)->prev == 0) {
                                queue->m_head = node;
                                ((QueueNode*)queue->m_current)->prev = node;
                            } else {
                                ((QueueNode*)queue->m_current)->prev->next = node;
                                ((QueueNode*)queue->m_current)->prev = node;
                            }
                        }
                        break;
                    }
                    
                    if (queue->m_tail == currentPtr) {
                        if (msg == 0) {
                            ShowError("queue fault 0112");
                        }
                        
                        mem = AllocateMemory(0xc);
                        QueueNode* node;
                        try {
                            if (mem != 0) {
                                ((QueueNode*)mem)->data = msg;
                                node = (QueueNode*)mem;
                                ((QueueNode*)mem)->prev = 0;
                                ((QueueNode*)mem)->next = 0;
                            } else {
                                node = 0;
                            }
                        } catch (...) {
                        }
                        
                        if (queue->m_current == 0) {
                            queue->m_current = queue->m_tail;
                        }
                        
                        if (queue->m_head == 0) {
                            queue->m_head = node;
                            queue->m_tail = node;
                            queue->m_current = node;
                        }
                        else {
                            if (queue->m_tail == 0 || ((QueueNode*)queue->m_tail)->next != 0) {
                                ShowError("queue fault 0113");
                            }
                            node->next = 0;
                            node->prev = (QueueNode*)queue->m_tail;
                            ((QueueNode*)queue->m_tail)->next = node;
                            queue->m_tail = node;
                        }
                        break;
                    }
                    
                    if (currentPtr != 0) {
                        queue->m_current = ((QueueNode*)currentPtr)->next;
                    }
                } while (queue->m_current != 0);
            }
            else {
                if (msg == 0) {
                    ShowError("queue fault 0102");
                }
                
                mem = AllocateMemory(0xc);
                QueueNode* node = 0;
                try {
                    if (mem != 0) {
                        node = (QueueNode*)mem;
                        node->data = msg;
                        node->prev = 0;
                        node->next = 0;
                    }
                } catch (...) {
                }
                
                if (queue->m_current == 0) {
                    queue->m_current = queue->m_head;
                }
                
                if (queue->m_head == 0) {
                    queue->m_head = node;
                    queue->m_tail = node;
                    queue->m_current = node;
                }
                else {
                    node->next = (QueueNode*)queue->m_current;
                    node->prev = ((QueueNode*)queue->m_current)->prev;
                    if (((QueueNode*)queue->m_current)->prev == 0) {
                        queue->m_head = node;
                        ((QueueNode*)queue->m_current)->prev = node;
                    } else {
                        ((QueueNode*)queue->m_current)->prev->next = node;
                        ((QueueNode*)queue->m_current)->prev = node;
                    }
                }
            }
        }
        else {
            queue->Insert(msg);
        }
    }
    else if (strcmp(local_34, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("SC_Question");
    }
    
    return 0;
}

/* Function start: 0x406F50 */
void SC_Question::DumpMessageQueue(int unused)
{
    Queue* queue;
    SC_Message* msgData;
    
    queue = messageQueue;
    if (queue == 0) {
        return;
    }
    if (queue->m_head == 0) {
        return;
    }
    
    WriteToMessageLog("\tmsgQ");
    queue = messageQueue;
    queue->m_current = queue->m_head;
    
    while (queue->m_current != 0) {
        if (queue->m_current == 0) {
            msgData = 0;
        } else {
            msgData = (SC_Message*)((QueueNode*)queue->m_current)->data;
        }
        msgData->Dump(0);
        
        if (queue->m_tail == queue->m_current) {
            break;
        }
        
        if (queue->m_current != 0) {
            queue->m_current = ((QueueNode*)queue->m_current)->next;
        }
    }
    
    WriteToMessageLog("end queue dump");
}
