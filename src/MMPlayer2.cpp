#include "MMPlayer2.h"
#include "Memory.h"
#include "Sprite.h"
#include "Queue.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    void* FUN_0041cd50(void*, char*); // Sprite ctor
    void FUN_0041d040(int); // Sprite::Update/Init?
    void FUN_00409160(void*, void*); // Queue::Add generic?
    void ShowError(const char*, ...);
}

/* Function start: 0x409030 */
int MMPlayer2::LBLParse(char* param_1)
{
    char local_34[32];
    
    sscanf(param_1, " %s ", local_34);
    
    if (_strcmpi(local_34, "SPRITE") == 0) {
        void* mem = AllocateMemory(0xd8);
        Sprite* s = 0;
        if (mem) {
            s = (Sprite*)FUN_0041cd50(mem, 0);
        }
        Parser::ProcessFile(s, this, 0);
        AddSprite(s);
    }
    else if (_strcmpi(local_34, "END") == 0) {
        return 1;
    }
    else {
        return Parser::LBLParse("MMPlayer2");
    }
    
    return 0;
}

/* Function start: 0x408C40 */
void MMPlayer2::AddSprite(Sprite* s)
{
    if (s != 0) {
        // FUN_0041d040((int)s); // Some Sprite init
        
        Queue* list = spriteList;
        if (s == 0) {
             ShowError("queue fault 0101");
        }
        
        list->m_current = list->m_head;
        if (list->m_field_0xc == 1 || list->m_field_0xc == 2) {
             if (list->m_head == 0) {
                 if (s == 0) {
                      ShowError("queue fault 0102");
                 }
                 list->Insert(s); // Simplified calling standard insert for empty list
             }
             else {
                 do {
                     QueueNode* curr = (QueueNode*)list->m_current;
                     Sprite* currSprite = (Sprite*)curr->data;
                     
                     // Compare field_b0 (Z-order?)
                     if (currSprite->field_0xb0 < s->field_0xb0) {
                         list->Insert(s); // Insert before current
                         break;
                     }
                     
                     if (list->m_tail == curr) {
                         list->Push(s); // Append
                         break;
                     }
                     
                     if (curr != 0) {
                         list->m_current = curr->next;
                     }
                 } while (list->m_current != 0);
             }
        }
        else {
            FUN_00409160(list, s);
        }
    }
}
