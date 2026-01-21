#include "OnScreenMessage.h"
#include "SoundManager.h"
#include "globals.h"
#include <string.h>
#include <new.h>

/* Function start: 0x40A1D0 */
OnScreenMessage::OnScreenMessage(char* message, unsigned int dur)
{
    Timer* t = new(&timer) Timer();
    memset(this, 0, sizeof(OnScreenMessage));
    if (message != 0) {
        strcpy(text, message);
    }
    t->Reset();
    OnScreenMessage::duration = dur;
    if (dur < 0xfb) {
        OnScreenMessage::duration = 0xfa;
    }
}

/* Function start: 0x40A290 */
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
