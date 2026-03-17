#include "SCI_SearchScreen.h"
#include "SpriteAction.h"
#include "Parser.h"
#include <string.h>
#include <stdio.h>

extern void ParseSpriteAction(void*, void*);
extern void EnqueueSpriteAction(void*);

/* Function start: 0x40B420 */
SCI_SearchScreen::SCI_SearchScreen()
{
    handlerId = 0x48;
}

/* Function start: 0x40B4B0 */
SCI_SearchScreen::~SCI_SearchScreen()
{
    ShutDown(0);
}

/* Function start: 0x40B25E */
int SCI_SearchScreen::LBLParse(char* line) {
    char token[32];

    sscanf(line, "%s", token);

    if (strcmp(token, "POSTMESSAGE") == 0) {
        SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ParseSpriteAction(&action, this);
        EnqueueSpriteAction((void*)&action);
    } else if (strcmp(token, "ENDPERIODANNOUNCE") == 0) {
        int index, value;
        sscanf(line, "%s %d %d", token, &index, &value);
        ((int*)&statusPtr)[index + 20] = value;
    } else if (strcmp(token, "ANNOUNCE") == 0) {
        int index, value;
        sscanf(line, "%s %d %d", token, &index, &value);
        ((int*)&statusPtr)[index] = value;
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SCI_SearchScreen");
    }

    return 0;
}
