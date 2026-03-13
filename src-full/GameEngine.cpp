#include "GameEngine.h"
#include "Handler.h"
#include "GameState.h"
#include "InputManager.h"
#include "Message.h"
#include "RenderEntry.h"
#include "Sample.h"
#include "SoundCommand.h"
#include "SpriteAction.h"
#include "ZBufferManager.h"
#include "smack.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new.h>

// External globals (C++ linkage - defined in stubs.cpp without extern "C")
extern void* DAT_0046aa24;   // ZBufferManager*
extern void* DAT_0046aa08;   // InputManager*
extern void* DAT_0046aa18;
extern char* DAT_0046aa00;   // string buffer

// GameState pointer
extern "C" extern void* DAT_0046aa30;
extern "C" int DAT_0046ac04;
extern char* DAT_0046aa2c;
extern GameState* DAT_0046aa3c;
extern GameState* g_StringTable_0046aa34;

// External functions
extern void __fastcall FUN_004047c0(void*);                              // ProcessRenderQueues on DAT_0046aa24
extern void __fastcall FUN_00404b90(void*);                              // UpdateScreen on DAT_0046aa24
extern void __fastcall FUN_00404230(void*, int, char*, int, int, int, int); // ShowSubtitle on DAT_0046aa24

extern void __cdecl FUN_00425c50(char*, ...);   // ShowError
extern "C" void FUN_00444e20(void*);
extern void __fastcall FUN_00426a90(void*);

// FUN_00426ce0 is thiscall with 1 param (not fastcall)
class InputObj {
public:
    int Refresh(int param);  // 0x426CE0
};
Handler* CreateHandler(int command);

class GameLoopHelper {
public:
    void PostProcess();
};

extern GameLoopHelper* g_GameLoopHelper;
extern "C" void WriteToLog(const char* format, ...);

/* Function start: 0x430A00 */
GameEngine::GameEngine() {
    memset(&m_exitFlag, 0, sizeof(GameEngine));

    m_timer1 = new Timer();
    m_timer2 = new Timer();
    m_handlerList = new LinkedList();
    m_activeHandler = 0;
    m_eventPool = new TimedEventPool(10);
    m_frameTime = 0x54;
    m_frameTimeCopy = 0x54;
}

/* Function start: 0x430B90 */
GameEngine::~GameEngine() {
    Timer* t1 = m_timer1;
    if (t1 != 0) {
        t1->~Timer();
        operator delete(t1);
        m_timer1 = 0;
    }
    Timer* t2 = m_timer2;
    if (t2 != 0) {
        t2->~Timer();
        operator delete(t2);
        m_timer2 = 0;
    }
    LinkedList* list = m_handlerList;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                void* data = list->RemoveCurrent();
                if (data != 0) {
                    delete (Handler*)data;
                }
            }
        }
        FreeMemory(list);
        m_handlerList = 0;
    }
    m_activeHandler = 0;
}

/* Function start: 0x430C90 */
void GameEngine::Init() {
    m_exitFlag = 0;
    if (m_timer2 != 0) {
        m_timer2->Reset();
    }
    if (m_timer1 != 0) {
        m_timer1->Reset();
    }
}

/* Function start: 0x430CC0 */
void GameEngine::CleanupLoop() {
    if (m_activeHandler != 0) {
        ((Handler*)m_activeHandler)->ShutDown(0);
    }
}

/* Function start: 0x430CD0 */
void GameEngine::RunGameLoop() {
    Init();
    if (m_exitFlag == 0) {
        do {
            ProcessInput();
            if (m_exitFlag != 0) break;
            ProcessEvents();
            if (m_exitFlag != 0) break;
            UpdateHandlers();

            FUN_004047c0(DAT_0046aa24);

            if (m_smackHandle != 0) {
                int result = SmackWait((HSMACK)m_smackHandle);
                while (result != 0) {
                    result = SmackWait((HSMACK)m_smackHandle);
                }
            } else {
                unsigned int elapsed = m_timer1->Update();
                while (elapsed < m_frameTime) {
                    SmackSoundCheck();
                    elapsed = m_timer1->Update();
                }
            }

            int* gs = (int*)DAT_0046aa30;
            if (gs[0x98 / 4] - 1 < 4) {
                FUN_00425c50("Invalid gamestate %d", 4);
            }

            if (*(int*)(gs[0x90 / 4] + 0x10) != 0) {
                int mouseX = 0;
                int mouseY = 0;
                int* device = *(int**)((char*)DAT_0046aa08 + 0x1a0);
                if (device != 0) {
                    mouseY = device[1];
                }
                if (device != 0) {
                    mouseX = device[0];
                } else {
                    mouseX = 0;
                }
                sprintf(DAT_0046aa00, "FT %d   %d %d ", m_timer1->Update(), mouseX, mouseY);
                FUN_00404230(DAT_0046aa24, 0, DAT_0046aa00, 0x14, 0x1e, 10000, -1);
            }

            m_timer1->Reset();
            FUN_00404b90(DAT_0046aa24);

        } while (m_exitFlag == 0);
    }
    CleanupLoop();
}

/* Function start: 0x430E10 */
void GameEngine::ProcessInput() {
    int hasInput;
    InputState* mouse;
    int* actionData;
    InputState** pMouse;

    m_exitFlag |= ((InputObj*)DAT_0046aa08)->Refresh(1);
    if (m_exitFlag != 0) {
        return;
    }

    mouse = ((InputManager*)DAT_0046aa08)->pMouse;
    if (mouse != 0 && (mouse->ext1 >= 1 || mouse->ext2 >= 1 || DAT_0046ac04 != 0)) {
        hasInput = 1;
    } else {
        hasInput = 0;
    }
    if (hasInput == 0) {
        return;
    }

    SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    action.field_08 = 1;

    if (DAT_0046ac04 != 0) {
        action.field_2C = WaitForInput();
    }

    pMouse = &((InputManager*)DAT_0046aa08)->pMouse;
    if (*pMouse == 0) {
        action.field_24 = 0;
    } else {
        action.field_24 = (*pMouse)->ext1;
    }
    if (*pMouse == 0) {
        action.field_28 = 0;
    } else {
        action.field_28 = (*pMouse)->ext2;
    }
    mouse = *pMouse;
    int mouseY = 0;
    int mouseX;
    if (mouse != 0) {
        mouseY = mouse->y;
    }
    if (mouse != 0) {
        mouseX = mouse->x;
    } else {
        mouseX = 0;
    }
    action.dim.field_0 = mouseX;
    action.dim.field_4 = mouseY;

    mouse = *pMouse;
    if (mouse != 0 && (mouse->ext1 >= 2 || mouse->ext2 >= 2)) {
        if (*(void**)((char*)DAT_0046aa18 + 0xa4) != 0) {
            ((Sample*)*(void**)((char*)DAT_0046aa18 + 0xa4))->Play(100, 1);
        }
    }

    if (m_activeHandler != 0) {
        ((Handler*)m_activeHandler)->AddMessage((SC_Message*)&action);
    }

    actionData = (int*)&action;
    if (actionData[0] != 0 && actionData[4] != 0) {
        FUN_00444e20(&action);
    }
}

/* Function start: 0x430FA0 */
int GameEngine::ProcessEvents() {
    int count;

    count = 0;
    while (m_eventPool->m_count != 0) {
        SpriteAction action;

        ProcessMessage((SC_Message*)m_eventPool->PopSafe(&action));
        count = count + 1;
    }

    return count;
}

/* Function start: 0x431030 */
void GameEngine::ProcessMessage(SC_Message* msg) {
    int handled;
    int* msgData;

    handled = 0;
    msgData = (int*)msg;

    if (msgData[4] == 4) {
        handled = 1;
        if (msgData[0] != 1) {
            HandleSystemMessage(msg);
        } else {
            strcpy(DAT_0046aa2c, DAT_0046aa3c->GetState(msgData[1]));
        }
    } else if (msgData[0] == 0) {
        handled = 1;
    } else if (msgData[0] == 1) {
        handled = 1;
        ProcessControlMessage(msg);
    } else {
        if (m_activeHandler != 0) {
            handled = ((Handler*)m_activeHandler)->Exit(msg);
        }
        if (handled == 0) {
            m_handlerList->current = m_handlerList->head;
            if (m_handlerList->current != 0) {
                do {
                    void* data;

                    data = m_handlerList->current->data;
                    if (data == 0) {
                        break;
                    }

                    handled = ((Handler*)m_handlerList->GetCurrentData())->Exit(msg);
                    if (handled != 0) {
                        break;
                    }

                    if (m_handlerList->tail == m_handlerList->current) {
                        break;
                    }
                    if (m_handlerList->current != 0) {
                        m_handlerList->current = m_handlerList->current->next;
                    }
                } while (m_handlerList->current != 0);
            }
        }
    }

    if (handled == 0) {
        GetOrCreateHandler(msgData[0])->Exit(msg);
    }
}

/* Function start: 0x431160 */
int GameEngine::ProcessControlMessage(SC_Message* msg) {
    int* msgData;

    msgData = (int*)msg;
    if (msgData[0] != 1) {
        return 0;
    }

    switch (msgData[4]) {
    case 5:
        m_exitFlag = 1;
        return 1;
    case 0x13:
        m_frameTimeCopy = m_frameTime;
        m_smackHandle = 0;
        m_frameTime = msgData[1];
        return 1;
    case 0x17:
        GetOrCreateHandler(msgData[1]);
        return 1;
    case 0x18:
        RemoveHandler(msgData[1]);
        return 1;
    default:
        return 0;
    }
}

/* Function start: 0x431210 */
void GameEngine::UpdateHandlers() {
    m_handlerList->current = m_handlerList->head;
    if (m_handlerList->current == 0) return;

    do {
        void* data = m_handlerList->current->data;
        if (data == 0) return;

        if (m_activeHandler != 0) {
            ((Handler*)m_handlerList->GetCurrentData())->Update(0, ((Handler*)m_activeHandler)->handlerId);
        } else {
            ((Handler*)m_handlerList->GetCurrentData())->Update(0, 0);
        }

        if (m_handlerList->tail == m_handlerList->current) return;
        if (m_handlerList->current != 0) {
            m_handlerList->current = m_handlerList->current->next;
        }
    } while (m_handlerList->current != 0);
}

/* Function start: 0x431290 */
void GameEngine::HandleSystemMessage(SC_Message* msg) {
    Handler* handler;
    Handler* foundHandler;
    ZBufferManager* manager;
    ZBQueue* queue;
    int* msgData;

    if (msg == 0) {
        return;
    }

    msgData = (int*)msg;
    if (DAT_0046aa08 != 0) {
        FUN_00426a90(DAT_0046aa08);
    }

    handler = (Handler*)m_activeHandler;
    if (handler != 0) {
        handler->ShutDown(msg);
    }

    manager = (ZBufferManager*)DAT_0046aa24;
    if (manager != 0) {
        queue = manager->m_queueA0;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                void* data;

                data = queue->Pop();
                if (data != 0) {
                    delete (SoundCommand*)data;
                }
            }
        }

        queue = manager->m_queueA4;
        if (queue->head != 0) {
            queue->current = queue->head;
            while (queue->head != 0) {
                void* data;

                data = queue->Pop();
                if (data != 0) {
                    delete (RenderEntry*)data;
                }
            }
        }

        manager->m_queue9c->ClearList();
        manager->m_palette = 0;
    }

    foundHandler = FindHandlerInList(msgData[0]);
    if (foundHandler == 0) {
        handler = GetOrCreateHandler(msgData[0]);
        m_activeHandler = handler;
    } else {
        LinkedList* list;

        handler = (Handler*)((EventList*)m_handlerList)->RemoveCurrent();
        list = m_handlerList;
        m_activeHandler = handler;
        if (handler == 0) {
            FUN_00425c50("queue fault 0101");
        }
        list->current = list->head;
        if (list->type != 1 && list->type != 2) {
            ((EventList*)list)->InsertNode(handler);
        } else if (list->head == 0) {
            ((EventList*)list)->InsertNode(handler);
        } else {
            while (list->current != 0) {
                Handler* currentHandler;

                currentHandler = (Handler*)list->current->data;
                if (currentHandler->handlerId < handler->handlerId) {
                    ((EventList*)list)->InsertNode(handler);
                    break;
                }
                if (list->tail == list->current) {
                    list->PushNode(handler);
                    break;
                }
                if (list->current != 0) {
                    list->current = list->current->next;
                }
            }
        }
    }

    if (g_GameLoopHelper != 0) {
        GameState* gs;

        gs = (GameState*)DAT_0046aa30;
        if (gs->maxStates - 1 < 4) {
            FUN_00425c50("Invalid gamestate %d", 4);
        }
        if (gs->stateValues[4] != 0) {
            handler = (Handler*)m_activeHandler;
            if (handler != 0) {
                WriteToLog("Switching to modual %s", g_StringTable_0046aa34->GetState(handler->handlerId));
                g_GameLoopHelper->PostProcess();
            }
        }
    }

    handler = (Handler*)m_activeHandler;
    if (handler == 0) {
        FUN_00425c50("missing modual %d", msgData[0]);
        return;
    }

    handler->Init(msg);
}

/* Function start: 0x431560 */
int GameEngine::AddHandler(Handler* handler) {
    LinkedList* list;

    if (handler == 0) {
        FUN_00425c50("illegal modual insertion");
    }

    list = m_handlerList;
    if (handler == 0) {
        FUN_00425c50("queue fault 0103");
    }

    list->current = list->head;
    while (list->current != 0) {
        Handler* currentHandler;

        currentHandler = (Handler*)list->GetCurrentData();
        if (currentHandler->handlerId == handler->handlerId) {
            FUN_00425c50("illegal modual insertion double");
            return 0;
        }

        if (list->tail == list->current) {
            break;
        }
        if (list->current != 0) {
            list->current = list->current->next;
        }
    }

    list = m_handlerList;
    if (handler == 0) {
        FUN_00425c50("queue fault 0102");
    }
    list->current = list->head;
    if (list->type != 1 && list->type != 2) {
        ((EventList*)list)->InsertNode(handler);
    } else if (list->head == 0) {
        ((EventList*)list)->InsertNode(handler);
    } else {
        do {
            Handler* currentHandler;

            currentHandler = (Handler*)list->current->data;
            if (currentHandler->handlerId < handler->handlerId) {
                list->InsertNode(handler);
                return 1;
            }
            if (list->tail == list->current) {
                list->PushNode(handler);
                return 1;
            }
            if (list->current != 0) {
                list->current = list->current->next;
            }
        } while (list->current != 0);
    }

    return 1;
}

/* Function start: 0x4317C0 */
int GameEngine::RemoveHandler(int command) {
    Handler* handler;
    LinkedList* list;

    handler = FindHandlerInList(command);
    if (handler != 0) {
        list = m_handlerList;
        handler = (Handler*)list->RemoveCurrent();
        if (m_activeHandler == handler) {
            m_activeHandler = 0;
        }
        if (handler != 0) {
            delete handler;
        }
        return 1;
    }
    return 0;
}

/* Function start: 0x431880 */
Handler* GameEngine::GetOrCreateHandler(int command) {
    Handler* handler;

    handler = CreateHandler(command);
    if (handler != 0) {
        AddHandler(handler);
    }
    return handler;
}

/* Function start: 0x4318B0 */
Handler* GameEngine::FindHandlerInList(int command) {
    if (m_handlerList == 0) {
        return 0;
    }

    m_handlerList->current = m_handlerList->head;
    if (m_handlerList->head == 0) {
        return 0;
    }

    do {
        Handler* handler;

        handler = (Handler*)m_handlerList->GetCurrentData();
        if (handler->handlerId == command) {
            if (m_handlerList->current == 0) {
                return 0;
            }
            return (Handler*)m_handlerList->current->data;
        }

        if (m_handlerList->tail == m_handlerList->current) {
            break;
        }
        if (m_handlerList->current != 0) {
            m_handlerList->current = m_handlerList->current->next;
        }
    } while (m_handlerList->head != 0);

    return 0;
}

/* Function start: 0x431A90 */
void GameEngine::EnqueueAction(SpriteAction* action) {
    int* pool;
    int* node;
    int oldTail;
    SpriteAction* saPtr;
    int i;

    pool = (int*)m_eventPool;
    oldTail = pool[1];
    if (pool[3] == 0) {
        int* block;
        int count;
        int* entry;

        block = (int*)malloc(pool[5] * 0x40 + 4);
        *block = pool[4];
        pool[4] = (int)block;
        count = pool[5];
        entry = block + count * 0x10 - 0xf;
        count = count - 1;
        while (count >= 0) {
            *entry = pool[3];
            pool[3] = (int)entry;
            entry = entry - 0x10;
            count = count - 1;
        }
    }

    node = (int*)pool[3];
    saPtr = (SpriteAction*)(node + 2);
    pool[3] = *node;
    node[1] = oldTail;
    node[0] = 0;
    pool[2] = pool[2] + 1;
    memset(node + 2, 0, sizeof(SpriteAction));

    i = 0;
    do {
        if (saPtr != 0) {
            new ((void*)saPtr) SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        }
        saPtr = (SpriteAction*)((char*)saPtr + sizeof(SpriteAction));
    } while (i-- != 0);

    ((SpriteAction*)(node + 2))->CopyFrom(action);

    if (pool[1] == 0) {
        pool[0] = (int)node;
    } else {
        *(int*)pool[1] = (int)node;
    }
    pool[1] = (int)node;
}
