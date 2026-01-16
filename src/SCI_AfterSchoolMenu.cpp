#include "SCI_AfterSchoolMenu.h"
#include "Parser.h"
#include "Palette.h"
#include "Memory.h"
#include "Hotspot.h"
#include <string.h>
#include <stdio.h>

extern "C" {
    void ShowError(const char*);
    void* MouseControl_Constructor(void*);
    char* FormatFilePath(char*);
}

/* Function start: 0x405C80 */
void SCI_AfterSchoolMenu::FillOptionQueue()
{
    if (field_0x66c >= 0 && field_0x66c <= 2) {
        optionMenu->SelectCharacter(field_0x66c);
        optionMenu->SetOptionState(-5, 0);
    } else {
        ShowError("Error in DMChoScr.cpp - FillOptionQueue");
    }
}

/* Function start: 0x405D20 */
int SCI_AfterSchoolMenu::LBLParse(char* param_1)
{
    char local_74[64];
    char local_34[32];

    sscanf(param_1, "%s", local_34);

    if (strcmp(local_34, "MOUSE") == 0) {
        mouseControl = MouseControl_Constructor(AllocateMemory_Wrapper(0x98));
        ((Parser*)mouseControl)->Copy(this);
    } else if (strcmp(local_34, "PALETTE") == 0) {
        sscanf(param_1, "%s %s", local_34, local_74);
        if (paletteBuffer == 0) {
            paletteBuffer = CreatePaletteBuffer((Palette*)AllocateMemory_Wrapper(8));
            char* path = FormatFilePath(local_74);
            ((Palette*)paletteBuffer)->Load(path);
        }
    } else if (strcmp(local_34, "HOTSPOT") == 0) {
        hotspotManagers[field_0x69c] = new HotspotManager();
        ((Parser*)hotspotManagers[field_0x69c])->Copy(this);
        field_0x69c++;
    } else if (strcmp(local_34, "CANCEL") == 0) {
        hotspotManager2 = new HotspotManager();
        ((Parser*)hotspotManager2)->Copy(this);
    } else if (strcmp(local_34, "OPTION_MENU") == 0) {
        optionMenu = new OptionMenu();
        ((Parser*)optionMenu)->Copy(this);
    } else if (strcmp(local_34, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SCI_AfterSchoolMenu");
    }

    return 0;
}
