#include "Handler2.h"
#include "globals.h"
#include "string.h"
#include "mss.h"
#include "SC_Question.h"
#include "MouseControl.h"
#include "Memory.h"

extern "C" char* CDData_FormatPath(char*, ...);

/* Function start: 0x40F710 */
Handler2::Handler2() {
    int* pA0;

    pA0 = (int*)&palette;

    // Zero pointers at 0xA0-0xAC
    pA0[0] = 0;
    pA0[1] = 0;
    pA0[2] = 0;
    pA0[3] = 0;

    handlerId = 2;
    moduleParam = 1;

    palette = new Palette();
    palette->Load(CDData_FormatPath("demo\\Dummy.col"));

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
        sample->Unload();
        operator delete(sample);
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

/* Function start: 0x40F9E0 */
void Handler2::Init(SC_Message* msg) {
    Palette* pal;
    Palette** palettePtr;
    WriteToMessageLog("\nENTER TEACHER SCREEN TO LEAVE");
    Handler2::CopyCommandData(msg);
    pal = palette;
    if (pal != 0) {
        palettePtr = &g_ZBufferManager_0043698c->m_palette;
        if (*palettePtr != 0) {
            WriteToMessageLog("ddouble palette");
        }
        *palettePtr = pal;
    }
}

/* Function start: 0x40FA30 */
int Handler2::ShutDown(SC_Message* msg) {
    if (sprite != 0) {
        delete sprite;
        sprite = 0;
    }
    if (palette != 0) {
        delete palette;
        palette = 0;
    }
    WriteToMessageLog("EXIT TEACHER SCREEN TO LEAVE\n");
    return 0;
}

/* Function start: 0x40FA90 */
int Handler2::AddMessage(SC_Message* msg) {
    Handler2::WriteMessageAddress(msg);
    if (msg->mouseX >= 2) {
        msg->targetAddress = 3;
        msg->priority = 6;
    }
    return 1;
}

/* Function start: 0x40FAD0 */
int Handler2::Exit(SC_Message* msg) {
    return handlerId == msg->targetAddress;
}

/* Function start: 0x40FAF0 */
void Handler2::Update(int param1, int param2) {
    Sprite* spr;
    Sample* samp;
    int iVar1;

    if (handlerId == param2) {
        spr = sprite;
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
        g_Mouse_00436978->DrawCursor();
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
                samp->Unload();
                operator delete(samp);
                sample = 0;
            }
        }
    }
}

