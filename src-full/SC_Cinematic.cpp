#include "SC_Cinematic.h"
#include <string.h>

/* Function start: 0x42FBD0 */
SC_Cinematic::SC_Cinematic() {
    memset(&field_A8, 0, 48);
    handlerId = 3;
}

/* Function start: 0x42FC90 */
SC_Cinematic::~SC_Cinematic() {
    CleanUp(0);
}

void SC_Cinematic::CleanUp(int param) {
}
