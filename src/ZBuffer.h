#ifndef ZBUFFER_H
#define ZBUFFER_H

#include "Timer.h"
#include "Parser.h"

class Message;
struct ZBQueue;

class ZBuffer : public Parser {
public:
    int m_address;
    int m_from;
    char pad2[0xa0 - 0x8c - 4];
    Timer timer; // 0xa0

    void Update(int, int);
    void AddMessage(int);
    int ProcessMessage(Message* msg);
    ~ZBuffer();
    void CleanUpVBuffer();
    static void ClearList(ZBQueue* queue);
    static void* PopNode(ZBQueue* queue);
    static void* PopNode_2(ZBQueue* queue);
};

#endif // ZBUFFER_H
