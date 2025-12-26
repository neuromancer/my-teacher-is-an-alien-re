#ifndef BIGENEMY_H
#define BIGENEMY_H

#include "Parser.h"

class BigEnemy : public Parser {
public:
    char field_0x4[0xe4];
    void* field_0xe8;
    char field_0xec[0x98];
    void* field_0x184;
    char field_0x188[0x4];


    BigEnemy();
    ~BigEnemy();

    void DestructorHelper();
};

#endif // BIGENEMY_H
