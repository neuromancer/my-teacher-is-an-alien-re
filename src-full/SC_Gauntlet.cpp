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
    memset(field_120, 0, 7 * 4);
    memset(field_118, 0, 0x448);
    handlerId = 0x50;
}

SC_Gauntlet::~SC_Gauntlet() {
}
