#ifndef MOUSE_H
#define MOUSE_H

#include "Parser.h"
#include "Sprite.h"
class AILData;
class Mouse : public Parser {
public:
    Mouse();
    ~Mouse();
    int LBLParse(char* line);
    void DrawCursor();
    int SetCursorVisible(unsigned int);


private:
    void CleanupFields();

    char* m_labels[25];        // 0x88
    Sprite* m_sprite;          // 0xEC
    int m_offsetY;             // 0xF0
    struct Point {
        int x;
        int y;
    };
    Point m_hotspots[25];      // 0xF4
    AILData* m_audio;          // 0x1BC
};
#endif // MOUSE_H