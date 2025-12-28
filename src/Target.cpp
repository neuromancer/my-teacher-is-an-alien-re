#include "Target.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    // void FUN_...
}

/* Function start: 0x408AA0 */
int Target::LBLParse(char* param_1)
{
    char key[32];
    sscanf(param_1, "%s", key);
    
    if (_strcmpi(key, "END") == 0) {
        return 1;
    }
    
    return Parser::LBLParse("Target");
}
