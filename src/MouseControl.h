#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "Parser.h"
#include "Queue.h"

/**
 * MouseControl - Mouse Interaction Area Management
 * 
 * This class manages clickable regions via a Queue of Sprite objects:
 * - Used by SC_Question, SearchScreen, SCI_AfterSchoolMenu etc. for interactive menus
 * - m_queue holds sprites representing clickable areas/buttons
 * - Destructor properly cleans up all queued sprites
 * 
 * Size: 0x98 (152 bytes)
 * Functions: 0x41F280-0x41F360
 * 
 * NOTE: This is DIFFERENT from Mouse (0x1C0 / 448 bytes), which manages
 * cursor rendering/appearance (sprites, hotspots, labels, audio). Do not merge.
 */
class MouseControl : public Parser {
public:
    MouseControl();
    ~MouseControl();

private:
    int field_0x88[3];  // Parser ends at 0x88, so from 0x88 to 0x94
    Queue* m_queue;     // 0x94
};

#endif // MOUSECONTROL_H
