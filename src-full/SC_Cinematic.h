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

    void Init(SC_Message* msg);         // 0x42FCF0
    int ShutDown(SC_Message* msg);      // 0x4300D0
    void Update(int param1, int param2);// 0x430350
    int AddMessage(SC_Message* msg);    // 0x4306A0
    void EndCinematic();                // 0x430730

    int field_A8;       // 0xA8 - Palette*
    int field_AC;       // 0xAC - Animation*
    int field_B0;       // 0xB0 - saved state index
    int field_B4;       // 0xB4 - saved render context
    int field_B8;       // 0xB8 - flags
    int field_BC;       // 0xBC - start X
    int field_C0;       // 0xC0 - start Y
    int field_C4;       // 0xC4 - volume (default 100)
    int field_C8;       // 0xC8 - param
    int field_CC;       // 0xCC - loop/ended flag
    int field_D0;       // 0xD0 - Sprite*
    int field_D4;       // 0xD4
};

#endif // SC_CINEMATIC_H
