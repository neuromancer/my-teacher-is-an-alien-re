#include "Character.h"
#include <string.h>
#include <new.h>
#include "Memory.h"
#include "string.h"

// External queue functions
extern PriorityQueueNode* __fastcall FUN_00404470(PriorityQueueNode* node, CharSprite* sprite);

// strstr is just strstr from the CRT - use standard library

/* Function start: 0x4043A0 */
void PriorityQueue::AddAfterCurrent(CharSprite* sprite)
{
    void* allocResult;
    PriorityQueueNode* newNode;

    if (sprite == 0) {
        ShowError("queue fault 0102");
    }

    allocResult = operator new(0xc);
    newNode = 0;
    if (allocResult != 0) {
        newNode = (PriorityQueueNode*)allocResult;
        newNode->data = sprite;
        newNode->next = 0;
        newNode->prev = 0;
    }

    if (PriorityQueue::current == 0) {
        PriorityQueue::current = PriorityQueue::head;
    }

    if (PriorityQueue::head == 0) {
        PriorityQueue::head = newNode;
        PriorityQueue::tail = newNode;
        PriorityQueue::current = newNode;
    } else {
        newNode->prev = PriorityQueue::current;
        newNode->next = PriorityQueue::current->next;
        if (PriorityQueue::current->next == 0) {
            PriorityQueue::head = newNode;
            PriorityQueue::current->next = newNode;
        } else {
            PriorityQueue::current->next->prev = newNode;
            PriorityQueue::current->next = newNode;
        }
    }
}

/* Function start: 0x408880 */
CharSprite::CharSprite()
{
    int* puVar3;
    int iVar2;

    bounds.left = 0;
    bounds.top = 0;
    bounds.right = 0;
    bounds.bottom = 0;
    puVar3 = (int*)this;
    for (iVar2 = 0x17; iVar2 != 0; iVar2--) {
        *puVar3 = 0;
        puVar3++;
    }
}

/* Function start: 0x403700 */
Character::Character(char* param_1) {
    int i;
    int* ptrClear;
    PriorityQueue* pQueue;
    PriorityQueue* queueTemp;
    CharSprite* sprite;
    PriorityQueueNode* node;
    PriorityQueueNode* nodeTemp;
    int counter;

    // Zero 16 dwords (0x40 bytes) starting at this
    ptrClear = (int*)this;
    for (i = 0x10; i != 0; i--) {
        *ptrClear = 0;
        ptrClear++;
    }

    // Initialize queue pointers to 0
    queue = 0;
    field_3c = 0;

    // Set character type to -1 (uninitialized)
    characterType = -1;

    // Allocate PriorityQueue (0x10 bytes)
    queueTemp = (PriorityQueue*)operator new(0x10);
    pQueue = 0;
    if (queueTemp != 0) {
        queueTemp->type = 0;
        queueTemp->head = 0;
        queueTemp->tail = 0;
        queueTemp->current = queueTemp->head;
        pQueue = queueTemp;
    }
    queue = pQueue;

    // Check for "peter"
    if (strstr(param_1, "peter") != 0) {
        counter = 0;
        characterType = 0;

        do {
            // Allocate CharSprite (0x5C bytes)
            sprite = new CharSprite();

            // Check queue exists
            if (queue == 0) {
                ShowError("Missing activeQ");
            } else if (strstr(param_1, "peter") != 0) {
                if ((unsigned int)counter <= 4) {
                    // Copy "MEET MY" to sprite->name
                    strcpy(sprite->name, "MEET MY");
                }
            }

            // Verify sprite allocated
            if (sprite == 0) {
                ShowError("queue fault 0101");
            }

            pQueue = queue;
            pQueue->current = pQueue->head;

            // Check queue type for priority insertion
            if (pQueue->type == 1 || pQueue->type == 2) {
                if (pQueue->head == 0) {
                    pQueue->AddAfterCurrent(sprite);
                } else {
                    // Priority insertion loop
                    while (pQueue->current != 0) {
                        PriorityQueueNode* curr = pQueue->current;

                        // Check if sprite priority is higher
                        if (curr->data->priority < sprite->priority) {
                            if (sprite == 0) {
                                ShowError("queue fault 0102");
                            }

                            // Allocate new node
                            nodeTemp = (PriorityQueueNode*)operator new(0xc);
                            node = 0;
                            if (nodeTemp != 0) {
                                node = nodeTemp;
                                nodeTemp->data = sprite;
                                nodeTemp->next = 0;
                                nodeTemp->prev = 0;
                            }

                            // Reset current if needed
                            if (pQueue->current == 0) {
                                pQueue->current = pQueue->head;
                            }

                            // Insert node
                            if (pQueue->head == 0) {
                                pQueue->head = node;
                                pQueue->tail = node;
                                pQueue->current = node;
                            } else {
                                node->prev = pQueue->current;
                                node->next = pQueue->current->next;
                                if (pQueue->current->next == 0) {
                                    pQueue->head = node;
                                    pQueue->current->next = node;
                                } else {
                                    pQueue->current->next->prev = node;
                                    pQueue->current->next = node;
                                }
                            }
                            break;
                        }

                        // Check if at tail
                        if (pQueue->tail == curr) {
                            if (sprite == 0) {
                                ShowError("queue fault 0112");
                            }

                            // Allocate node at tail
                            nodeTemp = (PriorityQueueNode*)operator new(0xc);
                            if (nodeTemp == 0) {
                                node = 0;
                            } else {
                                nodeTemp->data = sprite;
                                node = nodeTemp;
                                nodeTemp->next = 0;
                                nodeTemp->prev = 0;
                            }

                            if (pQueue->current == 0) {
                                pQueue->current = pQueue->tail;
                            }

                            if (pQueue->head == 0) {
                                pQueue->head = node;
                                pQueue->tail = node;
                                pQueue->current = node;
                            } else {
                                if (pQueue->tail == 0 || pQueue->tail->prev != 0) {
                                    ShowError("queue fault 0113");
                                }
                                node->prev = 0;
                                node->next = pQueue->tail;
                                pQueue->tail->prev = node;
                                pQueue->tail = node;
                            }
                            break;
                        }

                        // Move to previous node
                        if (curr != 0) {
                            pQueue->current = curr->prev;
                        }
                    }
                }
            } else {
                pQueue->AddAfterCurrent(sprite);
            }

            counter++;
        } while (counter < 1);
    }
    // Check for "susan"
    else if (strstr(param_1, "susan") != 0) {
        counter = 0;
        characterType = 1;

        do {
            sprite = new CharSprite();

            if (queue == 0) {
                ShowError("Missing activeQ");
            } else if (strstr(param_1, "susan") != 0) {
                if ((unsigned int)counter <= 4) {
                    strcpy(sprite->name, "SUS");
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
                        PriorityQueueNode* curr = pQueue->current;

                        if (curr->data->priority < sprite->priority) {
                            if (sprite == 0) {
                                ShowError("queue fault 0102");
                            }
                            nodeTemp = (PriorityQueueNode*)operator new(0xc);
                            node = 0;
                            if (nodeTemp != 0) {
                                node = nodeTemp;
                                nodeTemp->data = sprite;
                                nodeTemp->next = 0;
                                nodeTemp->prev = 0;
                            }

                            if (pQueue->current == 0) {
                                pQueue->current = pQueue->head;
                            }

                            if (pQueue->head == 0) {
                                pQueue->head = node;
                                pQueue->tail = node;
                                pQueue->current = node;
                            } else {
                                node->prev = pQueue->current;
                                node->next = pQueue->current->next;
                                if (pQueue->current->next == 0) {
                                    pQueue->head = node;
                                    pQueue->current->next = node;
                                } else {
                                    pQueue->current->next->prev = node;
                                    pQueue->current->next = node;
                                }
                            }
                            break;
                        }

                        if (pQueue->tail == curr) {
                            if (sprite == 0) {
                                ShowError("queue fault 0112");
                            }
                            nodeTemp = (PriorityQueueNode*)operator new(0xc);
                            if (nodeTemp == 0) {
                                node = 0;
                            } else {
                                nodeTemp->data = sprite;
                                node = nodeTemp;
                                nodeTemp->next = 0;
                                nodeTemp->prev = 0;
                            }

                            if (pQueue->current == 0) {
                                pQueue->current = pQueue->tail;
                            }

                            if (pQueue->head == 0) {
                                pQueue->head = node;
                                pQueue->tail = node;
                                pQueue->current = node;
                            } else {
                                if (pQueue->tail == 0 || pQueue->tail->prev != 0) {
                                    ShowError("queue fault 0113");
                                }
                                node->prev = 0;
                                node->next = pQueue->tail;
                                pQueue->tail->prev = node;
                                pQueue->tail = node;
                            }
                            break;
                        }

                        if (curr != 0) {
                            pQueue->current = curr->prev;
                        }
                    }
                }
            } else {
                pQueue->AddAfterCurrent(sprite);
            }

            counter++;
        } while (counter < 1);
    }
    // Check for "duncan"
    else if (strstr(param_1, "duncan") != 0) {
        counter = 0;
        characterType = 2;

        do {
            sprite = new CharSprite();

            if (queue == 0) {
                ShowError("Missing activeQ");
            } else if (strstr(param_1, "duncan") != 0) {
                if ((unsigned int)counter <= 4) {
                    strcpy(sprite->name, "TESTPUZZLE");
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
                        PriorityQueueNode* curr = pQueue->current;

                        if (curr->data->priority < sprite->priority) {
                            if (sprite == 0) {
                                ShowError("queue fault 0102");
                            }
                            nodeTemp = (PriorityQueueNode*)operator new(0xc);
                            node = 0;
                            if (nodeTemp != 0) {
                                node = nodeTemp;
                                nodeTemp->data = sprite;
                                nodeTemp->next = 0;
                                nodeTemp->prev = 0;
                            }

                            if (pQueue->current == 0) {
                                pQueue->current = pQueue->head;
                            }

                            if (pQueue->head == 0) {
                                pQueue->head = node;
                                pQueue->tail = node;
                                pQueue->current = node;
                            } else {
                                node->prev = pQueue->current;
                                node->next = pQueue->current->next;
                                if (pQueue->current->next == 0) {
                                    pQueue->head = node;
                                    pQueue->current->next = node;
                                } else {
                                    pQueue->current->next->prev = node;
                                    pQueue->current->next = node;
                                }
                            }
                            break;
                        }

                        if (pQueue->tail == curr) {
                            if (sprite == 0) {
                                ShowError("queue fault 0112");
                            }
                            nodeTemp = (PriorityQueueNode*)operator new(0xc);
                            if (nodeTemp == 0) {
                                node = 0;
                            } else {
                                nodeTemp->data = sprite;
                                node = nodeTemp;
                                nodeTemp->next = 0;
                                nodeTemp->prev = 0;
                            }

                            if (pQueue->current == 0) {
                                pQueue->current = pQueue->tail;
                            }

                            if (pQueue->head == 0) {
                                pQueue->head = node;
                                pQueue->tail = node;
                                pQueue->current = node;
                            } else {
                                if (pQueue->tail == 0 || pQueue->tail->prev != 0) {
                                    ShowError("queue fault 0113");
                                }
                                node->prev = 0;
                                node->next = pQueue->tail;
                                pQueue->tail->prev = node;
                                pQueue->tail = node;
                            }
                            break;
                        }

                        if (curr != 0) {
                            pQueue->current = curr->prev;
                        }
                    }
                }
            } else {
                pQueue->AddAfterCurrent(sprite);
            }

            counter++;
        } while (counter < 1);
    } else {
        // Unknown character name - error
        ShowError("Error in Charcter.cpp - Constructor");
    }

    // Set ability flags based on character type
    // Peter: all abilities enabled
    if (strstr(param_1, "peter") != 0) {
        int val = 1;
        abilities[0] = val;
        abilities[1] = val;
        abilities[2] = val;
        abilities[3] = val;
        abilities[4] = val;
        abilities[5] = val;
        abilities[6] = val;
        abilities[7] = val;
    }

    // Susan: most abilities, but abilities[2] and abilities[6] disabled
    if (strstr(param_1, "susan") != 0) {
        int val = 1;
        abilities[0] = val;
        abilities[1] = val;
        abilities[2] = 0;
        abilities[3] = val;
        abilities[4] = val;
        abilities[5] = val;
        abilities[6] = 0;
        abilities[7] = val;
    }

    // Duncan: most abilities, but abilities[4] disabled
    if (strstr(param_1, "duncan") != 0) {
        int val = 1;
        abilities[0] = val;
        abilities[1] = val;
        abilities[2] = val;
        abilities[3] = val;
        abilities[5] = val;
        abilities[6] = val;
        abilities[4] = 0;
        abilities[7] = val;
    }
}
