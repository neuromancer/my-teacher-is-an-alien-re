#include "Parser.h"
#include "string.h"

class QuestionInit : public Parser {
public:
    QuestionInit(const char* filename);
    ~QuestionInit();
};

/* Function start: 0x422880 */
QuestionInit::QuestionInit(const char* filename) {
    ParseFile(this, (char*)filename, (char*)0);
}

QuestionInit::~QuestionInit() {
}
