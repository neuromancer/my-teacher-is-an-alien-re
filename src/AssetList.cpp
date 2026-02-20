#include "AssetList.h"
#include "Memory.h"

// External functions
extern "C" void __fastcall FUN_004088f0(void* thisPtr);
extern "C" void __fastcall FUN_00404490(void* thisPtr, int param);

// Node structure for the linked list
struct AssetNode {
    AssetNode* next;      // 0x00
    AssetNode* prev;      // 0x04
    void* data;           // 0x08
};

// List header structure
struct AssetListHeader {
    AssetNode* head;      // 0x00
    AssetNode* tail;      // 0x04
    AssetNode* current;   // 0x08
};

AssetList::~AssetList()
{
    Cleanup();
}

/* Function start: 0x404230 */
void AssetList::Cleanup() {
    AssetListHeader* list1;
    AssetListHeader* list2;
    AssetNode* node;
    void* data;

    // Process first list at offset 0x38
    list1 = (AssetListHeader*)AssetList::field_0x38;
    if (list1 != 0) {
        if (list1->head != 0) {
            list1->current = list1->head;
            if (list1->head != 0) {
                do {
                    node = list1->current;
                    if (node == 0) {
                        data = 0;
                    } else {
                        if (list1->head == node) {
                            list1->head = node->prev;
                        }
                        if (list1->tail == node) {
                            list1->tail = node->next;
                        }
                        if (node->next != 0) {
                            node->next->prev = node->prev;
                        }
                        if (node->prev != 0) {
                            node->prev->next = node->next;
                        }
                        data = 0;
                        if (node != 0) {
                            data = node->data;
                        }
                        if (node != 0) {
                            node->data = 0;
                            node->next = 0;
                            node->prev = 0;
                            delete node;
                            list1->current = 0;
                        }
                        list1->current = list1->head;
                    }
                    if (data != 0) {
                        FUN_004088f0(data);
                        delete data;
                    }
                } while (list1->head != 0);
            }
        }
        delete list1;
        AssetList::field_0x38 = 0;
    }

    // Process second list at offset 0x3c
    list2 = (AssetListHeader*)AssetList::field_0x3c;
    if (list2 != 0) {
        if (list2->head != 0) {
            list2->current = list2->head;
            if (list2->head != 0) {
                do {
                    node = list2->current;
                    if (node == 0) {
                        data = 0;
                    } else {
                        if (list2->head == node) {
                            list2->head = node->prev;
                        }
                        if (list2->tail == node) {
                            list2->tail = node->next;
                        }
                        if (node->next != 0) {
                            node->next->prev = node->prev;
                        }
                        if (node->prev != 0) {
                            node->prev->next = node->next;
                        }
                        data = 0;
                        if (node != 0) {
                            data = node->data;
                        }
                        if (node != 0) {
                            FUN_00404490(node, 1);
                            list2->current = 0;
                        }
                        list2->current = list2->head;
                    }
                    if (data != 0) {
                        FUN_004088f0(data);
                        delete data;
                    }
                } while (list2->head != 0);
            }
        }
        delete list2;
        AssetList::field_0x3c = 0;
    }
}
