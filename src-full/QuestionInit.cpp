#include "QuestionInit.h"
#include "FlagArray.h"
#include <string.h>
#include <stdio.h>

extern FlagArray* g_FlagManager_00435a84;

/* Function start: 0x422880 */
QuestionInit::QuestionInit(const char* filename) {
    ParseFile(this, (char*)filename, (char*)0);
}

/* Function start: 0x4228F0 */
int QuestionInit::LBLParse(char* line) {
    char keyword[32];
    int index;

    sscanf(line, " %s ", keyword);

    if (strcmp(keyword, "DISABLE") == 0) {
        sscanf(line, "%s %d", keyword, &index);
        g_FlagManager_00435a84->SetFlag(index, 1);
        return 0;
    } else if (strcmp(keyword, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("QuestionInit");
    }

    return 0;
}

QuestionInit::~QuestionInit() {
}
