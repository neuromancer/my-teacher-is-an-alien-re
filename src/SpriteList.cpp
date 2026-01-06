#include "SpriteList.h"

/* Function start: 0x41F4F0 */
void SpriteList::Init()
{
    int iVar1;
    int iVar2;
    Sprite* sprite;

    spriteList->current = spriteList->head;
    iVar1 = (int)spriteList->head;
    while (iVar1 != 0) {
        sprite = NULL;
        iVar1 = (int)spriteList->current;
        if (iVar1 != 0) {
            sprite = *(Sprite**)(iVar1 + 8);
        }
        sprite->Init();
        iVar1 = (int)spriteList;
        iVar2 = *(int*)(iVar1 + 8);
        if (*(int*)(iVar1 + 4) == iVar2) break;
        if (iVar2 != 0) {
            *(int*)(iVar1 + 8) = *(int*)(iVar2 + 4);
        }
        iVar1 = *(int*)spriteList;
    }
    field_0x90 = 1;
    field_0x8c = field_0x8c | 0x2000;
}

/* Function start: 0x41F800 */
int SpriteList::DoAll()
{
    int iVar1;
    int iVar2;
    unsigned char bVar3;
    Sprite* sprite;

    field_0x90 = 1;
    if ((field_0x8d & 0x20) == 0) {
        Init();
    }
    spriteList->current = spriteList->head;
    iVar1 = (int)spriteList->head;
    while (iVar1 != 0) {
        sprite = NULL;
        iVar1 = (int)spriteList->current;
        if (iVar1 != 0) {
            sprite = *(Sprite**)(iVar1 + 8);
        }
        bVar3 = sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
        if (bVar3 != 0) {
            field_0x90 = 0;
        }
        iVar1 = (int)spriteList;
        iVar2 = *(int*)(iVar1 + 8);
        if (*(int*)(iVar1 + 4) == iVar2) break;
        if (iVar2 != 0) {
            *(int*)(iVar1 + 8) = *(int*)(iVar2 + 4);
        }
        iVar1 = *(int*)spriteList;
    }
    return field_0x90;
}

/* Function start: 0x41f360 */
SpriteList::~SpriteList() {
    // Stub
}
