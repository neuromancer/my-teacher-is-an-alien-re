#include "SC_Gauntlet.h"
#include <string.h>

GauntletEntry::GauntletEntry() {
    fields[3] = 0;
    fields[4] = 0;
    fields[5] = 0;
    fields[6] = 0;
    memset(this, 0, 7 * 4);
}

/* Function start: 0x42E540 */
SC_Gauntlet::SC_Gauntlet() {
    field_128 = 0;
    field_12C = 0;
    field_130 = 0;
    field_134 = 0;
    memset(field_120, 0, 7 * 4);
    field_548 = 0;
    field_54C = 0;
    field_550 = 0;
    field_554 = 0;
    memset(field_118, 0, 0x112 * 4);
    handlerId = 0x50;
}

SC_Gauntlet::~SC_Gauntlet() {
}
