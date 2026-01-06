#include "Handler2.h"
#include <string.h>

/* Function start: 0x40F710 */
Handler2::Handler2() {
    // Handler base class zeros handlerId through field_9C via memset
    // but we need to re-zero since we're about to set values

    // Zero pointers at 0xA0-0xAC
    palette = 0;
    sprite = 0;
    sample = 0;
    field_AC = 0;

    handlerId = 2;
    field_8C = 1;

    palette = new Palette();
    palette->Load("demo\\Dummy.col");

    sprite = new Sprite("demo\\Dummy.smk");
    sprite->flags &= ~2;
    sprite->priority = 0;
    sprite->loc_x = 0;
    sprite->loc_y = 0;

    sample = new Sample();
    sample->Load("audio\\Snd0022.wav");
    sample->Play(100, 1);
}


/* Function start: 0x40F8F0 */
Handler2::~Handler2() {
    if (sample) {
        delete sample;
        sample = 0;
    }
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
/* Function start: 0x40F9E0 */
void Handler2::Init(SC_Message* msg) {
    Handler::Init(msg);
}

/* Function start: 0x40FA90 */
int Handler2::HandleMessage(SC_Message* msg) {
    return 1;
}

void Handler2::Update(SC_Message* msg) {
}

void Handler2::Draw() {
}

void Handler2::Exit() {
}

