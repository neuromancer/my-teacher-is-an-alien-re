#include "Handler5.h"
#include <string.h>

/* Function start: 0x40FB80 */
Handler5::Handler5() {
    // Handler base class zeros handlerId through field_9C via memset
    // Timer constructor called implicitly

    // Zero 8 dwords at 0xC0-0xDF
    field_C0 = 0;
    field_C4 = 0;
    field_C8 = 0;
    field_CC = 0;
    field_D0 = 0;
    field_D4 = 0;
    field_D8 = 0;
    field_DC = 0;

    // Zero 0x14 dwords (80 bytes) at 0xA0, overwriting timer
    memset(&timer, 0, 0x14 * sizeof(int));

    handlerId = 5;
    
    // Reset Timer
    timer.Reset();

    // Set rectangle fields
    field_B8 = 0;
    field_BC = 0;
    field_C0 = 0;
    field_C4 = 0;
    field_C8 = 0x27f;
    field_D0 = 0;
    field_D8 = 0x27f;
    field_CC = 0x14;
    field_D4 = 0x1cb;
    field_DC = 0x1df;
    field_E0 = 10;
    field_E4 = 0x1d;
    field_E8 = 0x1e;

    sprite = new Sprite("elements\\gamestat.smk");
    sprite->flags &= ~2;
    sprite->priority = 0;
}

/* Function start: 0x40FD70 */
Handler5::~Handler5() {
    if (sprite) {
        delete sprite;
        sprite = 0;
    }
}

// Stub virtual method implementations
/* Function start: 0x40FE40 */
void Handler5::Init(SC_Message* msg) {
    Handler::Init(msg);
}

/* Function start: 0x4100E0 */
int Handler5::HandleMessage(SC_Message* msg) {
    return 1;
}

void Handler5::Update(SC_Message* msg) {
}

void Handler5::Draw() {
}

void Handler5::Exit() {
}


