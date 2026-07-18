#include "SCI_Inventory.h"
#include "T_Object.h"
#include "FileArchive.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "LinkedList.h"
#include "Sprite.h"
#include "Sample.h"
#include "Palette.h"
#include "InputManager.h"


#include "MsgList.h"
#include "string.h"
#include "MouseControl.h"
// The no-op cast is load-bearing: without it MSVC swaps the counter/curNode
// register assignment in SCI_Inventory::Update (94.74% -> 93.23%).
#define g_InventoryList ((LinkedList*)g_MsgList_0046a6dc)
#include "ZBufferManager.h"

#include "T_MenuHotspot.h"
// DAT_0046a6dc is g_MsgList_0046a6dc — use g_MsgList_0046a6dc via globals.h
// FUN_004407c0 = RemoveCurrent COMDAT — callers updated

// T_Object — now implemented in T_Object.cpp

// InvListItem = T_Object (confirmed: Cleanup() maps to T_Object::~T_Object at 0x40C6E0)

// SelectionDraw = ZBufferManager::DrawRect (same 7-param signature, confirmed)

#include "SpriteAction.h"  // for SlimeDim
#include "TimedEvent.h"    // TimedEventPool constructor provides the same 0x18-byte pool header

/* Function start: 0x43E360 */
SCI_Inventory::SCI_Inventory() {
    memset(&slots, 0, 0xF0);
    handlerId = 0x1E;

    itemPool = (InventoryPool*)new TimedEventPool(10);

    LinkedList* invList = g_InventoryList;
    if (invList->head != 0) {
        invList->current = invList->head;
        while (invList->head != 0) {
            void* data = invList->RemoveCurrent();
            if (data != 0) {
                delete (T_Object*)data;
            }
        }
    }

    ParseFile(this, "mis\\inventor.mis", "[INVENTORY]");
}

/* Function start: 0x43E900 */
void __fastcall FreePoolBlocks(void* blocks) {
    InventoryPoolBlock* block = (InventoryPoolBlock*)blocks;
    while (block != 0) {
        InventoryPoolBlock* next = block->next;
        FreeMemory(block);
        block = next;
    }
}

/* Function start: 0x43E5B0 */
SCI_Inventory::~SCI_Inventory() {
    Sprite* spr;
    Palette* pal;
    InventoryPool* pool;

    spr = bgSprite;
    if (spr != 0) {
        spr->~Sprite();
        FreeMemory(spr);
        bgSprite = 0;
    }

    pal = palette;
    if (pal != 0) {
        pal->~Palette();
        FreeMemory(pal);
        palette = 0;
    }

    pool = itemPool;
    if (pool != 0) {
        {
            InventoryPoolNode* node = pool->head;
            while (node != 0) {
                int counter = 0;
                do {
                    int tmp = counter;
                    counter--;
                    if (tmp == 0) break;
                } while (1);
                node = node->next;
            }
        }
        pool->count = 0;
        pool->freeList = 0;
        pool->tail = 0;
        pool->head = 0;
        FreePoolBlocks(pool->blocks);
        pool->blocks = 0;
        FreeMemory(pool);
        itemPool = 0;
    }

    if (putBackButton != 0) {
        delete putBackButton;
        putBackButton = 0;
    }

    if (useButton != 0) {
        delete useButton;
        useButton = 0;
    }

    if (scrollDownBtn != 0) {
        delete scrollDownBtn;
        scrollDownBtn = 0;
    }

    if (scrollUpBtn != 0) {
        delete scrollUpBtn;
        scrollUpBtn = 0;
    }

    {
        Sample* snd = clickSound;
        if (snd != 0) {
            snd->Unload();
            FreeMemory(snd);
            clickSound = 0;
        }
    }
}

/* Function start: 0x43E920 */ /* No assembly extracted */
void SCI_Inventory::Init(SC_MessageParser* msg) {
    
    int* palSlot;
    int* ptr;

    IconBar::InitIconBar(msg);

    // Original quirk: walks entry.field_14 through all six IconBarEntry slots
    // (stride 0x24) relying on g_InventoryState living right after the array.
    ptr = &g_IconBarState_00473334;
    do {
        *ptr = 0;
        ptr += sizeof(IconBarEntry) / sizeof(int);
    } while ((unsigned int)ptr < (unsigned int)&g_InventoryState_004733e8);
    g_InventoryState_004733e8 = 0;

    
    // Use typed ZBufferManager palette field
    if (g_ZBufferManager_0046aa24->m_palette != 0) {
        ShowError("double palette");
    }
    g_ZBufferManager_0046aa24->m_palette = palette;
    selectedSlot = -1;
    DisplayPanels(0);
}

/* Function start: 0x43E990 */
void SCI_Inventory::ShutDown(SC_MessageParser* msg) {
    Sprite* spr;
    void* obj;

    if (bgSprite != 0) {
        bgSprite->StopAnimationSound();
    }

    ProcessInventory();

    
    spr = g_Mouse_0046aa18->m_sprite;
    if (spr != 0) {
        spr->ResetAnimation(0, 0);
    }

    (g_Mouse_0046aa18)->DrawCursor();

    g_InventoryState_004733e8 = 1;
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
    return;
}

/* Function start: 0x43EA60 */
int SCI_Inventory::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action;
    int hitResult;
    int* cursorPtr;
    int* rect;
    int selectedIdx;
    InventoryPool* pool;
    InventoryPoolNode* node;
    InventoryPoolNode* prevNode;
    InventoryPoolNode* nextNode;

    action = (SpriteAction*)msg;

    if (CheckButtonClick(msg) != 0) {
        goto done;
    }

    if (action->lastKey == 0x1B) {
        goto handle_item_click;
    }
    if (action->button2 > 1) {
        goto handle_item_click;
    }
    if (action->button1 < 2) {
        goto done;
    }

    cursorPtr = &action->mousePos.x;

    /* Test against scrollDownBtn rectangle */
    {
        SlimeDim pos = *(SlimeDim*)cursorPtr;
        rect = (int*)scrollDownBtn;
        if (((T_MenuButton*)rect)->bounds.left <= pos.x &&
            ((T_MenuButton*)rect)->bounds.right >= pos.x &&
            ((T_MenuButton*)rect)->bounds.top <= pos.y &&
            ((T_MenuButton*)rect)->bounds.bottom >= pos.y) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        action->instruction = 0x12;
        goto done;
    }

    /* Test against scrollUpBtn rectangle */
    {
        SlimeDim pos = *(SlimeDim*)cursorPtr;
        rect = (int*)scrollUpBtn;
        if (((T_MenuButton*)rect)->bounds.left <= pos.x &&
            ((T_MenuButton*)rect)->bounds.right >= pos.x &&
            ((T_MenuButton*)rect)->bounds.top <= pos.y &&
            ((T_MenuButton*)rect)->bounds.bottom >= pos.y) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        action->instruction = 0x11;
        goto done;
    }

    /* Test against useButton rectangle */
    {
        SlimeDim pos = *(SlimeDim*)cursorPtr;
        rect = (int*)useButton;
        if (((T_MenuButton*)rect)->bounds.left <= pos.x &&
            ((T_MenuButton*)rect)->bounds.right >= pos.x &&
            ((T_MenuButton*)rect)->bounds.top <= pos.y &&
            ((T_MenuButton*)rect)->bounds.bottom >= pos.y) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        action->addressType = savedCommand;
        action->instruction = 4;
        action->addressValue = savedMsgData;

        selectedIdx = selectedSlot;
        if (selectedIdx == -1) {
            goto done;
        }

        selectedIdx += scrollOffset;
        pool = itemPool;
        if (pool->count <= selectedIdx) {
            node = 0;
        } else {
            node = pool->head;
            {
                int i = selectedIdx;
                selectedIdx = i - 1;
                while (i != 0) {
                    node = node->next;
                    i = selectedIdx;
                    selectedIdx = i - 1;
                }
            }
        }

        if (node == 0) {
            goto done;
        }

        g_SelectedItem_0046a6e4 = node->item;

        nextNode = node->next;
        pool = itemPool;
        if (pool->head == node) {
            pool->head = nextNode;
        } else {
            prevNode = node->prev;
            prevNode->next = nextNode;
        }

        prevNode = node->prev;
        if (pool->tail == node) {
            pool->tail = prevNode;
        } else {
            nextNode = node->next;
            nextNode->prev = node->prev;
        }

        { int i = 0; while (i-- != 0); }

        node->next = pool->freeList;
        pool->freeList = node;
        pool->count--;

        {
            void* invItem = g_SelectedItem_0046a6e4;
            Sprite* spr = g_Mouse_0046aa18->m_sprite;
            int yOffset = ((T_Object*)invItem)->itemId + 0x1D;
            if (spr != 0) {
                spr->ResetAnimation(yOffset, 0);
            }
        }
        goto done;
    }

    /* Test against putBackButton rectangle */
    {
        SlimeDim pos = *(SlimeDim*)cursorPtr;
        rect = (int*)putBackButton;
        if (((T_MenuButton*)rect)->bounds.left <= pos.x &&
            ((T_MenuButton*)rect)->bounds.right >= pos.x &&
            ((T_MenuButton*)rect)->bounds.top <= pos.y &&
            ((T_MenuButton*)rect)->bounds.bottom >= pos.y) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        if (g_SelectedItem_0046a6e4 == 0) {
            goto handle_item_click;
        }
        SendGameMessage(0x1E, g_SelectedItem_0046a6e4->itemId, 0x1E, 0, 0x17, 0, 0, 0, 0, 0);
        {
            Sprite* spr = g_Mouse_0046aa18->m_sprite;
            if (spr != 0) {
                spr->ResetAnimation(0, 0);
            }
        }
        g_SelectedItem_0046a6e4 = 0;
        goto handle_item_click;
    }

    action->instruction = 2;
    action->addressType = 0x1E;
    if (g_SelectedItem_0046a6e4 != 0) {
        action->extra1 = g_SelectedItem_0046a6e4->itemId;
    }
    goto done;

handle_item_click:
    action->addressType = savedCommand;
    action->instruction = 4;
    action->addressValue = savedMsgData;
done:
    return 1;
}

/* Function start: 0x43EE20 */
void SCI_Inventory::Update(int param1, int param2) {
    InventoryPoolNode* curNode;
    InventoryPool* pool;
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

    putBackButton->SimpleUpdate();
    useButton->SimpleUpdate();
    scrollDownBtn->SimpleUpdate();
    scrollUpBtn->SimpleUpdate();

    startIdx = scrollOffset;
    {
        pool = itemPool;
        if (pool->count <= startIdx) {
            curNode = 0;
        } else {
            curNode = pool->head;
            {
                int i = startIdx;
                startIdx = i - 1;
                while (i != 0) {
                    curNode = curNode->next;
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

            InventoryPoolNode* nextNode = curNode->next;
            T_Object* data = curNode->item;
            curNode = nextNode;

            if (data != 0) {
                int mouseY;
                if (g_InputManager_0046aa08->pMouse != 0) {
                    mouseY = ((int*)g_InputManager_0046aa08->pMouse)[1];
                } else {
                    mouseY = 0;
                }
                int mouseX;
                if (g_InputManager_0046aa08->pMouse != 0) {
                    mouseX = ((int*)g_InputManager_0046aa08->pMouse)[0];
                } else {
                    mouseX = 0;
                }

                if (mouseX >= slot->left &&
                    slot->right >= mouseX &&
                    slot->top <= mouseY &&
                    slot->bottom >= mouseY) {
                    data->Display(slot->left, slot->top, 1);
                } else {
                    data->Display(slot->left, slot->top, 0);
                }
            }

            slot++;
            counter++;
        } while (counter <= 7);
    }

    {
        int selectedIdx = selectedSlot;
        if (selectedIdx != -1) {
            g_ZBufferManager_0046aa24->DrawRect(
                slots[selectedIdx].left,
                slots[selectedIdx].top,
                slots[selectedIdx].right,
                slots[selectedIdx].bottom,
                0x2710, 0xFC, 2);
        }
    }

    if (g_SelectedItem_0046a6e4 != 0) {
        int yOffset = g_SelectedItem_0046a6e4->itemId + 0x1D;
        spr = g_Mouse_0046aa18->m_sprite;
        if (spr != 0) {
            spr->ResetAnimation(yOffset, 0);
        }
    } else {
        spr = g_Mouse_0046aa18->m_sprite;
        if (spr != 0) {
            spr->ResetAnimation(0, 0);
        }
    }

    (g_Mouse_0046aa18)->DrawCursor();
}

/* Function start: 0x43EFD0 */
int SCI_Inventory::Exit(SC_MessageParser* msg) {
    SpriteAction* action;

    action = (SpriteAction*)msg;

    if (handlerId != action->addressType) {
        return 0;
    }

    switch (action->instruction) {
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
            if (slot->left <= action->mousePos.x &&
                slot->right >= action->mousePos.x &&
                slot->top <= action->mousePos.y &&
                slot->bottom >= action->mousePos.y) {
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

        if (g_SelectedItem_0046a6e4 != 0) {
            SendGameMessage(0x1e, g_SelectedItem_0046a6e4->itemId, 0x1e, 0, 0x17, 0, 0, 0, 0, 0);
            {

                Sprite* spr = g_Mouse_0046aa18->m_sprite;
                if (spr != 0) {
                    goto do_reset;
                }
                goto do_clear;
            }
        }

        {
            int idx;
            InventoryPool* pool;
            InventoryPoolNode* node;

            idx = scrollOffset + selectedSlot;
            pool = itemPool;
            if (pool->count <= idx) {
                node = 0;
            } else {
                node = pool->head;
                {
                    int j = idx;
                    idx = j - 1;
                    while (j != 0) {
                        node = node->next;
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
                char* soundFile = GetSoundFilename(node->item->descriptionSound);
                newSample->Load(soundFile);
                (clickSound)->Play(0x64, 1);
            }
        }
        break;
    }
    case 6:
    case 0x35:
    case 0x36: {
        void* item = FindItem(action->addressValue);
        if (item == 0) {
            break;
        }
        ((T_Object*)item)->HandleMsg((SpriteAction*)action);
        break;
    }
    case 0x11:
        scrollOffset += 2;
        {
            int maxScroll = itemPool->count - 8;
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
        InventoryPool* pool;
        T_Object* item;
        InventoryPoolNode* head;
        int itemId;

        itemId = action->addressValue;
        if (itemId == 0) break;

        if (FindItemInList(itemId) != 0) break;

        if (g_SelectedItem_0046a6e4 != 0) {
            if (g_SelectedItem_0046a6e4->itemId == itemId) {
                Sprite* spr = g_Mouse_0046aa18->m_sprite;
                if (spr != 0) {
                    spr->ResetAnimation(0, 0);
                }
                g_SelectedItem_0046a6e4 = 0;
            }
        }

        item = (T_Object*)FindItem(itemId);
        item->Reset();

        pool = itemPool;
        head = pool->head;

        if (pool->freeList == 0) {
            InventoryPoolBlock* block = (InventoryPoolBlock*)operator new(
                pool->blockSize * sizeof(InventoryPoolNode) + sizeof(InventoryPoolBlock*));
            block->next = pool->blocks;
            pool->blocks = block;

            {
                int j = pool->blockSize - 1;
                InventoryPoolNode* nodePtr =
                    (InventoryPoolNode*)((char*)block + sizeof(InventoryPoolBlock*) +
                                         j * sizeof(InventoryPoolNode));
                while (j >= 0) {
                    nodePtr->next = pool->freeList;
                    j--;
                    pool->freeList = nodePtr;
                    nodePtr--;
                }
            }
        }

        {
            InventoryPoolNode* newNode = pool->freeList;
            pool->freeList = newNode->next;
            newNode->prev = 0;
            newNode->next = head;
            pool->count++;
            newNode->item = 0;

            { int k = 0; while (k-- != 0); }

            newNode->item = item;

            if (pool->head != 0) {
                pool->head->prev = newNode;
            } else {
                pool->tail = newNode;
            }
            pool->head = newNode;
        }

        DisplayPanels(itemId);
        break;
    }
    case 0x18: {
        InventoryPoolNode* node;
        InventoryPool* pool;

        if (action->addressValue == 0) break;

        node = FindItemInList(action->addressValue);
        if (node != 0) {
            pool = itemPool;
            {
                InventoryPoolNode* nextN = node->next;
                if (pool->head == node) {
                    pool->head = nextN;
                } else {
                    node->prev->next = nextN;
                }
            }
            if (pool->tail == node) {
                pool->tail = node->prev;
            } else {
                node->next->prev = node->prev;
            }

            { int k = 0; while (k-- != 0); }

            node->next = pool->freeList;
            pool->freeList = node;
            pool->count--;
            break;
        }

        if (g_SelectedItem_0046a6e4 == 0) break;
        if (g_SelectedItem_0046a6e4->itemId != action->addressValue) break;
        {
            Sprite* spr = g_Mouse_0046aa18->m_sprite;
            if (spr == 0) goto do_clear;
        }
    do_reset:
        (g_Mouse_0046aa18->m_sprite)->ResetAnimation(0, 0);
    do_clear:
        g_SelectedItem_0046a6e4 = 0;
        break;
    }
    }
    return 1;
}

/* Function start: 0x43F9B0 */
void SCI_Inventory::Serialize(void* param) {
    volatile int fp;
    volatile int self = (int)this;
    int strLen = strlen("INVENTORY_INFO") + 1;
    FileArchive* ar = (FileArchive*)param;
    LinkedList* esi_ptr;
    int handle;

    fp = (int)ar->fp;

    if (ar->mode != 0) {
        /* SAVE PATH */
        int id = ((SCI_Inventory*)self)->handlerId;
        fwrite(&id, 4, 1, (FILE*)fp);
        fwrite("INVENTORY_INFO", strLen, 1, (FILE*)fp);

        {
        InventoryPoolNode* curNode = ((SCI_Inventory*)self)->itemPool->head;
        self = (int)curNode;
        if (curNode != 0) {
            do {
                InventoryPoolNode* nextNode = curNode->next;
                T_Object* item = curNode->item;
                self = (int)nextNode;
                fwrite(&item->itemId, 4, 1, (FILE*)fp);
                fwrite(&item->objectFlags, 4, 1, (FILE*)fp);
                curNode = nextNode;
            } while (self != 0);
        }}

        handle = 999;
        fwrite(&handle, 4, 1, (FILE*)fp);

        if (g_SelectedItem_0046a6e4 != 0) {
            fwrite((char*)&g_SelectedItem_0046a6e4->itemId, 4, 1, (FILE*)fp);
            fwrite((char*)&g_SelectedItem_0046a6e4->objectFlags, 4, 1, (FILE*)fp);
        } else {
            fwrite(&handle, 4, 1, (FILE*)fp);
        }
        return;
    }

    /* LOAD PATH */
    g_Buffer_0046aa00[0] = 0;
    fread(g_Buffer_0046aa00, strLen, 1, (FILE*)fp);

    if (strcmp(g_Buffer_0046aa00, "INVENTORY_INFO") != 0) {
        ShowError("SCI_Inventory::Serialize() - Error Loading (Wrong ID '%s')", g_Buffer_0046aa00);
    }

    g_SelectedItem_0046a6e4 = 0;

    /* Free old inventory list */
    InventoryPool* pool = ((SCI_Inventory*)self)->itemPool;
    if (pool != 0) {
        {
            InventoryPoolNode* node = pool->head;
            while (node != 0) {
                int d = 0;
                int c;
                do { c = d; d--; } while (c != 0);
                node = node->next;
            }
        }

        pool->count = 0;
        pool->freeList = 0;
        pool->tail = 0;
        pool->head = 0;

        {
            InventoryPoolBlock* block = pool->blocks;
            while (block != 0) {
                InventoryPoolBlock* nextBlock = block->next;
                FreeMemory(block);
                block = nextBlock;
            }
        }

        pool->blocks = 0;
        FreeMemory(pool);
        ((SCI_Inventory*)self)->itemPool = 0;
    }

    /* Allocate new inventory list */
    ((SCI_Inventory*)self)->itemPool = (InventoryPool*)new TimedEventPool(10);

    /* Clean up global queue */
    if (g_MsgList_0046a6dc != 0) {
        esi_ptr = g_MsgList_0046a6dc;
        if (esi_ptr->head != 0) {
            esi_ptr->current = esi_ptr->head;
            while (esi_ptr->head != 0) {
                void* data = ((MessageList*)esi_ptr)->PopCurrent();
                if (data != 0) {
                    delete (T_Object*)data;
                }
            }
        }
        FreeMemory(esi_ptr);
        g_MsgList_0046a6dc = 0;
    }

    /* Allocate new global queue */
    esi_ptr = new LinkedList();
    g_MsgList_0046a6dc = esi_ptr;

    /* Read items loop */
    for (;;) {
        fread(&handle, 4, 1, (FILE*)fp);
        if (handle == 999) break;

        T_Object* item = (T_Object*)((SCI_Inventory*)self)->FindItem(handle);
        fread(&item->objectFlags, 4, 1, (FILE*)fp);

        {
            InventoryPool* listPtr = ((SCI_Inventory*)self)->itemPool;
            InventoryPoolNode* tail = listPtr->tail;

            if (listPtr->freeList == 0) {
                InventoryPoolBlock* block = (InventoryPoolBlock*)operator new(
                    listPtr->blockSize * sizeof(InventoryPoolNode) + sizeof(InventoryPoolBlock*));
                block->next = listPtr->blocks;
                listPtr->blocks = block;

                int j = listPtr->blockSize - 1;
                InventoryPoolNode* nodePtr =
                    (InventoryPoolNode*)((char*)block + sizeof(InventoryPoolBlock*) +
                                         j * sizeof(InventoryPoolNode));
                while (j >= 0) {
                    nodePtr->next = listPtr->freeList;
                    listPtr->freeList = nodePtr;
                    nodePtr--;
                    j--;
                }
            }

            {
                InventoryPoolNode* newNode = listPtr->freeList;
                listPtr->freeList = newNode->next;
                newNode->prev = tail;
                newNode->next = 0;
                listPtr->count++;
                newNode->item = 0;

                {
                    int d = 0;
                    int c;
                    do { c = d; d--; } while (c != 0);
                }

                newNode->item = item;

                if (listPtr->tail != 0) {
                    listPtr->tail->next = newNode;
                } else {
                    listPtr->head = newNode;
                }
                listPtr->tail = newNode;
            }
        }
    }

    /* Read optional held item. The save format stores a single ID or 999. */
    fread(&handle, 4, 1, (FILE*)fp);
    if (handle != 999) {
        g_SelectedItem_0046a6e4 = (T_Object*)((SCI_Inventory*)self)->FindItem(handle);
        fread((char*)&g_SelectedItem_0046a6e4->objectFlags, 4, 1, (FILE*)fp);
    }
}

// Stubs (moved from stubs.cpp)
Rect::~Rect() {}
#include "Palette.h"
#include "GameState.h"
#include <new.h>


/* Function start: 0x43FDD0 */
int SCI_Inventory::LBLParse(char* line) {
    Rect paramsRect;
    int* params = (int*)&paramsRect;
    int index;
    char name[32];
    char token[32];

    memset(&paramsRect, 0, sizeof(paramsRect));
    index = 0;
    name[0] = 0;
    token[0] = 0;

    sscanf(line, "%s", token);

    if (strcmp(token, "BACKGROUND") == 0) {
        if (bgSprite == 0) {
            bgSprite = new Sprite((char*)0);
        }
        Parser::ProcessFile(bgSprite, this, (char*)0);
    }
    else if (strcmp(token, "OBJECT") == 0) {
        void* mem;
        void* obj;
        sscanf(line, " %s %d", token, &index);
        obj = new T_Object(index);
        LinkedList* queue = g_MsgList_0046a6dc;
        if (obj == 0) {
            ShowError("queue fault 0101");
        }
        queue->current = queue->head;
        if (queue->type == 1 || queue->type == 2) {
            if (queue->head == 0) {
                queue->InsertNode(obj);
            } else {
                do {
                    ListNode* cur = queue->current;
                    if ((unsigned int)((T_Object*)cur->data)->itemId < (unsigned int)((T_Object*)obj)->itemId) {
                        queue->InsertNode(obj);
                        break;
                    }
                    if (queue->tail == cur) {
                        if (obj == 0) {
                            ShowError("queue fault 0112");
                        }
                        mem = new ListNode(obj);
                        obj = mem;
                        if (queue->current == 0) {
                            queue->current = queue->tail;
                        }
                        if (queue->head == 0) {
                            queue->head = (ListNode*)obj;
                            queue->tail = (ListNode*)obj;
                            queue->current = (ListNode*)obj;
                        } else {
                            if (queue->tail == 0 || queue->tail->next != 0) {
                                ShowError("queue fault 0113");
                            }
                            ((ListNode*)obj)->next = 0;
                            ((ListNode*)obj)->prev = queue->tail;
                            queue->tail->next = (ListNode*)obj;
                            queue->tail = (ListNode*)obj;
                        }
                        break;
                    }
                    if (cur != 0) {
                        queue->current = cur->next;
                    }
                } while (queue->current != 0);
            }
        } else {
            queue->InsertNode(obj);
        }
    }
    else if (strcmp(token, "PALETTE") == 0) {
        sscanf(line, " %s %s", token, name);
        palette = new Palette();
        palette->LoadFile(name);
    }
    else if (strcmp(token, "CANCEL") == 0) {
        sscanf(line, " %s %s %d %d %d %d", token, name, &params[0], &params[1], &params[2], &params[3]);
        putBackButton = new T_MenuButton(name, params);
    }
    else if (strcmp(token, "OK") == 0) {
        sscanf(line, " %s %s %d %d %d %d", token, name, &params[0], &params[1], &params[2], &params[3]);
        useButton = new T_MenuButton(name, params);
    }
    else if (strcmp(token, "SCROLLUP") == 0) {
        sscanf(line, " %s %s %d %d %d %d", token, name, &params[0], &params[1], &params[2], &params[3]);
        scrollDownBtn = new T_MenuButton(name, params);
    }
    else if (strcmp(token, "SCROLLDOWN") == 0) {
        sscanf(line, " %s %s %d %d %d %d", token, name, &params[0], &params[1], &params[2], &params[3]);
        scrollUpBtn = new T_MenuButton(name, params);
    }
    else if (strcmp(token, "AUTOCOMBINE") == 0) {
        {
        int panelIdx;
        sscanf(line, " %s %d %d %d %d %d %d %s", token, &panelIdx, &params[0], &params[1], &params[2], &params[3], &index, name);
        panels[panelIdx].item1 = params[0];
        panels[panelIdx].item2 = params[1];
        panels[panelIdx].item3 = params[2];
        panels[panelIdx].resultItem = params[3];
        panels[panelIdx].actionId = index;
        panels[panelIdx].gameStateIdx = g_GameState_0046aa30->FindState(name);
        }
    }
    else if (strcmp(token, "SLOT") == 0) {
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
        Parser::ReportUnknownLabel("SCI_Inventory");
    }

    return 0;
}

InvPanel::InvPanel() {}
InvPanel::~InvPanel() {}

/* Function start: 0x43F840 */
void SCI_Inventory::DisplayPanels(int param) {
    int count = 3;
    InvPanel* panelPtr = &panels[0];
    do {
        if (panelPtr->item1 == 0) {
            goto check_items;
        } else {
            InventoryPoolNode* found = FindItemInList(panelPtr->item1);
            if (found == 0) goto next_panel;
        }
check_items:
        if (panelPtr->item2 != 0) {
            InventoryPoolNode* found = FindItemInList(panelPtr->item2);
            if (found == 0) goto next_panel;
        }
        if (panelPtr->item3 != 0) {
            InventoryPoolNode* found = FindItemInList(panelPtr->item3);
            if (found == 0) goto next_panel;
        }
        {
            GameState* gs = g_GameState_0046aa30;
            int gsIdx = panelPtr->gameStateIdx;
            if (gsIdx < 0 || gs->maxStates - 1 < gsIdx) {
                ShowError("Invalid gamestate %d", gsIdx);
            }
            if (gs->stateValues[gsIdx] != 0) {
                SendGameMessage(0x1e, panelPtr->item1, handlerId, 0, 0x18, 0, 0, 0, 0, 0);
                SendGameMessage(0x1e, panelPtr->item2, handlerId, 0, 0x18, 0, 0, 0, 0, 0);
                SendGameMessage(0x1e, panelPtr->item3, handlerId, 0, 0x18, 0, 0, 0, 0, 0);
                SendGameMessage(0x1e, panelPtr->resultItem, handlerId, 0, 0x17, 0, 0, 0, 0, 0);
                if (panelPtr->item1 == param || panelPtr->item2 == param || panelPtr->item3 == param) {
                    SendGameMessage(4, panelPtr->actionId, handlerId, 0, 2, 0, 0, 0, 0, 0);
                }
            }
        }
next_panel:
        panelPtr += 1;
        count--;
        if (count == 0) {
            return;
        }
    } while (1);
}
/* Function start: 0x43F420 */
void SCI_Inventory::ProcessInventory() {
    if (g_MsgList_0046a6dc == 0) return;
    if (g_MsgList_0046a6dc->head == 0) return;
    g_MsgList_0046a6dc->current = g_MsgList_0046a6dc->head;
    while (1) {
        QueueNode* node = g_MsgList_0046a6dc->current;
        if (node != 0 && node->data != 0) {
            ((T_Object*)(((node < (QueueNode*)1) - 1) & (int)node->data))->StopSound();
        }
        if (g_MsgList_0046a6dc->tail == g_MsgList_0046a6dc->current) break;
        if (g_MsgList_0046a6dc->current != 0) {
            g_MsgList_0046a6dc->current = g_MsgList_0046a6dc->current->next;
        }
    }
}
/* Function start: 0x43F490 */
void* SCI_Inventory::FindItem(int itemID) {
    LinkedList* list;
    T_Object* item;

    if (g_MsgList_0046a6dc == 0) {
        return 0;
    }

    g_MsgList_0046a6dc->current = g_MsgList_0046a6dc->head;
    while (g_MsgList_0046a6dc->head != 0) {
        ListNode* node = g_MsgList_0046a6dc->current;
        if (node != 0) {
            if (((T_Object*)node->data)->itemId == itemID) {
found:
                {
                    ListNode* cur = g_MsgList_0046a6dc->current;
                    if (cur != 0) {
                        return cur->data;
                    }
                    return 0;
                }
            }
        } else {
            if (itemID == *(int*)0x94) {
                goto found;
            }
        }
        if (g_MsgList_0046a6dc->tail == node) {
            break;
        }
        if (node != 0) {
            g_MsgList_0046a6dc->current = node->next;
        }
    }

    item = new T_Object(itemID);
    list = g_MsgList_0046a6dc;
    if (item == 0) {
        ShowError("queue fault 0101");
    }

    list->current = list->head;
    if (list->type == 1 || list->type == 2) {
        if (list->head == 0) {
            if (item == 0) {
                ShowError("queue fault 0102");
            }
            ListNode* node = new ListNode(item);
            if (list->current == 0) {
                list->current = list->head;
            }
            if (list->head == 0) {
                list->head = node;
                list->tail = node;
                list->current = node;
            } else {
                node->next = list->current;
                node->prev = list->current->prev;
                if (list->current->prev == 0) {
                    list->head = node;
                } else {
                    list->current->prev->next = node;
                }
                list->current->prev = node;
            }
        } else {
            do {
                ListNode* node = list->current;
                if (((T_Object*)node->data)->itemId < item->itemId) {
                    if (item == 0) {
                        ShowError("queue fault 0102");
                    }
                    ListNode* newNode = new ListNode(item);
                    if (list->current == 0) {
                        list->current = list->head;
                    }
                    if (list->head == 0) {
                        list->head = newNode;
                        list->tail = newNode;
                        list->current = newNode;
                    } else {
                        newNode->next = list->current;
                        newNode->prev = list->current->prev;
                        if (list->current->prev == 0) {
                            list->head = newNode;
                        } else {
                            list->current->prev->next = newNode;
                        }
                        list->current->prev = newNode;
                    }
                    break;
                }
                if (list->tail == node) {
                    if (item == 0) {
                        ShowError("queue fault 0112");
                    }
                    ListNode* newNode = new ListNode(item);
                    if (list->current == 0) {
                        list->current = list->tail;
                    }
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
                if (node != 0) {
                    list->current = node->next;
                }
            } while (list->current != 0);
        }
    } else {
        ((PriorityQueue*)list)->AddAfterCurrent(item);
    }

    return item;
}
/* Function start: 0x43F7F0 */
InventoryPoolNode* SCI_Inventory::FindItemInList(int itemID) {
    InventoryPoolNode* volatile node = itemPool->head;
    if (node != 0) {
        do {
            if (node->item->itemId == itemID) {
                return node;
            }
            node = node->next;
        } while (node != 0);
    }
    return 0;
}
