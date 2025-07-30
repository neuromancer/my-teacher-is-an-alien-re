#include <windows.h>
#include "Timer.h"

extern "C" {
    void FreeFromGlobalHeap(void*);
}

// Global variable
#define g_timer_count (*(unsigned int*)0x00436684)

/* Function start: 0x418F50 */
Timer::Timer()
{
    Init();
}

/* Function start: 0x418F60 */
Timer::~Timer()
{
    Timer* pThis = this;
    void* pField8 = (void*)pThis->m_field8;
    if (pField8)
    {
        Timer_DecrementCounter();
        FreeFromGlobalHeap(pField8);
        pThis->m_field8 = 0;
    }
}

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

/* Function start: 0x418EE0 */
void Timer_DecrementCounter()
{
    g_timer_count--;
}

/* Function start: 0x418F30 */
void Timer::Wait(unsigned int delay)
{
    Reset();
    do {
        Update();
    } while (m_field10 < delay);
}

/* Function start: 0x418F10 */
unsigned int Timer::Update()
{
    m_fieldC = timeGetTime();
    m_field10 = (m_fieldC - m_field8) - m_field0;
    return m_field10;
}

/* Function start: 0x418EF0 */
void Timer::Reset()
{
    m_field8 = timeGetTime();
    m_field0 = 0;
}
