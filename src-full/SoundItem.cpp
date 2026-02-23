#include "SoundItem.h"
#include "mss.h"
#include "Memory.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>
#include "string.h"

/* Function start: 0x40B5D0 */ /* DEMO ONLY - no full game match */
SoundItem::SoundItem(int sndId)
{
    // Clear the whole object (32 bytes).
    memset(this, 0, sizeof(SoundItem));
    
    soundId = sndId;
    timer.Reset();

    sprintf(g_Buffer_00436960, "audio\\snd%4.4d.wav", soundId);

    Sample* sample = new Sample();
    soundPtr = sample;

    if (sample->Load(g_Buffer_00436960)) {
        if (soundPtr != 0) {
            soundPtr->Unload();
            operator delete(soundPtr);
            soundPtr = 0;
        }
    }
}

/* Function start: 0x40B6E0 */ /* DEMO ONLY - no full game match */
void SoundItem::LogSomething()
{
    WriteToMessageLog("hIam %d", soundId);
}

/* Function start: 0x40B700 */ /* DEMO ONLY - no full game match */
int SoundItem::IsFinished()
{
    unsigned int elapsed;
    Sample* sndPtr;
    HSAMPLE sample;

    if (soundPtr != 0) {
        elapsed = timer.Update();
        if (elapsed <= 60000) {
            sndPtr = soundPtr;
            sample = sndPtr->m_sample;
            if (sample != 0 && sndPtr->m_size == *(int *)((char *)sample + 0xc)) {
                if (AIL_sample_status(sample) == 4) {
                    timer.Reset();
                }
            }
            return 0;
        }
    }
    return 1;
}

/* Function start: 0x40B750 */ /* DEMO ONLY - no full game match */
void SoundItem::Resume()
{
    Sample* sndPtr;

    sndPtr = soundPtr;
    if (sndPtr != 0) {
        sndPtr->Play(100, 0);
    }
}

/* Function start: 0x40B770 */ /* DEMO ONLY - no full game match */
void SoundItem::Start()
{
    Sample* sndPtr;

    sndPtr = soundPtr;
    if (sndPtr != 0) {
        sndPtr->Play(100, 1);
    }
}

/* Function start: 0x40B790 */ /* DEMO ONLY - no full game match */
void SoundItem::AdjustVolume(int delta)
{
    HSAMPLE sample;
    int volume;

    if (soundPtr == 0) return;
    sample = soundPtr->m_sample;
    volume = AIL_sample_volume(sample);
    if (delta + volume == 0) return;
    soundPtr->Fade(volume + delta, 0);
}

/* Function start: 0x40B7C0 */ /* DEMO ONLY - no full game match */
void SoundItem::SetVolume(int volume)
{
    Sample* sndPtr;

    sndPtr = soundPtr;
    if (sndPtr != 0) {
        sndPtr->Fade(volume, 0);
    }
}
