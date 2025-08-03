#include "Mouse.h"
#include "Sprite.h"
#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"

extern "C" {
    void FreeFromGlobalHeap(void*);
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
        FreeFromGlobalHeap(m_fields[70]);
        m_fields[70] = 0;
    }

    if (m_fields[30] != 0) {
        SpriteArray_Cleanup(m_fields[30]);
        FreeFromGlobalHeap(m_fields[30]);
        m_fields[30] = 0;
    }

    if (m_fields[31] != 0) {
        SpriteArray_Cleanup(m_fields[31]);
        FreeFromGlobalHeap(m_fields[31]);
        m_fields[31] = 0;
    }

    for (int i = 0; i < 0x19; i++) {
        if (m_fields[i + 34] != 0) {
            FreeFromGlobalHeap(m_fields[i + 34]);
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
    try {
        char local_60[32];
        char local_40[32];
        int local_20;
        int local_1c;
        Sprite* local_18;
        int local_14;

        sscanf(line, "%s", local_40);

        if (_stricmp(local_40, "SPRITE") == 0) {
            sscanf(line, "%s %s", local_40, local_60);
            local_18 = (Sprite*)AllocateMemory(0x10);
            void* this_00 = 0;
            if (local_18 != 0) {
                this_00 = FUN_0041e460((void*)local_18);
            }
            m_fields[71] = (char*)this_00;
            if (this_00 != 0) {
                FUN_0041e490(this_00, local_60);
            }
        } else if (_stricmp(local_40, "SPRITE") == 0) {
            local_18 = (Sprite*)AllocateMemory(0xd8);
            Sprite* pSVar4 = 0;
            if (local_18 != 0) {
                local_18->Sprite::Sprite((char*)0);
                pSVar4 = local_18;
            }
            m_fields[59] = (char*)pSVar4;
            pSVar4->flags &= ~2;
            pSVar4->Init();
        } else if (_stricmp(local_40, "HOTPIXEL") == 0) {
            sscanf(line, "%s %d %d %d", local_40, &local_14, &local_20, &local_1c);
            *(int*)&m_fields[local_14 * 2 + 61] = local_20;
            *(int*)&m_fields[local_14 * 2 + 62] = local_1c;
        } else if (_stricmp(local_40, "LABLE") == 0) {
            sscanf(line, "%s %d %s", local_40, &local_14, local_60);
            FUN_00425fd0(m_fields[local_14 + 34], local_60, 0x10);
        } else if (_stricmp(local_40, "END") == 0) {
            m_fields[60] = 0;
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
    if (m_fields[59] == 0) {
        ShowError("missing mouse");
    }

    int mouse_x = 0;
    int* mouse_coords = *(int**)(DAT_00436968 + 0x1a0);
    if (mouse_coords != 0) {
        mouse_x = *mouse_coords;
    }

    Sprite* sprite = (Sprite*)m_fields[59];
    int hotspot_x = 0;
    if (sprite != 0) {
        hotspot_x = *(int*)&m_fields[sprite->handle * 2 + 61];
    }

    int mouse_y = 0;
    if (mouse_coords != 0) {
        mouse_y = mouse_coords[1];
    }

    int hotspot_y = 0;
    if (sprite != 0) {
        hotspot_y = *(int*)&m_fields[sprite->handle * 2 + 62];
    } else {
        hotspot_y = *(int*)m_fields[60];
    }

    sprite->Do(mouse_x - hotspot_x, mouse_y - hotspot_y, 0, 0x3ff00000);
}
