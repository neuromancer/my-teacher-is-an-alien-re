#include "ScoreDisplay.h"

/* Function start: 0x40C600 */
void ScoreDisplay::AdjustScore(int value) {
    int newVal = ScoreDisplay::fields[1] + value;
    ScoreDisplay::fields[1] = newVal;
    if (newVal < 0) {
        ScoreDisplay::fields[1] = 0;
        return;
    }
    if (newVal > 0xc8) {
        ScoreDisplay::fields[1] = 0xc8;
    }
}
