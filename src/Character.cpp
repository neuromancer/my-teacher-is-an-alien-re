#include "Character.h"
#include <string.h>
#include <new.h>
#include "Memory.h"
#include "string.h"

// External queue functions
// External queue functions

// strstr is just strstr from the CRT - use standard library

/* Function start: 0x403700 */
Character::Character(char* param_1) {
    int i;
    int* ptrClear;
    PriorityQueue* pQueue;
    CharSprite* sprite;
    PriorityQueueNode* newNode;
    int counter;

    // Zero 64 bytes (16 dwords)
    ptrClear = (int*)this;
    for (i = 16; i != 0; i--) {
        *ptrClear = 0;
        ptrClear++;
    }

    // Explicitly zero these members again
    queue = 0;
    field_3c = 0;

    // Set character type to -1
    characterType = -1;

    // Allocate PriorityQueue and initialize it manually
    pQueue = new PriorityQueue(0);
    queue = pQueue;

    // Peter block
    if (strstr(param_1, "peter") != 0) {
        counter = 0;
        characterType = 0;

        do {
            sprite = new CharSprite();
            if (strstr(param_1, "peter") != 0 && (unsigned int)counter <= 4) {
                switch (counter) {
                    case 0: strcpy(sprite->name, ""); break;
                    case 1: strcpy(sprite->name, "MEET MY"); break;
                    case 2: strcpy(sprite->name, "OOOO OOO OOO OOOO"); break;
                    case 3: strcpy(sprite->name, ""); break;
                    case 4: strcpy(sprite->name, "TESTPUZZLE"); break;
                }
                if (counter == 0) strcpy(sprite->name, "MEET MY");
            }

            pQueue = queue;
            pQueue->ResetForSortedAdd(sprite);
            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head != 0) {
                    while (1) {
                        if (((CharSprite*)pQueue->current->data)->priority < sprite->priority) {
                            pQueue->Insert(sprite);
                            goto next_peter;
                        }
                        if (pQueue->tail == pQueue->current) break;
                        pQueue->current = pQueue->current->next;
                    }
                }
                pQueue->Push(sprite);
            } else {
                pQueue->Insert(sprite);
            }
        next_peter:
            counter++;
        } while (counter < 1);
    }
    // Susan block
    else if (strstr(param_1, "susan") != 0) {
        counter = 0;
        characterType = 1;

        do {
            sprite = new CharSprite();
            if (strstr(param_1, "susan") != 0 && (unsigned int)counter <= 4) {
                switch (counter) {
                    case 0: strcpy(sprite->name, "SUS"); break;
                    case 1: strcpy(sprite->name, "DIALOG"); break;
                    case 2: strcpy(sprite->name, "COMBAT"); break;
                    case 3: strcpy(sprite->name, "TESTPUZZLE"); break;
                    case 4: strcpy(sprite->name, ""); break;
                }
            }

            pQueue = queue;
            pQueue->ResetForSortedAdd(sprite);
            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head != 0) {
                    while (1) {
                        if (((CharSprite*)pQueue->current->data)->priority < sprite->priority) {
                            pQueue->Insert(sprite);
                            goto next_susan;
                        }
                        if (pQueue->tail == pQueue->current) break;
                        pQueue->current = pQueue->current->next;
                    }
                }
                pQueue->Push(sprite);
            } else {
                pQueue->Insert(sprite);
            }
        next_susan:
            counter++;
        } while (counter < 1);
    }
    // Duncan block
    else if (strstr(param_1, "duncan") != 0) {
        counter = 0;
        characterType = 2;

        do {
            sprite = new CharSprite();
            if (strstr(param_1, "duncan") != 0 && (unsigned int)counter <= 4) {
                switch (counter) {
                    case 0: strcpy(sprite->name, "DIALOG"); break;
                    case 1: strcpy(sprite->name, "COMBATS"); break;
                    case 2: strcpy(sprite->name, "TESTPUZZLE"); break;
                    case 3: strcpy(sprite->name, ""); break;
                    case 4: strcpy(sprite->name, ""); break;
                }
                if (counter == 0) strcpy(sprite->name, "TESTPUZZLE");
            }

            pQueue = queue;
            pQueue->ResetForSortedAdd(sprite);
            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head != 0) {
                    while (1) {
                        if (((CharSprite*)pQueue->current->data)->priority < sprite->priority) {
                            pQueue->Insert(sprite);
                            goto next_duncan;
                        }
                        if (pQueue->tail == pQueue->current) break;
                        pQueue->current = pQueue->current->next;
                    }
                }
                pQueue->Push(sprite);
            } else {
                pQueue->Insert(sprite);
            }
        next_duncan:
            counter++;
        } while (counter < 1);
    } else {
        ShowError("Error in Charcter.cpp - Constructor");
    }

    if (strstr(param_1, "peter") != 0) {
        abilities[0] = 1; abilities[1] = 1; abilities[2] = 1; abilities[3] = 1;
        abilities[4] = 1; abilities[5] = 1; abilities[6] = 1; abilities[7] = 1;
    }
    if (strstr(param_1, "susan") != 0) {
        abilities[0] = 1; abilities[1] = 1; abilities[2] = 0; abilities[3] = 1;
        abilities[4] = 1; abilities[5] = 1; abilities[6] = 0; abilities[7] = 1;
    }
    if (strstr(param_1, "duncan") != 0) {
        abilities[0] = 1; abilities[1] = 1; abilities[2] = 1; abilities[3] = 1;
        abilities[4] = 0; abilities[5] = 1; abilities[6] = 1; abilities[7] = 1;
    }
}

/* Function start: 0x4043A0 */
void PriorityQueue::AddAfterCurrent(void* sprite)
{
    if (sprite == 0) {
        ShowError("queue fault 0102");
    }

    PriorityQueueNode* newNode = new PriorityQueueNode(sprite);

    if (current == 0) {
        current = head;
    }

    if (head == 0) {
        head = newNode;
        tail = newNode;
        current = newNode;
    } else {
        newNode->next = current;
        newNode->prev = current->prev;
        if (current->prev != 0) {
            current->prev->next = newNode;
            current->prev = newNode;
        } else {
            head = newNode;
            current->prev = newNode;
        }
    }
}

// Function 0x404470 now inlined via ListNode(void*) constructor in LinkedList.h

/* Function start: 0x408880 */
CharSprite::CharSprite()
{
    bounds.left = 0;
    bounds.top = 0;
    bounds.right = 0;
    bounds.bottom = 0;
    memset(this, 0, sizeof(CharSprite));
}
