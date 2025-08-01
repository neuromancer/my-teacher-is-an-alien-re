#include "Hotspot.h"

extern "C" void ShowError(const char* message, ...);

/* Function start: 0x409400 */
unsigned char Hotspot::Do()
{
    if (this->sprite != (Sprite *)0x0) {
        return this->sprite->Do(this->sprite->loc_x, this->sprite->loc_y, 0, 0x3ff00000);
    }
    // WARNING: Subroutine does not return
    ShowError("Missing sprite in Hotspot Do()");
    return 0;
}

