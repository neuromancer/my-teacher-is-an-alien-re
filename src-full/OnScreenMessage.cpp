#include "OnScreenMessage.h"
#include "SoundManager.h"
#include "globals.h"
#include <string.h>
#include <new.h>

/* Function start: 0x40A1D0 */ /* DEMO ONLY - no full game match */
OnScreenMessage::OnScreenMessage(char* message, unsigned int dur)
{
    memset(this, 0, sizeof(OnScreenMessage));
    if (message != 0) {
        strcpy(text, message);
    }
    timer.Reset();
    duration = dur;
    if (dur <= 250) {
        duration = 250;
    }
}


/* Function start: 0x40A290 */ /* DEMO ONLY - no full game match */
int OnScreenMessage::Update(int index)
{
    unsigned int dur;

    if (text[0] != '\0') {
        int y = (index * 4 + 0x14) * 5;
        g_ZBufferManager_0043698c->ShowSubtitle(text, 0x32, y, 10000, 8);
    }

    dur = duration;
    unsigned int elapsed = timer.Update();
    return dur < elapsed;
}
