#include "TimeOut.h"

extern "C" {
    void ShowError(const char*);
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
        ShowError((const char*)0x436688);
    }

    m_field0 = 1;
    m_timer.Reset();
}

/* Function start: 0x419060 */
int TimeOut::IsTimeOut()
{
    if (m_field0 == 1)
    {
        if ((unsigned int)m_timeout <= m_timer.Update())
        {
            return 1;
        }
    }
    return 0;
}
