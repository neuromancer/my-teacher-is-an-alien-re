#ifndef INVOBJECT_H
#define INVOBJECT_H

#include "Parser.h"

class InvObject : public Parser
{
public:
    /* Function start: 0x403E60 */
    virtual int LBLParse(char* line);
};

#endif
