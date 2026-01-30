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

    // Virtual method overrides
    virtual int LBLParse(char* param_1);
    virtual int ShutDown(Message* msg);
    virtual int Exit(Message* msg);
    virtual void OnInput(Message* msg);

    void Update(int, int);
    void AddMessage(int);
    int ProcessMessage(Message* msg);
    ~ZBuffer();
    void CleanUpVBuffer();
};

#endif // ZBUFFER_H
