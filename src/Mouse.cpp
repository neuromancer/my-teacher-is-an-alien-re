#include "globals.h"
#include <windows.h>
#include "Mouse.h"
#include "Sprite.h"
#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "AILData.h"
#include "Array.h"
#include "string.h"

extern Sprite* Sprite_Ctor(Sprite*, char*);
extern AILData* AILData_Ctor(AILData*);

extern "C" {
    void FUN_0041ef47();
    void FUN_00425fd0(char*, char*, int);

}

/* Function start: 0x41ECA0 */
Mouse::Mouse()
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

/* Function start: 0x41EF25 */
void Mouse::CleanupFields()
{
    Array_Cleanup(m_hotspots, 8, 0x19, (void(__cdecl*)(void*))0x405770);
}

/* Function start: 0x41EE30 */
Mouse::~Mouse()
{
    AILData* audio = m_audio;
    if (audio != 0) {
        audio->~AILData();
        FreeMemory(audio);
        m_audio = 0;
    }

    Sprite* sprite = m_sprite;
    if (sprite != 0) {
        sprite->~Sprite();
        FreeMemory(sprite);
        m_sprite = 0;
    }

    Sprite* sprite2 = m_sprite2;
    if (sprite2 != 0) {
        sprite2->~Sprite();
        FreeMemory(sprite2);
        m_sprite2 = 0;
    }

    for (int i = 0; i < 25; i++) {
        char* label = m_labels[i];
        if (label != 0) {
            FreeMemory(label);
            m_labels[i] = 0;
        }
    }

    CleanupFields();
    FUN_0041ef47();
}


/* Function start: 0x41EF50 */
int Mouse::LBLParse(char* line)
{
    char local_60[32];
    char local_40[32];
    int local_20;
    int local_1c;
    void* local_18;
    int local_14;

    local_40[0] = 0;
    local_60[0] = 0;

    sscanf(line, " %s ", local_40);

    try {
        if (strcmp(local_40, "AUDIO") == 0) {
            sscanf(line, " %s %s", local_40, local_60);
            local_18 = AllocateMemory(0x10);
            AILData* this_00 = 0;
            if (local_18 != 0) {
                this_00 = AILData_Ctor((AILData*)local_18);
            }
            m_audio = this_00;
            if (this_00 != 0) {
                this_00->Load(local_60);
            }
        } else if (strcmp(local_40, "SPRITE") == 0) {
            local_18 = AllocateMemory(0xd8);
            Sprite* pSVar4 = 0;
            if (local_18 != 0) {
                pSVar4 = Sprite_Ctor((Sprite*)local_18, (char*)0);
            }
            m_sprite = pSVar4;
            pSVar4->flags &= ~2;
            Parser::ProcessFile(pSVar4, this, 0);
        } else if (strcmp(local_40, "HOTPIXEL") == 0) {
            sscanf(line, " %s %d %d %d", local_40, &local_14, &local_20, &local_1c);
            if (local_14 >= 0 && local_14 < 25) {
                m_hotspots[local_14].x = local_20;
                m_hotspots[local_14].y = local_1c;
            }
        } else if (strcmp(local_40, "LABLE") == 0) {
            sscanf(line, " %s %d %s", local_40, &local_14, local_60);
            if (local_14 >= 0 && local_14 < 25) {
                FUN_00425fd0(m_labels[local_14], local_60, 0x10);
            }
        } else if (strcmp(local_40, "END") == 0) {
            m_sprite2 = 0;
            return 1;
        } else {
            Parser::LBLParse(line);
        }
    } catch (...) {
    }

    return 0;
}

/* Function start: 0x41F200 */
void Mouse::DrawCursor()
{
    Sprite* sprite = m_sprite;
    if (sprite == 0) {
        ShowError("missing mouse ");
    }

    int* mouse_coords = *(int**)((char*)g_JoystickManager_00436968 + 0x1a0);

    int final_x = 0;
    if (mouse_coords) {
        final_x = mouse_coords[0];
    }

    if (sprite) {
        if (sprite->current_state >= 0 && sprite->current_state < 25) {
            final_x -= m_hotspots[sprite->current_state].x;
        }
    }

    int final_y = 0;
    if (mouse_coords) {
        final_y = mouse_coords[1];
    }

    if (sprite) {
         if (sprite->current_state >= 0 && sprite->current_state < 25) {
            final_y -= m_hotspots[sprite->current_state].y;
         }
    } else {
        // final_y -= m_offsetY;
    }

    sprite->Do(final_x, final_y, 1.0);
}

/* Function start: 0x422D98 */
int Mouse::SetCursorVisible(unsigned int param_1)
{
    if (DAT_00437506 != '\0') {
        char cVar1 = (char)(param_1 & 1);
        if (cVar1 != DAT_00437507) {
            DAT_00437507 = cVar1;
            ShowCursor(param_1 & 1);
        }
    }
    return 0;
}
