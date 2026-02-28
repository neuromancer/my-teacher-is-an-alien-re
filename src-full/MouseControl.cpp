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

/* Function start: 0x4327C0 */
MouseControl::MouseControl()
{
    memset(&m_labels, 0, 24);
}

/* Function start: 0x432830 */
MouseControl::~MouseControl()
{
    Sample* audio = m_audio;
    if (audio != 0) {
        audio->Unload();
        operator delete(audio);
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

    if (m_hotspots != 0) {
        delete[] m_hotspots;
        m_hotspots = 0;
    }

    if (m_labels != 0) {
        int i = 0;
        if (m_maxMice > 0) {
            do {
                if (m_labels[i] != 0) {
                    operator delete(m_labels[i]);
                    m_labels[i] = 0;
                }
                i++;
            } while (i < m_maxMice);
        }
    }

    if (m_labels != 0) {
        operator delete(m_labels);
        m_labels = 0;
    }
}

/* Function start: 0x4329B0 */
int MouseControl::LBLParse(char* line)
{
    int index;
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
        Sprite* sprite = new Sprite(0);
        m_sprite = sprite;
        sprite->flags &= ~2;
        Parser::ProcessFile(m_sprite, this, 0);
    } else if (strcmp(cmd, "MAXMICE") == 0) {
        sscanf(line, " %s %d", cmd, &m_maxMice);
        m_hotspots = new Point[m_maxMice];
        m_labels = (char**)operator new(m_maxMice * 4);
        int i = 0;
        if (m_maxMice > 0) {
            do {
                m_labels[i] = 0;
                i++;
            } while (m_maxMice > i);
        }
    } else if (strcmp(cmd, "HOTPIXEL") == 0) {
        sscanf(line, " %s %d %d %d", cmd, &index, &x, &y);
        m_hotspots[index].x = x;
        m_hotspots[index].y = y;
    } else if (strcmp(cmd, "LABEL") == 0) {
        sscanf(line, " %s %d %s", cmd, &index, args);
        if (m_labels[index] != 0) {
            operator delete(m_labels[index]);
            m_labels[index] = 0;
        }
        m_labels[index] = (char*)operator new(0x10);
        strncpy(m_labels[index], args, 0x10);
    } else if (strcmp(cmd, "END") == 0) {
        m_sprite2 = 0;
        return 1;
    } else {
        Parser::LBLParse("MouseControl");
    }

    return 0;
}

/* Function start: 0x432DA0 */
void MouseControl::DrawCursor()
{
    Sprite* sprite;
    InputState* pMouse;

    pMouse = g_InputManager_00436968->pMouse;
    int final_x = 0;
    if (pMouse != 0) {
        final_x = pMouse->x;
    }

    sprite = m_sprite;
    if (sprite == 0) {
        final_x -= m_hotspots[-1].x;
    } else {
        final_x -= m_hotspots[sprite->current_state].x;
    }

    int final_y;
    if (pMouse == 0) {
        final_y = 0;
    } else {
        final_y = pMouse->y;
    }

    if (sprite == 0) {
        final_y -= m_hotspots[-1].y;
    } else {
        final_y -= m_hotspots[sprite->current_state].y;
    }

    sprite->Do(final_x, final_y, 1.0);
}

extern "C" int SetCursorVisible(unsigned int param_1);

/* Function start: 0x453473 */
int SetCursorVisible(unsigned int param_1)
{
    if (g_CursorVisible_00437506 != '\0') {
        unsigned int masked = param_1 & 1;
        if ((char)masked != g_CursorState_00437507) {
            g_CursorState_00437507 = (char)masked;
            ShowCursor(masked);
        }
    }
    return 0;
}
