#include "TimeOut.h"
#include "string.h"

/* Function start: 0x418F60 */ /* DEMO ONLY - no full game match */
TimeOut::~TimeOut()
{
    if (m_timer != 0)
    {
        delete m_timer;
        m_timer = 0;
    }
}

/* Function start: 0x421960 */
TimeOut::TimeOut()
{
    m_timer = new Timer();
    Stop();
}

/* Function start: 0x4219E0 */
void TimeOut::Stop()
{
    m_isActive = 0;
}

/* Function start: 0x4219F0 */ /* ~94% match */
void TimeOut::Start(int timeout)
{
    if (timeout != 0)
    {
        m_timeout = timeout;
    }

    if (m_timeout == 0)
    {
        ShowError("TimeOut::Start() - Must Set a timeout value");
    }

    m_isActive = 1;
    m_timer->Reset();
}

/* Function start: 0x421A30 */
int TimeOut::IsTimeOut()
{
    if (m_isActive == 1)
    {
        if (m_timer->Update() >= (unsigned int)m_timeout)
        {
            return 1;
        }
    }
    return 0;
}
