#include "Character.h"
#include <string.h>
#include <new.h>
#include "Memory.h"
#include "string.h"

// External queue functions
extern PriorityQueueNode* __fastcall FUN_00404470(PriorityQueueNode* node, CharSprite* sprite);

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
    pQueue = (PriorityQueue*)operator new(0x10);
    if (pQueue != 0) {
        pQueue->type = 0;
        pQueue->head = 0;
        pQueue->tail = 0;
        pQueue->current = pQueue->head;
        queue = pQueue;
    }

    // Peter block
    if (strstr(param_1, "peter") != 0) {
        counter = 0;
        characterType = 0;

        do {
            sprite = new CharSprite();

            if (queue == 0) {
                ShowError("Missing activeQ");
            } else if (strstr(param_1, "peter") != 0) {
                if ((unsigned int)counter <= 4) {
                    switch (counter) {
                        case 0: strcpy(sprite->name, ""); break;
                        case 1: strcpy(sprite->name, "MEET MY"); break;
                        case 2: strcpy(sprite->name, "OOOO OOO OOO OOOO"); break;
                        case 3: strcpy(sprite->name, ""); break;
                        case 4: strcpy(sprite->name, "TESTPUZZLE"); break;
                    }
                    if (counter == 0) strcpy(sprite->name, "MEET MY");
                }
            }

            if (sprite == 0) {
                ShowError("queue fault 0101");
            }

            pQueue = queue;
            pQueue->current = pQueue->head;

            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head == 0) {
                    pQueue->AddAfterCurrent(sprite);
                } else {
                    while (pQueue->current != 0) {
                        if (pQueue->current->data->priority < sprite->priority) {
                            if (sprite == 0) ShowError("queue fault 0102");
                            newNode = (PriorityQueueNode*)operator new(0xc);
                            if (newNode != 0) {
                                FUN_00404470(newNode, sprite);
                            }
                            if (pQueue->current == 0) pQueue->current = pQueue->head;
                            if (pQueue->head == 0) {
                                pQueue->head = newNode;
                                pQueue->tail = newNode;
                                pQueue->current = newNode;
                            } else {
                                newNode->prev = pQueue->current;
                                newNode->next = pQueue->current->next;
                                if (pQueue->current->next == 0) {
                                    pQueue->head = newNode;
                                    pQueue->current->next = newNode;
                                } else {
                                    pQueue->current->next->prev = newNode;
                                    pQueue->current->next = newNode;
                                }
                            }
                            break;
                        }
                        if (pQueue->tail == pQueue->current) {
                            if (sprite == 0) ShowError("queue fault 0112");
                            newNode = (PriorityQueueNode*)operator new(0xc);
                            if (newNode != 0) {
                                newNode->data = sprite;
                                newNode->next = 0;
                                newNode->prev = 0;
                            }
                            if (pQueue->current == 0) pQueue->current = pQueue->tail;
                            if (pQueue->head == 0) {
                                pQueue->head = newNode;
                                pQueue->tail = newNode;
                                pQueue->current = newNode;
                            } else {
                                if (pQueue->tail == 0 || pQueue->tail->prev != 0) {
                                    ShowError("queue fault 0113");
                                }
                                newNode->prev = 0;
                                newNode->next = pQueue->tail;
                                pQueue->tail->prev = newNode;
                                pQueue->tail = newNode;
                            }
                            break;
                        }
                        if (pQueue->current != 0) {
                            pQueue->current = pQueue->current->prev;
                        }
                    }
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
        characterType = 1;

        do {
            sprite = new CharSprite();

            if (queue == 0) {
                ShowError("Missing activeQ");
            } else if (strstr(param_1, "susan") != 0) {
                if ((unsigned int)counter <= 4) {
                    switch (counter) {
                        case 0: strcpy(sprite->name, "SUS"); break;
                        case 1: strcpy(sprite->name, "DIALOG"); break;
                        case 2: strcpy(sprite->name, "COMBAT"); break;
                        case 3: strcpy(sprite->name, "TESTPUZZLE"); break;
                        case 4: strcpy(sprite->name, ""); break;
                    }
                }
            }

            if (sprite == 0) {
                ShowError("queue fault 0101");
            }

            pQueue = queue;
            pQueue->current = pQueue->head;

            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head == 0) {
                    pQueue->AddAfterCurrent(sprite);
                } else {
                    while (pQueue->current != 0) {
                        if (pQueue->current->data->priority < sprite->priority) {
                            if (sprite == 0) ShowError("queue fault 0102");
                            newNode = (PriorityQueueNode*)operator new(0xc);
                            if (newNode != 0) {
                                FUN_00404470(newNode, sprite);
                            }
                            if (pQueue->current == 0) pQueue->current = pQueue->head;
                            if (pQueue->head == 0) {
                                pQueue->head = newNode;
                                pQueue->tail = newNode;
                                pQueue->current = newNode;
                            } else {
                                newNode->prev = pQueue->current;
                                newNode->next = pQueue->current->next;
                                if (pQueue->current->next == 0) {
                                    pQueue->head = newNode;
                                    pQueue->current->next = newNode;
                                } else {
                                    pQueue->current->next->prev = newNode;
                                    pQueue->current->next = newNode;
                                }
                            }
                            break;
                        }
                        if (pQueue->tail == pQueue->current) {
                            if (sprite == 0) ShowError("queue fault 0112");
                            newNode = (PriorityQueueNode*)operator new(0xc);
                            if (newNode != 0) {
                                newNode->data = sprite;
                                newNode->next = 0;
                                newNode->prev = 0;
                            }
                            if (pQueue->current == 0) pQueue->current = pQueue->tail;
                            if (pQueue->head == 0) {
                                pQueue->head = newNode;
                                pQueue->tail = newNode;
                                pQueue->current = newNode;
                            } else {
                                if (pQueue->tail == 0 || pQueue->tail->prev != 0) {
                                    ShowError("queue fault 0113");
                                }
                                newNode->prev = 0;
                                newNode->next = pQueue->tail;
                                pQueue->tail->prev = newNode;
                                pQueue->tail = newNode;
                            }
                            break;
                        }
                        if (pQueue->current != 0) {
                            pQueue->current = pQueue->current->prev;
                        }
                    }
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
        characterType = 2;

        do {
            sprite = new CharSprite();

            if (queue == 0) {
                ShowError("Missing activeQ");
            } else if (strstr(param_1, "duncan") != 0) {
                if ((unsigned int)counter <= 4) {
                    switch (counter) {
                        case 0: strcpy(sprite->name, "DIALOG"); break;
                        case 1: strcpy(sprite->name, "COMBATS"); break;
                        case 2: strcpy(sprite->name, "TESTPUZZLE"); break;
                        case 3: strcpy(sprite->name, ""); break;
                        case 4: strcpy(sprite->name, ""); break;
                    }
                    if (counter == 0) strcpy(sprite->name, "TESTPUZZLE");
                }
            }

            if (sprite == 0) {
                ShowError("queue fault 0101");
            }

            pQueue = queue;
            pQueue->current = pQueue->head;

            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head == 0) {
                    pQueue->AddAfterCurrent(sprite);
                } else {
                    while (pQueue->current != 0) {
                        if (pQueue->current->data->priority < sprite->priority) {
                            if (sprite == 0) ShowError("queue fault 0102");
                            newNode = (PriorityQueueNode*)operator new(0xc);
                            if (newNode != 0) {
                                FUN_00404470(newNode, sprite);
                            }
                            if (pQueue->current == 0) pQueue->current = pQueue->head;
                            if (pQueue->head == 0) {
                                pQueue->head = newNode;
                                pQueue->tail = newNode;
                                pQueue->current = newNode;
                            } else {
                                newNode->prev = pQueue->current;
                                newNode->next = pQueue->current->next;
                                if (pQueue->current->next == 0) {
                                    pQueue->head = newNode;
                                    pQueue->current->next = newNode;
                                } else {
                                    pQueue->current->next->prev = newNode;
                                    pQueue->current->next = newNode;
                                }
                            }
                            break;
                        }
                        if (pQueue->tail == pQueue->current) {
                            if (sprite == 0) ShowError("queue fault 0112");
                            newNode = (PriorityQueueNode*)operator new(0xc);
                            if (newNode != 0) {
                                newNode->data = sprite;
                                newNode->next = 0;
                                newNode->prev = 0;
                            }
                            if (pQueue->current == 0) pQueue->current = pQueue->tail;
                            if (pQueue->head == 0) {
                                pQueue->head = newNode;
                                pQueue->tail = newNode;
                                pQueue->current = newNode;
                            } else {
                                if (pQueue->tail == 0 || pQueue->tail->prev != 0) {
                                    ShowError("queue fault 0113");
                                }
                                newNode->prev = 0;
                                newNode->next = pQueue->tail;
                                pQueue->tail->prev = newNode;
                                pQueue->tail = newNode;
                            }
                            break;
                        }
                        if (pQueue->current != 0) {
                            pQueue->current = pQueue->current->prev;
                        }
                    }
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
void PriorityQueue::AddAfterCurrent(CharSprite* sprite)
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
        newNode->prev = current;
        newNode->next = current->next;
        if (current->next != 0) {
            current->next->prev = newNode;
            current->next = newNode;
        } else {
            head = newNode;
            current->next = newNode;
        }
    }
}

/* Function start: 0x408880 */
CharSprite::CharSprite()
{
    bounds.left = 0;
    bounds.top = 0;
    bounds.right = 0;
    bounds.bottom = 0;
    memset(this, 0, sizeof(CharSprite));
}
