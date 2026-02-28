#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "Parser.h"
#include "Sprite.h"
class Sample;

/**
 * MouseControl - Cursor Rendering and Appearance
 *
 * Full game version: dynamically allocated arrays sized by MAXMICE command.
 * Size: 0xA8 (168 bytes)
 * Functions: 0x4327C0-0x432DA0
 */
extern "C" int SetCursorVisible(unsigned int);

struct Point {
    Point() {}
    ~Point() {}
    int x;
    int y;
};

class MouseControl : public Parser {
public:
    MouseControl();
    ~MouseControl();
    int LBLParse(char* line);
    void DrawCursor();

    char** m_labels;           // 0x90
    Sprite* m_sprite;          // 0x94
    Sprite* m_sprite2;         // 0x98
    Point* m_hotspots;         // 0x9C
    int m_maxMice;             // 0xA0
    Sample* m_audio;           // 0xA4
};
#endif // MOUSECONTROL_H
