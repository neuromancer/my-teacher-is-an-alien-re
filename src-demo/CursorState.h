#ifndef CURSORSTATE_H
#define CURSORSTATE_H

// CursorState - Stores game score/stats data
// Size: 0x24 bytes (9 dwords)
// Constructor: 0x416B80
class CursorState {
public:
    int score;              // 0x00 - main score (adjusted by hitMissPoints)
    int scoreInitial;       // 0x04 - initial score value (set to 100)
    int completionCount;    // 0x08 - target completion count
    int missCount;          // 0x0c - miss count (tracked by m_prevMissCount)
    int field_0x10;         // 0x10
    int hitCount;           // 0x14 - hit count (tracked by m_prevHitCount)
    int shotsFired;         // 0x18 - shots fired count
    int field_0x1c;         // 0x1c
    int field_0x20;         // 0x20

    CursorState(); // 0x416B80
};

#endif // CURSORSTATE_H
