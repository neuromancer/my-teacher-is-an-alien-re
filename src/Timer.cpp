#include "Timer.h"

// Global variable
#define g_timer_count (*(unsigned int*)0x00436684)

/* Function start: 0x418EB0 */
Timer* Timer::Init()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_field10 = 0;
    g_timer_count++;
    Reset();
    return this;
}

/* Function start: 0x418EF0 */
void Timer::Reset()
{
    // TODO: Implement
}
