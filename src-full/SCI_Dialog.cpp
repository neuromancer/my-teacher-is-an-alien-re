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
extern "C" extern GameState* g_GameState_0046aa30;
#include "Message.h"
#include "globals.h"
#include "InputManager.h"
#include "FlagArray.h"
#include <string.h>
#include <stdio.h>

/* Function start: 0x417C80 */ /* ~95% match */
DialogQuestion* SCI_Dialog::GetDialogByIndex(int index) {
    int counter;
    QueueNode* node;
    QueueNode* tmp;
    DialogQuestion* result;
    Queue* queue;

    counter = 0;
    queue = field_C8;
    if (queue == 0) goto LAB_ret_zero;
    queue->current = queue->head;
    queue = field_C8;
    if (queue->current == 0) goto LAB_ret_zero;

    do {
        if (counter == index) {
            node = (QueueNode*)queue->current;
            result = 0;
            if (node != 0) {
                result = (DialogQuestion*)node->data;
            }
            return result;
        }
        counter++;
        tmp = (QueueNode*)queue->current;
        if (queue->tail == tmp) goto LAB_ret_zero;
        if (tmp != 0) {
            queue->current = tmp->next;
        }
    } while (queue->current != 0);

LAB_ret_zero:
    return 0;
}

/* Function start: 0x417E60 */ /* ~89% match */
DialogQuestion* SCI_Dialog::FindDialogById(int id) {
    DialogQuestion* searchQuestion = new SC_Question(id, this);
    Queue* queue;
    QueueNode* node;
    QueueNode* tmp;
    DialogQuestion* nodeData;
    DialogQuestion* result;

    queue = field_C8;
    if (queue == 0) goto not_found;
    queue->current = queue->head;
    if (field_C8->current == 0) goto not_found;

    do {
        node = (QueueNode*)field_C8->current;
        nodeData = 0;
        if (node != 0) {
            nodeData = (DialogQuestion*)node->data;
        }

        if (nodeData->questionId == searchQuestion->questionId) {
            if (searchQuestion != 0) {
                delete searchQuestion;
            }
            return nodeData;
        }

        tmp = (QueueNode*)field_C8->current;
        if (field_C8->tail == tmp) goto not_found;
        if (tmp != 0) {
            field_C8->current = tmp->next;
        }
    } while (field_C8->current != 0);

not_found:
    if (searchQuestion != 0) {
        delete searchQuestion;
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
