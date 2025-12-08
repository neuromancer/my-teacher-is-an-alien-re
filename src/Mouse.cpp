#include <windows.h>
#include <new>
#include "Mouse.h"
#include "Sprite.h"
#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"

extern "C" {
    void FUN_00424b00(void*, int, int, void*, void*);
    void FUN_0041e470(void*);
    void SpriteArray_Cleanup(void*);
    void Array_Cleanup(void*, int, int, void*);
    void FUN_0041ef47();
    void* FUN_0041e460(void*);
    int FUN_0041e490(void*, const char*);
    void FUN_00425fd0(char*, char*, int);
    void ShowError(const char*, ...);
    extern int DAT_00436968;
    extern char DAT_00437506;
    extern char DAT_00437507;
}

/* Function start: 0x41ECA0 */
Mouse::Mouse()
{
    memset(m_fields, 0, sizeof(m_fields));

    m_pointer_str = (char*)AllocateMemory(16);
    m_examine_str = (char*)AllocateMemory(16);
    m_pickup_str = (char*)AllocateMemory(16);
    m_unknown_str = (char*)AllocateMemory(16);

    strcpy(m_pointer_str, "POINTER");
    strcpy(m_examine_str, "EXAMINE");
    strcpy(m_pickup_str, "PICKUP");
    strcpy(m_unknown_str, "");
}

/* Function start: 0x41EE30 */
Mouse::~Mouse()
{
    if (m_fields[70] != 0) {
        FUN_0041e470(m_fields[70]);
        FreeMemory(m_fields[70]);
        m_fields[70] = 0;
    }

    if (m_fields[30] != 0) {
        SpriteArray_Cleanup(m_fields[30]);
        FreeMemory(m_fields[30]);
        m_fields[30] = 0;
    }

    if (m_fields[31] != 0) {
        SpriteArray_Cleanup(m_fields[31]);
        FreeMemory(m_fields[31]);
        m_fields[31] = 0;
    }

    for (int i = 0; i < 0x19; i++) {
        if (m_fields[i + 34] != 0) {
            FreeMemory(m_fields[i + 34]);
            m_fields[i + 34] = 0;
        }
    }

    CleanupFields();
    FUN_0041ef47();
}

/* Function start: 0x41EF25 */
void Mouse::CleanupFields()
{
    Array_Cleanup(&m_fields[61], 8, 0x19, (void*)0x405770);
}

/* Function start: 0x41EF50 */
int Mouse::LBLParse(char* line)
{
    char local_60[32];
    char local_40[32];
    int local_20;
    int local_1c;
    Sprite* local_18;
    int local_14;

    try {
        sscanf(line, "%s", local_40);

        if (strcmp(local_40, "AUDIO") == 0) {
            sscanf(line, " %s %s", local_40, local_60);
            local_18 = (Sprite*)AllocateMemory(0x10);
            void* this_00 = 0;
            if (local_18 != 0) {
                this_00 = FUN_0041e460((void*)local_18);
            }
            m_fields[73] = (char*)this_00;
            if (this_00 != 0) {
                FUN_0041e490(this_00, local_60);
            }
        } else if (strcmp(local_40, "SPRITE") == 0) {
            local_18 = (Sprite*)AllocateMemory(0xd8);
            Sprite* pSVar4 = 0;
            if (local_18 != 0) {
                new (local_18) Sprite((char*)0);
                pSVar4 = local_18;
            }
            m_fields[21] = (char*)pSVar4;
            pSVar4->flags &= ~2;
            Parser::ProcessFile(pSVar4, this, 0);
        } else if (strcmp(local_40, "HOTPIXEL") == 0) {
            sscanf(line, " %s %d %d %d", local_40, &local_14, &local_20, &local_1c);
            *(int*)&m_fields[23 + local_14 * 2] = local_20;
            *(int*)&m_fields[24 + local_14 * 2] = local_1c;
        } else if (strcmp(local_40, "LABLE") == 0) {
            sscanf(line, " %s %d %s", local_40, &local_14, local_60);
            char* dest = 0;
            if (local_14 == 0) dest = m_pointer_str;
            else if (local_14 == 1) dest = m_examine_str;
            else if (local_14 == 2) dest = m_pickup_str;
            else if (local_14 == 3) dest = m_unknown_str;

            if (dest) FUN_00425fd0(dest, local_60, 0x10);
        } else if (strcmp(local_40, "END") == 0) {
            m_fields[22] = 0;
            return 1;
        } else {
            Parser::LBLParse(line);
        }

        return 0;
    } catch (...) {
        return 0;
    }
}

/* Function start: 0x41F200 */
void Mouse::DrawCursor()
{
    Sprite* sprite = *(Sprite**)((char*)this + 0xec);
    if (sprite == 0) {
        ShowError("missing mouse ");
    }

    int* mouse_coords = *(int**)(DAT_00436968 + 0x1a0);

    int final_x = 0;
    if (mouse_coords) {
        final_x = mouse_coords[0];
    }

    if (sprite) {
        final_x -= *(int*)((char*)this + sprite->field_0x90 * 8 + 0xf4);
    }

    int final_y = 0;
    if (mouse_coords) {
        final_y = mouse_coords[1];
    }

    if (sprite) {
        final_y -= *(int*)((char*)this + sprite->field_0x90 * 8 + 0xf8);
    } else {
        final_y -= *(int*)((char*)this + 0xf0);
    }

    sprite->Do(final_x, final_y, 0, 0x3ff00000);
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
