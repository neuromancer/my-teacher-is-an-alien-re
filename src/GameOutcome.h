#ifndef GAMEOUTCOME_H
#define GAMEOUTCOME_H

// GameOutcome - Holds combat outcome state
// Size: 0x4 bytes (1 dword)
//   outcome: 0 = not set, 1 = lose, 2 = win
// Full game: trivial data class, ctor/dtor are inlined.
// Demo had PlayOutcomeVideo() but full game uses different outcome handling.
class GameOutcome {
public:
    int outcome; // 0x00

    GameOutcome() { outcome = 0; }
    ~GameOutcome() {}
};

#endif // GAMEOUTCOME_H
