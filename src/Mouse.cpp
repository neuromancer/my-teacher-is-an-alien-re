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
    m_offsetY = 0;
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
    if (m_audio != 0) {
        m_audio->~AILData();
        FreeMemory(m_audio);
        m_audio = 0;
    }

    if (m_sprite != 0) {
        m_sprite->~Sprite();
        FreeMemory(m_sprite);
        m_sprite = 0;
    }

    // Original code had check checks for fields[30] and [31] as Sprites.
    // Based on memory layout (0x88 + 30*4 = 0x100), this overlaps with m_hotspots[1].y and m_hotspots[2].x.
    // It is possible specific array entries are used as Sprites in some configurations,
    // but the LBLParse "HOTPIXEL" logic treats them as ints.
    // For now we only clean up what we allocated.

    for (int i = 0; i < 25; i++) {
        if (m_labels[i] != 0) {
            FreeMemory(m_labels[i]);
            m_labels[i] = 0;
        }
    }

    // offset 0x17C is inside m_hotspots (index 17).
    // This part of the logic remains unclear and is potentially legacy or incorrect decompliation.
    // We removed CleanupFields() call as we don't have m_fields.
    
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
            m_offsetY = 0;
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
        final_y -= m_offsetY;
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
