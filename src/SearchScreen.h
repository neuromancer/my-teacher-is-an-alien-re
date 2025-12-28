#ifndef SEARCHSCREEN_H
#define SEARCHSCREEN_H

#include "Parser.h"

class SearchScreen : public Parser
{
public:
    SearchScreen();
    
    /* Function start: 0x404A20 */
    virtual int LBLParse(char* line);

    // Padding to reach 0x600
    unsigned char padding[0x600 - sizeof(Parser)];

    void* field_600;      // 0x600
    void* field_604;      // 0x604
    void* field_608[11];  // 0x608 - assuming array size based on 0x634 offset
    int field_634;        // 0x634
};

#endif // SEARCHSCREEN_H
