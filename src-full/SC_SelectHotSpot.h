#ifndef SC_SELECTHOTSPOT_H
#define SC_SELECTHOTSPOT_H

#include "Handler.h"
#include <string.h>

class SC_SelectHotSpot;
struct LinkedList;

// SelectHotspot - Individual clickable area in SC_SelectHotSpot menu
// Constructor: 0x404FB0
// Size: 0xC8
// Vtable: 0x461050
// Extends Parser directly (NOT Handler)
class SelectHotspot : public Parser {
public:
    SelectHotspot(int key);
    ~SelectHotspot();                   // 0x405060
    void Draw();                        // 0x405250
    void Update();                      // 0x405260
    int OnClick(SC_Message* msg);       // 0x405490
    int LBLParse(char* line);           // 0x405530

    int state;              // 0x90 - 0=idle, 1=normal, 2=hover, 3=selected
    int x1;                 // 0x94
    int y1;                 // 0x98
    int x2;                 // 0x9C
    int y2;                 // 0xA0
    int keyCode;            // 0xA4
    int sortKey;            // 0xA8
    void* sprite;           // 0xAC - Sprite*
    void* rollonSound;      // 0xB0 - Sample*
    void* selectedSound;    // 0xB4 - Sample*
    void* rolloffSound;     // 0xB8 - Sample*
    LinkedList* messageList;// 0xBC
    SC_SelectHotSpot* parent; // 0xC0
    int field_C4;           // 0xC4
};

// SC_SelectHotSpot - Hotspot selection handler (case 45/0x2D)
// Constructor: 0x405C30
// Size: 0xC0
// Vtable: 0x461068
// References: "SC_SelectHotSpot::Update()"
class SC_SelectHotSpot : public Handler {
public:
    SC_SelectHotSpot();
    ~SC_SelectHotSpot();

    void Init(SC_Message* msg);         // 0x405FB0
    int AddMessage(SC_Message* msg);    // 0x4063A0
    int ShutDown(SC_Message* msg);      // 0x406240
    void Update(int param1, int param2);// 0x4062C0
    int Exit(SC_Message* msg);          // 0x4063E0
    void OnInput(void* param);          // 0x405D60
    void OnProcessStart();              // 0x4064A0
    int LBLParse(char* line);           // 0x4065E0

    int field_A8;           // 0xA8 - intro animation played flag
    void* ambient;          // 0xAC - Ambient*
    void* introSprite;      // 0xB0 - Sprite*
    void* currentHotspot;   // 0xB4 - temp SelectHotspot* during parsing
    void* palette;          // 0xB8 - Palette*
    LinkedList* hotspotList;// 0xBC
};

#endif // SC_SELECTHOTSPOT_H
