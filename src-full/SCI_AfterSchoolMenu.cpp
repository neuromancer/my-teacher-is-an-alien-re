#include "SCI_AfterSchoolMenu.h"
#include <string.h>
#include "Sprite.h"
#include "Palette.h"

extern void __fastcall FUN_004148f0(void*);
extern void* __fastcall FUN_00418540(void*);

/* Function start: 0x416E70 */
SCI_AfterSchoolMenu::SCI_AfterSchoolMenu() {
    memset(&field_A8, 0, 40);
    handlerId = 0x1F;

    sprite1 = new Sprite("\"elements\\option2.smk\"");
    sprite1->priority = 0x3E9;
    sprite1->flags |= 0x40;

    sprite2 = new Sprite("\"elements\\option2h.smk\"");
    sprite2->priority = 0x3E9;
    sprite2->flags |= 0x40;
}

/* Function start: 0x416FC0 */
SCI_AfterSchoolMenu::~SCI_AfterSchoolMenu() {
    if (sprite1 != 0) {
        delete sprite1;
        sprite1 = 0;
    }

    if (sprite2 != 0) {
        delete sprite2;
        sprite2 = 0;
    }

    if (field_C8 != 0) {
        int* list = (int*)field_C8;
        if (*list != 0) {
            list[2] = *list;
            while (*list != 0) {
                void* item = (void*)FUN_00418540((void*)field_C8);
                if (item != 0) {
                    FUN_004148f0(item);
                    operator delete(item);
                }
            }
        }
        operator delete((void*)field_C8);
        field_C8 = 0;
    }

    if (field_C4 != 0) {
        FUN_004148f0((void*)field_C4);
        operator delete((void*)field_C4);
        field_C4 = 0;
    }

    if (field_B4 != 0) {
        ((Palette*)field_B4)->~Palette();
        operator delete((void*)field_B4);
        field_B4 = 0;
    }
}
