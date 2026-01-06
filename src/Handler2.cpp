#include "Handler2.h"
#include "globals.h"
#include "string.h"
#include "mss.h"
#include "SC_Question.h"

extern "C" void __fastcall FUN_0041f200(void*);
extern "C" void __cdecl FUN_00424940(void*);

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

/* Function start: 0x40FA30 */
void Handler2::Update(SC_Message* msg) {
    if (sprite != 0) {
        delete sprite;
        sprite = 0;
    }
    if (palette != 0) {
        delete palette;
        palette = 0;
    }
    WriteToMessageLogIfEnabled(L"EXIT TEACHER SCREEN TO LEAVE\n");
}

/* Function start: 0x40FAF0 */
void Handler2::Draw(int param1, int param2) {
    Sprite* spr;
    Sample* samp;
    int iVar1;

    if (handlerId == param2) {
        spr = sprite;
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
        FUN_0041f200(g_Mouse_00436978);
        samp = sample;
        if (samp != 0) {
            iVar1 = (int)samp->m_sample;
            if (iVar1 != 0) {
                if (samp->m_size == *(int*)(iVar1 + 0xc)) {
                    if (AIL_sample_status((HSAMPLE)iVar1) == 4) {
                        return;
                    }
                }
            }
            samp = sample;
            if (samp != 0) {
                samp->~Sample();
                FUN_00424940(samp);
                sample = 0;
            }
        }
    }
}

/* Function start: 0x40FAD0 */
int Handler2::Exit(SC_Message* msg) {
    return handlerId == msg->targetAddress;
}

