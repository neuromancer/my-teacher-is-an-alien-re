#include "IconBar.h"
#include <string.h>
#include <stdio.h>
#include "Sprite.h"
#include "InputManager.h"
#include "SoundList.h"
#include "Sample.h"
#include "Memory.h"
#include "Parser.h"
#include "globals.h"
#include "Array.h"
#include "string.h"
#include "SC_Question.h"

// External functions
//extern "C" void __cdecl WriteToMessageLog(const char*);

/* Function start: 0x402730 */
IconBar::IconBar() {
    int i;
    Sprite* sprite;

    // Zero handler fields at 0x88-0x9F
    memset(&handlerId, 0, 6 * sizeof(int));

    // buttons[] array is automatically constructed via IconBarButton constructor

    // Zero more fields
    memset(&barBounds, 0, 0x560);

    // Set icon bar bounds
    barBounds.x1 = 0;
    barBounds.y1 = 0x1ab;
    barBounds.x2 = 0x280;
    barBounds.y2 = 0x1e0;

    // Create iconbar sprite
    iconbarSprite = new Sprite("elements\\iconbar.smk");
    iconbarSprite->flags &= ~2;  // Clear flag 0x2
    iconbarSprite->loc_x = 0;
    iconbarSprite->loc_y = 0x1ab;
    iconbarSprite->SetState(4);
    iconbarSprite->SetRange(0, 1, 1);
    iconbarSprite->SetRange(1, 2, 2);
    iconbarSprite->SetRange(2, 3, 3);
    iconbarSprite->SetRange(3, 4, 4);
    iconbarSprite->priority = 1000;

    // Create choice button (index 0)
    buttons[0].sprite = new Sprite("elements\\choice.smk");
    buttons[0].sprite->loc_x = 0x5e;
    buttons[0].sprite->loc_y = 0x1b5;

    // Create backpack button (index 1)
    buttons[1].sprite = new Sprite("elements\\backpack.smk");
    buttons[1].sprite->loc_x = 0xad;
    buttons[1].sprite->loc_y = 0x1b5;

    // Create aliencom button (index 2)
    buttons[2].sprite = new Sprite("elements\\aliencom.smk");
    buttons[2].sprite->loc_x = 0x115;
    buttons[2].sprite->loc_y = 0x1b4;

    // Create schedule button (index 3)
    buttons[3].sprite = new Sprite("elements\\schedule.smk");
    buttons[3].sprite->loc_x = 0x165;
    buttons[3].sprite->loc_y = 0x1b5;

    // Create mainmenu button (index 4)
    buttons[4].sprite = new Sprite("elements\\mainmenu.smk");
    buttons[4].sprite->loc_x = 0x1d1;
    buttons[4].sprite->loc_y = 0x1b6;

    // Create quit button (index 5)
    buttons[5].sprite = new Sprite("elements\\quit.smk");
    buttons[5].sprite->loc_x = 0x22a;
    buttons[5].sprite->loc_y = 0x1b6;

    // Configure all 6 buttons
    for (i = 0; i < 6; i++) {
        Sprite* btn = buttons[i].sprite;
        btn->flags &= ~2;  // Clear flag
        btn->SetState(2);
        btn->SetRange(0, 1, 1);
        btn->SetRange(1, 2, 2);
        btn->priority = 0x3e9;
    }

    // Set button bounds
    buttons[0].bounds.left = 0x5e;
    buttons[0].bounds.right = 0x9f;
    
    buttons[0].bounds.top = 0x1b5;
    buttons[1].bounds.top = 0x1b5;
    buttons[0].bounds.bottom = 0x1d3;
    buttons[1].bounds.left = 0xad;
    buttons[1].bounds.top = 0x1b5;
    buttons[1].bounds.right = 0xf9;
    buttons[1].bounds.bottom = 0x1d6;

    // Button 2 (aliencom)
    buttons[2].bounds.left = 0x115;
    buttons[2].bounds.top = 0x1b4;
    buttons[2].bounds.right = 0x155;
    buttons[2].bounds.bottom = 0x1d6;

    // Button 3 (schedule)
    buttons[3].bounds.left = 0x165;
    buttons[3].bounds.top = 0x1b5;
    buttons[3].bounds.right = 0x1ae;
    buttons[3].bounds.bottom = 0x1d2;

    // Button 4 (mainmenu)
    buttons[4].bounds.left = 0x1d1;
    buttons[4].bounds.top = 0x1b6;
    buttons[4].bounds.right = 0x207;
    buttons[4].bounds.bottom = 0x1d2;

    // Button 5 (quit)
    buttons[5].bounds.left = 0x22a;
    buttons[5].bounds.top = 0x1b6;
    buttons[5].bounds.right = 0x270;
    buttons[5].bounds.bottom = 0x1d5;

    // Button 0 template message
    buttons[0].message.command = 10;
    buttons[0].message.sourceAddress = 1;
    buttons[0].message.targetAddress = 6;
    buttons[0].message.priority = 5;

    // Button 4 template message
    buttons[4].message.command = 10;
    buttons[4].message.sourceAddress = 1;
    buttons[4].message.targetAddress = 6;
    buttons[4].message.priority = 5;

    // Button 5 template message
    buttons[5].message.command = 2;
    buttons[5].message.priority = 5;
}

IconBar::~IconBar() {
    // Destructor - cleanup handled by CleanupIconBar
}

/* Function start: 0x402CD0 */
IconBarButton::IconBarButton()
    : message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
{
}

/* Function start: 0x402D60 */
IconBarButton::~IconBarButton() {
}

/* Function start: 0x402ED0 */
void IconBar::InitIconBar(SC_Message* msg) {
    int i;

    // Call parent Init (CopyCommandData)
    CopyCommandData(msg);

    // Check DAT_00435a80 (used as pointer) and set iconbar state
    if (DAT_00435a80 != 0) {
        iconbarSprite->SetState2(*(int*)DAT_00435a80 + 1);
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
        delete soundList;
        soundList = 0;
    }
}

/* Function start: 0x403040 */
int IconBar::CheckButtonClick(SC_Message* msg) {
    unsigned int j;
    int i;
    int ok;

    WriteMessageAddress(msg);

    // Initial bar bounds check
    ok = (barBounds.x1 <= msg->clickX && barBounds.x2 >= msg->clickX &&
          barBounds.y1 <= msg->clickY && barBounds.y2 >= msg->clickY);
    if (!ok) return 0;

    // Only proceed for specific mouse states
    if (msg->mouseX < 2) {
        return 1;
    }

    // Loop through buttons using for loop
    for (i = 0; i < 6; i++) {
        if (buttons[i].enabled) {
            // Button bounds check
            ok = (buttons[i].bounds.left <= msg->clickX && buttons[i].bounds.right >= msg->clickX &&
                  buttons[i].bounds.top <= msg->clickY && buttons[i].bounds.bottom >= msg->clickY);
            
            if (ok) {
                PlayButtonSound(i);

                SC_Message* pSrc = &buttons[i].message;

                // Copy button template message to target message
                msg->m_subObject = pSrc->m_subObject;
                msg->isProcessingKey = pSrc->isProcessingKey;

                j = 0;
                do {
                    j++;
                    msg->currentKey[j - 1] = pSrc->currentKey[j - 1];
                } while (j < 32);

                msg->lineNumber = pSrc->lineNumber;
                
                int* pMsg38 = &msg->savedFilePos;
                pMsg38[0] = pSrc->savedFilePos;
                pMsg38[1] = pSrc->field_0x3c;

                j = 0;
                do {
                    j++;
                    msg->filename[j - 1] = pSrc->filename[j - 1];
                } while (j < 64);

                msg->pFile = pSrc->pFile;
                msg->command = pSrc->command;
                msg->sourceAddress = pSrc->sourceAddress;
                msg->targetAddress = pSrc->targetAddress;
                msg->data = pSrc->data;
                msg->priority = pSrc->priority;
                msg->param1 = pSrc->param1;
                msg->param2 = pSrc->param2;
                msg->clickX = pSrc->clickX;
                msg->clickY = pSrc->clickY;
                msg->mouseX = pSrc->mouseX;
                msg->mouseY = pSrc->mouseY;
                msg->field_b4 = pSrc->field_b4;
                msg->field_b8 = pSrc->field_b8;
                msg->userPtr = pSrc->userPtr;

                return 1;
            }
        }
    }

    return 1;
}

/* Function start: 0x403230 */
void IconBar::DrawIconBar(int param1, int param2) {
    int i;
    int mouseX;
    int mouseY;
    IconBarButton* pBtn;

    if (handlerId != param2) {
        return;
    }

    iconbarSprite->Do(iconbarSprite->loc_x, iconbarSprite->loc_y, 1.0);

    pBtn = buttons;
    for (i = 6; i > 0; i--) {
        if (pBtn->sprite != 0) {
            mouseY = 0;
            if (g_InputManager_00436968->pMouse != 0) {
                mouseY = g_InputManager_00436968->pMouse->y;
            }

            if (g_InputManager_00436968->pMouse != 0) {
                mouseX = g_InputManager_00436968->pMouse->x;
            } else {
                mouseX = 0;
            }

            pBtn->sprite->SetState2(pBtn->bounds.left <= mouseX && pBtn->bounds.right >= mouseX &&
                                    pBtn->bounds.top <= mouseY && pBtn->bounds.bottom >= mouseY);

            if (pBtn->enabled != 0) {
                pBtn->sprite->Do(pBtn->sprite->loc_x, pBtn->sprite->loc_y, 1.0);
            }
        }
        pBtn++;
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
