#include "Handler1.h"

/* Function start: 0x403340 */
Handler1::Handler1() {
    // Handler base class zeros handlerId through field_9C via memset

    handlerId = 1;
    field_8C = 1;
    palette = 0;
    sprite = 0;

    palette = new Palette();
    if (palette) {
        palette->Load("demo\\Demoscrn.col");
    }

    sprite = new Sprite("demo\\Demoscrn.smk");
    if (sprite) {
        sprite->flags &= ~2;
        sprite->priority = 0;
        sprite->loc_x = 0;
        sprite->loc_y = 0;
    }
}

/* Function start: 0x4034C0 */
Handler1::~Handler1() {
    if (sprite) {
        delete sprite;
        sprite = 0;
    }
    if (palette) {
        delete palette;
        palette = 0;
    }
}

// Stub virtual method implementations
/* Function start: 0x403570 */
void Handler1::Init(SC_Message* msg) {
    Handler::Init(msg);
}

/* Function start: 0x403650 */
int Handler1::HandleMessage(SC_Message* msg) {
    return 1;
}

void Handler1::Update(SC_Message* msg) {
}

void Handler1::Draw() {
}

void Handler1::Exit() {
}

