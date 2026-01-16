#include "IconBar.h"
#include <string.h>
#include <stdio.h>
#include "Sprite.h"
#include "SoundList.h"
#include "Sample.h"
#include "Memory.h"
#include "Parser.h"
#include "globals.h"
#include "Array.h"
#include "SC_Question.h"

// External functions
extern "C" void __cdecl WriteToMessageLog(const char*);

// Button constructor/destructor for Array_Iterate
static void IconBarButton_Constructor(void* ptr) {
    memset(ptr, 0, 0xE0);
}

static void IconBarButton_Destructor(void* ptr) {
    // Empty - buttons don't own their sprites
}

/* Function start: 0x402CD0 */
// Referenced as constructor callback in Array_Iterate
void __cdecl IconBar_ButtonInit(void* ptr) {
    memset(ptr, 0, 0xE0);
}

/* Function start: 0x402D60 */
// Referenced as destructor callback in Array_Iterate
void __cdecl IconBar_ButtonCleanup(void* ptr) {
    // Empty destructor - sprites cleaned up separately
}

/* Function start: 0x402730 */
IconBar::IconBar() {
    int i;
    Sprite* sprite;
    void* mem;

    // Zero handler fields at 0x88-0x9F
    memset(&handlerId, 0, 6 * sizeof(int));

    // Initialize button array (6 elements of 0xE0 bytes each)
    Array_Iterate(buttons, 0xE0, 6, IconBar_ButtonInit, IconBar_ButtonCleanup);

    // Zero more fields
    memset(&barX1, 0, 0x560);

    // Set icon bar bounds
    barX1 = 0;
    barY1 = 0x1ab;
    barX2 = 0x280;
    barY2 = 0x1e0;

    // Create iconbar sprite
    iconbarSprite = new Sprite("elements\\iconbar.smk");
    if (iconbarSprite != 0) {
        iconbarSprite->flags &= ~2;  // Clear flag 0x2
        iconbarSprite->loc_x = 0;
        iconbarSprite->loc_y = 0x1ab;
        iconbarSprite->SetState(4);
        iconbarSprite->SetRange(0, 1, 1);
        iconbarSprite->SetRange(1, 2, 2);
        iconbarSprite->SetRange(2, 3, 3);
        iconbarSprite->SetRange(3, 4, 4);
        iconbarSprite->priority = 1000;
    }

    // Create choice button (index 0)
    buttons[0].sprite = new Sprite("elements\\choice.smk");
    if (buttons[0].sprite != 0) {
        buttons[0].sprite->loc_x = 0x5e;
        buttons[0].sprite->loc_y = 0x1b5;
    }

    // Create backpack button (index 1)
    buttons[1].sprite = new Sprite("elements\\backpack.smk");
    if (buttons[1].sprite != 0) {
        buttons[1].sprite->loc_x = 0xad;
        buttons[1].sprite->loc_y = 0x1b5;
    }

    // Create aliencom button (index 2)
    buttons[2].sprite = new Sprite("elements\\aliencom.smk");
    if (buttons[2].sprite != 0) {
        buttons[2].sprite->loc_x = 0x115;
        buttons[2].sprite->loc_y = 0x1b4;
    }

    // Create schedule button (index 3)
    buttons[3].sprite = new Sprite("elements\\schedule.smk");
    if (buttons[3].sprite != 0) {
        buttons[3].sprite->loc_x = 0x165;
        buttons[3].sprite->loc_y = 0x1b5;
    }

    // Create mainmenu button (index 4)
    buttons[4].sprite = new Sprite("elements\\mainmenu.smk");
    if (buttons[4].sprite != 0) {
        buttons[4].sprite->loc_x = 0x1d1;
        buttons[4].sprite->loc_y = 0x1b6;
    }

    // Create quit button (index 5)
    buttons[5].sprite = new Sprite("elements\\quit.smk");
    if (buttons[5].sprite != 0) {
        buttons[5].sprite->loc_x = 0x22a;
        buttons[5].sprite->loc_y = 0x1b6;
    }

    // Configure all 6 buttons
    for (i = 0; i < 6; i++) {
        Sprite* btn = buttons[i].sprite;
        if (btn != 0) {
            btn->flags &= ~2;  // Clear flag
            btn->SetState(2);
            btn->SetRange(0, 1, 1);
            btn->SetRange(1, 2, 2);
            btn->priority = 0x3e9;
        }
    }

    // Set button bounds
    // Button 0 (choice)
    buttons[0].x1 = 0x5e;
    buttons[0].y1 = 0x1b5;
    buttons[0].x2 = 0x9f;
    buttons[0].y2 = 0x1d3;

    // Button 1 (backpack)
    buttons[1].x1 = 0xad;
    buttons[1].y1 = 0x1b5;
    buttons[1].x2 = 0xf9;
    buttons[1].y2 = 0x1d6;

    // Button 2 (aliencom)
    buttons[2].x1 = 0x115;
    buttons[2].y1 = 0x1b4;
    buttons[2].x2 = 0x155;
    buttons[2].y2 = 0x1d6;

    // Button 3 (schedule)
    buttons[3].x1 = 0x165;
    buttons[3].y1 = 0x1b5;
    buttons[3].x2 = 0x1ae;
    buttons[3].y2 = 0x1d2;

    // Button 4 (mainmenu)
    buttons[4].x1 = 0x1d1;
    buttons[4].y1 = 0x1b6;
    buttons[4].x2 = 0x207;
    buttons[4].y2 = 0x1d2;

    // Button 5 (quit)
    buttons[5].x1 = 0x22a;
    buttons[5].y1 = 0x1b6;
    buttons[5].x2 = 0x270;
    buttons[5].y2 = 0x1d5;

    // Set additional config values (from assembly)
    // These are at specific offsets in the button structures
    // Choice button (0): field at 0x148 = 10, 0x14c = 1, 0x150 = 6, 0x158 = 5
    // Mainmenu button (4): similar pattern
    // Quit button (5): field at 0x5a8 = 2, 0x5b8 = 5
}

IconBar::~IconBar() {
    // Destructor - cleanup handled by CleanupIconBar
}

/* Function start: 0x402ED0 */
void IconBar::InitIconBar(SC_Message* msg) {
    int i;

    // Call parent Init (CopyCommandData)
    CopyCommandData(msg);

    // Check DAT_00435a80 (used as pointer) and set iconbar state
    // The decompiled code treats DAT_00435a80 as int*, dereferences it
    int* pIconBarState = (int*)DAT_00435a80;
    if (pIconBarState != 0) {
        iconbarSprite->SetState2(*pIconBarState + 1);
    }

    // Enable all buttons
    for (i = 0; i < 6; i++) {
        buttons[i].enabled = 1;
    }

    // Create sound list
    soundList = new SoundList(0x32);

    // Register sounds
    if (soundList != 0) {
        // Store sound handles at specific offsets
        soundList->Register("audio\\Snd0023.wav");
        soundList->Register("audio\\Snd0024.wav");
        soundList->Register("audio\\Snd0025.wav");
    }
}

/* Function start: 0x402FD0 */
void IconBar::CleanupIconBar() {
    int i;

    // Stop iconbar sprite animation
    if (iconbarSprite != 0) {
        iconbarSprite->StopAnimationSound();
    }

    // Stop all button sprite animations
    for (i = 0; i < 6; i++) {
        if (buttons[i].sprite != 0) {
            buttons[i].sprite->StopAnimationSound();
        }
    }

    // Stop and destroy sound list
    if (soundList != 0) {
        soundList->StopAll();
    }

    if (soundList != 0) {
        soundList->~SoundList();
        FreeMemory(soundList);
        soundList = 0;
    }
}

/* Function start: 0x403040 */
int IconBar::CheckButtonClick(SC_Message* msg) {
    int i;
    int msgX;
    int msgY;
    int msgLevel;
    int inBarBounds;

    // Call parent message handler
    WriteMessageAddress(msg);

    // Get message coordinates (clickX is at 0xa4, clickY is at 0xa8, mouseX is at 0xac)
    msgX = msg->clickX;
    msgY = msg->clickY;
    msgLevel = msg->mouseX;

    // Check if click is within icon bar bounds
    if (msgX < barX1 || msgX > barX2 || msgY < barY1 || msgY > barY2) {
        inBarBounds = 0;
    } else {
        inBarBounds = 1;
    }

    if (inBarBounds == 0) {
        return 0;
    }

    // Check if this is a click event (level >= 2)
    if (msgLevel < 2) {
        return 1;
    }

    // Check each button
    for (i = 0; i < 6; i++) {
        if (buttons[i].enabled == 0) {
            continue;
        }

        // Check button bounds
        if (msgX < buttons[i].x1 || msgX > buttons[i].x2 ||
            msgY < buttons[i].y1 || msgY > buttons[i].y2) {
            continue;
        }

        // Button clicked - copy button data to message
        // This is simplified - full implementation would copy more data
        return 1;
    }

    return 1;
}

/* Function start: 0x403230 */
void IconBar::DrawIconBar(int param1, int param2) {
    int i;
    int mouseX;
    int mouseY;
    int* mouseData;
    int inBounds;

    // Check if this is for our handler
    if (handlerId != param2) {
        return;
    }

    // Draw iconbar sprite
    if (iconbarSprite != 0) {
        iconbarSprite->Do(iconbarSprite->loc_x, iconbarSprite->loc_y, 1.0);
    }

    // Get mouse position
    mouseData = *(int**)((char*)g_InputManager_00436968 + 0x1a0);
    if (mouseData == 0) {
        mouseX = 0;
        mouseY = 0;
    } else {
        mouseX = mouseData[0];
        mouseY = mouseData[1];
    }

    // Draw each button
    for (i = 0; i < 6; i++) {
        Sprite* btn = buttons[i].sprite;
        if (btn == 0) {
            continue;
        }

        // Check if mouse is over button
        if (mouseX < buttons[i].x1 || mouseX > buttons[i].x2 ||
            mouseY < buttons[i].y1 || mouseY > buttons[i].y2) {
            inBounds = 0;
        } else {
            inBounds = 1;
        }

        btn->SetState2(inBounds);

        // Draw button if enabled
        if (buttons[i].enabled != 0) {
            btn->Do(btn->loc_x, btn->loc_y, 1.0);
        }
    }
}

/* Function start: 0x403300 */
void IconBar::PlayButtonSound(int buttonIndex) {
    Sample* sound;

    soundList->StopAll();

    if (buttonIndex != -1) {
        sound = IconBar::buttons[buttonIndex].clickSound;
        if (sound != 0) {
            sound->Play(100, 1);
        }
    }
}
