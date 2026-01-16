#include "Character.h"
#include <string.h>
#include <new.h>
#include "Memory.h"
#include "string.h"

// External queue functions
extern void __fastcall FUN_004043a0(PriorityQueue* queue, CharSprite* sprite);
extern PriorityQueueNode* __fastcall FUN_00404470(PriorityQueueNode* node, CharSprite* sprite);

/* Function start: 0x403700 */
Character::Character(char* param_1) {
    int i;
    int* ptrClear;
    PriorityQueue* pQueue;
    PriorityQueue* queueTemp;
    CharSprite* sprite;
    CharSprite* spriteMem;
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
    if (FUN_00424c00(param_1, "peter") != 0) {
        counter = 0;
        characterType = 0;

        do {
            // Allocate CharSprite (0x5C bytes)
            spriteMem = (CharSprite*)operator new(0x5c);
            sprite = 0;
            if (spriteMem != 0) {
                sprite = FUN_00408880(spriteMem);
            }

            // Check queue exists
            if (queue == 0) {
                ShowError("Missing activeQ");
            } else if (FUN_00424c00(param_1, "peter") != 0) {
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
                    FUN_004043a0(pQueue, sprite);
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
                FUN_004043a0(pQueue, sprite);
            }

            counter++;
        } while (counter < 1);
    }
    // Check for "susan"
    else if (FUN_00424c00(param_1, "susan") != 0) {
        counter = 0;
        characterType = 1;

        do {
            spriteMem = (CharSprite*)operator new(0x5c);
            sprite = 0;
            if (spriteMem != 0) {
                sprite = FUN_00408880(spriteMem);
            }

            if (queue == 0) {
                ShowError("Missing activeQ");
            } else if (FUN_00424c00(param_1, "susan") != 0) {
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
                    FUN_004043a0(pQueue, sprite);
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
                FUN_004043a0(pQueue, sprite);
            }

            counter++;
        } while (counter < 1);
    }
    // Check for "duncan"
    else if (FUN_00424c00(param_1, "duncan") != 0) {
        counter = 0;
        characterType = 2;

        do {
            spriteMem = (CharSprite*)operator new(0x5c);
            sprite = 0;
            if (spriteMem != 0) {
                sprite = FUN_00408880(spriteMem);
            }

            if (queue == 0) {
                ShowError("Missing activeQ");
            } else if (FUN_00424c00(param_1, "duncan") != 0) {
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
                    FUN_004043a0(pQueue, sprite);
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
                FUN_004043a0(pQueue, sprite);
            }

            counter++;
        } while (counter < 1);
    } else {
        // Unknown character name - error
        ShowError("Error in Charcter.cpp - Constructor");
    }

    // Set ability flags based on character type
    // Peter: all abilities enabled
    if (FUN_00424c00(param_1, "peter") != 0) {
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
    if (FUN_00424c00(param_1, "susan") != 0) {
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
    if (FUN_00424c00(param_1, "duncan") != 0) {
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
