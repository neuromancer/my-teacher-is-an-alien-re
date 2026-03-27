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
#include "mss.h"

extern "C" {
    void ShowError(const char*, ...);
    void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
}

// FUN_00448140 = SoundEntry::SoundUpdate — implemented below

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
    SoundEntry* data;

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
        item = FindOrCreateSoundEntry(msgData[1]);
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
                data = (SoundEntry*)pList->PopCurrent();
                if (data != 0) {
                    delete data;
                }
            }
        }
        break;
    }

    case 0x11:
    {
        item = FindOrCreateSoundEntry(msgData[1]);
        snd = item->sample;
        if (snd == 0) break;
        int vol = AIL_sample_volume(snd->m_sample);
        snd->Fade(vol + 10, 0);
        break;
    }

    case 0x12:
    {
        item = FindOrCreateSoundEntry(msgData[1]);
        snd = item->sample;
        if (snd == 0) break;
        int vol = AIL_sample_volume(snd->m_sample);
        snd->Fade(vol - 10, 0);
        break;
    }

    case 0x13:
    {
        FindOrCreateSoundEntry(msgData[1]);
        item = FindOrCreateSoundEntry(msgData[1]);
        item->FadeVolume(msgData[5], msgData[6]);
        break;
    }

    case 0x17:
        FindOrCreateSoundEntry(msgData[1]);
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

                        data = (SoundEntry*)pList->GetCurrentData();

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
        item = FindOrCreateSoundEntry(msgData[1]);
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
void* MessageList::GetCurrentData() {
    MessageNode* node = (MessageNode*)current;
    if (node == 0) return 0;
    return node->data;
}

// FUN_00449050 = MessageList::PopCurrent — COMDAT of 0x431B60, defined in MessageList.cpp

extern "C" char* GetSoundFilename(int handle);
extern "C" GameState* g_GameState_0046aa30;
extern int g_PeriodStateIdx_0046cb90;

/* Function start: 0x447FF0 */
SoundEntry::SoundEntry(int id) {
    int i;
    int* p = (int*)this;
    for (i = 0xc; i != 0; i--) {
        *p = 0;
        p++;
    }
    soundId = id;
    timer.Reset();
    sample = new Sample();
    char* filename = GetSoundFilename(soundId);
    int loadResult = sample->Load(filename);
    if (loadResult != 0 && sample != 0) {
        sample->Unload();
        free(sample);
        sample = 0;
    }
    int gsIdx = g_PeriodStateIdx_0046cb90;
    GameState* gs = g_GameState_0046aa30;
    if (gsIdx < 0 || gs->maxStates - 1 < gsIdx) {
        ShowError("Invalid gamestate %d", gsIdx);
    }
    gameStateVal = gs->stateValues[gsIdx];
}

/* Function start: 0x4481A0 */
void __fastcall ProcessSoundFade(SoundEntry* entry) {
    Sample* snd;
    int currentVol;
    int targetVol;
    int step;

    snd = entry->sample;
    if (snd == 0) return;
    if (snd->m_sample == 0) return;
    if (snd->m_size != *(int*)((char*)snd->m_sample + 0xC)) return;
    if (AIL_sample_status(snd->m_sample) != 4) return;
    if (!(entry->field_04 & 1)) return;

    currentVol = AIL_sample_volume(entry->sample->m_sample);
    targetVol = entry->targetVolume;
    if (currentVol == targetVol) {
        if (currentVol == 0) {
            entry->sample->~Sample();
        }
        entry->field_04 &= ~1;
        return;
    }
    step = entry->volumeStep;
    currentVol += step;
    if (currentVol < 0 || currentVol > 100) {
        currentVol = targetVol;
    }
    if (step > 0) {
        if (currentVol >= targetVol) {
            currentVol = targetVol;
        }
    } else {
        if (currentVol <= targetVol) {
            currentVol = targetVol;
        }
    }
    entry->sample->Fade(currentVol, 0);
}

/* Function start: 0x448140 */
int SoundEntry::SoundUpdate() {
    if (sample != 0) {
        if (timer.Update() <= 60000) {
            Sample* snd = sample;
            HSAMPLE hs = snd->m_sample;
            if (hs != 0) {
                if (snd->m_size == *(int*)((char*)hs + 0xC)) {
                    if (AIL_sample_status(hs) == 4) {
                        if (field_04 & 1) {
                            ProcessSoundFade(this);
                        }
                        timer.Reset();
                    }
                }
            }
            return 0;
        }
    }
    return 1;
}

extern "C" int g_GameEngine_0046a6ec;
extern "C" int __stdcall AIL_sample_volume(void*);

/* Function start: 0x448220 */
void SoundEntry::FadeVolume(int volume, unsigned int duration) {
    if (sample == 0) return;
    if (sample->m_sample == 0) return;
    if (sample->m_size != *(int*)((char*)sample->m_sample + 0xc)) return;
    if (AIL_sample_status(sample->m_sample) != 4) return;
    if (field_04 & 1) return;

    if (duration == 0) goto simple_fade;

    {
    int currentVolume = AIL_sample_volume(sample->m_sample);
    targetVolume = volume;

    unsigned int frameTime;
    if (g_GameEngine_0046a6ec == 0) {
        frameTime = 0x54;
    } else {
        frameTime = *(unsigned int*)(g_GameEngine_0046a6ec + 0x1c);
        if (frameTime == 0) {
            frameTime = 0x54;
        }
    }

    unsigned int numFrames = duration / frameTime;
    int delta = volume - currentVolume;
    volumeStep = delta / (int)numFrames;

    if (volumeStep == 0) {
        if (volume >= currentVolume) {
            volumeStep = 1;
        } else {
            volumeStep = -1;
        }
    }

    field_04 |= 1;
    return;
    }

simple_fade:
    targetVolume = volume;
    sample->Fade(volume, 0);
}

/* Function start: 0x448C60 */
SoundEntry* SC_OnScreenMessage::FindOrCreateSoundEntry(int soundId) {
    MessageList* list = m_messageList;
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
