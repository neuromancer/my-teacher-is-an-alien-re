#include "TimeOut.h"
#include "string.h"

/* Function start: 0x418F60 */
TimeOut::~TimeOut()
{
    if (m_timer != 0)
    {
        delete m_timer;
        m_timer = 0;
    }
}

/* Function start: 0x418F90 */
TimeOut::TimeOut()
{
    m_timer = new Timer();
    Stop();
}

/* Function start: 0x419010 */
void TimeOut::Stop()
{
    m_isActive = 0;
}

/* Function start: 0x419020 */
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

/* Function start: 0x419060 */
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
