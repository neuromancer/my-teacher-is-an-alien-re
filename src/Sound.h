#ifndef SOUND_H
#define SOUND_H

#include "mss.h"

class Sound {
public:
    Sound(int param_1, unsigned short param_2, short param_3);
    void AllocateSampleHandles();
    HSAMPLE FindFreeSampleHandle();
    void StopAllSamples();

    short num_samples;
    HSAMPLE samples[13];
    char pad[0x38 - 2 - sizeof(HSAMPLE) * 13];
    HDIGDRIVER digital_driver;
};

#endif // SOUND_H
