#ifndef ZBUFFER_H
#define ZBUFFER_H

#include "Timer.h"
#include "Parser.h"

class Message;
#include "LinkedList.h"

class ZBuffer : public Parser {
public:
    int m_address;
    int m_from;
    char pad2[0xa0 - 0x8c - 4];
    Timer timer; // 0xa0

    ~ZBuffer();
    void CleanUpVBuffer();
};

#endif // ZBUFFER_H
