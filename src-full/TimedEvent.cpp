#include "globals.h"
#include "TimedEvent.h"
#include "SpriteAction.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "SC_OnScreenMessage.h"
#include "Message.h"
#include "string.h"
#include "SC_Question.h"


/* Function start: 0x4229D0 */
TimedEvent::TimedEvent()
{
    memset(this, 0, 10 * sizeof(int));
    m_timer.Reset();
}

/* Function start: 0x422A50 */
TimedEvent::~TimedEvent()
{
    SC_Message* eventData = m_eventData;
    if (eventData) {
        delete eventData;
        m_eventData = 0;
    }
}

/* Function start: 0x422AD0 */
void TimedEvent::SetType(int type)
{
    m_type = type;
}

extern void __fastcall FUN_00444af0(void*);
extern void FreeMemory(void*);
extern void __fastcall FUN_00404230(void*, int, char*, int, int, int, int);
extern char* DAT_0046aa00;
class ZBufferManager;
extern ZBufferManager* DAT_0046aa24;

/* Function start: 0x422AE0 */
int TimedEvent::Update()
{
    int remaining_time = m_duration - m_timer.Update();

    switch (m_type) {
    case 0:
        if (remaining_time > 0) {
            return 0;
        }
        if (m_eventData != 0) {
            EnqueueSpriteAction((void*)m_eventData);
        }
        if (m_eventData != 0) {
            FUN_00444af0(m_eventData);
            FreeMemory(m_eventData);
            m_eventData = 0;
        }
        return 1;

    case 1:
        if (remaining_time > 0) {
            return 0;
        }
        if (m_eventData != 0) {
            EnqueueSpriteAction((void*)m_eventData);
        }
        m_timer.Reset();
        return 0;

    case 2:
        sprintf(DAT_0046aa00, "%3.3d : %2.2d", remaining_time / 60000, (remaining_time / 1000) % 60);
        FUN_00404230(DAT_0046aa24, 0, DAT_0046aa00, 0x208, 0x1c2, 10000, -1);
        if (remaining_time > 0) {
            return 0;
        }
        if (m_eventData != 0) {
            EnqueueSpriteAction((void*)m_eventData);
        }
        if (m_eventData != 0) {
            FUN_00444af0(m_eventData);
            FreeMemory(m_eventData);
            m_eventData = 0;
        }
        return 1;

    default:
        ShowError("illegal type %d, TimedEvent::Update", m_type);
        return 1;
    }
}
