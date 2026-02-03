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
#include "string.h"
#include "SC_Question.h"
#include "Character.h"

// External functions
//extern "C" void __cdecl WriteToMessageLog(const char*);

/* Function start: 0x402730 */
IconBar::IconBar() {
    // The implicit call to Handler constructor handles memset(&handlerId, 0, 24)

    // Zero from barBounds (0xA0) to the end of the class (0x600)
    // 0x600 - 0xA0 = 0x560
    memset(&barBounds, 0, 0x560);

    handlerId = 0;

    // Set icon bar bounds
    barBounds.left = 0;
    barBounds.top = 0x1ab;
    barBounds.right = 0x280;
    barBounds.bottom = 0x1e0;

    // Create iconbar sprite
    iconbarSprite = new Sprite("elements\\iconbar.smk");
    iconbarSprite->flags &= ~2;
    iconbarSprite->loc_x = 0;
    iconbarSprite->loc_y = 0x1ab;
    iconbarSprite->SetState(4);
    iconbarSprite->SetRange(0, 1, 1);
    iconbarSprite->SetRange(1, 2, 2);
    iconbarSprite->SetRange(2, 3, 3);
    iconbarSprite->SetRange(3, 4, 4);
    iconbarSprite->priority = 1000;

    // Create sprites for buttons
    buttons[0].sprite = new Sprite("elements\\choice.smk");
    buttons[0].sprite->loc_x = 0x5e;
    buttons[0].sprite->loc_y = 0x1b5;

    buttons[1].sprite = new Sprite("elements\\backpack.smk");
    buttons[1].sprite->loc_x = 0xad;
    buttons[1].sprite->loc_y = 0x1b5;

    buttons[2].sprite = new Sprite("elements\\aliencom.smk");
    buttons[2].sprite->loc_x = 0x115;
    buttons[2].sprite->loc_y = 0x1b4;

    buttons[3].sprite = new Sprite("elements\\schedule.smk");
    buttons[3].sprite->loc_x = 0x165;
    buttons[3].sprite->loc_y = 0x1b5;

    buttons[4].sprite = new Sprite("elements\\mainmenu.smk");
    buttons[4].sprite->loc_x = 0x1d1;
    buttons[4].sprite->loc_y = 0x1b6;

    buttons[5].sprite = new Sprite("elements\\quit.smk");
    buttons[5].sprite->loc_x = 0x22a;
    buttons[5].sprite->loc_y = 0x1b6;

    // Configure all 6 buttons using common settings (do-while counting down)
    {
        int count = 6;
        IconBarButton* pBtn = buttons;
        do {
            Sprite* btn = pBtn->sprite;
            btn->flags &= ~2;
            btn->SetState(2);
            btn->SetRange(0, 1, 1);
            btn->SetRange(1, 2, 2);
            btn->priority = 1001;
            pBtn++;
        } while (--count);
    }

    // Set button bounds in the order they appear in assembly
    buttons[0].bounds.left = 0x5e;
    buttons[0].bounds.right = 0x9f;
    buttons[0].bounds.top = 0x1b5;
    buttons[1].bounds.top = 0x1b5;
    buttons[0].bounds.bottom = 0x1d3;
    buttons[1].bounds.left = 0xad;
    buttons[1].bounds.right = 0xf9;
    buttons[1].bounds.bottom = 0x1d6;
    buttons[2].bounds.bottom = 0x1d6;
    buttons[2].bounds.left = 0x115;
    buttons[2].bounds.top = 0x1b4;
    buttons[2].bounds.right = 0x155;
    buttons[3].bounds.top = 0x1b5;
    buttons[4].bounds.top = 0x1b6;
    buttons[5].bounds.top = 0x1b6;
    buttons[3].bounds.left = 0x165;
    buttons[3].bounds.bottom = 0x1d2;
    buttons[4].bounds.bottom = 0x1d2;
    buttons[3].bounds.right = 0x1ae;
    buttons[4].bounds.left = 0x1d1;
    buttons[4].bounds.right = 0x207;
    buttons[5].bounds.left = 0x22a;
    buttons[5].bounds.right = 0x270;
    buttons[5].bounds.bottom = 0x1d5;

    // Message templates
    SC_Message* pMsg = &buttons[0].message;
    pMsg->targetAddress = 10;
    pMsg->sourceAddress = 1;
    pMsg->command = 6;
    pMsg->priority = 5;

    pMsg = &buttons[4].message;
    pMsg->targetAddress = 10;
    pMsg->sourceAddress = 1;
    pMsg->command = 6;
    pMsg->priority = 5;

    SC_Message* pMsg5 = &buttons[5].message;
    pMsg5->targetAddress = 2;
    pMsg5->priority = 5;
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

    // Check selected character and set iconbar state
    if (DAT_00435a80 != 0) {
        iconbarSprite->SetState2(DAT_00435a80->characterType + 1);
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
        buttons[1].clickSound = (Sample*)soundList->Register("audio\\Snd0023.wav");
        buttons[2].clickSound = (Sample*)soundList->Register("audio\\Snd0024.wav");
        buttons[3].clickSound = (Sample*)soundList->Register("audio\\Snd0025.wav");
    }
}

/* Function start: 0x402FD0 */
void IconBar::CleanupIconBar(SC_Message* msg) {
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
    ok = (barBounds.left <= msg->clickPos.x && barBounds.right >= msg->clickPos.x &&
          barBounds.top <= msg->clickPos.y && barBounds.bottom >= msg->clickPos.y);
    if (!ok) return 0;

    // Only proceed for specific mouse states
    if (msg->mouseX >= 2) {
        // Loop through buttons
        for (i = 0; i < 6; i++) {
            if (buttons[i].enabled) {
                // Button bounds check
                ok = (buttons[i].bounds.left <= msg->clickPos.x && buttons[i].bounds.right >= msg->clickPos.x &&
                      buttons[i].bounds.top <= msg->clickPos.y && buttons[i].bounds.bottom >= msg->clickPos.y);

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
                    msg->targetAddress = pSrc->targetAddress;
                    msg->sourceAddress = pSrc->sourceAddress;
                    msg->command = pSrc->command;
                    msg->data = pSrc->data;
                    msg->priority = pSrc->priority;
                    msg->param1 = pSrc->param1;
                    msg->param2 = pSrc->param2;
                    msg->clickPos.x = pSrc->clickPos.x;
                    msg->clickPos.y = pSrc->clickPos.y;
                    msg->mouseX = pSrc->mouseX;
                    msg->mouseY = pSrc->mouseY;
                    msg->field_b4 = pSrc->field_b4;
                    msg->field_b8 = pSrc->field_b8;
                    msg->userPtr = pSrc->userPtr;
                    break;
                }
            }
        }
    }

    return 1;
}

/* Function start: 0x403230 */
void IconBar::Update(int param1, int param2) {
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
