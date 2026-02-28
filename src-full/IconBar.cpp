#include "IconBar.h"
#include <string.h>
#include "Sprite.h"
#include "Memory.h"
#include "Parser.h"
#include "globals.h"
#include "SC_Question.h"

/* Function start: 0x42D460 */
IconBar::IconBar() {
    handlerId = 0;
}

IconBar::~IconBar() {
}

/* Function start: 0x402ED0 */ /* DEMO ONLY - no full game match */
void IconBar::InitIconBar(SC_Message* msg) {
}

/* Function start: 0x402FD0 */ /* DEMO ONLY - no full game match */
void IconBar::CleanupIconBar(SC_Message* msg) {
}

/* Function start: 0x403040 */ /* DEMO ONLY - no full game match */
int IconBar::CheckButtonClick(SC_Message* msg) {
    return 0;
}

/* Function start: 0x403230 */ /* DEMO ONLY - no full game match */
void IconBar::Update(int param1, int param2) {
}

/* Function start: 0x403300 */ /* DEMO ONLY - no full game match */
void IconBar::PlayButtonSound(int buttonIndex) {
}
