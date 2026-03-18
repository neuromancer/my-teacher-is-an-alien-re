#include "SCI_Inventory.h"
#include "T_Object.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "LinkedList.h"
#include "Sprite.h"
#include "Sample.h"
#include "InputManager.h"

extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" char* GetSoundFilename(int handle);

#include "MsgList.h"
extern MsgList* g_MsgList; // DAT_0046a6dc
#define g_InventoryList ((LinkedList*)g_MsgList)
#include "ZBufferManager.h"
extern ZBufferManager* DAT_0046aa24;
class MouseControl;
extern MouseControl* DAT_0046aa18;
extern void* DAT_0046a6e4;
extern int DAT_00473334;
extern int DAT_004733e8;

#include "MouseControl.h"
#include "T_MenuHotspot.h"
extern InputManager* DAT_0046aa08;
extern void* DAT_0046a6dc;
extern char* DAT_0046aa00;
extern void* __fastcall FUN_004407c0(void* self);

// T_Object — now implemented in T_Object.cpp

// InvListItem = T_Object (confirmed: Cleanup() maps to T_Object::~T_Object at 0x40C6E0)

// SelectionDraw = ZBufferManager::DrawRect (same 7-param signature, confirmed)

#include "SpriteAction.h"  // for SlimeDim
#include "TimedEvent.h"    // TimedEventPool = InvListObj

/* Function start: 0x43E360 */
SCI_Inventory::SCI_Inventory() {
    int* listObj;

    memset(&slots, 0, 0xF0);
    handlerId = 0x1E;

    // Inline TimedEventPool(10) construction — inlined in original binary
    listObj = (int*)AllocateMemory(0x18);
    if (listObj != 0) {
        listObj[2] = 0;    // m_count
        listObj[3] = 0;    // m_free_list
        listObj[1] = 0;    // list.tail
        listObj[0] = 0;    // list.head
        listObj[4] = 0;    // m_pool
        listObj[5] = 10;   // m_pool_size
    }
    itemPool = (void*)listObj;

    LinkedList* invList = g_InventoryList;
    if (invList->head != 0) {
        invList->current = invList->head;
        while (invList->head != 0) {
            void* data = invList->RemoveCurrent();
            if (data != 0) {
                ((T_Object*)data)->~T_Object();
                FreeMemory(data);
            }
        }
    }

    ParseFile(this, "mis\\inventor.mis", "[INVENTORY]");
}

/* Function start: 0x43E5B0 */
SCI_Inventory::~SCI_Inventory() {
}

/* Function start: 0x43E920 */
void SCI_Inventory::Init(SC_Message* msg) {
    
    int* palSlot;
    int* ptr;

    IconBar::InitIconBar(msg);

    ptr = &DAT_00473334;
    do {
        *ptr = 0;
        ptr = (int*)((char*)ptr + 0x24);
    } while ((unsigned int)ptr < (unsigned int)&DAT_004733e8);
    DAT_004733e8 = 0;

    
    // Use typed ZBufferManager palette field
    if (DAT_0046aa24->m_palette != 0) {
        ShowError("double palette");
    }
    DAT_0046aa24->m_palette = palette;
    selectedSlot = -1;
    DisplayPanels(0);
}

/* Function start: 0x43E990 */
int SCI_Inventory::ShutDown(SC_Message* msg) {
    Sprite* spr;
    void* obj;

    if (bgSprite != 0) {
        bgSprite->StopAnimationSound();
    }

    ProcessInventory();

    
    spr = DAT_0046aa18->m_sprite;
    if (spr != 0) {
        spr->ResetAnimation(0, 0);
    }

    (DAT_0046aa18)->DrawCursor();

    DAT_004733e8 = 1;
    IconBar::CleanupIconBar(msg);

    spr = putBackButton->sprite;
    if (spr != 0) {
        spr->StopAnimationSound();
    }

    spr = useButton->sprite;
    if (spr != 0) {
        spr->StopAnimationSound();
    }

    spr = scrollDownBtn->sprite;
    if (spr != 0) {
        spr->StopAnimationSound();
    }

    spr = scrollUpBtn->sprite;
    if (spr != 0) {
        spr->StopAnimationSound();
    }

    {
        Sample* sample = clickSound;
        if (sample != 0) {
            sample->Unload();
            FreeMemory(sample);
            clickSound = 0;
        }
    }
    return 0;
}

/* Function start: 0x43EA60 */
int SCI_Inventory::AddMessage(SC_Message* msg) {
    int* msgData;
    int hitResult;
    int* cursorPtr;
    int* rect;
    int selectedIdx;
    int* listPtr;
    int* node;
    int* prevNode;
    int* nextNode;

    msgData = (int*)msg;

    if (CheckButtonClick(msg) != 0) {
        goto done;
    }

    if (msgData[0xB] == 0x1B) {
        goto handle_item_click;
    }
    if (msgData[0xA] > 1) {
        goto handle_item_click;
    }
    if (msgData[9] < 2) {
        goto done;
    }

    cursorPtr = &msgData[7];

    /* Test against scrollDownBtn rectangle */
    {
        SlimeDim pos; pos.field_0 = cursorPtr[0]; pos.field_4 = cursorPtr[1];
        rect = (int*)scrollDownBtn;
        if (((T_MenuHotspot*)rect)->field_94 <= pos.field_0 &&
            ((T_MenuHotspot*)rect)->bounds.left >= pos.field_0 &&
            ((T_MenuHotspot*)rect)->field_98 <= pos.field_4 &&
            ((T_MenuHotspot*)rect)->bounds.top >= pos.field_4) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        msgData[4] = 0x12;
        goto done;
    }

    /* Test against scrollUpBtn rectangle */
    {
        SlimeDim pos; pos.field_0 = cursorPtr[0]; pos.field_4 = cursorPtr[1];
        rect = (int*)scrollUpBtn;
        if (((T_MenuHotspot*)rect)->field_94 <= pos.field_0 &&
            ((T_MenuHotspot*)rect)->bounds.left >= pos.field_0 &&
            ((T_MenuHotspot*)rect)->field_98 <= pos.field_4 &&
            ((T_MenuHotspot*)rect)->bounds.top >= pos.field_4) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        msgData[4] = 0x11;
        goto done;
    }

    /* Test against useButton rectangle */
    {
        SlimeDim pos; pos.field_0 = cursorPtr[0]; pos.field_4 = cursorPtr[1];
        rect = (int*)useButton;
        if (((T_MenuHotspot*)rect)->field_94 <= pos.field_0 &&
            ((T_MenuHotspot*)rect)->bounds.left >= pos.field_0 &&
            ((T_MenuHotspot*)rect)->field_98 <= pos.field_4 &&
            ((T_MenuHotspot*)rect)->bounds.top >= pos.field_4) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        msgData[0] = savedCommand;
        msgData[4] = 4;
        msgData[1] = savedMsgData;

        if (selectedSlot == -1) {
            goto done;
        }

        selectedIdx = scrollOffset + selectedSlot;
        listPtr = (int*)itemPool;
        if (listPtr[2] <= selectedIdx) {
            node = 0;
        } else {
            node = (int*)listPtr[0];
            {
                int i = selectedIdx;
                selectedIdx = i - 1;
                while (i != 0) {
                    node = (int*)node[0];
                    i = selectedIdx;
                    selectedIdx = i - 1;
                }
            }
        }

        if (node == 0) {
            goto done;
        }

        DAT_0046a6e4 = ((QueueNode*)node)->data;

        nextNode = (int*)((QueueNode*)node)->next;
        listPtr = (int*)itemPool;
        if ((int*)listPtr[0] == node) {
            listPtr[0] = (int)nextNode;
        } else {
            prevNode = (int*)((QueueNode*)node)->prev;
            prevNode[0] = (int)nextNode;
        }

        nextNode = (int*)((QueueNode*)node)->prev;
        if ((int*)listPtr[1] == node) {
            listPtr[1] = (int)nextNode;
        } else {
            nextNode = (int*)((QueueNode*)node)->next;
            ((QueueNode*)nextNode)->prev = ((QueueNode*)node)->prev;
        }

        {
            volatile int zero = 0;
            int i = zero;
            int j = i - 1;
            while (i != 0) {
                i = j;
                j = i - 1;
            }
        }

        node[0] = listPtr[3];
        listPtr[3] = (int)node;
        listPtr[2]--;

        {
            void* invItem = DAT_0046a6e4;
            Sprite* spr = DAT_0046aa18->m_sprite;
            int yOffset = ((T_Object*)invItem)->itemId + 0x1D;
            if (spr != 0) {
                spr->ResetAnimation(0, yOffset);
            }
        }
        goto done;
    }

    /* Test against putBackButton rectangle */
    {
        SlimeDim pos; pos.field_0 = cursorPtr[0]; pos.field_4 = cursorPtr[1];
        rect = (int*)putBackButton;
        if (((T_MenuHotspot*)rect)->field_94 <= pos.field_0 &&
            ((T_MenuHotspot*)rect)->bounds.left >= pos.field_0 &&
            ((T_MenuHotspot*)rect)->field_98 <= pos.field_4 &&
            ((T_MenuHotspot*)rect)->bounds.top >= pos.field_4) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        if (DAT_0046a6e4 == 0) {
            goto handle_item_click;
        }
        SendGameMessage(0x1E, ((T_Object*)DAT_0046a6e4)->itemId, 0x1E, 0, 0x17, 0, 0, 0, 0, 0);
        {
            Sprite* spr = DAT_0046aa18->m_sprite;
            if (spr != 0) {
                spr->ResetAnimation(0, 0);
            }
        }
        DAT_0046a6e4 = 0;
        goto handle_item_click;
    }

    msgData[4] = 2;
    msgData[0] = 0x1E;
    if (DAT_0046a6e4 != 0) {
        msgData[5] = ((T_Object*)DAT_0046a6e4)->itemId;
    }
    goto done;

handle_item_click:
    msgData[0] = savedCommand;
    msgData[4] = 4;
    msgData[1] = savedMsgData;
done:
    return 1;
}

/* Function start: 0x43EE20 */
void SCI_Inventory::Update(int param1, int param2) {
    int* curNode;
    int startIdx;
    int counter;
    Sprite* spr;

    if (handlerId != param2) {
        return;
    }

    counter = 0;
    IconBar::Update(param1, param2);

    if (bgSprite != 0) {
        bgSprite->Do(0, 0, 1.0);
    }

    putBackButton->Update();
    useButton->Update();
    scrollDownBtn->Update();
    scrollUpBtn->Update();

    startIdx = scrollOffset;
    {
        int* listPtr = (int*)itemPool;
        if (listPtr[2] <= startIdx) {
            curNode = 0;
        } else {
            curNode = (int*)listPtr[0];
            {
                int i = startIdx;
                startIdx = i - 1;
                while (i != 0) {
                    curNode = (int*)curNode[0];
                    i = startIdx;
                    startIdx = i - 1;
                }
            }
        }
    }

    {
        Rect* slot = &slots[0];

        do {
            if (curNode == 0) break;

            int* nextNode = (int*)curNode[0];
            void* data = (void*)curNode[2];
            curNode = nextNode;

            if (data != 0) {
                int* mouseObj = (int*)DAT_0046aa08->pMouse;
                int mouseY = 0;
                if (mouseObj != 0) {
                    mouseY = mouseObj[1];
                }
                int mouseX = 0;
                if (mouseObj != 0) {
                    mouseX = mouseObj[0];
                }

                if (mouseX >= slot->left &&
                    slot->right >= mouseX &&
                    slot->top <= mouseY &&
                    slot->bottom >= mouseY) {
                    ((T_Object*)data)->Display(slot->left, slot->top, 1);
                } else {
                    ((T_Object*)data)->Display(slot->left, slot->top, 0);
                }
            }

            slot++;
            counter++;
        } while (counter <= 7);
    }

    {
        int selectedIdx = selectedSlot;
        if (selectedIdx != -1) {
            DAT_0046aa24->DrawRect(
                slots[selectedIdx].left,
                slots[selectedIdx].top,
                slots[selectedIdx].right,
                slots[selectedIdx].bottom,
                0x2710, 0xFC, 2);
        }
    }

    if (DAT_0046a6e4 != 0) {
        int yOffset = ((T_Object*)DAT_0046a6e4)->itemId + 0x1D;
        spr = DAT_0046aa18->m_sprite;
        if (spr != 0) {
            spr->ResetAnimation(yOffset, 0);
        }
    } else {
        spr = DAT_0046aa18->m_sprite;
        if (spr != 0) {
            spr->ResetAnimation(0, 0);
        }
    }

    (DAT_0046aa18)->DrawCursor();
}

/* Function start: 0x43EFD0 */
int SCI_Inventory::Exit(SC_Message* msg) {
    int* msgData;

    msgData = (int*)msg;

    if (handlerId != msgData[0]) {
        return 0;
    }

    switch (msgData[4]) {
    default:
        return 0;
    case 0:
    case 7:
    case 0x19:
    case 0x1A:
        break;
    case 2: {
        int i;
        Rect* slot;
        int hit;

        i = 0;
        slot = &slots[0];
        selectedSlot = -1;
        do {
            if (slot->left <= msgData[7] &&
                slot->right >= msgData[7] &&
                slot->top <= msgData[8] &&
                slot->bottom >= msgData[8]) {
                hit = 1;
            } else {
                hit = 0;
            }
            if (hit) {
                selectedSlot = i;
            }
            slot++;
            i++;
        } while (i < 8);

        if (selectedSlot < 0 || selectedSlot >= 8) {
            break;
        }

        if (DAT_0046a6e4 != 0) {
            SendGameMessage(0x1e, ((T_Object*)DAT_0046a6e4)->itemId, 0x1e, 0, 0x17, 0, 0, 0, 0, 0);
            {

                Sprite* spr = DAT_0046aa18->m_sprite;
                if (spr != 0) {
                    goto do_reset;
                }
                goto do_clear;
            }
        }

        {
            int idx;
            int* listPtr;
            int* node;

            idx = scrollOffset + selectedSlot;
            listPtr = (int*)itemPool;
            if (listPtr[2] <= idx) {
                node = 0;
            } else {
                node = (int*)listPtr[0];
                {
                    int j = idx;
                    idx = j - 1;
                    while (j != 0) {
                        node = (int*)node[0];
                        j = idx;
                        idx = j - 1;
                    }
                }
            }

            if (node == 0) {
                break;
            }

            {
                Sample* oldSample = clickSound;
                if (oldSample != 0) {
                    oldSample->Unload();
                    FreeMemory(oldSample);
                    clickSound = 0;
                }
            }

            {
                Sample* newSample = new Sample();
                clickSound = newSample;
                char* soundFile = GetSoundFilename(((T_Object*)((QueueNode*)node)->data)->field_98);
                newSample->Load(soundFile);
                (clickSound)->Play(0x64, 1);
            }
        }
        break;
    }
    case 6:
    case 0x35:
    case 0x36: {
        void* item = FindItem(msgData[1]);
        if (item == 0) {
            break;
        }
        ((T_Object*)item)->HandleMsg(msgData);
        break;
    }
    case 0x11:
        scrollOffset += 2;
        {
            int maxScroll = ((int*)itemPool)[2] - 8;
            if (maxScroll < scrollOffset) {
                scrollOffset = maxScroll;
            }
        }
        if (scrollOffset < 0) {
            scrollOffset = 0;
        }
        break;
    case 0x12:
        scrollOffset -= 2;
        if (scrollOffset < 0) {
            scrollOffset = 0;
        }
        break;
    case 0x17: {
        int* ebx;
        void* item;
        int* head;

        msgData = (int*)msgData[1];
        if (msgData == 0) break;

        if (FindItemInList((int)msgData) != 0) break;

        if (DAT_0046a6e4 != 0) {
            if (((T_Object*)DAT_0046a6e4)->itemId == (int)msgData) {
                Sprite* spr = DAT_0046aa18->m_sprite;
                if (spr != 0) {
                    spr->ResetAnimation(0, 0);
                }
                DAT_0046a6e4 = 0;
            }
        }

        item = FindItem((int)msgData);
        ((T_Object*)item)->Reset();

        ebx = (int*)itemPool;
        head = (int*)ebx[0];

        if (ebx[3] == 0) {
            int* block = (int*)AllocateMemory(ebx[5] * 12 + 4);
            block[0] = ebx[4];
            ebx[4] = (int)block;

            {
                int j = ebx[5] - 1;
                int* nodePtr = (int*)((char*)block + ebx[5] * 12 - 8);
                while (j >= 0) {
                    nodePtr[0] = ebx[3];
                    ebx[3] = (int)nodePtr;
                    nodePtr = (int*)((char*)nodePtr - 12);
                    j--;
                }
            }
        }

        {
            int* newNode = (int*)ebx[3];
            ebx[3] = newNode[0];
            newNode[1] = 0;
            newNode[0] = (int)head;
            ebx[2]++;
            newNode[2] = 0;

            {
                volatile int zero = 0;
                int k = zero;
                int l = k - 1;
                while (k != 0) {
                    k = l;
                    l = k - 1;
                }
            }

            newNode[2] = (int)item;

            if ((int*)ebx[0] != 0) {
                ((int*)ebx[0])[1] = (int)newNode;
            } else {
                ebx[1] = (int)newNode;
            }
            ebx[0] = (int)newNode;
        }

        DisplayPanels((int)msgData);
        break;
    }
    case 0x18: {
        int* node;
        int* listPtr;

        if (msgData[1] == 0) break;

        node = (int*)FindItemInList(msgData[1]);
        if (node != 0) {
            listPtr = (int*)itemPool;
            {
                QueueNode* nextN = ((QueueNode*)node)->next;
                if ((int*)listPtr[0] == node) {
                    listPtr[0] = (int)nextN;
                } else {
                    ((QueueNode*)node)->prev->next = nextN;
                }
            }
            if ((int*)listPtr[1] == node) {
                listPtr[1] = (int)((QueueNode*)node)->prev;
            } else {
                ((QueueNode*)node)->next->prev = ((QueueNode*)node)->prev;
            }

            {
                volatile int zero = 0;
                int k = zero;
                int l = k - 1;
                while (k != 0) {
                    k = l;
                    l = k - 1;
                }
            }

            node[0] = listPtr[3];
            listPtr[3] = (int)node;
            listPtr[2]--;
            break;
        }

        if (DAT_0046a6e4 == 0) break;
        if (((T_Object*)DAT_0046a6e4)->itemId != msgData[1]) break;
        {
            Sprite* spr = DAT_0046aa18->m_sprite;
            if (spr == 0) goto do_clear;
        }
    do_reset:
        (DAT_0046aa18->m_sprite)->ResetAnimation(0, 0);
    do_clear:
        DAT_0046a6e4 = 0;
        break;
    }
    }
    return 1;
}

/* Function start: 0x43F9B0 */
void SCI_Inventory::Serialize(void* param) {
    volatile int self = (int)this;
    int strLen = strlen("INVENTORY_INFO") + 1;
    volatile int fp = (int)*(FILE**)((char*)param + 0x44);
    int* esi_ptr;
    int handle;

    if (*(int*)param != 0) {
        /* SAVE PATH */
        int id = handlerId;
        fwrite(&id, 4, 1, (FILE*)fp);
        fwrite("INVENTORY_INFO", strLen, 1, (FILE*)fp);

        esi_ptr = (int*)itemPool;
        {
        QueueNode* curNode = ((LinkedList*)esi_ptr)->head;
        self = (int)curNode;
        if (curNode != 0) {
            do {
                QueueNode* nextNode = curNode->next;
                T_Object* item = (T_Object*)curNode->data;
                self = (int)nextNode;
                fwrite(&item->itemId, 4, 1, (FILE*)fp);
                fwrite(&item->field_90, 4, 1, (FILE*)fp);
                curNode = nextNode;
            } while (self != 0);
        }}

        handle = 999;
        fwrite(&handle, 4, 1, (FILE*)fp);

        if (DAT_0046a6e4 != 0) {
            fwrite((char*)&((T_Object*)DAT_0046a6e4)->itemId, 4, 1, (FILE*)fp);
            fwrite((char*)&((T_Object*)DAT_0046a6e4)->field_90, 4, 1, (FILE*)fp);
        } else {
            fwrite(&handle, 4, 1, (FILE*)fp);
        }
        return;
    }

    /* LOAD PATH */
    DAT_0046aa00[0] = 0;
    fread(DAT_0046aa00, strLen, 1, (FILE*)fp);

    if (strcmp(DAT_0046aa00, "INVENTORY_INFO") != 0) {
        ShowError("SCI_Inventory::Serialize() - Error Loading (Wrong ID '%s')", DAT_0046aa00);
    }

    DAT_0046a6e4 = 0;

    /* Free old inventory list */
    TimedEventPool* pool = (TimedEventPool*)((SCI_Inventory*)self)->itemPool;
    if (pool != 0) {
        {
            int* node = (int*)((int*)pool)[0];
            while (node != 0) {
                volatile int zero = 0;
                int d = zero;
                int c;
                do { c = d; d--; } while (c != 0);
                node = (int*)node[0];
            }
        }

        ((int*)pool)[2] = 0;
        ((int*)pool)[3] = 0;
        ((int*)pool)[1] = 0;
        ((int*)pool)[0] = 0;

        {
            int* block = (int*)((int*)pool)[4];
            while (block != 0) {
                int* nextBlock = (int*)block[0];
                FreeMemory(block);
                block = nextBlock;
            }
        }

        ((int*)pool)[4] = 0;
        FreeMemory(pool);
        ((SCI_Inventory*)self)->itemPool = 0;
    }

    /* Allocate new inventory list */
    ((SCI_Inventory*)self)->itemPool = new TimedEventPool(10);

    /* Clean up global queue */
    if (DAT_0046a6dc != 0) {
        esi_ptr = (int*)DAT_0046a6dc;
        if (esi_ptr[0] != 0) {
            esi_ptr[2] = esi_ptr[0];
            while (esi_ptr[0] != 0) {
                void* data = FUN_004407c0(esi_ptr);
                if (data != 0) {
                    ((T_Object*)data)->~T_Object();
                    FreeMemory(data);
                }
            }
        }
        FreeMemory(esi_ptr);
        DAT_0046a6dc = 0;
    }

    /* Allocate new global queue */
    esi_ptr = (int*)(new LinkedList());
    DAT_0046a6dc = (void*)esi_ptr;

    /* Read items loop */
    for (;;) {
        fread(&handle, 4, 1, (FILE*)fp);
        if (handle == 999) break;

        T_Object* item = (T_Object*)((SCI_Inventory*)self)->FindItem(handle);
        fread(&item->field_90, 4, 1, (FILE*)fp);

        {
            int* listPtr = (int*)((SCI_Inventory*)self)->itemPool;
            int* tail = (int*)listPtr[1];

            if (listPtr[3] == 0) {
                int* block = (int*)AllocateMemory(listPtr[5] * 12 + 4);
                block[0] = listPtr[4];
                listPtr[4] = (int)block;

                int j = listPtr[5] - 1;
                int* nodePtr = (int*)((char*)block + listPtr[5] * 12 - 8);
                while (j >= 0) {
                    nodePtr[0] = listPtr[3];
                    listPtr[3] = (int)nodePtr;
                    nodePtr = (int*)((char*)nodePtr - 12);
                    j--;
                }
            }

            {
                int* newNode = (int*)listPtr[3];
                listPtr[3] = newNode[0];
                newNode[1] = (int)tail;
                newNode[0] = 0;
                listPtr[2]++;
                newNode[2] = 0;

                {
                    volatile int zero = 0;
                    int d = zero;
                    int c;
                    do { c = d; d--; } while (c != 0);
                }

                newNode[2] = (int)item;

                if ((int*)listPtr[1] != 0) {
                    ((int*)listPtr[1])[0] = (int)newNode;
                } else {
                    listPtr[0] = (int)newNode;
                }
                listPtr[1] = (int)newNode;
            }
        }
    }

    /* Read held item */
    fread(&handle, 4, 1, (FILE*)fp);
    if (handle == 999) return;

    DAT_0046a6e4 = ((SCI_Inventory*)self)->FindItem(handle);
    fread((char*)&((T_Object*)DAT_0046a6e4)->field_90, 4, 1, (FILE*)fp);
}

// Stubs (moved from stubs.cpp)
Rect::~Rect() {}
#include "Palette.h"
#include "GameState.h"
#include <new.h>
extern void* DAT_0046a6dc;
extern "C" GameState* DAT_0046aa30;

/* Function start: 0x43FDD0 */
int SCI_Inventory::LBLParse(char* line) {
    char token[32];
    char name[32];
    int params[5];
    int index;
    void* mem;
    void* obj;

    params[0] = 0;
    params[1] = 0;
    params[2] = 0;
    params[3] = 0;
    params[4] = 0;
    token[0] = 0;
    name[0] = 0;
    index = 0;

    sscanf(line, "%s", token);

    if (strcmp(token, "BACKGROUND") == 0) {
        if (bgSprite == 0) {
            bgSprite = new Sprite((char*)0);
        }
        Parser::ProcessFile(bgSprite, this, (char*)0);
    }
    else if (strcmp(token, "OBJECT") == 0) {
        sscanf(line, " %s %d", token, &index);
        obj = new T_Object(index);
        int* queue = (int*)DAT_0046a6dc;
        if (obj == 0) {
            ShowError("queue fault 0101");
        }
        queue[2] = queue[0];
        if (queue[3] == 1 || queue[3] == 2) {
            if (queue[0] == 0) {
                ((Queue*)queue)->InsertAtCurrent(obj);
            } else {
                do {
                    int cur = queue[2];
                    if ((unsigned int)((T_Object*)((QueueNode*)cur)->data)->itemId < (unsigned int)((T_Object*)obj)->itemId) {
                        ((Queue*)queue)->InsertAtCurrent(obj);
                        break;
                    }
                    if (queue[1] == cur) {
                        if (obj == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = malloc(0xc);
                        if (mem == 0) {
                            obj = 0;
                        } else {
                            ((int*)mem)[2] = (int)obj;
                            ((int*)mem)[0] = 0;
                            ((int*)mem)[1] = 0;
                            obj = mem;
                        }
                        if (queue[2] == 0) {
                            queue[2] = queue[1];
                        }
                        if (queue[0] == 0) {
                            queue[0] = (int)obj;
                            queue[1] = (int)obj;
                            queue[2] = (int)obj;
                        } else {
                            if (queue[1] == 0 || *(int*)(queue[1] + 4) != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((int*)obj)[1] = 0;
                            ((int*)obj)[0] = queue[1];
                            *(int*)(queue[1] + 4) = (int)obj;
                            queue[1] = (int)obj;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue[2] = *(int*)(cur + 4);
                    }
                } while (queue[2] != 0);
            }
        } else {
            ((Queue*)queue)->InsertAtCurrent(obj);
        }
    }
    else if (strcmp(token, "PALETTE") == 0) {
        sscanf(line, " %s %s", token, name);
        palette = new Palette();
        palette->Load(name);
    }
    else if (strcmp(token, "CANCEL") == 0) {
        sscanf(line, " %s %s %d %d %d %d", token, name, &params[0], &params[1], &params[2], &params[3]);
        putBackButton = new T_MenuHotspot(name, params);
    }
    else if (strcmp(token, "USE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", token, name, &params[0], &params[1], &params[2], &params[3]);
        useButton = new T_MenuHotspot(name, params);
    }
    else if (strcmp(token, "SCROLLUP") == 0) {
        sscanf(line, " %s %s %d %d %d %d", token, name, &params[0], &params[1], &params[2], &params[3]);
        scrollDownBtn = new T_MenuHotspot(name, params);
    }
    else if (strcmp(token, "SCROLLDOWN") == 0) {
        sscanf(line, " %s %s %d %d %d %d", token, name, &params[0], &params[1], &params[2], &params[3]);
        scrollUpBtn = new T_MenuHotspot(name, params);
    }
    else if (strcmp(token, "AUTOCOMBINE") == 0) {
        sscanf(line, " %s %d %d %d %d %d %d %s", token, &index, &params[0], &params[1], &params[2], &params[3], &params[4], name);
        panels[index].field_0 = params[0];
        panels[index].field_4 = params[1];
        panels[index].field_8 = params[2];
        panels[index].field_C = params[3];
        panels[index].field_10 = params[4];
        panels[index].field_14 = DAT_0046aa30->FindState(name);
    }
    else if (strcmp(token, "HITAREA") == 0) {
        sscanf(line, " %s %d %d %d %d %d", token, &index, &params[0], &params[1], &params[2], &params[3]);
        slots[index].left = params[0];
        slots[index].top = params[1];
        slots[index].right = params[2];
        slots[index].bottom = params[3];
    }
    else if (strcmp(token, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("SCI_Inventory");
    }

    return 0;
}

InvPanel::InvPanel() {}
InvPanel::~InvPanel() {}

/* Function start: 0x43F840 */
void SCI_Inventory::DisplayPanels(int param) {
    int count = 3;
    int* panelPtr = (int*)&panels[0];
    do {
        if (panelPtr[0] == 0) {
            goto check_items;
        } else {
            int* found = FindItemInList(panelPtr[0]);
            if (found == 0) goto next_panel;
        }
check_items:
        if (panelPtr[1] != 0) {
            int* found = FindItemInList(panelPtr[1]);
            if (found == 0) goto next_panel;
        }
        if (panelPtr[2] != 0) {
            int* found = FindItemInList(panelPtr[2]);
            if (found == 0) goto next_panel;
        }
        {
            GameState* gs = DAT_0046aa30;
            int gsIdx = panelPtr[5];
            if (gsIdx < 0 || gs->maxStates - 1 < gsIdx) {
                ShowError("Invalid gamestate %d", gsIdx);
            }
            if (gs->stateValues[gsIdx] != 0) {
                SendGameMessage(0x1e, panelPtr[0], handlerId, 0, 0x18, 0, 0, 0, 0, 0);
                SendGameMessage(0x1e, panelPtr[1], handlerId, 0, 0x18, 0, 0, 0, 0, 0);
                SendGameMessage(0x1e, panelPtr[2], handlerId, 0, 0x18, 0, 0, 0, 0, 0);
                SendGameMessage(0x1e, panelPtr[3], handlerId, 0, 0x17, 0, 0, 0, 0, 0);
                if (panelPtr[0] == param || panelPtr[1] == param || panelPtr[2] == param) {
                    SendGameMessage(4, panelPtr[4], handlerId, 0, 2, 0, 0, 0, 0, 0);
                }
            }
        }
next_panel:
        panelPtr += 6;
        count--;
        if (count == 0) {
            return;
        }
    } while (1);
}
/* Function start: 0x43F420 */
void SCI_Inventory::ProcessInventory() {
    LinkedList* list = (LinkedList*)DAT_0046a6dc;
    if (list != 0 && list->head != 0) {
        list->current = list->head;
        while (1) {
            QueueNode* node = (QueueNode*)list->current;
            if (node != 0 && node->data != 0) {
                ((T_Object*)node->data)->StopSound();
            }
            QueueNode* cur = (QueueNode*)list->current;
            if (list->tail == cur) break;
            if (cur != 0) {
                list->current = cur->next;
            }
        }
    }
}
void* SCI_Inventory::FindItem(int itemID) { return 0; }
/* Function start: 0x43F7F0 */
int* SCI_Inventory::FindItemInList(int itemID) {
    QueueNode* node = ((LinkedList*)itemPool)->head;
    if (node == 0) {
        return 0;
    }
    do {
        if (((T_Object*)node->data)->itemId == itemID) {
            return (int*)node;
        }
        node = node->next;
    } while (node != 0);
    return 0;
}
