#include "SCI_Dialog.h"
#include "MouseControl.h"
#include "string.h"
#include "MMPlayer.h"
#include "Sprite.h"
#include "Queue.h"
#include "Memory.h"
#include "Parser.h"
#include "SC_Question.h"
#include "GameState.h"
#include "Message.h"
#include "globals.h"
#include "InputManager.h"
#include "FlagArray.h"
#include <string.h>
#include <stdio.h>

/* Function start: 0x417C80 */
DialogQuestion* SCI_Dialog::GetDialogByIndex(int index) {
    int counter;
    void* data;
    Queue* queue;

    counter = 0;
    queue = dialogQueue;
    if (queue == 0) goto LAB_ret_zero;
    queue->current = queue->head;
    if (dialogQueue->head == 0) goto LAB_ret_zero;

    do {
        queue = dialogQueue;
        if (counter == index) {
            data = queue->RemoveCurrent();
            return (DialogQuestion*)data;
        }
        if (queue->tail == queue->current) goto LAB_ret_zero;
        if (queue->current != 0) {
            queue->current = queue->current->next;
        }
        counter++;
        if (dialogQueue->head == 0) goto LAB_ret_zero;
    } while (1);

LAB_ret_zero:
    return 0;
}

/* Function start: 0x417E60 */
DialogQuestion* SCI_Dialog::FindDialogById(int id) {
    DialogQuestion* searchQuestion;
    Queue* queue;
    QueueNode* node;
    DialogQuestion* nodeData;

    searchQuestion = new SC_Question(id, this);

    queue = dialogQueue;
    if (queue == 0) goto not_found;

    if (searchQuestion == 0) {
        ShowError("queue fault 0103");
    }

    queue->current = queue->head;
    if (queue->current == 0) goto not_found;

    do {
        node = queue->current;
        nodeData = 0;
        if (node != 0) {
            nodeData = (DialogQuestion*)node->data;
        }

        if (nodeData->questionId == searchQuestion->questionId) {
            if (searchQuestion != 0) {
                searchQuestion->~SC_Question();
                FreeMemory(searchQuestion);
            }

            {
                Queue* q = dialogQueue;
                void* data = q->RemoveCurrent();
                return (DialogQuestion*)data;
            }
        }

        if (queue->tail == queue->current) goto not_found;
        if (queue->current != 0) {
            queue->current = queue->current->next;
        }
    } while (queue->current != 0);

not_found:
    if (searchQuestion != 0) {
        searchQuestion->~SC_Question();
        FreeMemory(searchQuestion);
    }
    return 0;
}

static char g_SpriteString[] = "SPRITE0";

/* Function start: 0x417FE0 */
void ResetSpriteStates()
{
    char name[8];
    char digit;
    int i;

    digit = '1';
    i = 5;
    *(int*)name = *(int*)g_SpriteString;
    *(int*)(name + 4) = *(int*)(g_SpriteString + 4);

    do {
        GameState* gs = g_GameState_0046aa30;
        name[6] = digit;
        int idx = gs->FindState(name);
        digit++;
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        i--;
        gs->stateValues[idx] = 0;
    } while (i != 0);
}
