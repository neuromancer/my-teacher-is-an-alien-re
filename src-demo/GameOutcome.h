#ifndef GAMEOUTCOME_H
#define GAMEOUTCOME_H

// GameOutcome - Holds combat outcome state
// Size: 0x4 bytes (1 dword)
//   outcome: 0 = not set, 1 = lose, 2 = win
class GameOutcome {
public:
    int outcome; // 0x00

    GameOutcome();          // 0x415D20
    ~GameOutcome();         // 0x415D30
    void PlayOutcomeVideo(); // 0x415D40
};

#endif // GAMEOUTCOME_H
