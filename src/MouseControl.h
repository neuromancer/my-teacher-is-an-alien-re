#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "Parser.h"
#include "Sprite.h"
class Sample;

/**
 * MouseControl - Cursor Rendering and Appearance
 * 
 * This class manages the visual cursor (pointer) display:
 * - Cursor sprite graphics (m_sprite, m_sprite2)
 * - Per-state hotspot offsets (m_hotspots) - where the "click point" is for each cursor state
 * - Cursor state labels (m_labels) - e.g., "POINTER", "EXAMINE", "PICKUP"
 * - Associated audio (m_audio)
 * 
 * Size: 0x1C0 (448 bytes)
 * Functions: 0x41ECA0-0x41F200, 0x422D98
 * 
 * NOTE: This is DIFFERENT from MMPlayer (0x98 bytes), which manages
 * clickable interaction areas via a Queue of Sprite objects. Do not merge.
 * The similarity in names is due to the string "MouseControl" being used
 * in MouseControl::LBLParse for unknown command delegation.
 */
extern "C" int SetCursorVisible(unsigned int);

class MouseControl : public Parser {
public:
    MouseControl();
    ~MouseControl();
    int LBLParse(char* line);
    void DrawCursor();

    char* m_labels[25];        // 0x88
    Sprite* m_sprite;          // 0xEC
    Sprite* m_sprite2;         // 0xF0
    struct Point {
        Point() {}
        ~Point() {}
        int x;
        int y;
    };
    Point m_hotspots[25];      // 0xF4
    Sample* m_audio;          // 0x1BC
};
#endif // MOUSE_H