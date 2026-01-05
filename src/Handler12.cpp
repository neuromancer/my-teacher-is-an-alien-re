#include "Handler12.h"
#include <string.h>

/* Function start: 0x401000 */
Handler12::Handler12()
    : handlerId(0)
    , field_0x8C(0)
    , field_0x90(0)
    , field_0x94(0)
    , field_0x98(0)
    , field_0x9C(0)
{
    // Set handler ID to 12
    handlerId = 12;
    
    // Call Timer::Reset() explicitly
    timer.Reset();
}
