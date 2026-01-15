#include "SearchScreen.h"
#include "Memory.h"
#include "Hotspot.h"
#include "Palette.h"
#include "MouseControl.h"
#include "string.h"

#include <stdio.h>
#include <string.h>

extern "C" {
    char* CDData_FormatPath(char*, ...);
}

/* Function start: 0x404A20 */
int SearchScreen::LBLParse(char* line)
{
    char value[64];
    char key[32];

    sscanf(line, "%s", key);
    WriteToMessageLog(line);

    if (strcmp(key, "PALETTE") == 0) {
        sscanf(line, "%*s %s", value);
        if (field_600 == 0) {
            Palette* palette = new Palette();
            field_600 = palette;
            char* path = CDData_FormatPath(value);
            palette->Load(path);
        }
    }
    else if (strcmp(key, "AMBIENT") == 0) {
        if (field_604 == 0) {
            MouseControl* mouseControl = new MouseControl();
            field_604 = mouseControl;
        }
        Parser::ProcessFile((Parser*)field_604, this, 0);
    }
    else if (strcmp(key, "HOTSPOT") == 0) {
        Hotspot* obj = new Hotspot();
        field_608[field_634] = obj;
        Parser::ProcessFile((Parser*)obj, this, 0);
        field_634++;
    }
    else if (strcmp(key, "END") == 0) {
        return 1;
    }
    else {
        return Parser::LBLParse("SearchScreen");
    }

    return 0;
}
