#include <windows.h>
#include "Timer.h"
#include "Memory.h"

// Global variable
#define g_timer_count (*(unsigned int*)0x00436684)

/* Function start: 0x418EB0 */
Timer* Timer::Init()
{
    m_pauseDuration = 0;
    m_unknown4 = 0;
    m_startTime = 0;
    m_currentTime = 0;
    m_elapsedTime = 0;
    g_timer_count++;
    Reset();
    return this;
}

/* Function start: 0x418EE0 */
void Timer_DecrementCounter()
{
    g_timer_count--;
}


/* Function start: 0x418EF0 */
void Timer::Reset()
{
    m_startTime = timeGetTime();
    m_pauseDuration = 0;
}

/* Function start: 0x418F10 */
unsigned int Timer::Update()
{
    m_currentTime = timeGetTime();
    m_elapsedTime = (m_currentTime - m_startTime) - m_pauseDuration;
    return m_elapsedTime;
}

/* Function start: 0x418F30 */
void Timer::Wait(unsigned int delay)
{
    Reset();
    do {
        Update();
    } while (m_elapsedTime < delay);
}

/* Function start: 0x418F50 */
Timer::Timer()
{
    Init();
}
