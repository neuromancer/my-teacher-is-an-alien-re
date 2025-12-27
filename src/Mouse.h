#ifndef MOUSE_H
#define MOUSE_H

#include "Parser.h"
#include "Sprite.h"

class Mouse : public Parser {
public:
    Mouse();
    ~Mouse();
    int LBLParse(char* line);
    void DrawCursor();
    int SetCursorVisible(unsigned int);


private:
    void CleanupFields();

    char* m_pointer_str;
    char* m_examine_str;
    char* m_pickup_str;
    char* m_unknown_str;

    char* m_fields[78];
};

#endif // MOUSE_H
