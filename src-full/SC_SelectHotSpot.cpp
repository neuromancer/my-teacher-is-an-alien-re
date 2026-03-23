#include "SC_SelectHotSpot.h"
#include "SpriteAction.h"
#include "LinkedList.h"
#include "Memory.h"
#include "Sprite.h"
#include "Palette.h"
#include "Sample.h"
#include "MMPlayer.h"
#include "SC_Question.h"
#include "ZBuffer.h"
#include <stdio.h>
#include <string.h>
#include <new.h>

extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" char* GetSoundFilename(int handle);
extern "C" int __stdcall AIL_sample_status(void*);

class ZBufferManager;
extern ZBufferManager* g_ZBufferManager_0046aa24;
class MouseControl;
extern MouseControl* g_Mouse_0046aa18;
#include "InputManager.h"
extern InputManager* g_InputManager_0046aa08;
extern SpriteAction DAT_00472d58;



// Palette functions
#include "Palette.h"

// Sound table
extern char* __cdecl ResolveAssetPath(char* name);

// Parser operations
// FUN_00413e70 = Parser::ProcessFile in Parser.cpp

// Message operations

// Engine/Misc
extern "C" void SetVideoRes(int, int);
extern "C" void WriteToLog(const char* format, ...);
#include "MouseControl.h"

// FUN_004061e0 = RenderEntry::~RenderEntry — callers updated
#include "RenderEntry.h"

// Queue operations (thiscall via fastcall + dummy EDX)
// FUN_00406cc0 = Queue::Add in LinkedList.h

// Parser fallback
// FUN_00412a50 = Parser::LBLParse in Parser.h

/* Function start: 0x404FB0 */
SelectHotspot::SelectHotspot(int key) {
    SelectHotspot::x1 = 0;
    SelectHotspot::y1 = 0;
    SelectHotspot::x2 = 0;
    SelectHotspot::y2 = 0;
    memset(&state, 0, 14 * 4);
    SelectHotspot::state = 1;
    SelectHotspot::sortKey = key;
}

/* Function start: 0x405060 */
SelectHotspot::~SelectHotspot() {
    void* tmp;
    tmp = SelectHotspot::sprite;
    if (tmp != 0) {
        delete (Sprite*)tmp;
        SelectHotspot::sprite = 0;
    }
    tmp = SelectHotspot::rollonSound;
    if (tmp != 0) {
        ((Sample*)tmp)->Unload();
        FreeMemory(tmp);
        SelectHotspot::rollonSound = 0;
    }
    tmp = SelectHotspot::selectedSound;
    if (tmp != 0) {
        ((Sample*)tmp)->Unload();
        FreeMemory(tmp);
        SelectHotspot::selectedSound = 0;
    }
    tmp = SelectHotspot::rolloffSound;
    if (tmp != 0) {
        ((Sample*)tmp)->Unload();
        FreeMemory(tmp);
        SelectHotspot::rolloffSound = 0;
    }
    LinkedList* list = SelectHotspot::messageList;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                void* data = list->RemoveCurrent();
                if (data != 0) {
                    delete (SpriteAction*)data;
                }
            }
        }
        FreeMemory(list);
        SelectHotspot::messageList = 0;
    }
    SelectHotspot::parent = 0;
}

/* Function start: 0x405250 */
void SelectHotspot::Draw() {
    if (SelectHotspot::sprite != 0) {
        ((Sprite*)SelectHotspot::sprite)->StopAnimationSound();
    }
}

/* Function start: 0x405260 */
void SelectHotspot::Update() {
    int mouseX;
    int mouseY;
    InputState* pMouse;

    switch (SelectHotspot::state) {
    case 0:
        break;
    case 1:
        pMouse = g_InputManager_0046aa08->pMouse;
        mouseY = 0;
        if (pMouse != 0) {
            mouseY = pMouse->y;
        }
        if (pMouse != 0) {
            mouseX = pMouse->x;
        } else {
            mouseX = 0;
        }
        if (SelectHotspot::x1 <= mouseX && mouseX <= SelectHotspot::x2 &&
            SelectHotspot::y1 <= mouseY && mouseY <= SelectHotspot::y2) {
            SelectHotspot::state = 2;
            if (SelectHotspot::sprite != 0) {
                ((Sprite*)SelectHotspot::sprite)->ResetAnimation(2, 0);
            }
            if (SelectHotspot::rollonSound != 0) {
                ((Sample*)SelectHotspot::rollonSound)->Play(100, 1);
            }
        }
        if (SelectHotspot::sprite != 0) {
            Sprite* spr = (Sprite*)SelectHotspot::sprite;
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
            return;
        }
        break;
    case 2:
        pMouse = g_InputManager_0046aa08->pMouse;
        mouseY = 0;
        if (pMouse != 0) {
            mouseY = pMouse->y;
        }
        if (pMouse != 0) {
            mouseX = pMouse->x;
        } else {
            mouseX = 0;
        }
        if (mouseX < SelectHotspot::x1 || SelectHotspot::x2 < mouseX ||
            mouseY < SelectHotspot::y1 || SelectHotspot::y2 < mouseY) {
            SelectHotspot::state = 1;
            if (SelectHotspot::sprite != 0) {
                ((Sprite*)SelectHotspot::sprite)->ResetAnimation(1, 0);
            }
            if (SelectHotspot::rolloffSound != 0) {
                ((Sample*)SelectHotspot::rolloffSound)->Play(100, 1);
            }
        }
        if (SelectHotspot::sprite != 0) {
            Sprite* spr = (Sprite*)SelectHotspot::sprite;
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
            return;
        }
        break;
    case 3: {
        Sample* snd = (Sample*)SelectHotspot::selectedSound;
        int sndDone;
        if (snd == 0 || snd->m_sample == 0 ||
            snd->m_size != *(int*)((char*)snd->m_sample + 0xC) ||
            (sndDone = AIL_sample_status(snd->m_sample), sndDone != 4)) {
            if (SelectHotspot::messageList != 0) {
                ((LinkedList*)SelectHotspot::messageList)->current = ((LinkedList*)SelectHotspot::messageList)->head;
                if (((LinkedList*)SelectHotspot::messageList)->head != 0) {
                    while (1) {
                        void* data = 0;
                        ListNode* node = ((LinkedList*)SelectHotspot::messageList)->current;
                        if (node != 0) {
                            data = node->data;
                        }
                        EnqueueSpriteAction(data);
                        ListNode* cur = ((LinkedList*)SelectHotspot::messageList)->current;
                        if (((LinkedList*)SelectHotspot::messageList)->tail == cur) break;
                        if (cur != 0) {
                            ((LinkedList*)SelectHotspot::messageList)->current = cur->next;
                        }
                        if (((LinkedList*)SelectHotspot::messageList)->head == 0) break;
                    }
                }
            }
            SelectHotspot::state = 1;
            if (SelectHotspot::sprite != 0) {
                ((Sprite*)SelectHotspot::sprite)->ResetAnimation(1, 0);
            }
        }
        if (SelectHotspot::sprite != 0) {
            Sprite* spr = (Sprite*)SelectHotspot::sprite;
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
        }
        break;
    }
    default:
        ShowError("SC_SelectHotSpot::Update()");
        return;
    }
}

/* Function start: 0x405490 */
int SelectHotspot::OnClick(SC_Message* msg) {
    int* pmsg = (int*)msg;
    int hit;

    if (pmsg[7] < SelectHotspot::x1 || SelectHotspot::x2 < pmsg[7] ||
        pmsg[8] < SelectHotspot::y1 || SelectHotspot::y2 < pmsg[8]) {
        hit = 0;
    } else {
        hit = 1;
    }

    if (hit && SelectHotspot::state != 0) {
        if (pmsg[9] > 1 ||
            (SelectHotspot::keyCode != 0 && pmsg[0xB] == SelectHotspot::keyCode)) {
            SelectHotspot::state = 3;
            if (SelectHotspot::sprite != 0) {
                ((Sprite*)SelectHotspot::sprite)->ResetAnimation(3, 0);
            }
            if (SelectHotspot::selectedSound != 0) {
                ((Sample*)SelectHotspot::selectedSound)->Play(100, 1);
            }
        }
        return 1;
    }
    return 0;
}

/* Function start: 0x405530 */
int SelectHotspot::LBLParse(char* line) {
    char keyword[32];
    int handle;
    void* mem;

    keyword[0] = 0;
    sscanf(line, "%s", keyword);

    WriteToLog("SelectHotspot::LBLParse keyword='%s'", keyword);

    if (strcmp(keyword, "SPRITE") == 0) {
        void* tmp = SelectHotspot::sprite;
        if (tmp != 0) {
            delete (Sprite*)tmp;
            SelectHotspot::sprite = 0;
        }
        Sprite* spr = new Sprite(0);
        SelectHotspot::sprite = spr;
        Parser::ProcessFile((Parser*)spr, this, 0);
        ((Sprite*)SelectHotspot::sprite)->ResetAnimation(SelectHotspot::state, 0);
    } else if (strcmp(keyword, "LOC") == 0) {
        sscanf(line, "%s %d %d %d %d", keyword, &x1, &y1, &x2, &y2);
    } else if (strcmp(keyword, "KEY") == 0) {
        sscanf(line, "%s %d", keyword, &keyCode);
    } else if (strcmp(keyword, "ROLLON") == 0) {
        sscanf(line, "%s %d", keyword, &handle);
        char* name = GetSoundFilename(handle);
        void* tmp = SelectHotspot::rollonSound;
        if (tmp != 0) {
            ((Sample*)tmp)->Unload();
            FreeMemory(tmp);
            SelectHotspot::rollonSound = 0;
        }
        Sample* smp = new Sample();
        SelectHotspot::rollonSound = smp;
        char* path = ResolveAssetPath(name);
        smp->Load(path);
    } else if (strcmp(keyword, "ROLLOFF") == 0) {
        sscanf(line, "%s %d", keyword, &handle);
        char* name = GetSoundFilename(handle);
        void* tmp = SelectHotspot::rolloffSound;
        if (tmp != 0) {
            ((Sample*)tmp)->Unload();
            FreeMemory(tmp);
            SelectHotspot::rolloffSound = 0;
        }
        Sample* smp = new Sample();
        SelectHotspot::rolloffSound = smp;
        char* path = ResolveAssetPath(name);
        smp->Load(path);
    } else if (strcmp(keyword, "SELECTED") == 0) {
        sscanf(line, "%s %d", keyword, &handle);
        char* name = GetSoundFilename(handle);
        void* tmp = SelectHotspot::selectedSound;
        if (tmp != 0) {
            ((Sample*)tmp)->Unload();
            FreeMemory(tmp);
            SelectHotspot::selectedSound = 0;
        }
        Sample* smp = new Sample();
        SelectHotspot::selectedSound = smp;
        char* path = ResolveAssetPath(name);
        smp->Load(path);
    } else if (strcmp(keyword, "MESSAGE") == 0) {
        SpriteAction* msgObj = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ParseSpriteAction(msgObj, this);
        if (SelectHotspot::messageList == 0) {
            LinkedList* newList = (LinkedList*)AllocateMemory(sizeof(LinkedList));
            if (newList != 0) {
                newList->type = 0;
                newList->head = 0;
                newList->tail = 0;
                newList->current = newList->head;
            }
            SelectHotspot::messageList = newList;
        }
        ((Queue*)SelectHotspot::messageList)->Add(msgObj);
    } else if (strcmp(keyword, "RETURNMESSAGE") == 0) {
        SpriteAction* msgObj = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        int* src = (int*)&DAT_00472d58;
        int* dst = (int*)msgObj;
        int i;
        for (i = 0x0E; i != 0; i--) {
            *dst = *src;
            src++;
            dst++;
        }
        if (SelectHotspot::messageList == 0) {
            LinkedList* newList = (LinkedList*)AllocateMemory(sizeof(LinkedList));
            if (newList != 0) {
                newList->type = 0;
                newList->head = 0;
                newList->tail = 0;
                newList->current = newList->head;
            }
            SelectHotspot::messageList = newList;
        }
        LinkedList* list = SelectHotspot::messageList;
        if (msgObj == 0) {
            ShowError("queue fault 0101");
        }
        list->current = list->head;
        if (list->type == 1 || list->type == 2) {
            if (list->head == 0) {
                ((Queue*)list)->InsertAtCurrent((void*)msgObj);
            } else {
                do {
                    int cur = (int)list->current;
                    if (*(int*)(*(int*)(cur + 8)) < *(int*)msgObj) {
                        ((Queue*)list)->InsertAtCurrent((void*)msgObj);
                        break;
                    }
                    if ((int)list->tail == cur) {
                        ((Queue*)list)->Push((void*)msgObj);
                        break;
                    }
                    if (cur != 0) {
                        list->current = (ListNode*)*(int*)(cur + 4);
                    }
                } while (list->current != 0);
            }
        } else {
            ((Queue*)list)->InsertAtCurrent((void*)msgObj);
        }
    } else if (strcmp(keyword, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_SelectHotSpot");
    }
    return 0;
}

/* Function start: 0x405C30 */
SC_SelectHotSpot::SC_SelectHotSpot() {
    memset(&field_A8, 0, 24);
    handlerId = 0x2D;
}

/* 0x405D60 = Handler::Serialize (base, empty) */

/* Function start: 0x405E00 */
SC_SelectHotSpot::~SC_SelectHotSpot() {
    if (SC_SelectHotSpot::ambient) {
        delete (MMPlayer*)SC_SelectHotSpot::ambient;
        SC_SelectHotSpot::ambient = 0;
    }
    if (SC_SelectHotSpot::introSprite) {
        delete (Sprite*)SC_SelectHotSpot::introSprite;
        SC_SelectHotSpot::introSprite = 0;
    }
    SC_SelectHotSpot::currentHotspot = 0;
    LinkedList* list = SC_SelectHotSpot::hotspotList;
    if (list) {
        if (list->head) {
            list->current = list->head;
            while (list->head) {
                ListNode* node = list->current;
                SelectHotspot* hs;
                if (!node) {
                    hs = 0;
                } else {
                    if (list->head == node) list->head = node->next;
                    if (list->tail == list->current) list->tail = (ListNode*)list->current->prev;
                    node = list->current;
                    if (node->prev) node->prev->next = node->next;
                    node = list->current;
                    if (node->next) node->next->prev = node->prev;
                    node = list->current;
                    hs = 0;
                    if (node) {
                        hs = (SelectHotspot*)node->data;
                    }
                    if (node) {
                        delete node;
                        list->current = 0;
                    }
                    list->current = list->head;
                }
                if (hs) {
                    hs->~SelectHotspot();
                    FreeMemory(hs);
                }
            }
        }
        FreeMemory(list);
        SC_SelectHotSpot::hotspotList = 0;
    }
    if (SC_SelectHotSpot::palette) {
        delete (Palette*)SC_SelectHotSpot::palette;
        SC_SelectHotSpot::palette = 0;
    }
    SC_SelectHotSpot::field_A8 = 0;
}

/* Function start: 0x405FB0 */
void SC_SelectHotSpot::Init(SC_Message* msg) {
    int* pmsg = (int*)msg;

    InitFromMessage(msg);
    SetVideoRes(0x280, 0x1e0);

    int iVar2 = (int)g_ZBufferManager_0046aa24;
    if (*(int*)(iVar2 + 0x98) != 2) {
        *(int*)(iVar2 + 0x98) = 2;

        int* piVar5 = *(int**)(iVar2 + 0xa0);
        if (*piVar5 != 0) {
            piVar5[2] = *piVar5;
            while (*piVar5 != 0) {
                void* obj = ((LinkedList*)piVar5)->RemoveCurrent();
                if (obj != 0) {
                    
                    FreeMemory(obj);
                }
            }
        }

        piVar5 = *(int**)(iVar2 + 0xa4);
        if (*piVar5 != 0) {
            piVar5[2] = *piVar5;
            while (*piVar5 != 0) {
                void* obj = ((LinkedList*)piVar5)->RemoveCurrent();
                if (obj != 0) {
                    ((ZBuffer*)obj)->CleanUpVBuffer();
                    FreeMemory(obj);
                }
            }
        }

        piVar5 = *(int**)(iVar2 + 0x9c);
        if (*piVar5 != 0) {
            piVar5[2] = *piVar5;
            while (*piVar5 != 0) {
                void* obj = ((LinkedList*)piVar5)->RemoveCurrent();
                if (obj != 0) {
                    ((RenderEntry*)obj)->~RenderEntry();
                    FreeMemory(obj);
                }
            }
        }

        *(int*)(iVar2 + 0xa8) = 0;
    }

    if (pmsg != 0) {
        SC_SelectHotSpot::moduleParam = pmsg[1];
        if (SC_SelectHotSpot::ambient != 0) {
            delete (MMPlayer*)SC_SelectHotSpot::ambient;
            SC_SelectHotSpot::ambient = 0;
        }
        SC_SelectHotSpot::currentHotspot = 0;

        LinkedList* hsList = SC_SelectHotSpot::hotspotList;
        if (hsList != 0) {
            if (hsList->head != 0) {
                hsList->current = hsList->head;
                while (hsList->head != 0) {
                    void* data = ((LinkedList*)hsList)->RemoveCurrent();
                    if (data != 0) {
                        delete (SelectHotspot*)data;
                    }
                }
            }
            FreeMemory(hsList);
            SC_SelectHotSpot::hotspotList = 0;
        }

        if (SC_SelectHotSpot::palette != 0) {
            delete (Palette*)SC_SelectHotSpot::palette;
            SC_SelectHotSpot::palette = 0;
        }

        ParseFile(this, "mis\\menu.mis", "[MENU%4.4d]", SC_SelectHotSpot::moduleParam);
    }

    int palVal = (int)SC_SelectHotSpot::palette;
    if (palVal != 0) {
        int* palSlot = (int*)((int)g_ZBufferManager_0046aa24 + 0xa8);
        if (*palSlot != 0) {
            WriteToLog("ddouble palette");
        }
        *palSlot = palVal;
    }

    Sprite* spr = *(Sprite**)((char*)g_Mouse_0046aa18 + 0x94);
    if (spr != 0) {
        spr->ResetAnimation(0, 0);
    }

    SendGameMessage(5, 0x3ed, SC_SelectHotSpot::handlerId, SC_SelectHotSpot::moduleParam, 0x1b, 0, 0, 0, 0, 0);
}

/* Function start: 0x406240 */
int SC_SelectHotSpot::ShutDown(SC_Message* msg) {
    if (SC_SelectHotSpot::ambient != 0) {
        ((MMPlayer*)SC_SelectHotSpot::ambient)->StopAll();
    }
    SC_SelectHotSpot::field_A8 = 0;
    LinkedList* list = SC_SelectHotSpot::hotspotList;
    if (list != 0) {
        list->current = list->head;
        if (list->head != 0) {
            while (1) {
                SelectHotspot* hs = 0;
                ListNode* node = ((LinkedList*)SC_SelectHotSpot::hotspotList)->current;
                if (node != 0) {
                    hs = (SelectHotspot*)node->data;
                }
                hs->Draw();
                LinkedList* l2 = SC_SelectHotSpot::hotspotList;
                ListNode* cur = l2->current;
                if (l2->tail == cur) break;
                if (cur != 0) {
                    l2->current = cur->next;
                }
                if (((LinkedList*)SC_SelectHotSpot::hotspotList)->head == 0) break;
            }
        }
    }
    return 0;
}

/* Function start: 0x4062C0 */
void SC_SelectHotSpot::Update(int param1, int param2) {
    if (SC_SelectHotSpot::handlerId != param2) {
        return;
    }
    if (SC_SelectHotSpot::field_A8 == 0) {
        void* intro = SC_SelectHotSpot::introSprite;
        if (intro != 0) {
            Sprite* spr = (Sprite*)intro;
            int result = spr->Do(spr->loc_x, spr->loc_y, 1.0);
            SC_SelectHotSpot::field_A8 = result;
            if (result == 0) {
                return;
            }
            ((Sprite*)SC_SelectHotSpot::introSprite)->StopAnimationSound();
            return;
        }
        SC_SelectHotSpot::field_A8 = 1;
    }
    if (SC_SelectHotSpot::ambient != 0) {
        ((MMPlayer*)SC_SelectHotSpot::ambient)->Draw();
    }
    LinkedList* list = SC_SelectHotSpot::hotspotList;
    if (list != 0) {
        list->current = list->head;
        if (list->head != 0) {
            while (1) {
                SelectHotspot* hs = 0;
                ListNode* node = ((LinkedList*)SC_SelectHotSpot::hotspotList)->current;
                if (node != 0) {
                    hs = (SelectHotspot*)node->data;
                }
                hs->Update();
                LinkedList* l2 = SC_SelectHotSpot::hotspotList;
                ListNode* cur = l2->current;
                if (l2->tail == cur) break;
                if (cur != 0) {
                    l2->current = cur->next;
                }
                if (((LinkedList*)SC_SelectHotSpot::hotspotList)->head == 0) break;
            }
        }
    }
    (g_Mouse_0046aa18)->DrawCursor();
}

/* Function start: 0x4063A0 */
int SC_SelectHotSpot::AddMessage(SC_Message* msg) {
    CopyCommandData(msg);
    ((int*)msg)[0] = 0x2D;
    if (((int*)msg)[0xB] == 0x1B) {
        ((int*)msg)[1] = 1;
        ((int*)msg)[4] = 4;
        return 1;
    }
    ((int*)msg)[4] = 2;
    return 1;
}

/* Function start: 0x4063E0 */
int SC_SelectHotSpot::Exit(SC_Message* msg) {
    int* pmsg = (int*)msg;
    if (pmsg[0] != SC_SelectHotSpot::handlerId) {
        return 0;
    }
    switch (pmsg[4]) {
    case 2: {
        LinkedList* list = SC_SelectHotSpot::hotspotList;
        if (list != 0) {
            list->current = list->head;
            if (list->head != 0) {
                while (1) {
                    SelectHotspot* hs = 0;
                    ListNode* node = ((LinkedList*)SC_SelectHotSpot::hotspotList)->current;
                    if (node != 0) {
                        hs = (SelectHotspot*)node->data;
                    }
                    int result = hs->OnClick(msg);
                    if (result != 0) break;
                    LinkedList* l2 = SC_SelectHotSpot::hotspotList;
                    ListNode* cur = l2->current;
                    if (l2->tail == cur) break;
                    if (cur != 0) {
                        l2->current = cur->next;
                    }
                    if (((LinkedList*)SC_SelectHotSpot::hotspotList)->head == 0) break;
                }
            }
        }
        break;
    }
    case 7:
        SendGameMessage(1, SC_SelectHotSpot::handlerId, SC_SelectHotSpot::handlerId, SC_SelectHotSpot::moduleParam, 0x18, 0, 0, 0, 0, 0);
        break;
    default:
        return 0;
    }
    return 1;
}

/* Function start: 0x4064A0 */
void SC_SelectHotSpot::OnProcessStart() {
    if (SC_SelectHotSpot::ambient != 0) {
        delete (MMPlayer*)SC_SelectHotSpot::ambient;
        SC_SelectHotSpot::ambient = 0;
    }
    if (SC_SelectHotSpot::introSprite != 0) {
        delete (Sprite*)SC_SelectHotSpot::introSprite;
        SC_SelectHotSpot::introSprite = 0;
    }
    SC_SelectHotSpot::currentHotspot = 0;
    LinkedList* list = SC_SelectHotSpot::hotspotList;
    if (list != 0) {
        if (list->head != 0) {
            list->current = list->head;
            while (list->head != 0) {
                ListNode* node = list->current;
                SelectHotspot* hs;
                if (!node) {
                    hs = 0;
                } else {
                    if (list->head == node) list->head = node->next;
                    if (list->tail == list->current) list->tail = (ListNode*)list->current->prev;
                    node = list->current;
                    if (node->prev) node->prev->next = node->next;
                    node = list->current;
                    if (node->next) node->next->prev = node->prev;
                    node = list->current;
                    hs = 0;
                    if (node) {
                        hs = (SelectHotspot*)node->data;
                    }
                    if (node) {
                        delete node;
                        list->current = 0;
                    }
                    list->current = list->head;
                }
                if (hs) {
                    hs->~SelectHotspot();
                    FreeMemory(hs);
                }
            }
        }
        FreeMemory(list);
        SC_SelectHotSpot::hotspotList = 0;
    }
    if (SC_SelectHotSpot::palette != 0) {
        delete (Palette*)SC_SelectHotSpot::palette;
        SC_SelectHotSpot::palette = 0;
    }
    SC_SelectHotSpot::field_A8 = 0;
}

/* Function start: 0x4065E0 */
int SC_SelectHotSpot::LBLParse(char* line) {
    char nameBuf[128];
    char keyword[32];

    nameBuf[0] = 0;
    keyword[0] = 0;
    sscanf(line, "%s", keyword);

    WriteToLog("SC_SelectHotSpot::LBLParse keyword='%s' line='%.80s'", keyword, line);

    if (strcmp(keyword, "HANDLE") == 0) {
        sscanf(line, "%s %d", keyword, &SC_SelectHotSpot::moduleParam);
    } else if (strcmp(keyword, "PALETTE") == 0) {
        sscanf(line, "%s %s", keyword, nameBuf);
        if (SC_SelectHotSpot::palette != 0) {
            delete (Palette*)SC_SelectHotSpot::palette;
            SC_SelectHotSpot::palette = 0;
        }
        void* mem = AllocateMemory(8);
        void* pal = 0;
        if (mem != 0) {
            pal = InitPalette((Palette*)mem);
        }
        SC_SelectHotSpot::palette = pal;
        ((Palette*)SC_SelectHotSpot::palette)->Load(nameBuf);
    } else if (strcmp(keyword, "HOTSPOT") == 0) {
        int sortKey;
        sscanf(line, "%s %d ", keyword, &sortKey);
        if (SC_SelectHotSpot::hotspotList == 0) {
            LinkedList* newList = (LinkedList*)AllocateMemory(sizeof(LinkedList));
            if (newList != 0) {
                newList->type = 0;
                newList->head = 0;
                newList->tail = 0;
                newList->current = newList->head;
            }
            SC_SelectHotSpot::hotspotList = newList;
        }
        SelectHotspot* hs = new SelectHotspot(sortKey);
        SC_SelectHotSpot::currentHotspot = hs;
        hs->parent = this;
        Parser::ProcessFile((Parser*)SC_SelectHotSpot::currentHotspot, this, 0);

        int hsAddr = (int)SC_SelectHotSpot::currentHotspot;
        LinkedList* hsList = SC_SelectHotSpot::hotspotList;

        if (hsAddr == 0) {
            ShowError("queue fault 0101");
        }
        hsList->current = hsList->head;
        if (hsList->type == 1 || hsList->type == 2) {
            if (hsList->head == 0) {
                hsList->InsertNode((void*)hsAddr);
            } else {
                do {
                    int cur = (int)hsList->current;
                    if (*(unsigned int*)(*(int*)(cur + 8) + 0xa8) < *(unsigned int*)(hsAddr + 0xa8)) {
                        hsList->InsertNode((void*)hsAddr);
                        break;
                    }
                    if ((int)hsList->tail == cur) {
                        hsList->PushNode((void*)hsAddr);
                        break;
                    }
                    if (cur != 0) {
                        hsList->current = (ListNode*)*(int*)(cur + 4);
                    }
                } while (hsList->current != 0);
            }
        } else {
            hsList->InsertNode((void*)hsAddr);
        }
        SC_SelectHotSpot::currentHotspot = 0;
    } else if (strcmp(keyword, "AMBIENT") == 0) {
        if (SC_SelectHotSpot::ambient == 0) {
            void* mem = AllocateMemory(0xa0);
            void* amb = 0;
            if (mem != 0) {
                amb = new (mem) MMPlayer();
            }
            SC_SelectHotSpot::ambient = amb;
        }
        Parser::ProcessFile((Parser*)SC_SelectHotSpot::ambient, this, 0);
    } else if (strcmp(keyword, "INTRO") == 0) {
        if (SC_SelectHotSpot::introSprite == 0) {
            Sprite* spr = new Sprite(0);
            SC_SelectHotSpot::introSprite = spr;
        }
        Parser::ProcessFile((Parser*)SC_SelectHotSpot::introSprite, this, 0);
    } else if (strcmp(keyword, "END") == 0) {
        SC_SelectHotSpot::currentHotspot = 0;
        return 1;
    } else {
        Parser::LBLParse("SC_SelectHotSpot");
    }
    return 0;
}
