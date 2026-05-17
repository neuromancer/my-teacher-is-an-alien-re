#include "SC_ZBuffer.h"
#include "stubs.h"
#include "SpriteAction.h"
#include "ZBufferManager.h"
#include "SoundCommand.h"
#include "ZBuffer.h"
#include "RenderEntry.h"
#include "Memory.h"
#include "globals.h"

extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x414220 */
SC_ZBuffer::SC_ZBuffer() {
    handlerId = 0x22;
    timer.Reset();
}

SC_ZBuffer::~SC_ZBuffer() {
}

/* Function start: 0x414370 */
void SC_ZBuffer::Init(SC_MessageParser* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = ((SpriteAction*)msg)->addressValue;
    }
}

/* Function start: 0x414390 */
int SC_ZBuffer::ShutDown(SC_MessageParser* msg) {
    (void)msg;
    return 0;
}

/* Function start: 0x4143A0 */
void SC_ZBuffer::Update(int param1, int param2) {
  unsigned int elapsed;

  elapsed = timer.Update();
  if (elapsed > 10000) {
    SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
  }
  if (handlerId == param2) {
    ShowError("SC_ZBuffer::Update");
  }
}

/* Function start: 0x414400 */
int SC_ZBuffer::AddMessage(SC_MessageParser* msg) {
    WriteMessageAddress(msg);
    ShowError("\"SC_ZBuffer::AddMessage\"");
    return 1;
}

/* Function start: 0x414420 */
int SC_ZBuffer::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    timer.Reset();

    int instr = action->instruction;
    switch (instr) {
    case 2: {
        g_ZBufferManager_0046aa24->m_flags |= 2;
        g_ZBufferManager_0046aa24->timer.Reset();
        return 1;
    }
    case 0xf: {
        ZBufferManager* zb = g_ZBufferManager_0046aa24;
        if (zb->m_state != 2) {
            zb->m_state = 2;
            ZBQueue* q = zb->m_queueA0;
            if (q->head != 0) {
                q->current = q->head;
                while (q->head != 0) {
                    SoundCommand* obj = (SoundCommand*)q->Pop();
                    if (obj != 0) {
                        delete obj;
                    }
                }
            }
            ZBQueue* q2 = zb->m_queueA4;
            if (q2->head != 0) {
                q2->current = q2->head;
                while (q2->head != 0) {
                    ZBuffer* entry = (ZBuffer*)q2->Pop();
                    if (entry != 0) {
                        entry->CleanUpVBuffer();
                        operator delete(entry);
                    }
                }
            }
            ZBQueue* q3 = zb->m_queue9c;
            if (q3->head != 0) {
                q3->current = q3->head;
                while (q3->head != 0) {
                    RenderEntry* obj = (RenderEntry*)q3->Pop();
                    if (obj != 0) {
                        delete obj;
                    }
                }
            }
            zb->m_palette = 0;
        }
        return 1;
    }
    case 0x10: {
        ZBufferManager* zb = g_ZBufferManager_0046aa24;
        if (zb->m_state != 1) {
            zb->m_state = 1;
            ZBQueue* q = zb->m_queueA0;
            if (q->head != 0) {
                q->current = q->head;
                while (q->head != 0) {
                    SoundCommand* obj = (SoundCommand*)q->Pop();
                    if (obj != 0) {
                        delete obj;
                    }
                }
            }
            ZBQueue* q2 = zb->m_queueA4;
            if (q2->head != 0) {
                q2->current = q2->head;
                while (q2->head != 0) {
                    ZBuffer* entry = (ZBuffer*)q2->Pop();
                    if (entry != 0) {
                        entry->CleanUpVBuffer();
                        operator delete(entry);
                    }
                }
            }
            zb->m_queue9c->ClearList();
            zb->m_palette = 0;
        }
        return 1;
    }
    }
    return 0;
}
