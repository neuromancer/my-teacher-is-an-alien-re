#include "SC_Question.h"
#include "Memory.h"
#include "MMPlayer.h"
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
    
    messageQueue = new Queue();
    state = 0;
    
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
    if (mouseControl != 0) {
        delete mouseControl;
        mouseControl = 0;
    }
    
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
                        queue->m_tail = current->prev;
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
        messageQueue = 0;
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
            if (mouseControl->Draw() == 0) {
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
    PooledEvent* event;
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
                delete current;
                queue->m_current = 0;
            }
            queue->m_current = queue->m_head;
        }
        
        // Create timed event in pool and add message to it
        pool = g_TimedEventPool2_00436988;
        event = pool->Create((void*)pool->list.tail, 0);
        event->GetEmbeddedMessage()->CopyFrom((PooledEvent*)msgData);

        // Link event to pool tail
        if (pool->list.tail == 0) {
            pool->list.head = event;
        }
        else {
            pool->list.tail->next = event;
        }
        pool->list.tail = event;
        
        // Destroy the original message if not null
        if (msgData != 0) {
            delete (SC_Message*)msgData;
        }
    } while (messageQueue->m_head != 0);
}

/* Function start: 0x406AF0 */
int SC_Question::LBLParse(char* param_1)
{
    char local_34[32];
    
    sscanf(param_1, "%s", local_34);
    
    if (strcmp(local_34, "OVERLAYS") == 0) {
        MMPlayer* mc = new MMPlayer();
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
                        
                        QueueNode* node = new QueueNode(msg);

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
                        
                        QueueNode* node = new QueueNode(msg);

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
                
                QueueNode* node = new QueueNode(msg);

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

/* Function start: 0x4198C0 */
SC_Message::SC_Message(int p_targetAddress, int p_sourceAddress, int p_command, int p_data, int p_priority, int p_param1, int p_param2, int p_userPtr, int p_clickX, int p_clickY)
{
    // Zero clickPos first (MousePoint at 0xa4)
    clickPos.x = 0;
    clickPos.y = 0;
    
    // Zero 14 dwords (0x38 bytes) starting at offset 0x88
    memset(&targetAddress, 0, 0xe * sizeof(int));
    
    // Assign all parameters
    targetAddress = p_targetAddress;
    sourceAddress = p_sourceAddress;
    command = p_command;
    data = p_data;
    priority = p_priority;
    param1 = p_param1;
    param2 = p_param2;
    userPtr = p_userPtr;
    clickPos.x = p_clickX;
    clickPos.y = p_clickY;
}

/* Function start: 0x4199A0 */
SC_Message::~SC_Message()
{
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
    
    // Log MOUSE field (clickPos at 0xa4)
    WriteToMessageLog("\t\t\tMOUSE\t\t%d\t%d", clickPos.x, clickPos.y);
    
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
