#ifndef TARGET_H
#define TARGET_H

#include "Parser.h"

class Target : public Parser
{
public:
    /* Function start: 0x408AA0 */
    virtual int LBLParse(char* line);
};

#endif
