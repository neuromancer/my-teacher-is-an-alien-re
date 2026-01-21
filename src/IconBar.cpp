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

/* Function start: 0x402CD0 */
IconBarButton::IconBarButton()
    : message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
{
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
}

/* Function start: 0x402D60 */
IconBarButton::~IconBarButton() {
}

/* Function start: 0x402730 */
IconBar::IconBar() {
    int i;
    Sprite* sprite;
    void* mem;

    // Zero handler fields at 0x88-0x9F
    memset(&handlerId, 0, 6 * sizeof(int));

    // buttons[] array is automatically constructed via IconBarButton constructor

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
    int inBounds;
    int* enabledPtr;
    int buttonOffset;
    char* buttonBase;
    unsigned int j;
    int* msgField38Ptr;

    WriteMessageAddress(msg);

    msgX = msg->clickX;

    // Check bar bounds
    if (barX1 > msgX || barX2 < msgX ||
        barY1 > msg->clickY || barY2 < msg->clickY) {
        inBounds = 0;
    } else {
        inBounds = 1;
    }

    if (inBounds == 0) {
        return 0;
    }

    if (msg->mouseX < 2) {
        return 1;
    }

    // Loop through buttons - use pointer to enabled field
    i = 0;
    enabledPtr = &buttons[0].enabled;

    do {
        if (*enabledPtr == 0) {
            goto nextButton;
        }

        // Check button bounds using offsets from enabled pointer
        if (enabledPtr[-4] > msgX || enabledPtr[-2] < msgX ||
            enabledPtr[-3] > msg->clickY || enabledPtr[-1] < msg->clickY) {
            inBounds = 0;
        } else {
            inBounds = 1;
        }

        if (inBounds != 0) {
            // Button found - play sound and copy button data to message
            PlayButtonSound(i);

            buttonOffset = i * 0xE0;
            buttonBase = (char*)this + buttonOffset;

            // Copy button's embedded SC_Message data to msg
            // Parser fields (inherited by SC_Message)
            msg->m_subObject = buttons[i].message.m_subObject;
            msg->isProcessingKey = buttons[i].message.isProcessingKey;

            // Copy 32 bytes for currentKey
            j = 0;
            do {
                j++;
                msg->currentKey[j - 1] = buttonBase[0xCF + j];
            } while (j < 0x20);

            // Copy lineNumber and savedFilePos/field_0x3c
            msg->lineNumber = buttons[i].message.lineNumber;
            msgField38Ptr = &msg->savedFilePos;
            msgField38Ptr[0] = buttons[i].message.savedFilePos;
            msgField38Ptr[1] = buttons[i].message.field_0x3c;

            // Copy 64 bytes for filename
            j = 0;
            do {
                char* thisJ = (char*)this + j;
                j++;
                msg->filename[j - 1] = thisJ[buttonOffset + 0x100];
            } while (j < 0x40);

            // Copy remaining fields from embedded SC_Message
            msg->pFile = buttons[i].message.pFile;
            msg->command = buttons[i].message.command;
            msg->sourceAddress = buttons[i].message.sourceAddress;
            msg->targetAddress = buttons[i].message.targetAddress;
            msg->data = buttons[i].message.data;
            msg->priority = buttons[i].message.priority;
            msg->param1 = buttons[i].message.param1;
            msg->param2 = buttons[i].message.param2;
            msg->clickX = buttons[i].message.clickX;
            msg->clickY = buttons[i].message.clickY;
            msg->mouseX = buttons[i].message.mouseX;
            msg->mouseY = buttons[i].message.mouseY;
            msg->field_b4 = buttons[i].message.field_b4;
            msg->field_b8 = buttons[i].message.field_b8;
            msg->userPtr = buttons[i].message.userPtr;

            return 1;
        }

nextButton:
        enabledPtr = (int*)((char*)enabledPtr + 0xE0);
        i++;
    } while (i < 6);

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
