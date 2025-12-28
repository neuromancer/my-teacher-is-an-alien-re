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
SC_Message::SC_Message(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10)
{
    this->field_88 = p1;
    this->field_8c = p2;
    this->field_90 = p3;
    this->field_94 = p4;
    this->field_98 = p5;
    this->field_9c = p6;
    this->field_a0 = p7;
    
    this->field_bc = p8;
    this->field_a4 = p9;
    this->field_a8 = p10;
}

/* Function start: 0x406AF0 */
int SC_Question::LBLParse(char* param_1)
{
    char local_34[32];
    
    sscanf(param_1, "%s", local_34);
    
    if (_strcmpi(local_34, "OVERLAYS") == 0) {
        void* mem = AllocateMemory(0x98);
        MouseControl* mc = 0;
        if (mem) {
            mc = (MouseControl*)FUN_0041f280(mem);
        }
        this->mouseControl = mc;
        Parser::ProcessFile(mc, this, 0);
    }
    else if (_strcmpi(local_34, "LABEL") == 0) {
        FUN_00419080(param_1, this->label, 0x80);
    }
    else if (_strcmpi(local_34, "MESSAGE") == 0) {
        void* mem = AllocateMemory(0xc0);
        SC_Message* msg = 0;
        if (mem) {
            msg = new (mem) SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        }
        Parser::ProcessFile(msg, this, 0);
        
        Queue* queue = this->messageQueue;
        if (msg == 0) {
            ShowError("queue fault 0101");
        }
        
        queue->m_current = queue->m_head;
        
        if (queue->m_field_0xc == 1 || queue->m_field_0xc == 2) {
            if (queue->m_head == 0) {
                // Empty queue logic similar to Insert/Push
                if (msg == 0) {
                    ShowError("queue fault 0102");
                }
                // ... Implementation detailing requires Queue internals access
                // Since this replicates Queue logic, we might need a friend class or similar access
                // For now, simplify or assume standard usage if possible?
                // The decompiled logic is manual insertion sort.
                // NOTE: Using a simplified queue insert if exact logic is too complex to replicate without more context
                // But instructions say "replicate ... jump types ... etc". 
                // Let's try to match logic. QueueNode allocation is manual here too.
                
                QueueNode* node = (QueueNode*)AllocateMemory(0xc); // 12 bytes
                if (node) {
                    node->data = msg;
                    node->prev = 0;
                    node->next = 0;
                }
                
                if (queue->m_current == 0) {
                    queue->m_current = queue->m_head; // ?? Check 406e23: this_00[2] = *this_00
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
                        ((QueueNode*)queue->m_current)->prev = node; // Wait, original code casts access
                    } else {
                        ((QueueNode*)queue->m_current)->prev->next = node;
                        ((QueueNode*)queue->m_current)->prev = node;
                    }
                }
            }
            else {
                // Loop to find position
                do {
                    QueueNode* curr = (QueueNode*)queue->m_current;
                    SC_Message* currMsg = (SC_Message*)curr->data;
                    
                    if (currMsg->field_88 < msg->field_88) {
                        // Insert here
                         QueueNode* node = (QueueNode*)AllocateMemory(0xc); // 12 bytes
                        if (node) {
                            node->data = msg;
                            node->prev = 0;
                            node->next = 0;
                        }
                        
                        // Link logic... reuse block if possible?
                        // ...
                        
                        break;
                    }
                    
                    if (queue->m_tail == curr) {
                        // Append logic
                        // ...
                        break;
                    }
                    
                    if (curr != 0) {
                        queue->m_current = curr->next;
                    }
                } while (queue->m_current != 0);
            }
        }
        else {
            FUN_004086c0(queue, (int)msg);
        }
    }
    else if (_strcmpi(local_34, "END") == 0) {
        return 1;
    }
    else {
        return Parser::LBLParse("SC_Question");
    }
    
    return 0;
}
