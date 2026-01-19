#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "Parser.h"
#include "Queue.h"

class Sprite;

/**
 * MouseControl - Mouse Interaction Area Management (also known as SpriteList)
 *
 * This class manages clickable regions via a Queue of Sprite objects:
 * - Used by SC_Question, SearchScreen, SCI_AfterSchoolMenu, Handler10 etc. for interactive menus
 * - m_queue holds sprites representing clickable areas/buttons
 * - Destructor properly cleans up all queued sprites
 *
 * Size: 0x98 (152 bytes)
 * Functions: 0x41F280, 0x41F360, 0x41F480, 0x41F4F0, 0x41F800
 *
 * NOTE: This is DIFFERENT from Mouse (0x1C0 / 448 bytes), which manages
 * cursor rendering/appearance (sprites, hotspots, labels, audio). Do not merge.
 */
class MouseControl : public Parser {
public:
    MouseControl();
    ~MouseControl();

    virtual int LBLParse(char* param_1);  // 0x41F8A0
    void Init();            // 0x41F4F0
    void StopAll();         // 0x41F480
    int DoAll();            // 0x41F800
    void AddSprite(Sprite* s);  // 0x41F560

    int field_0x88;           // 0x88
    int field_0x8c;           // 0x8c - flags (byte at 0x8d is accessed as (field_0x8c >> 8) & 0xff)
    int field_0x90;           // 0x90 - done/result flag
    Queue* m_queue;           // 0x94
};

// SpriteList is the same class as MouseControl
typedef MouseControl SpriteList;

#endif // MOUSECONTROL_H
