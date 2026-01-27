#include "globals.h"
#include <windows.h>
#include "MouseControl.h"
#include "Sprite.h"
#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "Sample.h"
#include "string.h"
#include "new.h"
#include "InputManager.h"

/* Function start: 0x41ECA0 */
MouseControl::MouseControl()
{
    memset(m_labels, 0, sizeof(m_labels));
    m_sprite = 0;
    m_sprite2 = 0;
    memset(m_hotspots, 0, sizeof(m_hotspots));
    m_audio = 0;

    m_labels[0] = (char*)AllocateMemory(16);
    m_labels[1] = (char*)AllocateMemory(16);
    m_labels[2] = (char*)AllocateMemory(16);
    m_labels[3] = (char*)AllocateMemory(16);

    strcpy(m_labels[0], "POINTER");
    strcpy(m_labels[1], "EXAMINE");
    strcpy(m_labels[2], "PICKUP");
    strcpy(m_labels[3], "");
}



/* Function start: 0x41EE30 */
MouseControl::~MouseControl()
{
    Sample* audio = m_audio;
    if (audio != 0) {
        delete audio;
        m_audio = 0;
    }

    Sprite* sprite = m_sprite;
    if (sprite != 0) {
        delete sprite;
        m_sprite = 0;
    }

    Sprite* sprite2 = m_sprite2;
    if (sprite2 != 0) {
        delete sprite2;
        m_sprite2 = 0;
    }

    for (int i = 0; i < 25; i++) {
        char* label = m_labels[i];
        if (label != 0) {
            FreeMemory(label);
            m_labels[i] = 0;
        }
    }
}

/* Function start: 0x41EF50 */
int MouseControl::LBLParse(char* line)
{
    int index;
    void* ptr;
    int y;
    int x;
    char cmd[32];
    char args[32];

    cmd[0] = 0;
    args[0] = 0;

    sscanf(line, " %s ", cmd);

    if (strcmp(cmd, "AUDIO") == 0) {
        sscanf(line, " %s %s", cmd, args);
        Sample* audio = new Sample();
        m_audio = audio;
        if (audio != 0) {
            audio->Load(args);
        }
    } else if (strcmp(cmd, "SPRITE") == 0) {
        ptr = AllocateMemory(0xd8);
        Sprite* sprite = 0;
        if (ptr != 0) {
            sprite = new (ptr) Sprite(0);
        }
        m_sprite = sprite;
        m_sprite->flags &= ~2;
        Parser::ProcessFile(m_sprite, this, 0);
    } else if (strcmp(cmd, "HOTPIXEL") == 0) {
        sscanf(line, " %s %d %d %d", cmd, &index, &x, &y);
        m_hotspots[index].x = x;
        m_hotspots[index].y = y;
    } else if (strcmp(cmd, "LABLE") == 0) {
        sscanf(line, " %s %d %s", cmd, &index, args);
        strncpy(m_labels[index], args, 0x10);
    } else if (strcmp(cmd, "END") == 0) {
        m_sprite2 = 0;
        return 1;
    } else {
        Parser::LBLParse("MouseControl");
    }

    return 0;
}

/* Function start: 0x41F200 */
void MouseControl::DrawCursor()
{
    if (m_sprite == 0) {
        ShowError("missing mouse ");
    }

    InputManager* mgr = g_InputManager_00436968;
    int final_x = 0;
    InputState* pMouse = mgr->pMouse;
    if (pMouse != 0) {
        final_x = pMouse->x;
    }

    Sprite* sprite = m_sprite;
    if (sprite != 0) {
        final_x -= m_hotspots[sprite->current_state].x;
    } else {
        final_x -= (int)sprite;
    }

    int final_y = 0;
    if (pMouse != 0) {
        final_y = pMouse->y;
    }

    if (sprite == 0) {
        final_y -= (int)m_sprite2;
    } else {
        final_y -= m_hotspots[sprite->current_state].y;
    }

    sprite->Do(final_x, final_y, 1.0);
}

extern "C" int SetCursorVisible(unsigned int param_1);

/* Function start: 0x422D98 */
int SetCursorVisible(unsigned int param_1)
{
    if (DAT_00437506 != '\0') {
        unsigned int masked = param_1 & 1;
        if ((char)masked != DAT_00437507) {
            DAT_00437507 = (char)masked;
            ShowCursor(masked);
        }
    }
    return 0;
}
