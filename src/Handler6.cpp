#include "Handler6.h"
#include <string.h>
#include <stdio.h>
#include "SpriteList.h"
#include "Sprite.h"
#include "Memory.h"
#include "Parser.h"
#include "globals.h"
#include "SC_Question.h"
#include "Message.h"
#include "Mouse.h"
#include "InputManager.h"

// External functions used by Handler6
extern "C" void __cdecl WriteToMessageLog(const char*);

/* Function start: 0x4044C0 */
Handler6::Handler6() {
    // Parent constructor IconBar::IconBar() is called automatically
    // This sets up all the parent class fields (icon bar sprites, buttons, etc.)

    // Zero 0x10 (16) dwords at offset 0x600 (0x40 bytes)
    // EDI pointed to this+0x600, then rep stosd with ECX=0x10
    memset(&palette, 0, 0x40);

    // Set handlerId at offset 0x88 to 6
    handlerId = 6;
}

/* Function start: 0x404560 */
Handler6::~Handler6() {
    // Set vtable back to Handler6 vtable (for destructor dispatch)
    // This is done automatically by C++

    // Cleanup palette at 0x600
    // Cleanup palette at 0x600
    if (palette != 0) {
        delete palette;
        palette = 0;
    }

    // Cleanup ambient at 0x604
    if (ambient != 0) {
        delete ambient;
        ambient = 0;
    }

    // Cleanup hotspots array at 0x608
    for (int i = 0; i < 10; i++) {
        if (hotspots[i] != 0) {
            delete hotspots[i];
            hotspots[i] = 0;
        }
    }

    // Parent destructor will be called by ~Parser
}

/* Function start: 0x404640 */
void Handler6::Init(SC_Message* msg) {
    char filePath[32];
    char searchScreenLabel[32];
    char staticLabel[32];
    char periodLabel[32];

    // Log entry
    WriteToMessageLog("\nENTER SEARCH SCREEN");

    // Call parent init (IconBar::InitIconBar)
    InitIconBar(msg);

    // Format the script file path and labels
    sprintf(filePath, "mis\\room%3.3d.mis", 5);
    sprintf(searchScreenLabel, "[SEARCH_SCREEN%d]", msg->sourceAddress);
    sprintf(staticLabel, "[STATIC]");
    sprintf(periodLabel, "[PERIOD%2.2d]", 1);

    // Log labels
    WriteToMessageLog(searchScreenLabel);
    WriteToMessageLog(staticLabel);
    WriteToMessageLog(periodLabel);

    // Parse static section
    ParseFile(this, filePath, staticLabel);

    // Log and parse period section
    WriteToMessageLog("Finished getting static, now get specific");
    ParseFile(this, filePath, periodLabel);

    // Count active hotspots
    activeHotspots = CountActiveHotspots();

    // Set palette if needed
    if (palette != 0) {
        int* palettePtr = (int*)((char*)g_ZBufferManager_0043698c + 0xa8);
        if (*palettePtr != 0) {
            WriteToMessageLog("ddouble palette");
        }
        *palettePtr = (int)palette;
    }
}

/* Function start: 0x404810 */
int Handler6::HandleMessage(SC_Message* msg) {
    // Call parent handler (IconBar::CheckButtonClick)
    int result = CheckButtonClick(msg);
    if (result != 0) {
        return 1;
    }

    // Check if button 2 was pressed (mouseX >= 2) and no hotspot currently selected
    if (msg->mouseX >= 2 && currentHotspot == 0) {
        int clickedIndex = FindClickedHotspot();
        if (clickedIndex > -1) {
            currentHotspot = hotspots[clickedIndex];
        }
    }
    return 1;
}

/* Function start: 0x404760 */
int Handler6::Update(SC_Message* msg) {
    // Cleanup palette at 0x600
    if (palette != 0) {
        delete palette;
        palette = 0;
    }

    // Cleanup ambient at 0x604
    if (ambient != 0) {
        delete ambient;
        ambient = 0;
    }

    // Cleanup hotspots array at 0x608
    Hotspot** ptr = hotspots;
    int remaining = 10;
    do {
        Hotspot* hs = *ptr;
        if (hs != 0) {
            delete hs;
            *ptr = 0;
        }
        ptr++;
        remaining--;
    } while (remaining != 0);

    // Reset counters
    hotspotCount = 0;
    activeHotspots = 0;
    counter = 0;

    // Call parent cleanup (IconBar::CleanupIconBar)
    CleanupIconBar();

    // Log message
    WriteToMessageLog("EXIT SEARCH SCREEN\n");
    return 0;
}

/* Function start: 0x404890 */
void Handler6::Draw(int param1, int param2) {
    // Check if this is for us
    if (handlerId != param2) {
        return;
    }

    // Call parent draw (IconBar::DrawIconBar)
    DrawIconBar(param1, param2);

    // Draw ambient
    if (ambient) ambient->DoAll();
    
    // Handle hotspot animation
    if (currentHotspot != 0) {
        int result = currentHotspot->Draw_40d610();
        if (result == 0) {
            return;
        }
        currentHotspot = 0;
        activeHotspots = CountActiveHotspots();
        return;
    }
    
    // Check if there are active hotspots
    if (activeHotspots != 0) {
        // Has active hotspots - check for clicked hotspot
        int clickedIndex = FindClickedHotspot();
        Sprite* sprite = g_Mouse_00436978->m_sprite;

        if (clickedIndex == -1) {
            if (sprite != 0) {
                sprite->SetState2(0);
            }
        } else {
            if (sprite != 0) {
                sprite->SetState2(1);
            }
        }
        g_Mouse_00436978->DrawCursor();
        return;
    }
    
    // No active hotspots - increment counter
    counter++;
    g_Mouse_00436978->DrawCursor();
    if (counter != 0x3c) {
        return;
    }
    // Send message after 60 frames (0x3c = 60)
    SC_Message_Send(10, 1, 0xb, 1, 5, 0, 0, 0, 0, 0);
}

/* Function start: 0x404870 */
int Handler6::Exit(SC_Message* msg) {
    // Compare handlerId == msg->command
    return handlerId == msg->command;
}

/* Function start: 0x4049F0 */
int Handler6::CountActiveHotspots() {
    int count = 0;
    Hotspot** ptr = hotspots;
    int remaining = 10;

    do {
        Hotspot* hotspot = *ptr;
        if (hotspot != 0 && hotspot->field_D0 != 0) {
            count++;
        }
        ptr++;
        remaining--;
    } while (remaining != 0);

    return count;
}

/* Function start: 0x404970 */
int Handler6::FindClickedHotspot() {
    int index = 0;
    Hotspot** ptr = hotspots;

    do {
        Hotspot* hotspot = *ptr;
        if (hotspot != 0) {
            int mouseY = 0;
            InputState* pMouse = g_InputManager_00436968->pMouse;

            if (pMouse != 0) {
                mouseY = pMouse->y;
            }

            int mouseX;
            if (pMouse != 0) {
                mouseX = pMouse->x;
            } else {
                mouseX = 0;
            }

            int inBounds;
            if (hotspot->field_D0 == 0 ||
                hotspot->field_E0 > mouseX ||
                hotspot->field_E8 < mouseX ||
                hotspot->field_E4 > mouseY ||
                hotspot->field_EC < mouseY) {
                inBounds = 0;
            } else {
                inBounds = 1;
            }

            if (inBounds != 0) {
                return index;
            }
        }
        ptr++;
        index++;
    } while (index < 10);

    return -1;
}
