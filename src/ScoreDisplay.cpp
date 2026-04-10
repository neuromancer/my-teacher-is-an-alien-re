#include "ScoreDisplay.h"

/* Function start: 0x40C600 */
void ScoreDisplay::AdjustScore(int value) {
    int newVal = ScoreDisplay::scoreInitial + value;
    ScoreDisplay::scoreInitial = newVal;
    if (newVal < 0) {
        ScoreDisplay::scoreInitial = 0;
        return;
    }
    if (newVal > 0xc8) {
        ScoreDisplay::scoreInitial = 0xc8;
    }
}
