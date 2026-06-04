#include <windows.h>
#include "Timer.h"
#include "Memory.h"
#include "globals.h"

// g_TimerCount_0046a5ac — defined in globals.cpp

/* Function start: 0x421880 */
Timer::Timer()
{
    m_pauseDuration = 0;
    Timer* timer = this;
    timer->m_unknown4 = 0;
    timer->m_startTime = 0;
    timer->m_currentTime = 0;
    timer->m_elapsedTime = 0;
    g_TimerCount_0046a5ac++;
    Reset();
}

/* Function start: 0x4218B0 */
Timer::~Timer()
{
    g_TimerCount_0046a5ac--;
}


/* Function start: 0x4218C0 */
void Timer::Reset()
{
    m_startTime = timeGetTime();
    m_pauseDuration = 0;
}

/* Function start: 0x4218E0 */
unsigned int Timer::Update()
{
    m_currentTime = timeGetTime();
    m_elapsedTime = (m_currentTime - m_startTime) - m_pauseDuration;
    return m_elapsedTime;
}

/* Function start: 0x421900 */
void Timer::Wait(unsigned int delay)
{
    Reset();
    do {
        Update();
    } while (m_elapsedTime < delay);
}

// Dead duplicate of TimeOut::~TimeOut.
void __fastcall DestroyTimerField(int* obj)
{
    Timer* t = (Timer*)obj[2];  // offset 8 from obj
    if (t != 0) {
        t->~Timer();
        FreeMemory(t);
        obj[2] = 0;
    }
}
