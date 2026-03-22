/* SoundItem - all method implementations are demo-only.
   The class declaration remains in SoundItem.h for use by SC_Sound.
   Full game SoundItem methods are inlined or differ from demo versions. */

#include "SoundItem.h"

SoundItem::SoundItem(int sndId) {
    soundId = sndId;
    soundPtr = 0;
}

void SoundItem::Start() {}
void SoundItem::Resume() {}
void SoundItem::AdjustVolume(int delta) {}
void SoundItem::SetVolume(int volume) {}
