#include "Handler4.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "globals.h"
#include "SC_Question.h"
#include "Mouse.h"
#include "string.h"

// External functions - CDData_FormatPath at 0x4195c0
extern "C" char* __cdecl CDData_FormatPath(char*, ...);

/* Function start: 0x40E220 */
Handler4::Handler4() {
    int i;
    Sprite** spritePtr;
    Sprite* sprite;
    char* path;

    // Parent constructor IconBar::IconBar() is called automatically

    // Initialize rects to zero (at 0x69c) - handled by GlyphRect constructor

    // Zero 0x3c dwords at offset 0x600 (sprites, sounds, states)
    memset(&palette, 0, 0xf0);

    // Set handlerId to 4
    handlerId = 4;

    // Set initialized flag at 0x698
    needsUpdate = 1;

    // Initialize rect coordinates (from original disassembly)
    // rect1 at 0x69c: left=0, top=0, right=0x197, bottom=0x1aa
    rect1.right = 0x197;
    rect1.bottom = 0x1aa;
    // rect2 at 0x6ac: left=0x1d7, top=0x3b, right=0x25c, bottom=0xbb
    rect2.left = 0x1d7;
    rect2.top = 0x3b;
    rect2.right = 0x25c;
    rect2.bottom = 0xbb;
    // rect3 at 0x6bc: left=0x1de, top=0xf1, right=0x25b, bottom=0x16d
    rect3.left = 0x1de;
    rect3.top = 0xf1;
    rect3.right = 0x25b;
    rect3.bottom = 0x16d;
    // rect4 at 0x6cc: left=0x73, top=0x11d, right=0x12f, bottom=0x149
    rect4.left = 0x73;
    rect4.top = 0x11d;
    rect4.right = 0x12f;
    rect4.bottom = 0x149;
    // rect5 at 0x6dc: left=0x32, top=0x14a, right=0x172, bottom=0x192
    rect5.left = 0x32;
    rect5.top = 0x14a;
    rect5.right = 0x172;
    rect5.bottom = 0x192;

    // Create Palette
    palette = new Palette();
    path = CDData_FormatPath("puzzle1\\Puzztest.col");
    palette->Load(path);

    // Create sprites
    buttons1 = new Sprite("puzzle1\\buttons1.smk");
    buttons1->loc_x = 0x1dc;
    buttons1->loc_y = 0x147;

    buttons2 = new Sprite("puzzle1\\buttons2.smk");
    buttons2->loc_x = 0x1dc;
    buttons2->loc_y = 0x11c;

    buttons3 = new Sprite("puzzle1\\buttons3.smk");
    buttons3->loc_x = 0x1dc;
    buttons3->loc_y = 0xf1;

    paths1 = new Sprite("puzzle1\\paths1.smk");
    paths1->loc_x = 0x1ee;
    paths1->loc_y = 0x8d;

    paths2 = new Sprite("puzzle1\\paths2.smk");
    paths2->loc_x = 0x1ee;
    paths2->loc_y = 0x70;

    paths3 = new Sprite("puzzle1\\paths3.smk");
    paths3->loc_x = 0x1ee;
    paths3->loc_y = 0x53;

    // Initialize buttons and paths sprites
    spritePtr = &buttons1;
    for (i = 0; i < 3; i++) {
        sprite = spritePtr[i];
        sprite->flags &= ~2;
        sprite->priority = 10;
        sprite->InitLogic(3);
        sprite->SetRange(0, 1, 1);
        sprite->SetRange(1, 2, 2);
        sprite->SetRange(2, 3, 3);

        sprite = (&paths1)[i];
        sprite->flags &= ~2;
        sprite->priority = 10;
        sprite->InitLogic(3);
        sprite->SetRange(0, 1, 1);
        sprite->SetRange(1, 2, 2);
        sprite->SetRange(2, 3, 3);
    }

    // Create puzztest sprite
    puzztest = new Sprite("puzzle1\\puzztest.smk");
    puzztest->flags &= ~2;
    puzztest->priority = 5;
    puzztest->loc_x = 0;
    puzztest->loc_y = 0;

    // Create litdoors sprite
    litdoors = new Sprite("puzzle1\\litdoors.smk");
    litdoors->flags &= ~2;
    litdoors->priority = 10;
    litdoors->flags |= 0x40;
    litdoors->InitLogic(3);
    litdoors->SetRange(0, 1, 1);
    litdoors->SetRange(1, 2, 2);
    litdoors->SetRange(2, 3, 3);

    // Create floor sprites
    lowfloor = new Sprite("puzzle1\\lowfloor.smk");
    lowfloor->loc_x = 0x1d;
    lowfloor->loc_y = 0x150;

    midfloor = new Sprite("puzzle1\\midfloor.smk");
    midfloor->loc_x = 0x2c;
    midfloor->loc_y = 0x130;

    topfloor = new Sprite("puzzle1\\topfloor.smk");
    topfloor->loc_x = 0x5a;
    topfloor->loc_y = 0x11c;

    // Initialize floor sprites
    spritePtr = &lowfloor;
    for (i = 0; i < 3; i++) {
        sprite = spritePtr[i];
        sprite->flags &= ~2;
        sprite->priority = 10;
        sprite->flags |= 0x40;
        sprite->InitLogic(4);
        sprite->SetRange(0, 1, 4);
        sprite->SetRange(1, 5, 8);
        sprite->SetRange(2, 9, 0xc);
        sprite->SetRange(3, 0xd, 0x10);
    }

    // Initialize soundStates to 1
    for (i = 0; i < 10; i++) {
        soundStates[i] = 1;
    }
}

/* Function start: 0x40E9A0 */
Handler4::~Handler4() {
    int i;
    Sprite* sprite;
    Sprite** floorPtr;

    // Cleanup floor, paths, and button sprites
    // Original uses pointer at 0x624 (lowfloor) and accesses paths at -0x18, buttons at -0xc
    floorPtr = &lowfloor;
    for (i = 0; i < 3; i++) {
        sprite = *floorPtr;
        if (sprite != 0) {
            sprite->FreeAnimation();
            delete sprite;
            *floorPtr = 0;
        }

        sprite = floorPtr[-6];  // paths1: offset -0x18 = -24 bytes = -6 pointers
        if (sprite != 0) {
            sprite->FreeAnimation();
            delete sprite;
            floorPtr[-6] = 0;
        }

        sprite = floorPtr[-3];  // buttons1: offset -0xc = -12 bytes = -3 pointers
        if (sprite != 0) {
            sprite->FreeAnimation();
            delete sprite;
            floorPtr[-3] = 0;
        }

        floorPtr++;
    }

    // Cleanup litdoors
    if (litdoors != 0) {
        litdoors->FreeAnimation();
        delete litdoors;
        litdoors = 0;
    }

    // Cleanup puzztest
    if (puzztest != 0) {
        puzztest->FreeAnimation();
        delete puzztest;
        puzztest = 0;
    }

    // Cleanup palette
    if (palette != 0) {
        palette->Cleanup();
        delete palette;
        palette = 0;
    }
}

/* Function start: 0x40EB80 */
void Handler4::Init(SC_Message* msg) {
    int* rendererPtr;
    Palette* p;

    WriteToMessageLog("\nENTER FORCEFIELD PUZZLE");

    InitIconBar(msg);

    // Set palette
    p = palette;
    if (p != 0) {
        rendererPtr = (int*)((char*)g_ZBufferManager_0043698c + 0xa8);
        if (*rendererPtr != 0) {
            WriteToMessageLog("ddouble palette");
        }
        *rendererPtr = (int)p;
    }

    // Create sound if soundStates[9] is set (0x65c offset check)
    if (soundStates[9] != 0) {
        sound2 = new Sample();
        sound2->Load("audio\\Snd0020.wav");
        sound2->Play(100, 0);
    }

    // Initialize puzzle if soundStates[4] is set (0x648 offset check)
    if (soundStates[4] != 0) {
        PlaySound(4, 0);
    }
}

/* Function start: 0x40EC80 */
int Handler4::Exit(SC_Message* msg) {
    int i;
    Sprite** spritePtr;

    // Render puzztest sprite
    if (puzztest != 0) {
        puzztest->Do(0, 0, 1.0);
    }

    // Render litdoors sprite
    if (litdoors != 0) {
        litdoors->Do(0, 0, 1.0);
    }

    // Render path, button and floor sprites
    spritePtr = &paths1;
    for (i = 0; i < 3; i++) {
        spritePtr[0]->Do(0, 0, 1.0);
        spritePtr[3]->Do(0, 0, 1.0);
        spritePtr[6]->Do(0, 0, 1.0);
        spritePtr++;
    }

    // Stop and cleanup sound1
    if (sound1 != 0) {
        sound1->Stop();
    }
    if (sound1 != 0) {
        sound1->Unload();
        delete sound1;
        sound1 = 0;
    }

    // Stop and cleanup sound2
    if (sound2 != 0) {
        sound2->Stop();
        if (sound2 != 0) {
            sound2->Unload();
            delete sound2;
            sound2 = 0;
        }
    }

    // Call parent exit
    CleanupIconBar();

    WriteToMessageLog("EXIT FORCEFIELD PUZZLE\n");

    return 1;
}

/* Function start: 0x40ED50 */
int Handler4::HandleMessage(SC_Message* msg) {
    int result;

    // Call parent handler
    result = CheckButtonClick(msg);
    if (result != 0) {
        return 1;
    }

    // Check click type
    if (msg->mouseX < 2) {
        return 1;
    }

    // Get mouse position and handle clicks
    // Complex click handling logic would go here

    return 1;
}

/* Function start: 0x40EEB0 */
void Handler4::Draw(int param1, int param2) {
    int* msgPtr;
    int diff;

    msgPtr = (int*)param2;
    diff = handlerId - msgPtr[0x22];
    // Return (diff >= 2) ? 0 : 1 - handled by vtable
}

/* Function start: 0x40EED0 */
void Handler4::Update(SC_Message* msg) {
    if (handlerId != msg->command) {
        return;
    }

    // Stop and cleanup sound1 if playing
    if (sound1 != 0) {
        // Check if sound finished
        if (sound1->m_sample != 0) {
            // ... check sound status
        }
        sound1->Unload();
        delete sound1;
        sound1 = 0;
    }

    // Display various elements
    DisplayButtons();
    DisplayPaths();

    if (litdoors != 0) {
        DisplayLitDoors();
    }

    if (initialized != 0) {
        DisplayFloors();
    }

    // Update mouse
    g_Mouse_00436978->DrawCursor();
}

/* Function start: 0x40F310 */
void Handler4::ResetPuzzle() {
    int i;
    int* floorPtr;
    Sprite** spritePtr;

    // Reset floor states using pointer access
    floorPtr = floorStates;
    floorPtr[0] = 0;
    floorPtr[1] = 0;
    floorPtr[2] = 0;

    // Reset button states (memset generates rep stosd)
    memset(buttonStates, 0, 9 * sizeof(int));

    puzzleSolved = 0;
    initialized = 0;

    // Reset floor sprite states
    spritePtr = &lowfloor;
    for (i = 0; i < 3; i++) {
        spritePtr[0]->SetState(3);
        spritePtr++;
    }

    // Reset sound states - need rep stosd with value 1
    for (i = 0; i < 10; i++) {
        soundStates[i] = 1;
    }
}

/* Function start: 0x40F490 */
void Handler4::DisplayButtons() {
    int i;
    int row;
    int col;
    Sprite** buttonPtr;

    for (i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            row = i / 3;
            buttonPtr = &buttons1 + row;
            col = i % 3;
            (*buttonPtr)->SetState(col);
            (*buttonPtr)->Do((*buttonPtr)->loc_x, (*buttonPtr)->loc_y, 1.0);
        }
    }
}

/* Function start: 0x40F4F0 */
void Handler4::DisplayPaths() {
    int i;
    int row;
    int col;
    Sprite** pathPtr;

    if (paths1 == 0 || paths2 == 0 || paths3 == 0) {
        WriteToMessageLog("Error in SCIpuzz1.cpp - DisplayMap()");
    }

    for (i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            row = i / 3;
            pathPtr = &paths1 + row;
            col = i % 3;
            (*pathPtr)->SetState(col);
            (*pathPtr)->Do((*pathPtr)->loc_x, (*pathPtr)->loc_y, 1.0);
        }
    }
}

/* Function start: 0x40F580 */
void Handler4::DisplayLitDoors() {
    int i;
    int x;
    int y;

    for (i = 0; i < 3; i++) {
        if (floorStates[i] != 0) {
            litdoors->SetState(i);
            x = 0;
            if (i != 0) {
                if ((unsigned)(i - 1) < 1) {
                    x = 0x138 - 0xa6;
                } else {
                    x = 0x138;
                }
            }
            if ((unsigned)(i - 1) < 1) {
                y = 0x60 + 0xe;
            } else {
                y = 0x60;
            }
            litdoors->Do(x, y, 1.0);
        }
    }
}

/* Function start: 0x40F5F0 */
void Handler4::DisplayFloors() {
    int floorRow[3];
    int i;
    Sprite** floorPtr;
    int state;
    Sprite* floor;

    floorRow[0] = -1;
    floorRow[1] = -1;
    floorRow[2] = -1;

    if (lowfloor == 0 || midfloor == 0 || topfloor == 0) {
        return;
    }

    // Determine which buttons are active in each row
    for (i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            if (i >= 0 && i <= 2) {
                floorRow[0] = i;
            } else if (i >= 3 && i <= 5) {
                floorRow[1] = i % 3;
            } else if (i >= 6 && i <= 8) {
                floorRow[2] = i % 3;
            }
        }
    }

    // Update floor sprites if needed
    if (needsUpdate != 0) {
        needsUpdate = 0;

        floorPtr = &lowfloor;
        for (i = 0; i < 3; i++) {
            floorPtr[i]->flags |= 0x20;
            state = floorRow[i];
            if (state < -1 || state > 2) {
                WriteToMessageLog("Error in SCIpuzz1.cpp - DisplayThisFloorRow:  Invalid case value");
            }
            if (state == -1) {
                state = 3;
            }
            floorPtr[i]->SetState(state);
        }
    }

    // Render floors
    for (i = 0; i < 3; i++) {
        floor = (&lowfloor)[i];
        floor->Do(floor->loc_x, floor->loc_y, 1.0);
    }
}

/* Function start: 0x40F370 */
void Handler4::PlaySound(int index, int loop) {
    char filename[48];

    // Range check
    if (index < 0 || index > 9) {
        if (index != 9) {
            ShowError("%d is out of sound array range", index);
        }
    }

    // Stop current sound
    if (sound1 != 0) {
        sound1->Stop();
        if (sound1 != 0) {
            sound1->Unload();
            delete sound1;
            sound1 = 0;
        }
    }

    // Play new sound if state allows
    if (soundStates[index] != 0) {
        sprintf(filename, "audio\\snd%4.4d.wav", index + 11);
        soundStates[index] = loop;
        sound1 = new Sample();
        sound1->Load(filename);
        sound1->Play(100, 1);
    }
}
