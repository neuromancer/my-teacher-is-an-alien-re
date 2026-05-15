#include "ScoreDisplay.h"
#include <string.h>

/* Function start: 0x40C5E0 */
ScoreDisplay::ScoreDisplay() {
    memset(this, 0, 9 * sizeof(int));
}

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
