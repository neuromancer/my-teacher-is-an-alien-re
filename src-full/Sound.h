#ifndef SOUND_H
#define SOUND_H

#include "mss.h"

class Sound {
public:
    Sound(int param_1, short param_2, int param_3);
    void AllocateSampleHandles();
    HSAMPLE FindFreeSampleHandle();
    void StopAllSamples();
    int OpenDigitalDriver(int rate, unsigned short bits, unsigned short channels);

    short num_samples;        // 0x00
    short pad_02;             // 0x02 - explicit padding to align samples to 0x04
    HSAMPLE samples[13];      // 0x04-0x37 (52 bytes)
    HDIGDRIVER digital_driver; // 0x38
};

#endif // SOUND_H
