#include "SC_BgSnd.h"
#include "stubs.h"
#include "Sample.h"
#include "SpriteAction.h"
#include "GameState.h"
#include "GameEngine.h"
#include "globals.h"
#include "GameConfig.h"
#include "Memory.h"
#include <mss.h>
#include "string.h"

extern "C" void ShowError(const char* format, ...);
extern "C" char* GetSoundFilename(int handle);

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
        FreeMemory(s);
        snd = 0;
        sndId = 0;
    }
}

/* Function start: 0x439470 */
void SC_BgSnd::Init(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    CopyCommandData(msg);
    if (action != 0) {
        moduleParam = action->addressValue;
    }
}

/* Function start: 0x439490 */
void SC_BgSnd::ShutDown(SC_MessageParser* msg) {
    (void)msg;
    return;
}

/* Function start: 0x4394A0 */
void SC_BgSnd::Update(int p1, int p2) {
    unsigned int elapsed = timer.Update();
    if (elapsed > 60000) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    Sample* s = snd;
    if (s != 0) {
        HSAMPLE hsamp = s->m_sample;
        if (hsamp != 0) {
            if (s->m_size == *(int*)((char*)hsamp + 0x0C)) {
                if (AIL_sample_status(hsamp) == 4) {
                    if (flags & 1) {
                        SC_BgSnd::OnProcessEnd();
                    }
                    timer.Reset();
                }
            }
        }
    }
}

/* Function start: 0x439520 */
int SC_BgSnd::AddMessage(SC_MessageParser* msg) {
    WriteMessageAddress(msg);
    ShowError("\"SC_BgSnd::AddMessage\"");
    return 1;
}

/* Function start: 0x439540 */
int SC_BgSnd::Exit(SC_MessageParser* msg) {
    SpriteAction* action;

    action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }

    timer.Reset();

    switch (action->instruction) {
    case 7:
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    default:
        return 0;
    case 0x10:
        SetVolume(0, 0);
        return 1;
    case 0x11: {
        Sample* s = snd;
        if (s != 0) {
            s->Fade(AIL_sample_volume(s->m_sample) + 10, 0);
            return 1;
        }
        break;
    }
    case 0x12: {
        Sample* s = snd;
        if (s != 0) {
            s->Fade(AIL_sample_volume(s->m_sample) - 10, 0);
            return 1;
        }
        break;
    }
    case 0x13:
        SetVolume(action->extra1, action->extra2);
        return 1;
    case 0x1b:
        AddMessage(action->addressValue);
    }
    return 1;
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
            snd->Stop();
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
    if (g_GameConfig2_0046aa10->data.rawData[2] != 2) return;

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
        char* filename = GetSoundFilename(soundHandle);
        if (newSnd->Load(filename) == 0) {
            if (sndId != 0 && soundHandle != sndId) {
                Sample* old = snd;
                if (old != 0) {
                    old->Unload();
                    FreeMemory(old);
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
        FreeMemory(newSnd);
    }
    SetVolume(0, 0);
}

/* Function start: 0x4398C0 */
void SC_BgSnd::SetVolume(int volume, int duration) {
    if (duration == 0) {
        Sample* s = snd;
        if (volume == 0) {
            if (s != 0) {
                s->Stop();
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
                        if (g_GameEngine_0046a6ec != 0) {
                            timerDiv = g_GameEngine_0046a6ec->m_frameTime;
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
        snd->Stop();
    }
    sndId = 0;
}
