#include "SC_DuctNav.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include <string.h>

extern void __fastcall FUN_0044c740(void*);                        // Sprite dtor
extern void* __fastcall FUN_0041dbf0(void*, int, char*);           // Palette ctor with name
extern void* __fastcall FUN_00421a50(void*, int, char*, int*);     // Button ctor
extern void __cdecl FUN_0044d210(void*, int, int, int, int);       // Sprite animation setup

/* Function start: 0x43AF10 */
SC_DuctNav::SC_DuctNav()
{
}

#define DESTROY_BUTTON(slot) \
    { \
        int* _btn = (int*)(slot); \
        if (_btn != 0) { \
            _btn[0] = 0x4613D0; \
            void* _spr = (void*)_btn[0x24]; \
            if (_spr != 0) { \
                FUN_0044c740(_spr); \
                FreeMemory(_spr); \
                _btn[0x24] = 0; \
            } \
            FreeMemory(_btn); \
            (slot) = 0; \
        } \
    }

#define CREATE_BUTTON(slot, _name, _params) \
    { \
        void* _mem = operator new(0xA8); \
        void* _obj = 0; \
        if (_mem != 0) { \
            _obj = FUN_00421a50(_mem, 0, _name, _params); \
        } \
        (slot) = _obj; \
        FUN_0044d210(*(void**)((char*)_obj + 0x90), 1, 2, 10, 1); \
    }

/* Function start: 0x43D5B0 */
int SC_DuctNav::LBLParse(char* line)
{
    char label[32];
    char name[128];
    int slotIdx = 0;
    int params[4];
    params[0] = 0;
    params[1] = 0;
    params[2] = 0;
    params[3] = 0;

    sscanf(line, "%s", label);

    if (strcmp(label, "FONT") == 0) {
        sscanf(line, "%s %s", label, name);
        if (fontPalette != 0) {
            fontPalette->~Palette();
            FreeMemory(fontPalette);
            fontPalette = 0;
        }
        void* mem = operator new(8);
        Palette* pal = 0;
        if (mem != 0) {
            pal = (Palette*)FUN_0041dbf0(mem, 0, name);
        }
        fontPalette = pal;
    } else if (strcmp(label, "SPRITE") == 0) {
        if (menuSprite != 0) {
            FUN_0044c740(menuSprite);
            FreeMemory(menuSprite);
            menuSprite = 0;
        }
        menuSprite = new Sprite((char*)0);
        Parser::ProcessFile(menuSprite, this, (char*)0);
    } else if (strcmp(label, "CANCEL") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(cancelBtn)
        CREATE_BUTTON(cancelBtn, name, params)
    } else if (strcmp(label, "SAVE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(field_0x220)
        CREATE_BUTTON(field_0x220, name, params)
    } else if (strcmp(label, "LOAD") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(field_0x224)
        CREATE_BUTTON(field_0x224, name, params)
    } else if (strcmp(label, "OVERWRITE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(field_0x228)
        CREATE_BUTTON(field_0x228, name, params)
    } else if (strcmp(label, "DELETE") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(deleteBtn)
        CREATE_BUTTON(deleteBtn, name, params)
    } else if (strcmp(label, "SCROLLUP") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(scrollUpBtn)
        CREATE_BUTTON(scrollUpBtn, name, params)
    } else if (strcmp(label, "SCROLLDOWN") == 0) {
        sscanf(line, " %s %s %d %d %d %d", label, name, &params[0], &params[1], &params[2], &params[3]);
        DESTROY_BUTTON(scrollDownBtn)
        CREATE_BUTTON(scrollDownBtn, name, params)
    } else if (strcmp(label, "EDIT_FOCUS_SPRITE") == 0) {
        editFocusSprite = new Sprite((char*)0);
        Parser::ProcessFile(editFocusSprite, this, (char*)0);
    } else if (strcmp(label, "CHOICE_FOCUS_SPRITE") == 0) {
        choiceFocusSprite = new Sprite((char*)0);
        Parser::ProcessFile(choiceFocusSprite, this, (char*)0);
    } else if (strcmp(label, "SLOT") == 0) {
        sscanf(line, " %s %d %d %d %d %d", label, &slotIdx, &params[0], &params[1], &params[2], &params[3]);
        int* rect = &slotRects[slotIdx * 4];
        rect[0] = params[0];
        rect[1] = params[1];
        rect[2] = params[2];
        rect[3] = params[3];
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_DuctNav");
    }

    return 0;
}

SC_DuctNav::~SC_DuctNav()
{
}

void SC_DuctNav::OnProcessStart() {}
void SC_DuctNav::Init(SC_Message* msg) {}
int SC_DuctNav::AddMessage(SC_Message* msg) { return 0; }
int SC_DuctNav::ShutDown(SC_Message* msg) { return 0; }
void SC_DuctNav::Update(int p1, int p2) {}
int SC_DuctNav::Exit(SC_Message* msg) { return 0; }
