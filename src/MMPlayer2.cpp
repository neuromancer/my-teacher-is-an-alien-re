#include "MMPlayer2.h"
#include "Memory.h"
#include "Sprite.h"
#include "ZBufferManager.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    void FUN_0041d040(int); // Sprite::Update/Init?
    void ShowError(const char*, ...);
}

/* Function start: 0x409030 */
int MMPlayer2::LBLParse(char* param_1)
{
    char local_34[32];
    
    sscanf(param_1, " %s ", local_34);
    
    if (_strcmpi(local_34, "SPRITE") == 0) {
        Sprite* s = new Sprite(0);
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

        ZBQueue* list = spriteList;
        if (s == 0) {
             ShowError("queue fault 0101");
        }

        list->current = list->head;
        if (list->type == 1 || list->type == 2) {
             if (list->head == 0) {
                 if (s == 0) {
                      ShowError("queue fault 0102");
                 }
                 list->Insert(s);
             }
             else {
                 do {
                     ZBQueueNode* curr = (ZBQueueNode*)list->current;
                     Sprite* currSprite = (Sprite*)curr->data;

                     // Compare field_b0 (Z-order?)
                     if (currSprite->field_0xb0 < s->field_0xb0) {
                         list->Insert(s);
                         break;
                     }

                     if (list->tail == curr) {
                         // Push to end - simplified
                         list->Insert(s);
                         break;
                     }

                     if (curr != 0) {
                         list->current = curr->prev;
                     }
                 } while (list->current != 0);
             }
        }
        else {
            list->Insert(s);
        }
    }
}
