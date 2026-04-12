#include "GameEngine.h"
#include "Handler.h"
#include "GameState.h"
#include "InputManager.h"
#include "Message.h"
#include "RenderEntry.h"
#include "DrawEntry.h"
#include "Sample.h"
#include "SoundCommand.h"
#include "SpriteAction.h"
#include "ZBufferManager.h"
#include "MouseControl.h"
#include "GameLoop.h"
#include "globals.h"
#include "smack.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new.h>
#include <windows.h>

// External functions
#include "GameLoopHelper.h"
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

            (g_ZBufferManager_0046aa24)->ProcessRenderQueues();

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

            GameState* gs = g_GameState_0046aa30;
            if (gs->maxStates - 1 < 4) {
                ShowError("Invalid gamestate %d", 4);
            }

            if (gs->stateValues[4] != 0) {
                int mouseX = 0;
                int mouseY = 0;
                InputState* mouse = g_InputManager_0046aa08->pMouse;
                if (mouse != 0) {
                    mouseY = mouse->y;
                }
                if (mouse != 0) {
                    mouseX = mouse->x;
                } else {
                    mouseX = 0;
                }
                sprintf(g_Buffer_0046aa00, "FT %d   %d %d ", m_timer1->Update(), mouseX, mouseY);
                (g_ZBufferManager_0046aa24)->ShowSubtitle(g_Buffer_0046aa00, 0x14, 0x1e, 10000, -1);
            }

            m_timer1->Reset();
            (g_ZBufferManager_0046aa24)->UpdateScreen();

        } while (m_exitFlag == 0);
    }
    CleanupLoop();
}

/* Function start: 0x430E10 */
void GameEngine::ProcessInput() {
    int hasInput;
    InputState* mouse;
    InputState** pMouse;

    m_exitFlag |= (g_InputManager_0046aa08)->Refresh(1);
    if (m_exitFlag != 0) {
        return;
    }

    mouse = (g_InputManager_0046aa08)->pMouse;
    if (mouse != 0 && (mouse->ext1 >= 1 || mouse->ext2 >= 1 || g_WaitForInputValue_0046ac04 != 0)) {
        hasInput = 1;
    } else {
        hasInput = 0;
    }
    if (hasInput == 0) {
        return;
    }

    SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    action.fromType = 1;

    if (g_WaitForInputValue_0046ac04 != 0) {
        action.lastKey = WaitForInput();
    }

    pMouse = &(g_InputManager_0046aa08)->pMouse;
    if (*pMouse == 0) {
        action.button1 = 0;
    } else {
        action.button1 = (*pMouse)->ext1;
    }
    if (*pMouse == 0) {
        action.button2 = 0;
    } else {
        action.button2 = (*pMouse)->ext2;
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
    action.mousePos.x = mouseX;
    action.mousePos.y = mouseY;

    mouse = *pMouse;
    if (mouse != 0 && (mouse->ext1 >= 2 || mouse->ext2 >= 2)) {
        if (g_Mouse_0046aa18->m_audio != 0) {
            g_Mouse_0046aa18->m_audio->Play(100, 1);
        }
    }

    if (m_activeHandler != 0) {
        ((Handler*)m_activeHandler)->AddMessage((SC_Message*)&action);
    }

    if (action.addressType != 0 && action.instruction != 0) {
        EnqueueSpriteAction((void*)&action);
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

    //if (count > 0) {
    //}
    return count;
}

/* Function start: 0x431030 */
void GameEngine::ProcessMessage(SC_Message* msg) {
    int handled;
    SpriteAction* action;

    handled = 0;
    action = (SpriteAction*)msg;

    if (action->instruction == 4) {
        handled = 1;
        if (action->addressType != 1) {
            HandleSystemMessage(msg);
        } else {
            strcpy(g_StateString_0046aa2c, g_GameState2_0046aa3c->GetState(action->addressValue));
        }
    } else if (action->addressType == 0) {
        handled = 1;
    } else if (action->addressType == 1) {
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
        GetOrCreateHandler(action->addressType)->Exit(msg);
    }
}

/* Function start: 0x431160 */
int GameEngine::ProcessControlMessage(SC_Message* msg) {
    SpriteAction* action;

    action = (SpriteAction*)msg;
    if (action->addressType != 1) {
        return 0;
    }

    switch (action->instruction) {
    case 5:
        m_exitFlag = 1;
        return 1;
    case 0x13:
        m_frameTimeCopy = m_frameTime;
        m_smackHandle = 0;
        m_frameTime = action->addressValue;
        return 1;
    case 0x17:
        GetOrCreateHandler(action->addressValue);
        return 1;
    case 0x18:
        RemoveHandler(action->addressValue);
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
    SpriteAction* action;

    if (msg == 0) {
        return;
    }

    action = (SpriteAction*)msg;
    if (g_InputManager_0046aa08 != 0) {
        (g_InputManager_0046aa08)->ResetClickState();
    }

    handler = (Handler*)m_activeHandler;
    if (handler != 0) {
        handler->ShutDown(msg);
    }

    manager = g_ZBufferManager_0046aa24;
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
                    delete (DrawEntry*)data;
                }
            }
        }

        manager->m_queue9c->ClearList();
        manager->m_palette = 0;
    }

    foundHandler = FindHandlerInList(action->addressType);
    if (foundHandler == 0) {
        handler = GetOrCreateHandler(action->addressType);
        m_activeHandler = handler;
    } else {
        LinkedList* list;

        handler = (Handler*)((EventList*)m_handlerList)->RemoveCurrent();
        list = m_handlerList;
        m_activeHandler = handler;
        if (handler == 0) {
            ShowError("queue fault 0101");
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

    if (g_GameLoopHelper_0046a6f0 != 0) {
        GameState* gs;

        gs = g_GameState_0046aa30;
        if (gs->maxStates - 1 < 4) {
            ShowError("Invalid gamestate %d", 4);
        }
        if (gs->stateValues[4] != 0) {
            handler = (Handler*)m_activeHandler;
            if (handler != 0) {
                WriteToLog("Switching to modual %s", g_StringTable_0046aa34->GetState(handler->handlerId));
                g_GameLoopHelper_0046a6f0->PostProcess();
            }
        }
    }

    handler = (Handler*)m_activeHandler;
    if (handler == 0) {
        ShowError("missing modual %d", action->addressType);
        return;
    }

    handler->Init(msg);
}

/* Function start: 0x431560 */
int GameEngine::AddHandler(Handler* handler) {
    LinkedList* list;

    if (handler == 0) {
        ShowError("illegal modual insertion");
    }

    list = m_handlerList;
    if (handler == 0) {
        ShowError("queue fault 0103");
    }

    list->current = list->head;
    while (list->current != 0) {
        Handler* currentHandler;

        currentHandler = (Handler*)list->GetCurrentData();
        if (currentHandler->handlerId == handler->handlerId) {
            ShowError("illegal modual insertion double");
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
        ShowError("queue fault 0101");
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
    LinkedList* list;
    ListNode* node;

    list = m_handlerList;
    if (list == 0) {
        return 0;
    }

    list->current = list->head;
    list = m_handlerList;
    if (list->head == 0) {
        return 0;
    }

    do {
        list = m_handlerList;
        node = list->current;
        if (node != 0) {
            if (((Handler*)node->data)->handlerId == command) {
                goto found;
            }
        } else {
            if (command == *(int*)0x90) {
found:
                list = m_handlerList;
                node = list->current;
                if (node == 0) {
                    return 0;
                }
                return (Handler*)node->data;
            }
        }

        if (list->tail == node) {
            break;
        }
        if (node != 0) {
            list->current = node->next;
        }
        list = m_handlerList;
    } while (list->head != 0);

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

        block = (int*)operator new(pool[5] * 0x40 + 4);
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
            saPtr->SpriteAction::SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
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

extern void ShowError(const char* format, ...);

/* Function start: 0x431930 */
void GameEngine::Serialize(void* param) {
    int headerLen = strlen("GAME_INFO") + 1;
    FILE* fp = *(FILE**)((char*)param + 0x44);

    if (*(int*)param != 0) {
        fwrite("GAME_INFO", headerLen, 1, fp);

        LinkedList* list = m_handlerList;
        list->current = list->head;
        while (list->head != 0) {
            ListNode* node = list->current;
            if (node != 0) {
                Handler* h = (Handler*)node->data;
                if (h != 0) {
                    h->Serialize(param);
                }
            }
            if (list->tail == list->current) break;
            if (list->current != 0) {
                list->current = list->current->next;
            }
        }

        int sentinel = 0x270f;
        fwrite(&sentinel, 4, 1, fp);
        return;
    }

    g_Buffer_0046aa00[0] = 0;
    fread(g_Buffer_0046aa00, headerLen, 1, fp);
    if (strcmp(g_Buffer_0046aa00, "GAME_INFO") != 0) {
        ShowError("SC_Game::Serialize() - Error Loading (Wrong ID '%s')", g_Buffer_0046aa00);
    }

    int sentinel = 0x270f;
    while (1) {
        int handlerId;
        fread(&handlerId, 4, 1, fp);
        if (handlerId == sentinel) break;
        Handler* h = FindHandlerInList(handlerId);
        if (h == 0) {
            h = GetOrCreateHandler(handlerId);
        }
        h->Serialize(param);
    }
}
