#ifndef SC_ONSCREENMESSAGE_H
#define SC_ONSCREENMESSAGE_H

#include "Timer.h"
#include "ScriptHandler.h"

// MessageNode for linked list items
// Layout: prev(0), next(4), data(8)
struct MessageNode {
    MessageNode* prev;  // 0x0
    MessageNode* next;  // 0x4
    void* data;         // 0x8

    ~MessageNode() {
        data = 0;
        prev = 0;
        next = 0;
    }

    MessageNode* Destroy(int flag);      // 0x40C580
    MessageNode* Init(void* nodeData);   // 0x40C5B0
    MessageNode* Init_40ACA0(void* nodeData); // 0x40ACA0
};

// MessageList control structure for SC_OnScreenMessage
// Layout: head(0), tail(4), current(8), flags(0xc)
struct MessageList {
    void* head;     // 0x0
    void* tail;     // 0x4
    void* current;  // 0x8
    int flags;      // 0xc

    MessageList() {
        flags = 0;
        head = 0;
        tail = 0;
        current = head;
    }

    void* GetCurrentData();                  // 0x40C0D0
    void InsertBeforeCurrent(void* data);    // 0x40C430
    void InsertNode(void* data);             // 0x40ABD0
    void UnlinkNode(MessageNode* node);      // 0x40ACC0
    void* PopCurrent();                      // 0x40C500
};

class SC_OnScreenMessage : ScriptHandler {
public:
    SC_OnScreenMessage();
    ~SC_OnScreenMessage();
    void Destroy(int free);
    void Update(int, int);
    void AddMessage();
    void Copy(SC_OnScreenMessage* other);

    // targetAddress inherited from ScriptHandler at 0x88
    // sourceAddress inherited from ScriptHandler at 0x8c
    // command inherited from ScriptHandler at 0x90
    // data inherited from ScriptHandler at 0x94
    char pad2[0xa0 - 0x98];
    MessageList* m_messageList; // 0xa0 - pointer to message list control structure
    Timer timer;  // 0xa4
};

#endif // SC_ONSCREENMESSAGE_H
