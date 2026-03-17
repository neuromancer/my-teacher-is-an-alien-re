#ifndef SCI_SEARCHSCREEN_H
#define SCI_SEARCHSCREEN_H

#include "SC_Combat.h"

class SC_Message;

// SCI_SearchScreen - Search screen handler (full game: case 72/0x48)
// Constructor: 0x40B420
// Vtable: 0x461178
// Size: 0x118 (same as SC_Combat, no additional fields)
// Extends SC_Combat (0x118 bytes)
//
// NOTE: In the demo, this was case 11/0x0B extending IconBar (0x648 bytes).
//       The full game completely redesigned this class.
//
// field_A8[0..N] used by ANNOUNCE labels (indexed stores)
// field_A8[20..N] used by ENDPERIODANNOUNCE labels (offset 0xF8 = 0xA8 + 0x50)
class SCI_SearchScreen : public SC_Combat {
public:
    SCI_SearchScreen();                         // 0x40B420
    ~SCI_SearchScreen();                        // 0x40B4B0

    virtual int LBLParse(char* line);           // 0x40B25E (body), 0x40B240 (SEH entry)
};

#endif // SCI_SEARCHSCREEN_H
