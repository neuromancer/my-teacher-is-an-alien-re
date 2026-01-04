#include "OnScreenMessage.h"
#include "SoundManager.h"
#include "globals.h"

/* Function start: 0x40A290 */
int OnScreenMessage::Update(int index)
{
    unsigned int dur;

    if (this->text[0] != '\0') {
        int y = (index * 4 + 0x14) * 5;
        DAT_0043698c->ShowSubtitle(this->text, 0x32, y, 10000, 8);
    }

    dur = this->duration;
    unsigned int elapsed = this->timer.Update();
    return dur < elapsed;
}
