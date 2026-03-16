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

    listObj = (int*)AllocateMemory(0x18);
    if (listObj != 0) {
        listObj[2] = 0;
        listObj[3] = 0;
        listObj[1] = 0;
        listObj[0] = 0;
        listObj[4] = 0;
        listObj[5] = 10;
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

        DAT_0046a6e4 = (void*)node[2];

        nextNode = (int*)node[0];
        listPtr = (int*)itemPool;
        if ((int*)listPtr[0] == node) {
            listPtr[0] = (int)nextNode;
        } else {
            prevNode = (int*)node[1];
            prevNode[0] = (int)nextNode;
        }

        nextNode = (int*)node[1];
        if ((int*)listPtr[1] == node) {
            listPtr[1] = (int)nextNode;
        } else {
            nextNode = (int*)node[0];
            nextNode[1] = node[1];
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
        InvSlotItem* slot = &slots[0];

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

                if (mouseX >= slot->field_0 &&
                    slot->field_8 >= mouseX &&
                    slot->field_4 <= mouseY &&
                    slot->field_C >= mouseY) {
                    ((T_Object*)data)->Display(slot->field_0, slot->field_4, 1);
                } else {
                    ((T_Object*)data)->Display(slot->field_0, slot->field_4, 0);
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
                slots[selectedIdx].field_0,
                slots[selectedIdx].field_4,
                slots[selectedIdx].field_8,
                slots[selectedIdx].field_C,
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
        InvSlotItem* slot;
        int hit;

        i = 0;
        slot = &slots[0];
        selectedSlot = -1;
        do {
            if (slot->field_0 <= msgData[7] &&
                slot->field_8 >= msgData[7] &&
                slot->field_4 <= msgData[8] &&
                slot->field_C >= msgData[8]) {
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
                char* soundFile = GetSoundFilename(*(int*)((char*)(node[2]) + 0x9C));
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
                int* nextN = (int*)node[0];
                if ((int*)listPtr[0] == node) {
                    listPtr[0] = (int)nextN;
                } else {
                    ((int*)node[1])[0] = (int)nextN;
                }
            }
            if ((int*)listPtr[1] == node) {
                listPtr[1] = node[1];
            } else {
                ((int*)node[0])[1] = node[1];
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
        int id = *(int*)(self + 0x90);
        fwrite(&id, 4, 1, (FILE*)fp);
        fwrite("INVENTORY_INFO", strLen, 1, (FILE*)fp);

        esi_ptr = *(int**)(self + 0x184);
        {
        int firstNode = (int)esi_ptr[0];
        self = firstNode;
        if (firstNode != 0) {
            do {
                int* curNode = (int*)self;
                int* nextNode = (int*)curNode[0];
                char* itemBase = (char*)curNode[2] + 0x90;
                self = (int)nextNode;
                fwrite(itemBase + 4, 4, 1, (FILE*)fp);
                fwrite(itemBase, 4, 1, (FILE*)fp);
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
    *(char*)DAT_0046aa00 = 0;
    fread(DAT_0046aa00, strLen, 1, (FILE*)fp);

    if (strcmp(DAT_0046aa00, "INVENTORY_INFO") != 0) {
        ShowError("SCI_Inventory::Serialize() - Error Loading (Wrong ID '%s')", DAT_0046aa00);
    }

    DAT_0046a6e4 = 0;

    /* Free old inventory list */
    esi_ptr = *(int**)(self + 0x184);
    if (esi_ptr != 0) {
        {
            int* node = (int*)esi_ptr[0];
            while (node != 0) {
                volatile int zero = 0;
                int d = zero;
                int c;
                do { c = d; d--; } while (c != 0);
                node = (int*)node[0];
            }
        }

        esi_ptr[2] = 0;
        esi_ptr[3] = 0;
        esi_ptr[1] = 0;
        esi_ptr[0] = 0;

        {
            int* block = (int*)esi_ptr[4];
            while (block != 0) {
                int* nextBlock = (int*)block[0];
                FreeMemory(block);
                block = nextBlock;
            }
        }

        esi_ptr[4] = 0;
        FreeMemory(esi_ptr);
        *(int*)(self + 0x184) = 0;
    }

    /* Allocate new inventory list */
    esi_ptr = (int*)(new TimedEventPool(10));
    *(int*)(self + 0x184) = (int)esi_ptr;

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

        void* item = ((SCI_Inventory*)self)->FindItem(handle);
        fread((char*)item + 0x90, 4, 1, (FILE*)fp);

        {
            int* listPtr = *(int**)(self + 0x184);
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
InvSlotItem::~InvSlotItem() {}
InvPanel::InvPanel() {}
InvPanel::~InvPanel() {}

void SCI_Inventory::DisplayPanels(int param) {}
void SCI_Inventory::ProcessInventory() {}
void* SCI_Inventory::FindItem(int itemID) { return 0; }
int* SCI_Inventory::FindItemInList(int itemID) { return 0; }
