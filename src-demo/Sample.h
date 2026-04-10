#ifndef SAMPLE_H
#define SAMPLE_H

#include "mss.h"
#include "Timer.h"

class Sample {
public:
    Sample();
    ~Sample();
    void Init(int volume);
    void Fade(int volume, unsigned int duration);
    void Stop();
    int Play(int volume, int loop_count);
    int Load(char* filename);
    void Unload();

    // Layout: 0x0=m_data, 0x4=m_size, 0x8=m_field8, 0xC=m_sample
    char* m_data;
    int m_size;
    int m_field8;
    HSAMPLE m_sample;
};

#endif // SAMPLE_H
