#include "SearchScreen.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    void FUN_004191d0(char*);
    void* FUN_0041ea50(void*);
    char* FUN_004195c0(char*);
    void FUN_0041eab0(void*, char*);
    void* FUN_0041f280(void*);
    void* FUN_0040d300(void*);
}

/* Function start: 0x404A20 */
int SearchScreen::LBLParse(char* line)
{
    char key[32];
    char value[64];

    sscanf(line, "%s", key);
    FUN_004191d0(line);

    if (_strcmpi(key, "PALETTE") == 0) {
        sscanf(line, "%*s %s", value);
        if (this->field_600 == 0) {
            void* mem = AllocateMemory(8);
            void* palette = 0;
            if (mem) {
                palette = FUN_0041ea50(mem);
            }
            this->field_600 = palette;
            char* path = FUN_004195c0(value);
            FUN_0041eab0(palette, path);
        }
    }
    else if (_strcmpi(key, "AMBIENT") == 0) {
        if (this->field_604 == 0) {
            void* mem = AllocateMemory(0x98);
            void* obj = 0;
            if (mem) {
                obj = FUN_0041f280(mem);
            }
            this->field_604 = obj;
        }
        Parser::ProcessFile((Parser*)this->field_604, this, 0);
    }
    else if (_strcmpi(key, "HOTSPOT") == 0) {
        void* mem = AllocateMemory(0xf0);
        void* obj = 0;
        if (mem) {
            obj = FUN_0040d300(mem); // T_Hotspot constructor?
        }
        this->field_608[this->field_634] = obj;
        Parser::ProcessFile((Parser*)obj, this, 0);
        this->field_634++;
    }
    else if (_strcmpi(key, "END") == 0) {
        return 1;
    }
    else {
        return Parser::LBLParse("SearchScreen");
    }

    return 0;
}
