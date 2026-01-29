#include "SCI_AfterSchoolMenu.h"
#include <string.h>
#include <stdio.h>
#include <new.h>
#include <mss.h>
#include "Memory.h"
#include "globals.h"
#include "Palette.h"
#include "Sprite.h"
#include "Sample.h"
#include "MouseControl.h"
#include "MMPlayer.h"
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

// Global state for SCI_AfterSchoolMenu
int DAT_00435448 = 0;  // SCI_AfterSchoolMenu initialized flag

extern "C" {
    char* CDData_FormatPath(char*, ...);
}

/* Function start: 0x405D20 */
int SCI_AfterSchoolMenu::LBLParse(char* line) {
    char label[32];
    char filepath[64];
    MMPlayer* mc;
    Palette* pal;
    T_Hotspot* hotspot;
    OptionMenu* optMenu;

    sscanf(line, "%s", label);

    if (strcmp(label, "BACK") == 0) {
        mc = new MMPlayer();
        background = mc;
        Parser::ProcessFile(mc, this, (char*)0);
    } else if (strcmp(label, "PALE") == 0) {
        sscanf(line, "%s %s", label, filepath);
        if (palette == 0) {
            pal = new Palette();
            palette = pal;
            pal->Load(CDData_FormatPath(filepath));
        }
    } else if (strcmp(label, "CHAR") == 0) {
        hotspot = new T_Hotspot();
        characters[characterCount] = hotspot;
        Parser::ProcessFile(hotspot, this, (char*)0);
        characterCount = characterCount + 1;
    } else if (strcmp(label, "CANCEL") == 0) {
        hotspot = new T_Hotspot();
        cancelButton = hotspot;
        Parser::ProcessFile(hotspot, this, (char*)0);
    } else if (strcmp(label, "OKAY") == 0) {
        hotspot = new T_Hotspot();
        goButton = hotspot;
        Parser::ProcessFile(hotspot, this, (char*)0);
    } else if (strcmp(label, "OPTION_MENU") == 0) {
        optMenu = new OptionMenu();
        choiceScreen = optMenu;
        Parser::ProcessFile(optMenu, this, (char*)0);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SCI_AfterSchoolMenu");
    }

    return 0;
}

/* Function start: 0x404CA0 */
SCI_AfterSchoolMenu::SCI_AfterSchoolMenu() {
    int i;

    // Note: IconBar constructor is called automatically via inheritance

    // Zero all fields from palette to the end of the class (0x2a dwords)
    memset(&palette, 0, 0x2a * 4);

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

    // Create character objects
    DAT_00435a74 = new Character("peter");
    DAT_00435a78 = new Character("susan");
    DAT_00435a7c = new Character("duncan");
}

/* Function start: 0x404E60 */
SCI_AfterSchoolMenu::~SCI_AfterSchoolMenu() {
    int i;

    if (palette != 0) {
        delete palette;
        palette = 0;
    }

    if (background != 0) {
        delete background;
        background = 0;
    }

    if (choiceScreen != 0) {
        delete choiceScreen;
        choiceScreen = 0;
    }

    if (goButton != 0) {
        delete goButton;
        goButton = 0;
    }

    if (cancelButton != 0) {
        delete cancelButton;
        cancelButton = 0;
    }

    for (i = 0; i < 3; i++) {
        if (characters[i] != 0) {
            delete characters[i];
            characters[i] = 0;
        }
    }

    for (i = 0; i < 8; i++) {
        Sample* smp = sounds[i].sample;
        if (smp != 0) {
            delete smp;
            sounds[i].sample = 0;
        }
    }
}

/* Function start: 0x404FF0 */
void SCI_AfterSchoolMenu::Init(SC_Message* msg) {
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
int SCI_AfterSchoolMenu::Exit(SC_Message* msg) {
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
            T_Hotspot* obj = characters[i];
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

        IconBar::CleanupIconBar(msg);
        WriteToMessageLog("EXIT AFTERSCHOOL MENU\n");
    }

    return 1;
}

/* Function start: 0x4052D0 */
int SCI_AfterSchoolMenu::AddMessage(SC_Message* msg) {
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
            FillOptionQueue();
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
            msg->targetAddress = 4;  // Handler 4
        } else {
            int handlerVal = (currentCharacterIndex == 1) ? 11 : 16;
            msg->targetAddress = handlerVal;
        }
        msg->command = handlerId;
        msg->data = field_8C;
        msg->priority = 5;
    }

    return 1;
}

/* Function start: 0x405420 */
int SCI_AfterSchoolMenu::ShutDown(SC_Message* msg) {
    return handlerId == msg->targetAddress;
}

/* Function start: 0x405490 */
void SCI_AfterSchoolMenu::Update(int param1, int param2) {
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
    DisplaySubmenuHover(mouseX, mouseY);

    // Update iconbar
    IconBar::Update(param1, param2);

    // Render background sprite
    if (background != 0) {
        background->Draw();
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
void SCI_AfterSchoolMenu::ResetHoverState() {
    hoverCharacterIndex = -1;
    hoverSubmenuIndex = -1;
    updateFlag = 0;
    confirmFlag = 0;
}

/* Function start: 0x4057A0 */
int SCI_AfterSchoolMenu::IsSelectionComplete() {
    if (currentCharacterIndex != -1 && currentSubmenuIndex != -1) {
        return 1;
    }
    return 0;
}

/* Function start: 0x4057C0 */
void SCI_AfterSchoolMenu::ResetSelection() {
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
void SCI_AfterSchoolMenu::PlayCharacterSound(int soundIndex) {
    int idx;
    SoundSlot* slot;

    PlayButtonSound(-1);

    if (currentSound != 0) {
        currentSound->~Sample();
        currentSound = 0;
    }

    // Force recalculation to match assembly reloading behavior
    idx = currentCharacterIndex * 2 + soundIndex;
    slot = &sounds[idx];

    if (slot->enabled != 0) {
        slot->enabled = 0;
        currentSound = sounds[currentCharacterIndex * 2 + soundIndex].sample;
        currentSound->Play(100, 1);
    }
}

/* Function start: 0x405590 */
void SCI_AfterSchoolMenu::DisplaySubmenuHover(int mouseX, int mouseY) {
    MousePoint pos;

    pos.x = 0;
    pos.y = 0;
    pos.x = mouseX;
    pos.y = mouseY;

    ResetHoverState();
    
    ProcessCharacterHover(pos);
    ProcessSubmenuHover(pos);
    ProcessGoButtonHover(pos, goButton, &confirmFlag);

    if (sounds[0].enabled != 0) {
        hoverCharacterIndex = 1;
    }

    if (prevCharacter != -1 && currentCharacterIndex != prevCharacter) {
        characters[prevCharacter]->SetState(0);
        currentSubmenuIndex = -1;
        prevSubmenu = -1;
        prevCharacter = currentCharacterIndex;
        FillOptionQueue();
    }

    if (currentCharacterIndex != -1) {
        if (hoverCharacterIndex != currentCharacterIndex) {
            characters[currentCharacterIndex]->SetState(1);
            prevCharacter = currentCharacterIndex;
        }
        if (currentCharacterIndex != -1 && hoverCharacterIndex == currentCharacterIndex) {
            characters[currentCharacterIndex]->SetState(2);
        }
    }

    if (prevSubmenu != -1 && currentSubmenuIndex != prevSubmenu) {
        SetSubmenuOption(prevSubmenu, 0);
    }

    if (currentSubmenuIndex != -1) {
        if (hoverSubmenuIndex != currentSubmenuIndex) {
            SetSubmenuOption(currentSubmenuIndex, 1);
            prevSubmenu = currentSubmenuIndex;
        }
        if (currentSubmenuIndex != -1 && hoverSubmenuIndex == currentSubmenuIndex) {
            SetSubmenuOption(currentSubmenuIndex, 2);
        }
    }

    if (needsRefresh != 0 && currentCharacterIndex != -1) {
        FillOptionQueue();
        needsRefresh = 0;
    }
}

/* Function start: 0x405AA0 */
void SCI_AfterSchoolMenu::ProcessGoButtonHover(MousePoint pt, T_Hotspot* button, int* outConfirmFlag) {
    if (IsSelectionComplete()) {
        int isHit = 0;
        if (button->enabled != 0) {
            if (pt.x >= button->rect.left && pt.x <= button->rect.right &&
                pt.y >= button->rect.top && pt.y <= button->rect.bottom) {
                isHit = 1;
            }
        }
        if (isHit != 0) {
            *outConfirmFlag = 1;
            button->SetState(2);
        } else {
            *outConfirmFlag = 0;
            button->SetState(1);
        }
    } else {
        *outConfirmFlag = 0;
        button->SetState(0);
    }
    cancelButton->SetState(0);
}

/* Function start: 0x405880 */
void SCI_AfterSchoolMenu::RenderGoButton() {
    if (goButton != 0) {
        goButton->Do();
    }
    if (cancelButton != 0) {
        cancelButton->Do();
    }
}

/* Function start: 0x4058B0 */
void SCI_AfterSchoolMenu::RenderCharacters() {
    int i;
    T_Hotspot* charSprite;

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
void SCI_AfterSchoolMenu::RenderChoiceScreen(int characterIndex) {
    int charIdx;

    charIdx = currentCharacterIndex;
    if (charIdx >= 0 && charIdx <= 2) {
        choiceScreen->Render(charIdx);
    }
}

/* Function start: 0x405900 */
void SCI_AfterSchoolMenu::ProcessCharacterHover(MousePoint pt) {
    int i;
    T_Hotspot** charPtr;
    int isHit;
    int noHit = 1;

    for (i = 0, charPtr = characters; i < 3; i++, charPtr++) {
        isHit = 0;
        {
            MousePoint local_pt = pt;
            if ((*charPtr)->enabled != 0) {
                if ((*charPtr)->rect.left > local_pt.x || (*charPtr)->rect.right < local_pt.x ||
                    (*charPtr)->rect.top > local_pt.y || (*charPtr)->rect.bottom < local_pt.y) {
                    isHit = 0;
                } else {
                    isHit = 1;
                }
            }
        }

        if (isHit != 0) {
            hoverCharacterIndex = i;

            if ((*charPtr)->GetState() != 2) {
                (*charPtr)->SetState(3);
            }

            if (prevHoverCharacter == -1) {
                prevHoverCharacter = i;
                (*charPtr)->SetState(3);
            } else if (i != prevHoverCharacter) {
                characters[prevHoverCharacter]->SetState(0);
                prevHoverCharacter = i;
            }
            noHit = 0;
        }
    }

    if (noHit != 0 && prevHoverCharacter != -1) {
        characters[prevHoverCharacter]->SetState(0);
        prevHoverCharacter = -1;
    }
}

/* Function start: 0x405BB0 */
void SCI_AfterSchoolMenu::ProcessSubmenuHover(MousePoint pt) {
    int indexOut = 0;
    int hitOut = 1;

    if (currentCharacterIndex != -1) {
        if (choiceScreen->HitTest(pt, &indexOut, &hitOut)) {
            hoverSubmenuIndex = indexOut;
        }

        if (hoverSubmenuIndex != prevSubmenuHover && prevSubmenuHover != -1) {
            indexOut = prevSubmenuHover;
            SetSubmenuOption(indexOut, 0);
        }

        if (hitOut != 0) {
            prevSubmenuHover = -1;
        } else {
            prevSubmenuHover = hoverSubmenuIndex;
        }
    }
}

/* Function start: 0x405CC0 */
void SCI_AfterSchoolMenu::SetCharacterOption(int characterIndex) {
    if (choiceScreen != 0) {
        choiceScreen->SelectCharacter(characterIndex);
    } else {
        ShowError("Missing option_menu in GetOptionQ - opmenu.cpp");
    }
}

/* Function start: 0x405CF0 */
void SCI_AfterSchoolMenu::SetSubmenuOption(int submenuIndex, int state) {
    if (choiceScreen != 0) {
        choiceScreen->SetOptionState(submenuIndex, state);
    } else {
        ShowError("Missing option_menu in SetOptionState");
    }
}

/* Function start: 0x405440 */
void SCI_AfterSchoolMenu::PlaySoundsIfNeeded() {
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
void SCI_AfterSchoolMenu::FillOptionQueue() {
    int charIdx;

    charIdx = currentCharacterIndex;
    if (charIdx < 0 || charIdx > 2) {
        ShowError("Error in DMChoScr.cpp - FillOptionQueue");
    } else {
        SetCharacterOption(charIdx);
    }
    SetSubmenuOption(-5, 0);
}
