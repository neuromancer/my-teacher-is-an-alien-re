#ifndef QUESTIONINIT_H
#define QUESTIONINIT_H

#include "Parser.h"

class QuestionInit : public Parser {
public:
    QuestionInit(const char* filename);  // 0x422880
    ~QuestionInit();
    int LBLParse(char* line);            // 0x4228F0
};

#endif // QUESTIONINIT_H
