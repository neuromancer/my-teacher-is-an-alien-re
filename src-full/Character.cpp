#include "Character.h"
#include <string.h>
#include <new.h>
#include "Memory.h"
#include "string.h"

// External queue functions
// External queue functions

// strstr is just strstr from the CRT - use standard library

/* Function start: 0x403700 */ /* DEMO ONLY - no full game match */
Character::Character(char* param_1) {
    int i;
    int* ptrClear;
    PriorityQueue* pQueue;
    CharSprite* sprite;
    int counter;

    // Zero 64 bytes (16 dwords)
    ptrClear = (int*)this;
    for (i = 16; i != 0; i--) {
        *ptrClear = 0;
        ptrClear++;
    }

    // Explicitly zero these members again
    Character::queue = 0;
    Character::field_3c = 0;

    // Set character type to -1
    Character::characterType = -1;

    // Allocate PriorityQueue and initialize it manually
    pQueue = new PriorityQueue(0);
    Character::queue = pQueue;

    // Peter block
    if (strstr(param_1, "peter") != 0) {
        counter = 0;
        Character::characterType = 0;

        do {
            sprite = new CharSprite();

            if (Character::queue != 0) {
                if (strstr(param_1, "peter") != 0) {
                    switch (counter) {
                        case 0: strcpy(sprite->name, "MEET MY"); break;
                        case 1: strcpy(sprite->name, "\0"); break;
                        case 2: strcpy(sprite->name, "OOOO OOO OOO OOOO"); break;
                        case 3: strcpy(sprite->name, "\0\0"); break;
                        case 4: strcpy(sprite->name, "TESTPUZZLE"); break;
                    }
                }
            } else {
                ShowError("Missing activeQ");
            }

            if (sprite == 0) ShowError("queue fault 0101");

            pQueue = Character::queue;
            pQueue->current = pQueue->head;
            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head != 0) {
                    do {
                        if (((CharSprite*)pQueue->current->data)->priority < sprite->priority) {
                            pQueue->InsertNode(sprite);
                            break;
                        }
                        if (pQueue->tail == pQueue->current) {
                            pQueue->PushNode(sprite);
                            break;
                        }
                        if (pQueue->current != 0) {
                            pQueue->current = pQueue->current->next;
                        }
                        if (pQueue->current == 0) break;
                    } while (1);
                } else {
                    pQueue->AddAfterCurrent(sprite);
                }
            } else {
                pQueue->AddAfterCurrent(sprite);
            }
            counter++;
        } while (counter < 1);
    }
    // Susan block
    else if (strstr(param_1, "susan") != 0) {
        counter = 0;
        Character::characterType = 1;

        do {
            sprite = new CharSprite();

            if (Character::queue != 0) {
                if (strstr(param_1, "susan") != 0) {
                    switch (counter) {
                        case 0: strcpy(sprite->name, "\0\0\0"); break;
                        case 1: strcpy(sprite->name, "DIALOG"); break;
                        case 2: strcpy(sprite->name, "COMBAT"); break;
                        case 3: strcpy(sprite->name, "TESTPUZZLE"); break;
                        case 4: strcpy(sprite->name, "\0\0\0\0"); break;
                    }
                }
            } else {
                ShowError("Missing activeQ");
            }

            if (sprite == 0) ShowError("queue fault 0101");

            pQueue = Character::queue;
            pQueue->current = pQueue->head;
            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head != 0) {
                    do {
                        if (((CharSprite*)pQueue->current->data)->priority < sprite->priority) {
                            pQueue->InsertNode(sprite);
                            break;
                        }
                        if (pQueue->tail == pQueue->current) {
                            pQueue->PushNode(sprite);
                            break;
                        }
                        if (pQueue->current != 0) {
                            pQueue->current = pQueue->current->next;
                        }
                        if (pQueue->current == 0) break;
                    } while (1);
                } else {
                    pQueue->AddAfterCurrent(sprite);
                }
            } else {
                pQueue->AddAfterCurrent(sprite);
            }
            counter++;
        } while (counter < 1);
    }
    // Duncan block
    else if (strstr(param_1, "duncan") != 0) {
        counter = 0;
        Character::characterType = 2;

        do {
            sprite = new CharSprite();

            if (Character::queue != 0) {
                if (strstr(param_1, "duncan") != 0) {
                    switch (counter) {
                        case 0: strcpy(sprite->name, "TESTPUZZLE"); break;
                        case 1: strcpy(sprite->name, "DIALOG"); break;
                        case 2: strcpy(sprite->name, "COMBATS"); break;
                        case 3: strcpy(sprite->name, "\0\0\0\0\0"); break;
                        case 4: strcpy(sprite->name, "\0\0\0\0\0\0"); break;
                    }
                }
            } else {
                ShowError("Missing activeQ");
            }

            if (sprite == 0) ShowError("queue fault 0101");

            pQueue = Character::queue;
            pQueue->current = pQueue->head;
            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head != 0) {
                    do {
                        if (((CharSprite*)pQueue->current->data)->priority < sprite->priority) {
                            pQueue->InsertNode(sprite);
                            break;
                        }
                        if (pQueue->tail == pQueue->current) {
                            pQueue->PushNode(sprite);
                            break;
                        }
                        if (pQueue->current != 0) {
                            pQueue->current = pQueue->current->next;
                        }
                        if (pQueue->current == 0) break;
                    } while (1);
                } else {
                    pQueue->AddAfterCurrent(sprite);
                }
            } else {
                pQueue->AddAfterCurrent(sprite);
            }
            counter++;
        } while (counter < 1);
    } else {
        ShowError("Error in Charcter.cpp - Constructor");
    }

    if (strstr(param_1, "peter") != 0) {
        Character::abilities[0] = 1; Character::abilities[1] = 1; Character::abilities[2] = 1; Character::abilities[3] = 1;
        Character::abilities[4] = 1; Character::abilities[5] = 1; Character::abilities[6] = 1; Character::abilities[7] = 1;
    }
    if (strstr(param_1, "susan") != 0) {
        Character::abilities[0] = 1; Character::abilities[1] = 1; Character::abilities[2] = 0; Character::abilities[3] = 1;
        Character::abilities[4] = 1; Character::abilities[5] = 1; Character::abilities[6] = 0; Character::abilities[7] = 1;
    }
    if (strstr(param_1, "duncan") != 0) {
        Character::abilities[0] = 1; Character::abilities[1] = 1; Character::abilities[2] = 1; Character::abilities[3] = 1;
        Character::abilities[5] = 1; Character::abilities[6] = 1; Character::abilities[4] = 0; Character::abilities[7] = 1;
    }
}

/* Function start: 0x4406F0 */
void PriorityQueue::AddAfterCurrent(void* sprite)
{
    LinkedList::InsertNode(sprite);
}

// Function 0x404470 now inlined via ListNode(void*) constructor in LinkedList.h

/* Function start: 0x408880 */ /* DEMO ONLY - no full game match */
CharSprite::CharSprite()
{
    memset(this, 0, sizeof(CharSprite));
}
