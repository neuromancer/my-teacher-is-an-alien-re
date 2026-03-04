#include "SC_BgSnd.h"
#include "Sample.h"
#include "GameState.h"
#include <mss.h>
#include "string.h"

extern "C" void ShowError(const char* format, ...);
extern "C" char* FUN_0044e530(int handle);
extern GameState* g_GameState_0046aa30;
extern "C" extern void* DAT_0046aa10;
extern "C" extern int DAT_0046a6ec;

/* Function start: 0x4392E0 */
SC_BgSnd::SC_BgSnd() {
    memset(&flags, 0, 40);
    handlerId = 5;
    timer.Reset();
}

/* Function start: 0x4393D0 */
SC_BgSnd::~SC_BgSnd() {
    Sample* s = snd;
    if (s != 0) {
        s->Unload();
        operator delete(s);
        snd = 0;
        sndId = 0;
    }
}

/* Function start: 0x439690 */
void SC_BgSnd::OnProcessEnd() {
    Sample* s = snd;
    if (s == 0) return;
    HSAMPLE hsamp = s->m_sample;
    if (hsamp == 0) return;
    if (s->m_size != *(int *)((char *)hsamp + 0xc)) return;
    if (AIL_sample_status(hsamp) != 4) return;
    if ((flags & 1) == 0) return;

    int currentVol = AIL_sample_volume(snd->m_sample);
    if (targetVol == currentVol) {
        if (currentVol == 0) {
            snd->~Sample();
            sndId = 0;
        }
        flags &= ~1;
        return;
    }

    int newVol = currentVol + volStep;
    if (newVol < 0 || newVol > 100) {
        newVol = targetVol;
    }
    if (volStep > 0) {
        if (targetVol >= newVol) {
            targetVol = newVol;
        }
    } else {
        if (targetVol <= newVol) {
            targetVol = newVol;
        }
    }
    snd->Fade(targetVol, 0);
}

/* Function start: 0x439740 */
void SC_BgSnd::AddMessage(int soundHandle) {
    if (((char*)DAT_0046aa10)[0x46] != 2) return;

    GameState* gs = g_GameState_0046aa30;
    int idx = gs->FindState("GS_MUSICOFF");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] != 0) return;

    if (sndId == soundHandle && snd != 0) {
        snd->Fade(100, 0);
        return;
    }

    Sample* newSnd = new Sample();
    if (newSnd != 0) {
        char* filename = FUN_0044e530(soundHandle);
        if (newSnd->Load(filename) == 0) {
            if (sndId != 0 && soundHandle != sndId) {
                Sample* old = snd;
                if (old != 0) {
                    old->Unload();
                    operator delete(old);
                    snd = 0;
                }
            }
            snd = newSnd;
            sndId = soundHandle;
            if (newSnd != 0) {
                newSnd->Play(100, 0);
            }
            return;
        }
    }
    if (newSnd != 0) {
        newSnd->Unload();
        operator delete(newSnd);
    }
    SetVolume(0, 0);
}

/* Function start: 0x4398C0 */
void SC_BgSnd::SetVolume(int volume, int duration) {
    if (duration == 0) {
        Sample* s = snd;
        if (volume == 0) {
            if (s != 0) {
                s->~Sample();
            }
            sndId = 0;
            return;
        }
        if (s == 0) return;
        s->Fade(volume, 0);
        return;
    }

    Sample* s = snd;
    if (s != 0) {
        HSAMPLE hsamp = s->m_sample;
        if (hsamp != 0) {
            if (s->m_size == *(int *)((char *)hsamp + 0xc)) {
                if (AIL_sample_status(hsamp) == 4) {
                    if ((flags & 1) == 0) {
                        int currentVol = AIL_sample_volume(snd->m_sample);
                        targetVol = volume;
                        int timerDiv;
                        if (DAT_0046a6ec != 0) {
                            timerDiv = *(int *)(DAT_0046a6ec + 0x1c);
                            if (timerDiv != 0) goto calc;
                        }
                        timerDiv = 0x54;
                    calc:
                        {
                            unsigned int numFrames = (unsigned int)duration / (unsigned int)timerDiv;
                            int step = (volume - currentVol) / (int)numFrames;
                            volStep = step;
                            if (step == 0) {
                                step = 1;
                                if (volume < currentVol) {
                                    step = -1;
                                }
                                volStep = step;
                            }
                            flags |= 1;
                        }
                        return;
                    }
                }
            }
        }
    }
    if (snd != 0) {
        snd->~Sample();
    }
    sndId = 0;
}
