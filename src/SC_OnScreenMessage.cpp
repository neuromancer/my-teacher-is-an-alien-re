#include "SC_OnScreenMessage.h"
#include "Memory.h"
#include <new.h>
#include "Message.h"
#include "SpriteAction.h"
#include "globals.h"
#include "GameEngine.h"
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

    m_messageList = new MessageList();
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
void SC_OnScreenMessage::Init(SC_MessageParser* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = ((SpriteAction*)msg)->addressValue;
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
int SC_OnScreenMessage::Exit(SC_MessageParser* msg) {
    SpriteAction* action;
    MessageList* pList;
    SoundEntry* data;
    SoundEntry* item;
    Sample* snd;

    action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }

    timer.Reset();

    switch (action->instruction) {
    case 2:
    {
        item = FindOrCreateSoundEntry(action->addressValue);
        snd = item->sample;
        if (snd == 0) break;
        int loop = 1;
        if (action->extra2 != 0) {
            loop = action->extra2;
        }
        int dur = 100;
        if (action->extra1 != 0) {
            dur = action->extra1;
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
        item = FindOrCreateSoundEntry(action->addressValue);
        snd = item->sample;
        if (snd == 0) break;
        int vol = AIL_sample_volume(snd->m_sample);
        snd->Fade(vol + 10, 0);
        break;
    }

    case 0x12:
    {
        item = FindOrCreateSoundEntry(action->addressValue);
        snd = item->sample;
        if (snd == 0) break;
        int vol = AIL_sample_volume(snd->m_sample);
        snd->Fade(vol - 10, 0);
        break;
    }

    case 0x13:
    {
        FindOrCreateSoundEntry(action->addressValue);
        item = FindOrCreateSoundEntry(action->addressValue);
        item->FadeVolume(action->extra1, action->extra2);
        break;
    }

    case 0x17:
        FindOrCreateSoundEntry(action->addressValue);
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
                        if (action->addressValue != (int)ent) goto no_match;
                    } else {
                        ent = (SoundEntry*)nd->data;
                        if (ent->soundId != action->addressValue) goto no_match;
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
        item = FindOrCreateSoundEntry(action->addressValue);
        snd = item->sample;
        if (snd == 0) break;
        int dur = 100;
        if (action->extra1 != 0) {
            dur = action->extra1;
        }
        snd->Play(dur, 0);
        break;
    }

    default:
        return 0;
    }

    return 1;
}

/* Function start: 0x448810 */
int SC_OnScreenMessage::AddMessage(SC_MessageParser* msg) {
    WriteMessageAddress(msg);
    ShowError("\"SC_Sound::AddMessage\"");
    return 1;
}
/* Function start: 0x448600 */
void SC_OnScreenMessage::ShutDown(SC_MessageParser* msg) { return; }

/* Function start: 0x448C50 */
void* MessageList::GetCurrentData() {
    if (MessageList::current != 0) {
        return ((MessageNode*)MessageList::current)->data;
    }
    return 0;
}

// FUN_00449050 = MessageList::PopCurrent — COMDAT of 0x431B60, defined in MessageList.cpp

extern "C" char* GetSoundFilename(int handle);

/* Function start: 0x448120 */ /* SoundEntry sdtor (COMDAT, compiler-generated) */
/* Function start: 0x447FF0 */
SoundEntry::SoundEntry(int id) {
    memset(this, 0, sizeof(SoundEntry));
    soundId = id;
    timer.Reset();
    sample = new Sample();
    char* filename = GetSoundFilename(soundId);
    int loadResult = sample->Load(filename);
    if (loadResult != 0 && sample != 0) {
        sample->Unload();
        operator delete(sample);
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
    if (!(entry->activeFlags & 1)) return;

    currentVol = AIL_sample_volume(entry->sample->m_sample);
    targetVol = entry->targetVolume;
    if (currentVol == targetVol) {
        if (currentVol == 0) {
            entry->sample->Stop();
        }
        entry->activeFlags &= ~1;
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
                        if (activeFlags & 1) {
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

extern "C" int __stdcall AIL_sample_volume(void*);

/* Function start: 0x448220 */
void SoundEntry::FadeVolume(int volume, unsigned int duration) {
    if (sample == 0) return;
    if (sample->m_sample == 0) return;
    if (sample->m_size != *(int*)((char*)sample->m_sample + 0xc)) return;
    if (AIL_sample_status(sample->m_sample) != 4) return;
    if (activeFlags & 1) return;

    if (duration == 0) goto simple_fade;

    {
    int currentVolume = AIL_sample_volume(sample->m_sample);
    targetVolume = volume;

    unsigned int frameTime;
    if (g_GameEngine_0046a6ec == 0) {
        frameTime = 0x54;
    } else {
        frameTime = g_GameEngine_0046a6ec->m_frameTime;
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

    activeFlags |= 1;
    return;
    }

simple_fade:
    sample->Fade(volume, 0);
}

/* Function start: 0x448C60 */
SoundEntry* SC_OnScreenMessage::FindOrCreateSoundEntry(int soundId) {
    if (m_messageList->head != 0) {
        m_messageList->current = m_messageList->head;
        while (m_messageList->head != 0) {
            MessageNode* node = (MessageNode*)m_messageList->current;
            int entrySoundId;
            if (node != 0) {
                entrySoundId = ((SoundEntry*)node->data)->soundId;
            } else {
                entrySoundId = *(int*)0x24;
            }
            if (soundId == entrySoundId) {
                GameState* gs = g_GameState_0046aa30;
                int idx = g_PeriodStateIdx_0046cb90;
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                MessageNode* curNode = (MessageNode*)m_messageList->current;
                int entryGS;
                if (curNode == 0) {
                    entryGS = *(int*)0x2C;
                } else {
                    entryGS = ((SoundEntry*)curNode->data)->gameStateVal;
                }
                if (gs->stateValues[idx] == entryGS) {
                    MessageNode* found = (MessageNode*)m_messageList->current;
                    if (found != 0) {
                        return (SoundEntry*)found->data;
                    }
                    return 0;
                }
            }
            if (m_messageList->tail == m_messageList->current) break;
            if (m_messageList->current != 0) {
                m_messageList->current = ((MessageNode*)m_messageList->current)->next;
            }
        }
    }

    SoundEntry* newEntry = new SoundEntry(soundId);

    MessageList* list = m_messageList;
    if (newEntry == 0) {
        ShowError("queue fault 0101");
    }

    list->current = list->head;
    if (list->type == 1 || list->type == 2) {
        if (list->head == 0) {
            list->InsertBeforeCurrent(newEntry);
        } else {
            while (1) {
                MessageNode* cur = (MessageNode*)list->current;
                if ((unsigned)((SoundEntry*)cur->data)->soundId < (unsigned)newEntry->soundId) {
                    if (newEntry == 0) ShowError("queue fault 0102");
                    ListNode* newNode = new ListNode(newEntry);
                    if (list->current == 0) list->current = list->head;
                    if (list->head == 0) {
                        list->head = newNode;
                        list->tail = newNode;
                        list->current = newNode;
                    } else {
                        newNode->next = list->current;
                        newNode->prev = list->current->prev;
                        if (list->current->prev != 0) {
                            list->current->prev->next = newNode;
                            list->current->prev = newNode;
                        } else {
                            list->head = newNode;
                            list->current->prev = newNode;
                        }
                    }
                    break;
                }
                if (list->tail == cur) {
                    if (newEntry == 0) ShowError("queue fault 0112");
                    ListNode* newNode = new ListNode(newEntry);
                    if (list->current == 0) list->current = list->tail;
                    if (list->head == 0) {
                        list->head = newNode;
                        list->tail = newNode;
                        list->current = newNode;
                    } else {
                        if (list->tail == 0 || list->tail->next != 0) {
                            ShowError("queue fault 0113");
                        }
                        newNode->next = 0;
                        newNode->prev = list->tail;
                        list->tail->next = newNode;
                        list->tail = newNode;
                    }
                    break;
                }
                if (cur != 0) {
                    list->current = cur->next;
                }
                if (list->current == 0) break;
            }
        }
    } else {
        list->InsertBeforeCurrent(newEntry);
    }

    return newEntry;
}
