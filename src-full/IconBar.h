#ifndef ICONBAR_H
#define ICONBAR_H

#include "Handler.h"

class SC_Message;
class Sprite;
class SpriteAction;

// IconBarEntry - 0x24 byte (36 byte) struct for icon bar button entries
// Static array at 0x473320 (6 entries)
// Constructor: 0x42E330
struct IconBarEntry {
    Sprite* sprite;        // 0x00
    int left;              // 0x04
    int top;               // 0x08
    int right;             // 0x0C
    int bottom;            // 0x10
    int field_14;          // 0x14
    int field_18;           // 0x18
    SpriteAction* slot0;   // 0x1C
    SpriteAction* slot1;   // 0x20

    IconBarEntry();                                    // 0x42E330
    ~IconBarEntry();
    void RegisterSlot(SpriteAction* obj);              // 0x42E450
    void SetSlotProperties(int p1, int p2);            // 0x42E480
    void CreateAction(int p1, int p2, int p3);         // 0x42E3A0
};

// IconBar - Base class for handlers with icon bar UI (full game)
// Size: 0xA8 (same as Handler - no additional member fields in full game)
// vtable: 0x4615D0
// Constructor: 0x42D460
// In the full game, icon bar data is stored in static/global variables
class IconBar : public Handler {
public:
    IconBar();
    virtual ~IconBar();

    // Virtual overrides
    virtual int LBLParse(char* param);             // 0x42E010
    virtual void Init(SC_Message* msg);            // 0x42D6E0
    virtual int AddMessage(SC_Message* msg);       // 0x42D9A0
    virtual int ShutDown(SC_Message* msg);         // 0x42D920
    virtual void Update(int param1, int param2);   // 0x42DD40
    virtual int Exit(SC_Message* msg);             // 0x42DD30

    // Non-virtual aliases used by derived classes to call IconBar base
    // implementation directly (bypassing virtual dispatch).
    // Always inlined at call sites in the original binary — no separate
    // function body exists. Confirmed by assembly in callers:
    //   SCI_SchoolMenu::Init  → CALL 0x42D6E0 (IconBar::Init)
    //   SCI_Inventory::ShutDown → CALL 0x42D920 (IconBar::ShutDown)
    //   SCI_SchoolMenu::AddMessage → CALL 0x42D9A0 (IconBar::AddMessage)
    void InitIconBar(SC_Message* msg) { IconBar::Init(msg); }
    void CleanupIconBar(SC_Message* msg) { IconBar::ShutDown(msg); }
    int CheckButtonClick(SC_Message* msg) { return IconBar::AddMessage(msg); }
    void SetIconBarRect();                         // 0x42D340
    int FindClickedEntry(int* param);              // 0x42DEC0
    void PlayButtonSound(int buttonIndex);         // 0x42DF10
    void UpdateAllSlots();                         // 0x42DFA0
};

#endif // ICONBAR_H
