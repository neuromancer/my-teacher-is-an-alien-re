#include "SCI_Inventory.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "LinkedList.h"
#include "Sprite.h"
#include "Sample.h"
#include "InputManager.h"

extern "C" void ShowError(const char* format, ...);
extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern "C" char* FUN_0044e530(int handle);

extern LinkedList* g_InventoryList;
extern void* DAT_0046aa24;
extern void* DAT_0046aa18;
extern void* DAT_0046a6e4;
extern int DAT_00473334;
extern int DAT_004733e8;

extern void __fastcall FUN_00432da0(void* self);
extern void __fastcall FUN_00421bc0(void* self);
extern void* DAT_0046aa08;
extern void* DAT_0046a6dc;
extern char* DAT_0046aa00;
extern void* __fastcall FUN_004407c0(void* self);
extern void __fastcall FUN_0040c6e0(void* self);

class InvItemObj {
public:
    void Display(int x, int y, int visible);
    void Reset();
    void HandleMsg(int* msg);
};

class SelectionDraw {
public:
    void DrawSelection(int x1, int y1, int x2, int y2, int param5, int param6, int param7);
};

struct HitPoint {
    int x;
    int y;
    HitPoint(int* ptr) { x = ptr[0]; y = ptr[1]; }
    ~HitPoint();
};

class InvListItem {
public:
    void Cleanup();
};

// 0x18-byte list object, same layout as inventory list in constructor
class InvListObj {
public:
    int head, tail, count, freeList, blocks, growBy;
    InvListObj() { count = 0; freeList = 0; tail = 0; head = 0; blocks = 0; growBy = 10; }
    ~InvListObj();
};

// 0x10-byte queue object for global queue
class InvQueueObj {
public:
    int head, tail, current, count;
    InvQueueObj() { count = 0; head = 0; tail = 0; current = head; }
    ~InvQueueObj();
};

/* Function start: 0x43E360 */
SCI_Inventory::SCI_Inventory() {
    int* listObj;

    memset(&field_A8, 0, 0xF0);
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
    field_184 = (int)listObj;

    LinkedList* invList = g_InventoryList;
    if (invList->head != 0) {
        invList->current = invList->head;
        while (invList->head != 0) {
            void* data = invList->RemoveCurrent();
            if (data != 0) {
                ((InvListItem*)data)->Cleanup();
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
    int palette;
    int* palSlot;
    int* ptr;

    IconBar::InitIconBar(msg);

    ptr = &DAT_00473334;
    do {
        *ptr = 0;
        ptr = (int*)((char*)ptr + 0x24);
    } while ((unsigned int)ptr < (unsigned int)&DAT_004733e8);
    DAT_004733e8 = 0;

    palette = field_180;
    palSlot = (int*)((char*)DAT_0046aa24 + 0xA8);
    if (*palSlot != 0) {
        ShowError("double palette");
    }
    *palSlot = palette;
    field_128 = -1;
    DisplayPanels(0);
}

/* Function start: 0x43E990 */
int SCI_Inventory::ShutDown(SC_Message* msg) {
    Sprite* spr;
    void* obj;

    if (field_178 != 0) {
        ((Sprite*)field_178)->StopAnimationSound();
    }

    ProcessInventory();

    obj = DAT_0046aa18;
    spr = *(Sprite**)((char*)obj + 0x94);
    if (spr != 0) {
        spr->ResetAnimation(0, 0);
    }

    FUN_00432da0(DAT_0046aa18);

    DAT_004733e8 = 1;
    IconBar::CleanupIconBar(msg);

    spr = *(Sprite**)((char*)field_188 + 0x90);
    if (spr != 0) {
        spr->StopAnimationSound();
    }

    spr = *(Sprite**)((char*)field_18C + 0x90);
    if (spr != 0) {
        spr->StopAnimationSound();
    }

    spr = *(Sprite**)((char*)field_190 + 0x90);
    if (spr != 0) {
        spr->StopAnimationSound();
    }

    spr = *(Sprite**)((char*)field_194 + 0x90);
    if (spr != 0) {
        spr->StopAnimationSound();
    }

    {
        Sample* sample = (Sample*)field_17C;
        if (sample != 0) {
            sample->Unload();
            FreeMemory(sample);
            field_17C = 0;
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

    /* Test against field_190 rectangle */
    {
        HitPoint pos(cursorPtr);
        rect = (int*)field_190;
        if (*(int*)((char*)rect + 0x94) <= pos.x &&
            *(int*)((char*)rect + 0x9C) >= pos.x &&
            *(int*)((char*)rect + 0x98) <= pos.y &&
            *(int*)((char*)rect + 0xA0) >= pos.y) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        msgData[4] = 0x12;
        goto done;
    }

    /* Test against field_194 rectangle */
    {
        HitPoint pos(cursorPtr);
        rect = (int*)field_194;
        if (*(int*)((char*)rect + 0x94) <= pos.x &&
            *(int*)((char*)rect + 0x9C) >= pos.x &&
            *(int*)((char*)rect + 0x98) <= pos.y &&
            *(int*)((char*)rect + 0xA0) >= pos.y) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        msgData[4] = 0x11;
        goto done;
    }

    /* Test against field_18C rectangle */
    {
        HitPoint pos(cursorPtr);
        rect = (int*)field_18C;
        if (*(int*)((char*)rect + 0x94) <= pos.x &&
            *(int*)((char*)rect + 0x9C) >= pos.x &&
            *(int*)((char*)rect + 0x98) <= pos.y &&
            *(int*)((char*)rect + 0xA0) >= pos.y) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        msgData[0] = savedCommand;
        msgData[4] = 4;
        msgData[1] = savedMsgData;

        if (field_128 == -1) {
            goto done;
        }

        selectedIdx = field_12C + field_128;
        listPtr = (int*)field_184;
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
        listPtr = (int*)field_184;
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
            Sprite* spr = *(Sprite**)((char*)DAT_0046aa18 + 0x94);
            int yOffset = *(int*)((char*)invItem + 0x94) + 0x1D;
            if (spr != 0) {
                spr->ResetAnimation(0, yOffset);
            }
        }
        goto done;
    }

    /* Test against field_188 rectangle */
    {
        HitPoint pos(cursorPtr);
        rect = (int*)field_188;
        if (*(int*)((char*)rect + 0x94) <= pos.x &&
            *(int*)((char*)rect + 0x9C) >= pos.x &&
            *(int*)((char*)rect + 0x98) <= pos.y &&
            *(int*)((char*)rect + 0xA0) >= pos.y) {
            hitResult = 1;
        } else {
            hitResult = 0;
        }
    }
    if (hitResult) {
        if (DAT_0046a6e4 == 0) {
            goto handle_item_click;
        }
        FUN_00444d90(0x1E, *(int*)((char*)DAT_0046a6e4 + 0x94), 0x1E, 0, 0x17, 0, 0, 0, 0, 0);
        {
            Sprite* spr = *(Sprite**)((char*)DAT_0046aa18 + 0x94);
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
        msgData[5] = *(int*)((char*)DAT_0046a6e4 + 0x94);
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

    if (field_178 != 0) {
        ((Sprite*)field_178)->Do(0, 0, 1.0);
    }

    FUN_00421bc0((void*)field_188);
    FUN_00421bc0((void*)field_18C);
    FUN_00421bc0((void*)field_190);
    FUN_00421bc0((void*)field_194);

    startIdx = field_12C;
    {
        int* listPtr = (int*)field_184;
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
        InvSlotItem* slot = &field_A8[0];

        do {
            if (curNode == 0) break;

            int* nextNode = (int*)curNode[0];
            void* data = (void*)curNode[2];
            curNode = nextNode;

            if (data != 0) {
                int* mouseObj = *(int**)((char*)DAT_0046aa08 + 0x1A0);
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
                    ((InvItemObj*)data)->Display(slot->field_0, slot->field_4, 1);
                } else {
                    ((InvItemObj*)data)->Display(slot->field_0, slot->field_4, 0);
                }
            }

            slot++;
            counter++;
        } while (counter <= 7);
    }

    {
        int selectedIdx = field_128;
        if (selectedIdx != -1) {
            ((SelectionDraw*)DAT_0046aa24)->DrawSelection(
                field_A8[selectedIdx].field_0,
                field_A8[selectedIdx].field_4,
                field_A8[selectedIdx].field_8,
                field_A8[selectedIdx].field_C,
                0x2710, 0xFC, 2);
        }
    }

    if (DAT_0046a6e4 != 0) {
        int yOffset = *(int*)((char*)DAT_0046a6e4 + 0x94) + 0x1D;
        spr = *(Sprite**)((char*)DAT_0046aa18 + 0x94);
        if (spr != 0) {
            spr->ResetAnimation(yOffset, 0);
        }
    } else {
        spr = *(Sprite**)((char*)DAT_0046aa18 + 0x94);
        if (spr != 0) {
            spr->ResetAnimation(0, 0);
        }
    }

    FUN_00432da0(DAT_0046aa18);
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
        slot = &field_A8[0];
        field_128 = -1;
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
                field_128 = i;
            }
            slot++;
            i++;
        } while (i < 8);

        if (field_128 < 0 || field_128 >= 8) {
            break;
        }

        if (DAT_0046a6e4 != 0) {
            FUN_00444d90(0x1e, *(int*)((char*)DAT_0046a6e4 + 0x94), 0x1e, 0, 0x17, 0, 0, 0, 0, 0);
            {
                void* obj = DAT_0046aa18;
                Sprite* spr = *(Sprite**)((char*)obj + 0x94);
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

            idx = field_12C + field_128;
            listPtr = (int*)field_184;
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
                Sample* oldSample = (Sample*)field_17C;
                if (oldSample != 0) {
                    oldSample->Unload();
                    FreeMemory(oldSample);
                    field_17C = 0;
                }
            }

            {
                Sample* newSample = new Sample();
                field_17C = (int)newSample;
                char* soundFile = FUN_0044e530(*(int*)((char*)(node[2]) + 0x9C));
                newSample->Load(soundFile);
                ((Sample*)field_17C)->Play(0x64, 1);
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
        ((InvItemObj*)item)->HandleMsg(msgData);
        break;
    }
    case 0x11:
        field_12C += 2;
        {
            int maxScroll = ((int*)field_184)[2] - 8;
            if (maxScroll < field_12C) {
                field_12C = maxScroll;
            }
        }
        if (field_12C < 0) {
            field_12C = 0;
        }
        break;
    case 0x12:
        field_12C -= 2;
        if (field_12C < 0) {
            field_12C = 0;
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
            if (*(int*)((char*)DAT_0046a6e4 + 0x94) == (int)msgData) {
                Sprite* spr = *(Sprite**)((char*)DAT_0046aa18 + 0x94);
                if (spr != 0) {
                    spr->ResetAnimation(0, 0);
                }
                DAT_0046a6e4 = 0;
            }
        }

        item = FindItem((int)msgData);
        ((InvItemObj*)item)->Reset();

        ebx = (int*)field_184;
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
            listPtr = (int*)field_184;
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
        if (*(int*)((char*)DAT_0046a6e4 + 0x94) != msgData[1]) break;
        {
            Sprite* spr = *(Sprite**)((char*)DAT_0046aa18 + 0x94);
            if (spr == 0) goto do_clear;
        }
    do_reset:
        ((Sprite*)*(int*)((char*)DAT_0046aa18 + 0x94))->ResetAnimation(0, 0);
    do_clear:
        DAT_0046a6e4 = 0;
        break;
    }
    }
    return 1;
}

/* Function start: 0x43F9B0 */
void SCI_Inventory::OnInput(void* param) {
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
            fwrite((char*)DAT_0046a6e4 + 0x94, 4, 1, (FILE*)fp);
            fwrite((char*)DAT_0046a6e4 + 0x90, 4, 1, (FILE*)fp);
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
    esi_ptr = (int*)(new InvListObj());
    *(int*)(self + 0x184) = (int)esi_ptr;

    /* Clean up global queue */
    if (DAT_0046a6dc != 0) {
        esi_ptr = (int*)DAT_0046a6dc;
        if (esi_ptr[0] != 0) {
            esi_ptr[2] = esi_ptr[0];
            while (esi_ptr[0] != 0) {
                void* data = FUN_004407c0(esi_ptr);
                if (data != 0) {
                    FUN_0040c6e0(data);
                    FreeMemory(data);
                }
            }
        }
        FreeMemory(esi_ptr);
        DAT_0046a6dc = 0;
    }

    /* Allocate new global queue */
    esi_ptr = (int*)(new InvQueueObj());
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
    fread((char*)DAT_0046a6e4 + 0x90, 4, 1, (FILE*)fp);
}
