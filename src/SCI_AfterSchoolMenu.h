#ifndef SCI_AFTERSCHOOLMENU_H
#define SCI_AFTERSCHOOLMENU_H

#include "Parser.h"
#include "OptionMenu.h"

class SCI_AfterSchoolMenu : public Parser {
public:
    // ... (other fields)
    char pad_88[0x578];
    void* paletteBuffer; // 0x600
    void* mouseControl; // 0x604
    OptionMenu* optionMenu; // 0x608
    void* hotspotManager1; // 0x60c
    void* hotspotManager2; // 0x610
    void* hotspotManagers[10]; // 0x614
    char pad_63c[0x20];
    int field_0x66c;
    char pad_670[0x24];
    int field_0x694;
    char pad_698[0x4];
    int field_0x69c;


public:
    void FillOptionQueue();
    virtual int LBLParse(char*);
};

#endif // SCI_AFTERSCHOOLMENU_H
