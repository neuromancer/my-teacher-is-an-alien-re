#include "SCI_Dialog.h"
#include "Memory.h"
#include "Sprite.h"
#include "Queue.h"
#include "SC_Question.h"
#include <stdio.h>
#include <string.h>
#include "string.h"

/* Function start: 0x407EC6 */
int SCI_Dialog::LBLParse(char* param_1)
{
    char local_34[128];
    char key[32];
    
    sscanf(param_1, "%s", key);
    
    if (_strcmpi(key, "BACKGROUND") == 0) {
        ShowError("SCI_Dialog::LBLParse %s: a background has already been loaded", key);
    }
    else if (_strcmpi(key, "PALETTE") == 0) {
        ShowError("SCI_Dialog::LBLParse %s: a palette has already been loaded", key);
        sscanf(param_1, "%*s %s", local_34); // Parse filename but ignore?
    }
    else if (_strcmpi(key, "BUTTON") == 0) {
        Sprite* s = new Sprite(NULL);
        this->SetButton(s);
        Parser::ProcessFile(s, this, 0);
    }
    else if (_strcmpi(key, "AMBIENT_SPRS") == 0) {
        ShowError("SCI_Dialog::LBLParse %s: ambient sprites have already been loaded", key);
    }
    else if (_strcmpi(key, "HILITE") == 0) {
        Sprite* s = new Sprite(NULL);
        this->SetHilite(s);
        Parser::ProcessFile(s, this, 0);
    }
    else if (_strcmpi(key, "HANDLE") == 0) {
        // Param 0x8c? 
        // SearchScreen is 0x638+? 
        // 0x8c is NOT in SearchScreen fields which start at 0x600?
        // Wait, SearchScreen inherits Parser (0x88). 0x88, 0x8c are available?
        // Parser has fields up to 0x88 (0x84 actually, size is 0x88).
        // 0x88 -> Parser end.
        // So 0x8c is free if SearchScreen adds fields? 
        // My SearchScreen definition: field_600.
        // It has padding [0x600 - sizeof(Parser)].
        // So 0x8c is inside padding?
        // If SCI_Dialog uses 0x8c, I should expose it in SearchScreen or SCI_Dialog.
        // I'll add field_8c to SearchScreen or SCI_Dialog (via shadowing padding).
        // For now, I'll access via byte offset.
        
        sscanf(param_1, "%*s %d", (int*)((char*)this + 0x8c));
    }
    else if (_strcmpi(key, "AMBIENTSCONTROLLER") == 0) {
        int val;
        sscanf(param_1, "%*s %d", &val);
        int i = 0;
        int copy = val;
        // logic to check digits...
        int digits = 1;
        while (copy >= 10) {
            digits++;
            copy /= 10;
        }
        
        copy = val;
        if (digits > 0) {
            do {
                int digit = copy % 10;
                this->field_608[4 + digit] = 0; // 0x618 + digit*4
                copy /= 10;
                digits--;
            } while (digits != 0);
        }
    }
    else if (_strcmpi(key, "QUESTION") == 0) {
        int id;
        sscanf(param_1, "%*s %d", &id);
        
        SC_Question* q = new SC_Question(id);
        // Logic checking if q is valid...
        // ...
        
        // Insert into queue at 0x610
        Queue* queue = this->GetQuestionQueue();
        // ... Queue insertion logic ...
        // Using generic insert if valid
    }
    else {
        // Fallback
        if (SearchScreen::LBLParse(param_1)) {
            return 1;
        }
        // if not handled by SearchScreen?
        // Original calls FUN_00418b30(s_SCI_Dialog_004356c0); which logs unknown label.
        return Parser::LBLParse("SCI_Dialog");
    }
    
    return 0;
}
