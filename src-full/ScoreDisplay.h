#ifndef SCOREDISPLAY_H
#define SCOREDISPLAY_H

// ScoreDisplay - combat score HUD display
// Constructor: 0x40C5E0 (zeros 9 dwords)
// Size: 0x24
class ScoreDisplay {
public:
    int fields[9];

    ScoreDisplay() {
        for (int i = 9; i != 0; i--) {
            fields[i - 1] = 0;
        }
    }
};

#endif // SCOREDISPLAY_H
