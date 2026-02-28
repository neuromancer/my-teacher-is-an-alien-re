#ifndef ICONBAR_H
#define ICONBAR_H

#include "Handler.h"

class SC_Message;
class Sprite;

// IconBar - Base class for handlers with icon bar UI (full game)
// Size: 0xA8 (same as Handler - no additional member fields in full game)
// vtable: 0x4615D0
// Constructor: 0x42D460
// In the full game, icon bar data is stored in static/global variables
class IconBar : public Handler {
public:
    IconBar();
    virtual ~IconBar();

    // Methods (implementations in IconBar.cpp)
    void InitIconBar(SC_Message* msg);
    void CleanupIconBar(SC_Message* msg);
    int CheckButtonClick(SC_Message* msg);
    virtual void Update(int param1, int param2);
    void PlayButtonSound(int buttonIndex);
};

#endif // ICONBAR_H
