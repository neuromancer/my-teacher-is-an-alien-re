#include "SoundItem.h"
#include "mss.h"
#include "Memory.h"
#include "globals.h"
#include <stdio.h>

/* Function start: 0x40B5D0 */
SoundItem::SoundItem(int sndId)
{
    Sample* sample;
    int result;

    // Store soundId
    soundId = sndId;

    // Reset timer
    timer.Reset();

    // Format filename "audio\snd0001.wav" etc.
    sprintf(g_Buffer_00436960, "audio\\snd%4.4d.wav", soundId);

    // Allocate and construct Sample (0x10 bytes)
    sample = new Sample();
    soundPtr = sample;

    // Load sound file
    result = sample->Load(g_Buffer_00436960);
    if (result != 0 && soundPtr != 0) {
        // Load failed - cleanup
        soundPtr->Unload();
        FreeMemory(soundPtr);
        soundPtr = 0;
    }
}

/* Function start: 0x40B6E0 */
SoundItem::~SoundItem()
{
    if (soundPtr != 0) {
        soundPtr->Unload();
        FreeMemory(soundPtr);
        soundPtr = 0;
    }
}

/* Function start: 0x40B700 */
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

/* Function start: 0x40B750 */
void SoundItem::Resume()
{
    Sample* sndPtr;

    sndPtr = soundPtr;
    if (sndPtr != 0) {
        sndPtr->Play(100, 0);
    }
}

/* Function start: 0x40B770 */
void SoundItem::Start()
{
    Sample* sndPtr;

    sndPtr = soundPtr;
    if (sndPtr != 0) {
        sndPtr->Play(100, 1);
    }
}

/* Function start: 0x40B790 */
void SoundItem::AdjustVolume(int delta)
{
    Sample* sndPtr;
    int volume;

    sndPtr = soundPtr;
    if (sndPtr != 0) {
        volume = AIL_sample_volume(sndPtr->m_sample);
        if (delta + volume) {
            soundPtr->Fade(volume + delta, 0);
        }
    }
}

/* Function start: 0x40B7C0 */
void SoundItem::SetVolume(int volume)
{
    Sample* sndPtr;

    sndPtr = soundPtr;
    if (sndPtr != 0) {
        sndPtr->Fade(volume, 0);
    }
}
