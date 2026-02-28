#ifndef SC_CINEMATIC_H
#define SC_CINEMATIC_H

#include "Handler.h"
#include <string.h>

// SC_Cinematic - Cinematic/cutscene player (case 3)
// Constructor: 0x42FBD0
// Destructor: 0x42FC90
// Size: 0xD8
// Vtable: 0x461648
// References: "CINE_SUMMARY"
class SC_Cinematic : public Handler {
public:
    SC_Cinematic();
    ~SC_Cinematic();

    void CleanUp(int param);  // 0x4300D0

    int field_A8;       // 0xA8
    int field_AC;       // 0xAC
    int field_B0;       // 0xB0
    int field_B4;       // 0xB4
    int field_B8;       // 0xB8
    int field_BC;       // 0xBC
    int field_C0;       // 0xC0
    int field_C4;       // 0xC4
    int field_C8;       // 0xC8
    int field_CC;       // 0xCC
    int field_D0;       // 0xD0
    int field_D4;       // 0xD4
};

#endif // SC_CINEMATIC_H
