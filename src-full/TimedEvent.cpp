#include "globals.h"
#include "TimedEvent.h"
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

/* Function start: 0x4019A0 */ /* DEMO ONLY - no full game match */
int TimedEvent::Update()
{
    int remaining_time = m_duration - m_timer.Update();
    TimedEventPool* pool;

    switch (m_type) {
    case 0:
        if (remaining_time > 0) {
            return 0;
        }
        {
            void* eventData = m_eventData;
            if (eventData) {
                pool = g_TimedEventPool2_00436988;
                PooledEvent* node = pool->Create((void*)pool->list.tail, 0);
                node->GetEmbeddedEvent()->CopyFrom((PooledEvent*)eventData);

                if (pool->list.tail == 0) {
                    pool->list.head = node;
                } else {
                    pool->list.tail->next = node;
                }
                pool->list.tail = node;
            }

            eventData = m_eventData;
            if (eventData) {
                delete m_eventData;
                m_eventData = 0;
            }
        }
        return 1;

    case 1:
        if (remaining_time > 0) {
            return 0;
        }
        {
            void* eventData = m_eventData;
            if (eventData) {
                pool = g_TimedEventPool2_00436988;
                PooledEvent* node = pool->Create((void*)pool->list.tail, 0);
                node->GetEmbeddedEvent()->CopyFrom((PooledEvent*)eventData);

                if (pool->list.tail == 0) {
                    pool->list.head = node;
                } else {
                    pool->list.tail->next = node;
                }
                pool->list.tail = node;
            }
        }
        m_timer.Reset();
        return 0;

    case 2:
        sprintf(g_Buffer_00436960, "%3.3d : %2.2d", remaining_time / 60000, (remaining_time / 1000) % 60);
        g_ZBufferManager_0043698c->ShowSubtitle(g_Buffer_00436960, 0x208, 0x1c2, 10000, 8);

        if (remaining_time > 0) {
            return 0;
        }
        {
            void* eventData = m_eventData;
            if (eventData) {
                pool = g_TimedEventPool2_00436988;
                PooledEvent* node = pool->Create((void*)pool->list.tail, 0);
                node->GetEmbeddedEvent()->CopyFrom((PooledEvent*)eventData);

                if (pool->list.tail == 0) {
                    pool->list.head = node;
                } else {
                    pool->list.tail->next = node;
                }
                pool->list.tail = node;
            }

            eventData = m_eventData;
            if (eventData) {
                delete m_eventData;
                m_eventData = 0;
            }
        }
        return 1;

    default:
        ShowError("illegal type %d, TimedEvent::Update", m_type);
        return 1;
    }
}

