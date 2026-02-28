#ifndef SC_SPACESHIPNAV_H
#define SC_SPACESHIPNAV_H

#include "SC_Combat.h"

// SC_SpaceShipNav - Spaceship navigation handler (case 70/0x46)
// Constructor: 0x447520
// Size: 0x120
// Vtable: 0x4619D8
// Extends SC_Combat (0x118)
// References: "NAVIGATION", "SC_SpaceShipNav::Process_Action"
class SC_SpaceShipNav : public SC_Combat {
public:
    SC_SpaceShipNav();
    ~SC_SpaceShipNav();

    int field_118[2];   // 0x118-0x11F (0x08 bytes)
};

#endif // SC_SPACESHIPNAV_H
