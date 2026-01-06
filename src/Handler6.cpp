#include "Handler6.h"
#include <string.h>
#include <stdio.h>

// External parent constructor - this is the parent class constructor at 0x402730
extern "C" void __fastcall FUN_00402730(void*);

// External functions used by Handler6
extern "C" void __fastcall FUN_0041ea80(void*);  // Palette cleanup
extern "C" void __cdecl FUN_00424940(void*);     // FreeMemory
extern "C" void __fastcall FUN_0041f360(void*);  // Ambient cleanup
extern "C" void __fastcall FUN_0040d3a0(void*);  // Hotspot cleanup
extern "C" void __fastcall FUN_00402fd0(void*, void*);  // Parent Update
extern "C" void __cdecl WriteToMessageLog(const char*);
extern "C" void __fastcall FUN_00403230(void*, int, int);  // Parent Draw
extern "C" void __fastcall FUN_0041f800(void*);             // Ambient::Draw
extern "C" int __fastcall FUN_0040d610(void*);              // Hotspot::Draw
extern "C" void __fastcall FUN_0041f200(void*);             // Draw update
extern "C" void __fastcall FUN_0041d190(void*, int);        // Sprite::SetState
extern "C" void __cdecl SC_Message_Send(int, int, int, int, int, int, int, int, int, int);
extern "C" int __fastcall FUN_00403040(void*, void*);  // Parent HandleMessage
extern "C" void __fastcall FUN_00402ed0(void*, void*);  // Parent Init
extern "C" void __fastcall FUN_00418d60(void*, char*, char*);  // Parse script section

// Global variables
extern void* g_GameLoop_00436978;
extern void* g_Renderer_0043698c;

/* Function start: 0x4044C0 */
Handler6::Handler6() {
    // Call parent constructor at 0x402730
    // This sets up all the parent class fields  
    FUN_00402730(this);

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
    if (palette != 0) {
        // FUN_0041ea80 - destructor for palette-like object
        // FUN_00424940 - FreeMemory
        palette = 0;
    }

    // Cleanup ambient at 0x604
    if (ambient != 0) {
        // FUN_0041f360 - destructor for ambient-like object
        // FUN_00424940 - FreeMemory
        ambient = 0;
    }

    // Cleanup hotspots array at 0x608
    for (int i = 0; i < 10; i++) {
        if (hotspots[i] != 0) {
            // FUN_0040d3a0 - destructor for hotspot-like object
            // FUN_00424940 - FreeMemory
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
    
    // Call parent init
    FUN_00402ed0(this, msg);
    
    // Format the script file path and labels
    sprintf(filePath, "mis\\room%3.3d.mis", 5);
    sprintf(searchScreenLabel, "[SEARCH_SCREEN%d]", *((int*)((char*)msg + 0x8c)));
    sprintf(staticLabel, "[STATIC]");
    sprintf(periodLabel, "[PERIOD%2.2d]", 1);
    
    // Log labels
    WriteToMessageLog(searchScreenLabel);
    WriteToMessageLog(staticLabel);
    WriteToMessageLog(periodLabel);
    
    // Parse static section
    FUN_00418d60(this, filePath, staticLabel);
    
    // Log and parse period section
    WriteToMessageLog("Finished getting static, now get specific");
    FUN_00418d60(this, filePath, periodLabel);
    
    // Count active hotspots
    activeHotspots = CountActiveHotspots();
    
    // Set palette if needed
    if (palette != 0) {
        int* palettePtr = (int*)((char*)g_Renderer_0043698c + 0xa8);
        if (*palettePtr != 0) {
            WriteToMessageLog("ddouble palette");
        }
        *palettePtr = (int)palette;
    }
}

/* Function start: 0x404810 */
int Handler6::HandleMessage(SC_Message* msg) {
    // Call parent handler
    int result = FUN_00403040(this, msg);
    if (result != 0) {
        return 1;
    }
    
    // Check if button 2 was pressed (msg+0xac >= 2) and no hotspot currently selected
    if (*((int*)((char*)msg + 0xac)) >= 2 && currentHotspot == 0) {
        int clickedIndex = FindClickedHotspot();
        if (clickedIndex > -1) {
            currentHotspot = hotspots[clickedIndex];
        }
    }
    return 1;
}

/* Function start: 0x404760 */
void Handler6::Update(SC_Message* msg) {
    // Cleanup palette at 0x600
    if (palette != 0) {
        FUN_0041ea80(palette);
        FUN_00424940(palette);
        palette = 0;
    }
    
    // Cleanup ambient at 0x604
    if (ambient != 0) {
        FUN_0041f360(ambient);
        FUN_00424940(ambient);
        ambient = 0;
    }
    
    // Cleanup hotspots array at 0x608
    void** ptr = hotspots;
    int remaining = 10;
    do {
        void* hs = *ptr;
        if (hs != 0) {
            FUN_0040d3a0(hs);
            FUN_00424940(hs);
            *ptr = 0;
        }
        ptr++;
        remaining--;
    } while (remaining != 0);
    
    // Reset counters
    hotspotCount = 0;
    activeHotspots = 0;
    counter = 0;
    
    // Call parent Update
    FUN_00402fd0(this, msg);
    
    // Log message
    WriteToMessageLog("EXIT SEARCH SCREEN\n");
}

/* Function start: 0x404890 */
void Handler6::Draw(int param1, int param2) {
    // Check if this is for us
    if (handlerId != param2) {
        return;
    }
    
    // Call parent draw
    FUN_00403230(this, param1, param2);
    
    // Draw ambient  
    FUN_0041f800(ambient);
    
    // Handle hotspot animation
    if (currentHotspot != 0) {
        int result = FUN_0040d610(currentHotspot);
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
        void* sprite = *(void**)((char*)g_GameLoop_00436978 + 0xec);
        
        if (clickedIndex == -1) {
            if (sprite != 0) {
                FUN_0041d190(sprite, 0);
            }
        } else {
            if (sprite != 0) {
                FUN_0041d190(sprite, 1);
            }
        }
        FUN_0041f200(g_GameLoop_00436978);
        return;
    }
    
    // No active hotspots - increment counter
    counter++;
    FUN_0041f200(g_GameLoop_00436978);
    if (counter != 0x3c) {
        return;
    }
    // Send message after 60 frames (0x3c = 60)
    SC_Message_Send(10, 1, 0xb, 1, 5, 0, 0, 0, 0, 0);
}

/* Function start: 0x404870 */
int Handler6::Exit(SC_Message* msg) {
    // Simple comparison - return 1 if handlerIds match, 0 otherwise
    int msgHandlerId = *((int*)((char*)msg + 0x88));
    return (handlerId == msgHandlerId) ? 1 : 0;
}

/* Function start: 0x4049F0 */
int Handler6::CountActiveHotspots() {
    int count = 0;
    void** ptr = hotspots;
    int remaining = 10;

    do {
        void* hotspot = *ptr;
        if (hotspot != 0 && *((int*)((char*)hotspot + 0xd0)) != 0) {
            count++;
        }
        ptr++;
        remaining--;
    } while (remaining != 0);

    return count;
}

/* Function start: 0x404970 */
int Handler6::FindClickedHotspot() {
    extern void* g_InputManager_00436968;
    
    int index = 0;
    void** ptr = hotspots;
    int zero = 0;
    
    do {
        void* hotspot = *ptr;
        if (hotspot != 0) {
            int mouseY = 0;
            int* mouseData = *(int**)((char*)g_InputManager_00436968 + 0x1a0);
            int mouseX;
            
            if (mouseData == 0) {
                mouseX = 0;
            } else {
                mouseY = mouseData[1];
                if (mouseData == 0) {
                    mouseX = 0;
                } else {
                    mouseX = *mouseData;
                }
            }
            
            // Direct comparisons matching original
            if (*((int*)((char*)hotspot + 0xd0)) == zero) goto notInBounds;
            if (*((int*)((char*)hotspot + 0xe0)) > mouseX) goto notInBounds;
            if (*((int*)((char*)hotspot + 0xe8)) < mouseX) goto notInBounds;
            if (*((int*)((char*)hotspot + 0xe4)) > mouseY) goto notInBounds;
            if (*((int*)((char*)hotspot + 0xec)) < mouseY) goto notInBounds;
            
            return index;
            
        notInBounds:
            ;
        }
        ptr++;
        index++;
    } while (index < 10);
    
    return -1;
}
