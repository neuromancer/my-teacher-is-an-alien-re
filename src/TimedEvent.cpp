#include "TimedEvent.h"
#include <string.h>

/* Function start: 0x402310 */
TimedEvent::TimedEvent(const TimedEvent& other)
{
    field_0x8 = other.field_0x8;
    field_0xc = other.field_0xc;
    memcpy(field_0x10, other.field_0x10, sizeof(field_0x10));
    field_0x30 = other.field_0x30;
    field_0x38 = other.field_0x38;
    field_0x3c = other.field_0x3c;
    memcpy(field_0x40, other.field_0x40, sizeof(field_0x40));
    field_0x80 = other.field_0x80;
    field_0x88 = other.field_0x88;
    field_0x8c = other.field_0x8c;
    field_0x90 = other.field_0x90;
    field_0x94 = other.field_0x94;
    field_0x98 = other.field_0x98;
    field_0x9c = other.field_0x9c;
    field_0xa0 = other.field_0xa0;
    field_0xa4 = other.field_0xa4;
    field_0xa8 = other.field_0xa8;
    field_0xac = other.field_0xac;
    field_0xb0 = other.field_0xb0;
    field_0xb4 = other.field_0xb4;
    field_0xb8 = other.field_0xb8;
    field_0xbc = other.field_0xbc;
}

/* Function start: 0x402420 */
void* TimedEvent::Create(void* callback, void* data)
{
    // This function is complex and I will skip it for now.
    return 0;
}
