#ifndef SCOREDISPLAY_H
#define SCOREDISPLAY_H

// ScoreDisplay - combat score HUD display
// Constructor: 0x40C5E0 (zeros 9 dwords)
// Size: 0x24
class ScoreDisplay {
public:
    int score;              // 0x00 — main score
    int scoreInitial;       // 0x04 — initial score (100)
    int completionCount;    // 0x08 — target completion count
    int missCount;          // 0x0C — miss count
    int field_0x10;         // 0x10
    int hitCount;           // 0x14 — hit count
    int shotsFired;         // 0x18 — shots fired
    int field_0x1c;         // 0x1C
    int field_0x20;         // 0x20

    ScoreDisplay(); // 0x40C5E0

    void AdjustScore(int value); // 0x40C600
};

#endif // SCOREDISPLAY_H
