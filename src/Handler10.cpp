#include "Handler10.h"
#include <string.h>
#include <stdio.h>
#include <new.h>
#include <mss.h>
#include "Memory.h"
#include "globals.h"
#include "Palette.h"
#include "Sprite.h"
#include "Sample.h"
#include "Mouse.h"
#include "MouseControl.h"
#include "Hotspot.h"
#include "InputManager.h"
#include "SC_Question.h"
#include "OptionMenu.h"

// Include string.h for ShowError and WriteToMessageLog
#include "string.h"

// CharSprite/CharButton destructor with SEH (complex, still in stubs)
extern void __fastcall FUN_004092e0(void*);  // CharSprite cleanup/destructor

// Character object constructor (implemented in Character.cpp)
#include "Character.h"

// MouseControl methods are now in MouseControl.h:
// IconBar::PlayButtonSound is now in IconBar.h/cpp

// Global state for Handler10
int DAT_00435448 = 0;  // Handler10 initialized flag
// DAT_00435a80 is declared as int in globals.h, cast as needed

/* Function start: 0x404CA0 */
Handler10::Handler10() {
    int i;
    int* ptrClear;

    // Note: IconBar constructor is called automatically via inheritance

    // Zero 0x2a dwords at offset 0x600 (palette through sounds array initialization area)
    ptrClear = (int*)&palette;
    for (i = 0x2a; i != 0; i--) {
        *ptrClear = 0;
        ptrClear++;
    }

    // Initialize state fields
    needsRefresh = 0;
    savedCharacterIndex = -1;
    savedSubmenuIndex = -1;
    isInitialized = 0;
    prevHoverCharacter = -1;
    prevCharacter = -1;
    prevSubmenuHover = -1;
    prevSubmenu = -1;

    // Set handlerId to 10
    handlerId = 10;
    field_8C = 1;

    // Parse demo.mis file
    ParseFile(this, "mis\\demo.mis", (char*)0);

    // Initialize sound slots - set all enabled flags to 1
    for (i = 0; i < 8; i++) {
        sounds[i].enabled = 1;
    }

    // Create peter character object
    DAT_00435a74 = new Character("peter");

    // Create susan character object
    DAT_00435a78 = new Character("susan");

    // Create duncan character object
    DAT_00435a7c = new Character("duncan");
}

/* Function start: 0x404E60 */
Handler10::~Handler10() {
    int i;
    Hotspot* obj;

    // Cleanup palette
    if (palette != 0) {
        delete palette;
        palette = 0;
    }

    // Cleanup background sprite
    if (background != 0) {
        delete background;
        background = 0;
    }

    // Cleanup choice screen (OptionMenu)
    if (choiceScreen != 0) {
        delete choiceScreen;
        choiceScreen = 0;
    }

    // Cleanup go button
    if (goButton != 0) {
        FUN_004092e0(goButton);
        delete goButton;
        goButton = 0;
    }

    // Cleanup cancel button
    if (cancelButton != 0) {
        FUN_004092e0(cancelButton);
        delete cancelButton;
        cancelButton = 0;
    }

    // Cleanup character sprites
    for (i = 0; i < 3; i++) {
        obj = characters[i];
        if (obj != 0) {
            FUN_004092e0(obj);
            delete obj;
            characters[i] = 0;
        }
    }

    // Cleanup sound samples
    for (i = 0; i < 8; i++) {
        Sample* smp = sounds[i].sample;
        if (smp != 0) {
            smp->Unload();
            delete smp;
            sounds[i].sample = 0;
        }
    }
}

/* Function start: 0x404FF0 */
void Handler10::Init(SC_Message* msg) {
    int i;
    int soundNum;
    char filename[32];
    Sample* sample;

    if (DAT_00435448 == 0) {
        DAT_00435448 = 1;
        WriteToMessageLog("\nENTER AFTERSCHOOL MENU");

        IconBar::InitIconBar(msg);

        // Clear some fields
        buttons[0].enabled = 0;
        buttons[4].enabled = 0;

        // Set palette if exists
        if (palette != 0) {
            if (g_ZBufferManager_0043698c->m_fieldA8 != 0) {
                WriteToMessageLog("ddouble palette");
            }
            g_ZBufferManager_0043698c->m_fieldA8 = palette;
        }

        // Copy saved state to current state
        currentCharacterIndex = savedCharacterIndex;
        currentSubmenuIndex = savedSubmenuIndex;

        // If character was saved, initialize choice screen
        if (savedCharacterIndex != -1) {
            SetCharacterOption(currentCharacterIndex);
            isInitialized = 1;
        }

        // Reset hover states
        hoverCharacterIndex = -1;
        hoverSubmenuIndex = -1;
        confirmFlag = 0;
        updateFlag = 0;

        // Create and load sound samples
        for (i = 0; i < 8; i++) {
            soundNum = i + 1;
            sprintf(filename, "audio\\Snd00%2.2d.wav", soundNum);

            sample = new Sample();
            sounds[i].sample = sample;
            sample->Load(filename);
        }

        // Copy ambient sound pointer
        ambientSound = sounds[7].sample;

        // If first sound slot enabled, start voiceover
        if (sounds[0].enabled != 0) {
            currentCharacterIndex = 1;
            currentSound = sounds[0].sample;
        }

        playSoundsFlag = 1;
    }
}

/* Function start: 0x405190 */
int Handler10::Exit(SC_Message* msg) {
    int i;
    Sample* smp;

    if (DAT_00435448 != 0) {
        DAT_00435448 = 0;

        // Exit background sprite
        if (background != 0) {
            background->StopAll();
        }

        // Exit choice screen
        if (choiceScreen != 0) {
            choiceScreen->Deactivate();
        }

        // Exit go button
        if (goButton != 0) {
            goButton->Exit();
        }

        // Exit cancel button
        if (cancelButton != 0) {
            cancelButton->Exit();
        }

        // Exit character sprites
        for (i = 0; i < 3; i++) {
            Hotspot* obj = characters[i];
            if (obj != 0) {
                obj->Exit();
            }
        }

        // Save current state
        isInitialized = 1;
        needsRefresh = 1;
        savedCharacterIndex = currentCharacterIndex;
        savedSubmenuIndex = currentSubmenuIndex;

        // Stop ambient sound
        if (ambientSound != 0) {
            ambientSound->~Sample();
        }
        ambientSound = 0;

        // Cleanup first sound slot
        smp = sounds[0].sample;
        if (smp != 0) {
            smp->Unload();
            delete smp;
            sounds[0].sample = 0;
        }

        // Stop current sound
        if (currentSound != 0) {
            currentSound->~Sample();
        }
        currentSound = 0;

        // Cleanup all sound samples
        for (i = 0; i < 8; i++) {
            smp = sounds[i].sample;
            if (smp != 0) {
                smp->Unload();
                delete smp;
                sounds[i].sample = 0;
            }
        }

        IconBar::CleanupIconBar();
        WriteToMessageLog("EXIT AFTERSCHOOL MENU\n");
    }

    return 1;
}

/* Function start: 0x4052D0 */
int Handler10::HandleMessage(SC_Message* msg) {
    int result;

    // Check iconbar button click first
    result = IconBar::CheckButtonClick(msg);
    if (result != 0) {
        return 1;
    }

    // If voiceover playing, return
    if (sounds[0].enabled != 0) {
        return 1;
    }

    // Check click type - need at least 2 (left click)
    if (msg->mouseX < 2) {
        return 1;
    }

    // Handle character selection click
    if (hoverCharacterIndex != -1) {
        currentCharacterIndex = hoverCharacterIndex;

        if (prevCharacter == -1) {
            SetCharacterOption(currentCharacterIndex);
        }

        characters[currentCharacterIndex]->SetState(1);
        isInitialized = 1;

        // Set selected character global
        if (currentCharacterIndex == 0) {
            DAT_00435a80 = (int)DAT_00435a74;  // peter
        } else if (currentCharacterIndex == 1) {
            DAT_00435a80 = (int)DAT_00435a78;  // susan
        } else {
            DAT_00435a80 = (int)DAT_00435a7c;  // duncan
        }

        // Update iconbar sprite with character state
        iconbarSprite->SetState2(*(int*)DAT_00435a80 + 1);

        PlayCharacterSound(2);
    }
    // Handle submenu selection click
    else if (hoverSubmenuIndex != -1 && isInitialized == 1) {
        currentSubmenuIndex = hoverSubmenuIndex;
        SetSubmenuOption(hoverSubmenuIndex, 1);
        PlayCharacterSound(1);
    }

    // Check if selection is complete
    result = IsSelectionComplete();
    if (result != 0 && confirmFlag != 0) {
        // Start game with selected character
        if (currentCharacterIndex == 0) {
            msg->command = 4;  // Handler 4
        } else {
            int handlerVal = ((currentCharacterIndex == 1) ? -5 : 0) + 0x10;
            msg->command = handlerVal;
        }
        msg->targetAddress = handlerId;
        msg->priority = 5;
        msg->sourceAddress = field_8C;
    }

    return 1;
}

/* Function start: 0x405420 */
int Handler10::Draw(SC_Message* msg) {
    int diff;

    diff = handlerId - msg->command;
    return (diff < 2) ? 1 : 0;
}

/* Function start: 0x405490 */
void Handler10::Update(SC_Message* param1, int param2) {
    int mouseX;
    int mouseY;
    int* mousePtr;
    HSAMPLE sampleHandle;
    int status;

    if (handlerId != param2) {
        return;
    }

    if (DAT_00435448 == 0) {
        return;
    }

    // Get mouse position
    mouseX = 0;
    mouseY = 0;
    mousePtr = (int*)g_InputManager_00436968->pMouse;
    if (mousePtr != 0) {
        mouseY = mousePtr[1];
        mouseX = mousePtr[0];
    }

    // Render all elements and process hover
    RenderAll(mouseX, mouseY);

    // Draw iconbar
    IconBar::DrawIconBar((int)param1, param2);

    // Render background sprite
    if (background != 0) {
        background->DoAll();
    }

    // Render buttons
    RenderGoButton();
    RenderCharacters();

    // Render choice screen if valid character selected
    if (choiceScreen != 0) {
        RenderChoiceScreen(currentCharacterIndex);
    }

    // Draw mouse cursor
    g_Mouse_00436978->DrawCursor();

    // Play sounds if needed
    PlaySoundsIfNeeded();

    // Check if current sound finished playing
    if (currentSound != 0) {
        sampleHandle = currentSound->m_sample;
        if (sampleHandle != 0) {
            if (currentSound->m_size == *(int*)((char*)sampleHandle + 0xc)) {
                status = AIL_sample_status(sampleHandle);
                if (status == 4) {  // Playing
                    return;
                }
            }
        }

        // Sound finished
        currentSound = 0;

        if (sounds[0].enabled != 0) {
            sounds[0].enabled = 0;
            characters[currentCharacterIndex]->SetState(0);
            ResetSelection();
        }
    }
}

/* Function start: 0x405780 */
void Handler10::ResetHoverState() {
    hoverCharacterIndex = -1;
    hoverSubmenuIndex = -1;
    updateFlag = 0;
    confirmFlag = 0;
}

/* Function start: 0x4057A0 */
int Handler10::IsSelectionComplete() {
    if (currentCharacterIndex != -1 && currentSubmenuIndex != -1) {
        return 1;
    }
    return 0;
}

/* Function start: 0x4057C0 */
void Handler10::ResetSelection() {
    savedCharacterIndex = -1;
    savedSubmenuIndex = -1;
    currentCharacterIndex = -1;
    currentSubmenuIndex = -1;
    needsRefresh = 0;
    isInitialized = 0;
    prevHoverCharacter = -1;
    prevCharacter = -1;
    prevSubmenuHover = -1;
    prevSubmenu = -1;
}

/* Function start: 0x405810 */
void Handler10::PlayCharacterSound(int soundIndex) {
    int slotIndex;
    SoundSlot* slot;

    PlayButtonSound(-1);

    // Stop current sound
    if (currentSound != 0) {
        currentSound->~Sample();
        currentSound = 0;
    }

    // Calculate slot index: (soundIndex + currentCharacterIndex * 2)
    slotIndex = soundIndex + currentCharacterIndex * 2;
    slot = &sounds[slotIndex];

    if (slot->enabled != 0) {
        slot->enabled = 0;
        currentSound = slot->sample;
        currentSound->Play(100, 1);
    }
}

/* Function start: 0x405880 */
void Handler10::RenderGoButton() {
    if (goButton != 0) {
        goButton->Do();
    }
    if (cancelButton != 0) {
        cancelButton->Do();
    }
}

/* Function start: 0x4058B0 */
void Handler10::RenderCharacters() {
    int i;
    Hotspot* charSprite;

    for (i = 0; i < 3; i++) {
        charSprite = characters[i];
        if (charSprite != 0) {
            charSprite->Do();
        } else {
            ShowError("Error in demo.cpp - missing character hotspot");
        }
    }
}

/* Function start: 0x4058E0 */
void Handler10::RenderChoiceScreen(int characterIndex) {
    int charIdx;

    charIdx = currentCharacterIndex;
    if (charIdx >= 0 && charIdx <= 2) {
        choiceScreen->Render(charIdx);
    }
}

/* Function start: 0x405900 */
void Handler10::RenderAll(int mouseX, int mouseY) {
    int noHover;
    int i;
    Hotspot** charPtr;
    Hotspot* charSprite;
    int isHit;
    int state;
    int prevChar;

    noHover = 1;
    i = 0;
    charPtr = &characters[0];

    do {
        charSprite = *charPtr;

        // Check if character is enabled
        if (charSprite->enabled == 0) {
            // Character not enabled, skip
        } else {
            // Hit test mouse against character bounds
            if (mouseX < charSprite->rect_x ||
                mouseX > charSprite->rect_w ||
                mouseY < charSprite->rect_y ||
                mouseY > charSprite->rect_h) {
                isHit = 0;
            } else {
                isHit = 1;
            }

            if (isHit != 0) {
                hoverCharacterIndex = i;

                // Get current state
                state = charSprite->GetState();
                if (state != 2) {
                    charSprite->SetState(3);
                }

                // Update prev hover character
                prevChar = prevHoverCharacter;
                if (prevChar == -1) {
                    prevHoverCharacter = i;
                    charSprite->SetState(3);
                } else if (i != prevChar) {
                    characters[prevChar]->SetState(0);
                    prevHoverCharacter = i;
                }

                noHover = 0;
            }
        }

        charPtr++;
        i++;
    } while (i < 3);

    // If nothing was hovered, reset previous character
    if (noHover != 0) {
        if (prevHoverCharacter != -1) {
            characters[prevHoverCharacter]->SetState(0);
            prevHoverCharacter = -1;
        }
    }
}

/* Function start: 0x405CC0 */
void Handler10::SetCharacterOption(int characterIndex) {
    if (choiceScreen != 0) {
        choiceScreen->SelectCharacter(characterIndex);
    } else {
        ShowError("Missing option_menu in GetOptionQ - opmenu.cpp");
    }
}

/* Function start: 0x405CF0 */
void Handler10::SetSubmenuOption(int submenuIndex, int state) {
    if (choiceScreen != 0) {
        choiceScreen->SetOptionState(submenuIndex, state);
    } else {
        ShowError("Missing option_menu in SetOptionState");
    }
}

/* Function start: 0x405440 */
void Handler10::PlaySoundsIfNeeded() {
    if (playSoundsFlag != 0) {
        playSoundsFlag = 0;

        // Play ambient sound
        if (ambientSound != 0) {
            ambientSound->Play(100, 0);
        }

        // Play intro voice if enabled
        if (sounds[0].enabled != 0 && sounds[0].sample != 0) {
            sounds[0].sample->Play(100, 1);
        }
    }
}

/* Function start: 0x405C80 */
void Handler10::SetCharacterOptionInternal() {
    int charIdx;

    charIdx = currentCharacterIndex;
    if (charIdx < 0 || charIdx > 2) {
        ShowError("Error in DMChoScr.cpp - FillOptionQueue");
    } else {
        SetCharacterOption(charIdx);
    }
    SetSubmenuOption(-5, 0);
}
