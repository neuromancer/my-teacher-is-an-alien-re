#ifndef MMPLAYER_H
#define MMPLAYER_H

#include "Parser.h"
#include "Queue.h"

class Sprite;

/**
 * MMPlayer - Mouse Interaction Area Management (also known as SpriteList)
 *
 * This class manages clickable regions via a Queue of Sprite objects:
 * - Used by SC_Question, SearchScreen, Handler31 etc. for interactive menus
 * - m_queue holds sprites representing clickable areas/buttons
 * - Destructor properly cleans up all queued sprites
 *
 * Size: 0x98 (152 bytes)
 * Functions: 0x4438A0, 0x443990, 0x443AB0, 0x443B20, 0x443B90, 0x443E30, 0x443ED0, 0x443F40
 *
 * NOTE: This is DIFFERENT from MouseControl (0x1C0 / 448 bytes), which manages
 * cursor rendering/appearance (sprites, hotspots, labels, audio). Do not merge.
 */
class MMPlayer : public Parser {
public:
    MMPlayer();
    ~MMPlayer();

    virtual int LBLParse(char* param_1);  // 0x443F40
    void Init();            // 0x443B20
    void StopAll();         // 0x443AB0
    int Draw();            // 0x443E30
    void AddSprite(Sprite* s);  // 0x443B90
    void ResetAnimations(int param_1);  // 0x443ED0

    int field_0x88;           // 0x88
    int field_0x8c;           // 0x8c - flags (byte at 0x8d is accessed as (field_0x8c >> 8) & 0xff)
    int field_0x90;           // 0x90 - done/result flag
    ZBQueue* m_queue;           // 0x94
};

// SpriteList is the same class as MMPlayer
typedef MMPlayer SpriteList;

#endif // MMPLAYER_H
