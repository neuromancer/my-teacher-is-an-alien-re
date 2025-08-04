#include "SCI_AfterSchoolMenu.h"
#include "Parser.h"
#include <string.h>
#include <stdio.h>

extern "C" {
    void OpMenu__GetOptionQ(void*, int);
    void OpMenu__SetOptionState(void*, int, int);
    void ShowError(const char*);
    void* AllocateMemory_Wrapper(int);
    void* MouseControl_Constructor(void*);
    void* CreatePaletteBuffer(void*);
    char* FormatFilePath(char*);
    void FUN_0041eab0(void*, char*);
    void* HotspotManager_Init(void*);
    void* OptionMenu__Init(void*);
}

/* Function start: 0x405C80 */
void SCI_AfterSchoolMenu::FillOptionQueue()
{
    if (this->field_0x66c >= 0 && this->field_0x66c <= 2) {
        OpMenu__GetOptionQ(this, this->field_0x66c);
        OpMenu__SetOptionState(this, -5, 0);
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
        this->mouseControl = MouseControl_Constructor(AllocateMemory_Wrapper(0x98));
        ((Parser*)this->mouseControl)->Copy(this);
    } else if (strcmp(local_34, "PALETTE") == 0) {
        sscanf(param_1, "%s %s", local_34, local_74);
        if (this->paletteBuffer == 0) {
            this->paletteBuffer = CreatePaletteBuffer(AllocateMemory_Wrapper(8));
            char* path = FormatFilePath(local_74);
            FUN_0041eab0(this->paletteBuffer, path);
        }
    } else if (strcmp(local_34, "HOTSPOT") == 0) {
        this->hotspotManagers[this->field_0x69c] = HotspotManager_Init(AllocateMemory_Wrapper(0x140));
        ((Parser*)this->hotspotManagers[this->field_0x69c])->Copy(this);
        this->field_0x69c++;
    } else if (strcmp(local_34, "CANCEL") == 0) {
        this->hotspotManager2 = HotspotManager_Init(AllocateMemory_Wrapper(0x140));
        ((Parser*)this->hotspotManager2)->Copy(this);
    } else if (strcmp(local_34, "OPTION_MENU") == 0) {
        this->optionMenu = (OptionMenu*)OptionMenu__Init(AllocateMemory_Wrapper(0xa0));
        ((Parser*)this->optionMenu)->Copy(this);
    } else if (strcmp(local_34, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse(param_1);
    }

    return 0;
}
