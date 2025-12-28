#include "Credits.h"
#include <stdio.h>
#include <string.h>

/* Function start: 0x409510 */
int Credits::LBLParse(char* param_1)
{
    char key[32];
    sscanf(param_1, "%s", key);
    
    if (_strcmpi(key, "END") == 0) {
        return 1;
    }
    
    return Parser::LBLParse("Credits");
}
