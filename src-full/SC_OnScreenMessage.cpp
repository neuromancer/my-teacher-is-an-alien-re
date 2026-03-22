#include "SC_OnScreenMessage.h"
#include "Memory.h"
#include "Message.h"
#include "globals.h"
#include "string.h"
#include "GameState.h"
#include "StringTable.h"
#include "OnScreenMessage.h"
#include "SC_Question.h"
#include "Sample.h"

extern "C" {
    void ShowError(const char*, ...);
    void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
}

extern int __fastcall FUN_00448140(SoundEntry* entry);
extern SoundEntry* __fastcall FUN_00448c60(SC_OnScreenMessage* self, int dummy, int soundId);
extern void* __fastcall FUN_00449050(MessageList* list);

/* Function start: 0x4482F0 */
SC_OnScreenMessage::SC_OnScreenMessage() {
    memset(&timer, 0, 24);
    handlerId = 4;
    timer.Reset();

    // Allocate and initialize list header structure
    m_messageList = new MessageList();
    if (m_messageList != 0) {
        m_messageList->type = 1;
    }
}

/* Function start: 0x448420 */
SC_OnScreenMessage::~SC_OnScreenMessage() {
    MessageList* pList;
    MessageNode* node;
    OnScreenMessage* data;

    pList = m_messageList;
    if (pList != 0) {
        if (pList->head != 0) {
            pList->current = pList->head;
            while (pList->head != 0) {
                node = (MessageNode*)pList->current;
                if (node == 0) {
                    data = 0;
                } else {
                    // Unlink node from head
                    if (pList->head == node) {
                        pList->head = node->next;
                    }
                    // Unlink node from tail
                    if (pList->tail == pList->current) {
                        pList->tail = ((MessageNode*)pList->current)->prev;
                    }
                    // Update next node's prev pointer
                    node = (MessageNode*)pList->current;
                    if (node->prev != 0) {
                        node->prev->next = node->next;
                    }
                    // Update prev node's next pointer
                    node = (MessageNode*)pList->current;
                    if (node->next != 0) {
                        node->next->prev = node->prev;
                    }
                    // Get data and free node
                    node = (MessageNode*)pList->current;
                    data = 0;
                    if (node != 0) {
                        data = (OnScreenMessage*)node->data;
                    }
                    if (node != 0) {
                        node->data = 0;
                        node->prev = 0;
                        node->next = 0;
                        ::operator delete(node);
                        pList->current = 0;
                    }
                    pList->current = pList->head;
                }
                // Cleanup data object
                if (data != 0) {
                    delete data;
                }
            }
        }
        ::operator delete(pList);
        m_messageList = 0;
    }
}

/* Function start: 0x4485E0 */
void SC_OnScreenMessage::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = msg->sourceAddress;
    }
}

/* Function start: 0x448610 */
void SC_OnScreenMessage::Update(int param1, int param2) {
    MessageNode* node;
    SoundEntry* data;
    MessageList* pList;

    if (timer.Update() > 60000) {
        if (m_messageList->head == 0) {
            SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        }
    }

    if (m_messageList != 0) {
        m_messageList->current = m_messageList->head;
        if (m_messageList->head != 0) {
            do {
                SoundEntry* entry = 0;
                node = (MessageNode*)m_messageList->current;
                if (node != 0) {
                    entry = (SoundEntry*)node->data;
                }

                if (entry->SoundUpdate() != 0) {
                    pList = m_messageList;
                    // Inline RemoveCurrent
                    node = (MessageNode*)pList->current;
                    if (node == 0) {
                        data = 0;
                    } else {
                        if (pList->head == node) {
                            pList->head = node->next;
                        }
                        if (pList->tail == pList->current) {
                            pList->tail = ((MessageNode*)pList->current)->prev;
                        }
                        node = (MessageNode*)pList->current;
                        if (node->prev != 0) {
                            node->prev->next = node->next;
                        }
                        node = (MessageNode*)pList->current;
                        if (node->next != 0) {
                            node->next->prev = node->prev;
                        }
                        node = (MessageNode*)pList->current;
                        data = 0;
                        if (node != 0) {
                            data = (SoundEntry*)node->data;
                        }
                        if (node != 0) {
                            node->data = 0;
                            node->prev = 0;
                            node->next = 0;
                            ::operator delete(node);
                            pList->current = 0;
                        }
                        pList->current = pList->head;
                    }

                    if (data != 0) {
                        delete data;
                    }
                }

                // Advance to next
                {
                    MessageList* lst = m_messageList;
                    if (lst->tail == lst->current) break;
                    if (lst->current != 0) {
                        lst->current = ((MessageNode*)lst->current)->next;
                    }
                }
            } while (m_messageList->head != 0);
        }
    }

    if (handlerId == param2) {
        ShowError("SC_Sound::Update");
    }
}

/* Function start: 0x448830 */
int SC_OnScreenMessage::Exit(SC_Message* msg) {
    int* msgData;
    MessageList* pList;
    SoundEntry* data;
    SoundEntry* item;
    Sample* snd;

    msgData = (int*)msg;
    if (msgData[0] != handlerId) {
        return 0;
    }

    timer.Reset();

    switch (msgData[4]) {
    case 2:
    {
        item = FUN_00448c60(this, 0, msgData[1]);
        snd = item->sample;
        if (snd == 0) break;
        int loop = 1;
        if (msgData[6] != 0) {
            loop = msgData[6];
        }
        int dur = 100;
        if (msgData[5] != 0) {
            dur = msgData[5];
        }
        snd->Play(dur, loop);
        break;
    }

    case 7:
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        break;

    case 0x10:
    {
        pList = m_messageList;
        if (pList->head != 0) {
            pList->current = pList->head;
            while (pList->head != 0) {
                data = (SoundEntry*)FUN_00449050(pList);
                if (data != 0) {
                    delete data;
                }
            }
        }
        break;
    }

    case 0x11:
    {
        item = FUN_00448c60(this, 0, msgData[1]);
        snd = item->sample;
        if (snd == 0) break;
        int vol = AIL_sample_volume(snd->m_sample);
        snd->Fade(vol + 10, 0);
        break;
    }

    case 0x12:
    {
        item = FUN_00448c60(this, 0, msgData[1]);
        snd = item->sample;
        if (snd == 0) break;
        int vol = AIL_sample_volume(snd->m_sample);
        snd->Fade(vol - 10, 0);
        break;
    }

    case 0x13:
    {
        FUN_00448c60(this, 0, msgData[1]);
        item = FUN_00448c60(this, 0, msgData[1]);
        item->FadeVolume(msgData[5], msgData[6]);
        break;
    }

    case 0x17:
        FUN_00448c60(this, 0, msgData[1]);
        break;

    case 0x18:
    {
        m_messageList->current = m_messageList->head;
        if (m_messageList->head != 0) {
            do {
                {
                    MessageNode* nd;
                    SoundEntry* ent;
                    pList = m_messageList;
                    nd = (MessageNode*)pList->current;
                    if (nd == 0) {
                        ent = (SoundEntry*)*(int*)0x24;
                        if (msgData[1] != (int)ent) goto no_match;
                    } else {
                        ent = (SoundEntry*)nd->data;
                        if (ent->soundId != msgData[1]) goto no_match;
                    }

                    // match found
                    if (nd == 0) {
                        data = 0;
                    } else {
                        if (pList->head == nd) {
                            pList->head = nd->next;
                        }
                        if (pList->tail == pList->current) {
                            pList->tail = ((MessageNode*)pList->current)->prev;
                        }
                        nd = (MessageNode*)pList->current;
                        if (nd->prev != 0) {
                            nd->prev->next = nd->next;
                        }
                        nd = (MessageNode*)pList->current;
                        if (nd->next != 0) {
                            nd->next->prev = nd->prev;
                        }

                        extern void* __fastcall FUN_00448c50(MessageList*);
                        data = (SoundEntry*)FUN_00448c50(pList);

                        nd = (MessageNode*)pList->current;
                        if (nd != 0) {
                            delete nd;
                            pList->current = 0;
                        }
                        pList->current = pList->head;
                    }

                    if (data != 0) {
                        delete data;
                    }
                }

            no_match:
                // Advance
                {
                    MessageList* lst = m_messageList;
                    if (lst->tail == lst->current) break;
                    if (lst->current != 0) {
                        lst->current = ((MessageNode*)lst->current)->next;
                    }
                }
            } while (m_messageList->head != 0);
        }
        break;
    }

    case 0x1b:
    {
        item = FUN_00448c60(this, 0, msgData[1]);
        snd = item->sample;
        if (snd == 0) break;
        int dur = 100;
        if (msgData[5] != 0) {
            dur = msgData[5];
        }
        snd->Play(dur, 0);
        break;
    }

    default:
        return 0;
    }

    return 1;
}

int SC_OnScreenMessage::AddMessage(SC_Message* msg) { return 1; }
int SC_OnScreenMessage::ShutDown(SC_Message* msg) { return 0; }

/* Function start: 0x448C50 */
void* __fastcall FUN_00448c50(MessageList* list) {
    MessageNode* node = (MessageNode*)list->current;
    if (node == 0) return 0;
    return node->data;
}

/* Function start: 0x449050 */
void* __fastcall FUN_00449050(MessageList* list) {
    MessageNode* node = (MessageNode*)list->current;
    if (node == 0) return 0;

    if (list->head == node) {
        list->head = node->next;
    }
    if (list->tail == node) {
        list->tail = node->prev;
    }
    if (node->prev != 0) {
        node->prev->next = node->next;
    }
    if (node->next != 0) {
        node->next->prev = node->prev;
    }

    void* data = 0;
    if (node != 0) {
        data = node->data;
    }
    if (node != 0) {
        node->data = 0;
        node->prev = 0;
        node->next = 0;
        FreeMemory(node);
        list->current = 0;
    }
    list->current = list->head;
    return data;
}

/* Function start: 0x447FF0 */
SoundEntry::SoundEntry(int id) {
    memset(this, 0, sizeof(SoundEntry));
    soundId = id;
    sample = new Sample();
    if (sample != 0) {
        char buf[260];
        sprintf(buf, "audio\\snd%4.4d.wav", id);
        sample->Load(buf);
    }
}

/* Function start: 0x448140 */
int SoundEntry::SoundUpdate() {
    return 0;
}

/* Function start: 0x448220 */
void SoundEntry::FadeVolume(int volume, unsigned int duration) {
    if (sample != 0) {
        sample->Fade(volume, duration);
    }
}

/* Function start: 0x448C60 */
SoundEntry* __fastcall FUN_00448c60(SC_OnScreenMessage* self, int dummy, int soundId) {
    MessageList* list = self->m_messageList;
    MessageNode* node;

    // Search for existing entry with this soundId
    list->current = list->head;
    if (list->head != 0) {
        do {
            node = (MessageNode*)list->current;
            if (node != 0) {
                SoundEntry* entry = (SoundEntry*)node->data;
                if (entry->soundId == soundId) {
                    return entry;
                }
            }
            if (list->tail == list->current) break;
            if (list->current != 0) {
                list->current = ((MessageNode*)list->current)->next;
            }
        } while (list->current != 0);
    }

    // Not found - create new entry
    SoundEntry* newEntry = new SoundEntry(soundId);

    // Insert into list
    if (newEntry == 0) {
        ShowError("queue fault 0101");
    }
    list->current = list->head;
    list->InsertNode(newEntry);

    return newEntry;
}
