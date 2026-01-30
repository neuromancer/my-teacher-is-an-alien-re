#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// Unified doubly-linked list node
// Layout: prev(+0x00), next(+0x04), data(+0x08)
// Size: 12 bytes (0x0C)
struct ListNode {
    ListNode* prev;  // 0x00
    ListNode* next;  // 0x04
    void* data;      // 0x08

    ListNode() : prev(0), next(0), data(0) {}
    ListNode(void* d) { data = d; prev = 0; next = 0; }
    ~ListNode() { data = 0; prev = 0; next = 0; }

    ListNode* Init(void* nodeData);   // 0x40C5B0
};

// Unified doubly-linked list header
// Layout: head(+0x00), tail(+0x04), current(+0x08), type(+0x0C)
// Size: 16 bytes (0x10)
struct LinkedList {
    ListNode* head;     // 0x00
    ListNode* tail;     // 0x04
    ListNode* current;  // 0x08
    int type;           // 0x0C

    LinkedList() {
        type = 0;
        head = 0;
        tail = 0;
        current = head;
    }

    LinkedList(int t) {
        type = t;
        head = 0;
        tail = 0;
        current = head;
    }

    ~LinkedList() {
        current = 0;
        head = 0;
        tail = 0;
    }
};

struct Queue : public LinkedList {
    Queue() : LinkedList() {}
    Queue(int t) : LinkedList(t) {}
    // Queue methods (0x4024D0, 0x408790, 0x4025A0, 0x402680, 0x417680)
    void Insert(void* data);
    void InsertAtCurrent(void* data);
    void Push(void* data);
    void* Pop();
    void* GetCurrentData();
};

struct EventList : public LinkedList {
    EventList() : LinkedList() {}
    EventList(int t) : LinkedList(t) {}
    // EventList methods (0x4188D0)
    void InsertNode(void* data);
};

struct MessageList : public LinkedList {
    MessageList() : LinkedList() {}
    MessageList(int t) : LinkedList(t) {}
    // MessageList methods (0x40C430, 0x40ABD0, 0x40ACC0, 0x40AD50, 0x40C500)
    void InsertNode(void* data);
    void InsertBeforeCurrent(void* data);
    void UnlinkNode(ListNode* node);
    void* PopCurrent();
    void* GetCurrentData();
};

struct ZBQueue : public LinkedList {
    ZBQueue() : LinkedList() {}
    ZBQueue(int t) : LinkedList(t) {}
    // ZBQueue methods (0x41BB10, 0x401810, 0x401710, 0x401790, 0x401560, 0x409160, 0x41CB40, 0x41CC10)
    void* GetCurrentData();
    void* PopNode();
    void* PopNode2();
    void* PopNode2_2();
    void ClearList();
    void Insert(void* data);
    void InsertBeforeCurrent(void* data);
    void InsertBefore(void* data);
};

struct PriorityQueue : public LinkedList {
    PriorityQueue() : LinkedList() {}
    PriorityQueue(int t) : LinkedList(t) {}
    // PriorityQueue methods (0x4043A0)
    void AddAfterCurrent(void* data);
};

// Backward-compatible type aliases
typedef ListNode QueueNode;
typedef ListNode EventNode;
typedef ListNode MessageNode;
typedef ListNode ZBQueueNode;
typedef ListNode PriorityQueueNode;
typedef ListNode SpriteListNode;

typedef LinkedList SpriteNode; // This one is tricky, check usage


#endif // LINKEDLIST_H
