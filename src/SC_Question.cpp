#include "SC_Question.h"
#include "Memory.h"
#include "MouseControl.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    void FUN_00419080(char* src, char* dst, int maxlen);
    void FUN_004086c0(void*, int);
    void ShowError(const char*, ...);
    void* MouseControl_Constructor(void*);
    void* FUN_0041f280(void*); // MouseControl ctor
    void FUN_00424940(void*); // Free
}

void* operator new(unsigned int, void* p) { return p; }

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

/* Function start: 0x406AF0 */
int SC_Question::LBLParse(char* param_1)
{
    void* mem;
    char local_34[32];
    
    sscanf(param_1, "%s", local_34);
    
    if (strcmp(local_34, "OVERLAYS") == 0) {
        mem = AllocateMemory(0x98);
        MouseControl* mc = 0;
        try {
            if (mem != 0) {
                mc = (MouseControl*)FUN_0041f280(mem);
            }
        } catch (...) {
        }
        this->mouseControl = mc;
        Parser::ProcessFile(mc, this, 0);
    }
    else if (strcmp(local_34, "LABEL") == 0) {
        FUN_00419080(param_1, this->label, 0x80);
    }
    else if (strcmp(local_34, "MESSAGE") == 0) {
        mem = AllocateMemory(0xc0);
        SC_Message* msg = 0;
        try {
            if (mem != 0) {
                msg = new (mem) SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            }
        } catch (...) {
        }
        Parser::ProcessFile(msg, this, 0);
        
        Queue* queue = this->messageQueue;
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
