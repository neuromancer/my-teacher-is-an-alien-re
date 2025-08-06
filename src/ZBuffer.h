#ifndef ZBUFFER_H
#define ZBUFFER_H

#include "Timer.h"
#include "Parser.h"

class ZBuffer : public Parser {
public:
    int m_address;
    int m_from;
    char pad2[0xa0 - 0x8c - 4];
    Timer timer; // 0xa0

    void Update(int, int);
    void AddMessage(int);
    int ProcessMessage(void*);
    void CleanUpVBuffer();
};

#endif // ZBUFFER_H
