#ifndef SC_ONSCREENMESSAGE_H
#define SC_ONSCREENMESSAGE_H

#include "Handler.h"
#include "Timer.h"
#include "Memory.h"

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

    MessageNode* Destroy(int flag) {
        data = 0;
        prev = 0;
        next = 0;
        if (flag & 1) {
            FreeMemory(this);
        }
        return this;
    }
    MessageNode* Init(void* nodeData);   // 0x40C5B0
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

// SC_OnScreenMessage - Handler class with ID 15
// Size: 0xB8 bytes
// vtable: 0x4311e8
// Constructor: 0x40A2E0
// Destructor: 0x40A410
class SC_OnScreenMessage : public Handler {
public:
    SC_OnScreenMessage();
    virtual ~SC_OnScreenMessage();

    // Virtual method overrides from Handler
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual int Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    void Copy(SC_OnScreenMessage* other);

    // Fields starting at 0xA0 (0x00-0x87 is Parser, 0x88-0x9F is Handler base fields)
    MessageList* m_messageList; // 0xA0
    Timer timer;                // 0xA4 - 0xB7 (size 0x14)
};

#endif // SC_ONSCREENMESSAGE_H
