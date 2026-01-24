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
    memset(&palette, 0, 0xf0);

    rect1.left = 0;
    rect1.top = 0;
    handlerId = 4;
    needsUpdate = 1;
    rect1.right = 0x197;
    rect1.bottom = 0x1aa;
    rect2.left = 0x1d7;
    rect2.top = 0x3b;
    rect2.right = 0x25c;
    rect2.bottom = 0xbb;
    rect3.top = 0xf1;
    rect3.right = 0x25b;
    rect3.bottom = 0x16d;
    rect4.left = 0x73;
    rect4.top = 0x11d;
    rect4.right = 0x12f;
    rect4.bottom = 0x149;
    rect5.left = 0x32;
    
    palette = new Palette();
    rect3.left = 0x1de;
    rect5.top = 0x14a;
    rect5.right = 0x172;
    rect5.bottom = 0x192;

    palette->Load(CDData_FormatPath("puzzle1\\Puzztest.col"));

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

    for (i = 0; i < 3; i++) {
        (&buttons1)[i]->flags &= ~2;
        (&buttons1)[i]->priority = 10;
        (&buttons1)[i]->InitLogic(3);
        (&buttons1)[i]->SetRange(0, 1, 1);
        (&buttons1)[i]->SetRange(1, 2, 2);
        (&buttons1)[i]->SetRange(2, 3, 3);

        (&paths1)[i]->flags &= ~2; 
        (&paths1)[i]->priority = 10;
        (&paths1)[i]->InitLogic(3);
        (&paths1)[i]->SetRange(0, 1, 1);
        (&paths1)[i]->SetRange(1, 2, 2);
        (&paths1)[i]->SetRange(2, 3, 3);
    }

    puzztest = new Sprite("puzzle1\\puzztest.smk");
    puzztest->flags &= ~2;
    puzztest->priority = 5;
    puzztest->loc_x = 0;
    puzztest->loc_y = 0;

    litdoors = new Sprite("puzzle1\\litdoors.smk");
    litdoors->flags &= ~2;
    litdoors->priority = 10;
    litdoors->flags |= 0x40;
    litdoors->InitLogic(3);
    litdoors->SetRange(0, 1, 1);
    litdoors->SetRange(1, 2, 2);
    litdoors->SetRange(2, 3, 3);

    lowfloor = new Sprite("puzzle1\\lowfloor.smk");
    lowfloor->loc_x = 0x1d;
    lowfloor->loc_y = 0x150;

    midfloor = new Sprite("puzzle1\\midfloor.smk");
    midfloor->loc_x = 0x2c;
    midfloor->loc_y = 0x130;

    topfloor = new Sprite("puzzle1\\topfloor.smk");
    topfloor->loc_x = 0x5a;
    topfloor->loc_y = 0x11c;

    for (i = 0; i < 3; i++) {
        (&lowfloor)[i]->flags &= ~2;
        (&lowfloor)[i]->priority = 10;
        (&lowfloor)[i]->flags |= 0x40;
        (&lowfloor)[i]->InitLogic(4);
        (&lowfloor)[i]->SetRange(0, 1, 4);
        (&lowfloor)[i]->SetRange(1, 5, 8);
        (&lowfloor)[i]->SetRange(2, 9, 12);
        (&lowfloor)[i]->SetRange(3, 13, 16);
    }

    for (i = 0; i < 10; i++) {
        soundStates[i] = 1;
    }
}

/* Function start: 0x40E9A0 */
Handler4::~Handler4() {
    for (int i = 0; i < 3; i++) {
        if ((&lowfloor)[i]) {
            (&lowfloor)[i]->FreeAnimation();
            delete (&lowfloor)[i];
            (&lowfloor)[i] = 0;
        }
        if ((&paths1)[i]) {
            (&paths1)[i]->FreeAnimation();
            delete (&paths1)[i];
            (&paths1)[i] = 0;
        }
        if ((&buttons1)[i]) {
            (&buttons1)[i]->FreeAnimation();
            delete (&buttons1)[i];
            (&buttons1)[i] = 0;
        }
    }

    if (litdoors) {
        litdoors->FreeAnimation();
        delete litdoors;
        litdoors = 0;
    }

    if (puzztest) {
        puzztest->FreeAnimation();
        delete puzztest;
        puzztest = 0;
    }

    if (palette) {
        palette->Cleanup();
        delete palette;
        palette = 0;
    }
}

/* Function start: 0x40EB80 */
void Handler4::Init(SC_Message* msg) {
    WriteToMessageLog("\nENTER FORCEFIELD PUZZLE");
    InitIconBar(msg);

    if (palette != 0) {
        if (g_ZBufferManager_0043698c->m_fieldA8 != 0) {
            WriteToMessageLog("ddouble palette");
        }
        g_ZBufferManager_0043698c->m_fieldA8 = palette;
    }

    if (soundStates[9] != 0) {
        sound2 = new Sample();
        sound2->Load("audio\\Snd0020.wav");
        sound2->Play(100, 0);
    }

    if (soundStates[4] != 0) {
        PlaySound(4, 0);
    }
}

/* Function start: 0x40EC80 */
int Handler4::Exit(SC_Message* msg) {
    if (puzztest) puzztest->FreeAnimation();
    if (litdoors) litdoors->FreeAnimation();

    for (int i = 0; i < 3; i++) {
        (&paths1)[i]->FreeAnimation();
        (&buttons1)[i]->FreeAnimation();
        (&lowfloor)[i]->FreeAnimation();
    }

    if (sound1 != 0) {
        sound1->Stop();
        delete sound1;
        sound1 = 0;
    }

    if (sound2 != 0) {
        sound2->Stop();
        delete sound2;
        sound2 = 0;
    }

    CleanupIconBar();
    WriteToMessageLog("EXIT FORCEFIELD PUZZLE\n");
    return 1;
}

/* Function start: 0x40ED50 */
int Handler4::HandleMessage(SC_Message* msg) {
    int i;
    if (CheckButtonClick(msg)) {
        return 1;
    }

    if (msg->mouseX < 2) {
        return 1;
    }

    Rect* pRect = (Rect*)0x43d100;
    for (i = 0; i < 3; i++) {
        if (floorStates[i]) {
            if (msg->clickX >= pRect->x1 && msg->clickX <= pRect->x2 &&
                msg->clickY >= pRect->y1 && msg->clickY <= pRect->y2) {
                
                if (i == 0) ResetPuzzle();
                
                msg->targetAddress = 8;
                msg->sourceAddress = (i >= 1) ? 3 : 2;
                msg->command = (i == 0) ? 10 : handlerId;
                msg->data = (i == 0) ? 1 : field_8C;
                msg->priority = 5;
            }
        }
        pRect++;
    }

    return 1;
}

/* Function start: 0x40EEB0 */
int Handler4::Update(SC_Message* msg) {
    return handlerId == msg->command;
}

/* Function start: 0x40EED0 */
void Handler4::Draw(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }

    if (sound1 != 0) {
        Sample* s = sound1;
        if (s->m_sample != 0 && *(int*)((char*)s->m_sample + 0xc) == s->m_size) {
            if (AIL_sample_status(s->m_sample) != 4) {
                delete sound1;
                sound1 = 0;
            }
        }
    }

    DrawIconBar(param1, param2);

    if (puzztest != 0) {
        puzztest->Do(puzztest->loc_x, puzztest->loc_y, 1.0);
    }

    DisplayButtons();
    DisplayPaths();
    if (litdoors != 0) {
        DisplayLitDoors();
    }
    if (initialized != 0) {
        DisplayFloors();
    }

    g_Mouse_00436978->DrawCursor();
}

/* Function start: 0x40F310 */
void Handler4::ResetPuzzle() {
    int i;
    floorStates[0] = 0;
    floorStates[1] = 0;
    floorStates[2] = 0;
    memset(buttonStates, 0, 9 * sizeof(int));
    puzzleSolved = 0;
    initialized = 0;

    Sprite** pFloor = &lowfloor;
    i = 3;
    while (i--) {
        (*pFloor)->SetState(3);
        pFloor++;
    }

    for (i = 0; i < 10; i++) {
        soundStates[i] = 1;
    }
}

/* Function start: 0x40F370 */
void Handler4::PlaySound(int index, int loop) {
    char filename[48];

    if (index < 0 || index > 9) {
        if (index != 9) {
            ShowError("%d is out of sound array range", index);
        }
    }

    if (sound1 != 0) {
        sound1->Stop();
        delete sound1;
        sound1 = 0;
    }

    if (soundStates[index] != 0) {
        sprintf(filename, "audio\\snd%4.4d.wav", index + 11);
        soundStates[index] = loop;
        sound1 = new Sample();
        sound1->Load(filename);
        sound1->Play(100, 1);
    }
}

/* Function start: 0x40F490 */
void Handler4::DisplayButtons() {
    for (int i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            Sprite* btn = (&buttons1)[i / 3];
            btn->SetState(i % 3);
            btn->Do(btn->loc_x, btn->loc_y, 1.0);
        }
    }
}

/* Function start: 0x40F4F0 */
void Handler4::DisplayPaths() {
    if (paths1 == 0 || paths2 == 0 || paths3 == 0) {
        WriteToMessageLog("Error in SCIpuzz1.cpp - DisplayMap()");
    }
    for (int i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            Sprite* path = (&paths1)[i / 3];
            path->SetState(i % 3);
            path->Do(path->loc_x, path->loc_y, 1.0);
        }
    }
}

/* Function start: 0x40F580 */
void Handler4::DisplayLitDoors() {
    for (int i = 0; i < 3; i++) {
        if (floorStates[i] != 0) {
            litdoors->SetState(i);
            int x = (i == 1) ? (0x138 - 0xa6) : ((i == 2) ? 0x138 : 0);
            int y = (i == 1) ? (0x60 + 0xe) : 0x60;
            litdoors->Do(x, y, 1.0);
        }
    }
}

/* Function start: 0x40F5F0 */
void Handler4::DisplayFloors() {
    int floorRow[3];
    int i;
    floorRow[0] = -1;
    floorRow[1] = -1;
    floorRow[2] = -1;

    if (lowfloor == 0 || midfloor == 0 || topfloor == 0) return;

    for (i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            if (i < 3) floorRow[0] = i;
            else if (i < 6) floorRow[1] = i % 3;
            else floorRow[2] = i % 3;
        }
    }

    if (needsUpdate != 0) {
        needsUpdate = 0;
        for (i = 0; i < 3; i++) {
            Sprite* s = (&lowfloor)[i];
            s->flags |= 0x20;
            int state = floorRow[i];
            if (state < -1 || state > 2) {
                 WriteToMessageLog("Error in SCIpuzz1.cpp - DisplayThisFloorRow:  Invalid case value");
            }
            if (state == -1) state = 3;
            s->SetState(state);
        }
    }

    for (i = 0; i < 3; i++) {
        Sprite* s = (&lowfloor)[i];
        s->Do(s->loc_x, s->loc_y, 1.0);
    }
}
