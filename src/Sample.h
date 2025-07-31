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
    HSAMPLE m_sample;

private:
    void* m_data;
    int m_field4;
    int m_field8;

};

#endif // SAMPLE_H
