#include "Handler11.h"
#include <string.h>

extern void FUN_418d60(void*, const char*, const char*);

/* Function start: 0x40ACC0 */
Handler11::Handler11() {
    Palette** ptr = &field_600;
    memset(ptr, 0, 0x12 * sizeof(int));
    handlerId = 0xb;
    FUN_418d60(this, "mis\\SrSc0001.mis", 0);
}

/* Function start: 0x40AD70 */
Handler11::~Handler11() {
    Palette* pal;
    MouseControl* mouse;
    SC_Dialog* dialog;
    UnknownClass_4092e0* unk;
    int i;

    pal = field_600;
    if (pal != 0) {
        delete pal;
        field_600 = 0;
    }

    mouse = field_604;
    if (mouse != 0) {
        delete mouse;
        field_604 = 0;
    }

    dialog = field_608;
    if (dialog != 0) {
        delete dialog;
        field_608 = 0;
    }

    for (i = 0; i < 10; i++) {
        unk = field_60C[i];
        if (unk != 0) {
            delete unk;
            field_60C[i] = 0;
        }
    }
}
