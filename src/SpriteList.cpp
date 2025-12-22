#include "SpriteList.h"

/* Function start: 0x41F4F0 */
void SpriteList::Init()
{
    int iVar1;
    int iVar2;
    Sprite* sprite;

    this->spriteList->current = this->spriteList->head;
    iVar1 = (int)this->spriteList->head;
    while (iVar1 != 0) {
        sprite = NULL;
        iVar1 = (int)this->spriteList->current;
        if (iVar1 != 0) {
            sprite = *(Sprite**)(iVar1 + 8);
        }
        sprite->Init();
        iVar1 = (int)this->spriteList;
        iVar2 = *(int*)(iVar1 + 8);
        if (*(int*)(iVar1 + 4) == iVar2) break;
        if (iVar2 != 0) {
            *(int*)(iVar1 + 8) = *(int*)(iVar2 + 4);
        }
        iVar1 = *(int*)this->spriteList;
    }
    this->field_0x90 = 1;
    this->field_0x8c = this->field_0x8c | 0x2000;
}

/* Function start: 0x41F800 */
int SpriteList::DoAll()
{
    int iVar1;
    int iVar2;
    unsigned char bVar3;
    Sprite* sprite;

    this->field_0x90 = 1;
    if ((this->field_0x8d & 0x20) == 0) {
        this->Init();
    }
    this->spriteList->current = this->spriteList->head;
    iVar1 = (int)this->spriteList->head;
    while (iVar1 != 0) {
        sprite = NULL;
        iVar1 = (int)this->spriteList->current;
        if (iVar1 != 0) {
            sprite = *(Sprite**)(iVar1 + 8);
        }
        bVar3 = sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        if (bVar3 != 0) {
            this->field_0x90 = 0;
        }
        iVar1 = (int)this->spriteList;
        iVar2 = *(int*)(iVar1 + 8);
        if (*(int*)(iVar1 + 4) == iVar2) break;
        if (iVar2 != 0) {
            *(int*)(iVar1 + 8) = *(int*)(iVar2 + 4);
        }
        iVar1 = *(int*)this->spriteList;
    }
    return this->field_0x90;
}
