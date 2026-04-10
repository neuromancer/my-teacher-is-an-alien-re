#ifndef SCI_PRACTICEROOM_H
#define SCI_PRACTICEROOM_H

#include "Handler.h"

class MMPlayer;
class Sprite;
class Palette;
class LinkedList;
class SlimeTable;
class T_MenuHotspot;

// SCI_PracticeRoom - Practice room handler (case 43/0x2B)
// Constructor: 0x42A8D0
// Size: 0xD0
// Vtable: 0x461520
// References: "IN_PRACTICEROOM", "SCI_PracticeRoom"
class SCI_PracticeRoom : public Handler {
public:
    SCI_PracticeRoom();
    ~SCI_PracticeRoom();

    // Virtual overrides
    void OnProcessStart() {}                 // 0x42B3A0 (empty override)
    void Init(SC_Message* msg);              // 0x42A9F0
    int AddMessage(SC_Message* msg);         // 0x42AE80
    int ShutDown(SC_Message* msg);           // 0x42AC20
    void Update(int param1, int param2);     // 0x42AD80
    int Exit(SC_Message* msg);               // 0x42B030
    int LBLParse(char* param_1);             // 0x42B3B0

    int introPlayed;                // 0xA8 - intro animation completed flag
    MMPlayer* ambient;              // 0xAC - ambient background animation
    Sprite* introSprite;            // 0xB0 - intro animation sprite
    Palette* palette;               // 0xB4 - room palette
    LinkedList* hotspotList;        // 0xB8 - list of SC_PRHotSpot entries
    T_MenuHotspot* periodSprites[3];// 0xBC-0xC4 - sprites for each period (D/S/P)
    SlimeTable* slimeTable;         // 0xC8 - slime table for practice activities
    int bgSoundHandle;              // 0xCC - background sound identifier
};

#endif // SCI_PRACTICEROOM_H
