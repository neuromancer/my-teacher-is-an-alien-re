#ifndef LINKEDLIST_H
#define LINKEDLIST_H

extern "C" void ShowError(const char* format, ...);
extern void FreeMemory(void* ptr);

// Unified doubly-linked list node
// Layout: prev(+0x00), next(+0x04), data(+0x08)
// Size: 12 bytes (0x0C)
struct ListNode {
    ListNode* prev;  // 0x00
    ListNode* next;  // 0x04
    void* data;      // 0x08

    ListNode() : prev(0), next(0), data(0) {}
    ListNode(void* d) { data = d; prev = 0; next = 0; }
    ~ListNode() {}

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

    inline void* GetCurrentData() {
        if (current != 0) return current->data;
        return 0;
    }

    inline void InsertNode(void* data) {
        if (data == 0) ShowError("queue fault 0102");
        ListNode* node = new ListNode(data);
        if (current == 0) current = head;
        if (head == 0) {
            head = node;
            tail = node;
            current = node;
        } else {
            node->next = current;
            node->prev = current->prev;
            if (current->prev == 0) {
                head = node;
            } else {
                current->prev->next = node;
            }
            current->prev = node;
        }
    }

    inline void Insert(void* data) { InsertNode(data); }

    inline void PushNode(void* data) {
        if (data == 0) ShowError("queue fault 0112");
        ListNode* node = new ListNode(data);
        if (current == 0) current = tail;
        if (head == 0) {
            head = node;
            tail = node;
            current = node;
        } else {
            if (tail == 0 || tail->next != 0) {
                ShowError("queue fault 0113");
            }
            node->next = 0;
            node->prev = tail;
            tail->next = node;
            tail = node;
        }
    }

    inline void* RemoveCurrent() {
        ListNode* node;
        void* data;

        node = current;
        if (node == 0) {
            data = 0;
        } else {
            // Unlink from head
            if (head == node) {
                head = node->next;
            }
            // Unlink from tail
            if (tail == current) {
                tail = current->prev;
            }
            // Update next node's prev pointer
            node = current;
            if (node->prev != 0) {
                node->prev->next = node->next;
            }
            // Update prev node's next pointer
            node = current;
            if (node->next != 0) {
                node->next->prev = node->prev;
            }
            // Get data
            node = current;
            data = 0;
            if (node != 0) {
                data = node->data;
            }
            // Free node
            if (node != 0) {
                node->data = 0;
                node->prev = 0;
                node->next = 0;
                FreeMemory(node);
                current = 0;
            }
            current = head;
        }
        return data;
    }


    inline void Push(void* data) { PushNode(data); }
    inline void* Pop() { return RemoveCurrent(); }

    inline void ResetForSortedAdd(void* data) {
        if (data == 0) ShowError("queue fault 0101");
        current = head;
    }
};

struct Queue : public LinkedList {
    Queue() : LinkedList() {}
    Queue(int t) : LinkedList(t) {}

    void Insert(void* data);         // 0x4024D0
    void Push(void* data);           // 0x4025A0
    void* Pop();                     // 0x402680
    void* GetCurrentData();          // 0x417680
    void InsertAtCurrent(void* data);
};

struct EventList : public LinkedList {
    EventList() : LinkedList() {}
    EventList(int t) : LinkedList(t) {}

    // Non-inline to force CALL (generates COMDAT per compilation unit)
    void InsertNode(void* data);
};

struct MessageList : public LinkedList {
    MessageList() : LinkedList() {}
    MessageList(int t) : LinkedList(t) {}
    // MessageList methods (0x40ABD0, 0x40C430, 0x40C500)
    void InsertNode(void* data);
    void InsertBeforeCurrent(void* data);
    void* PopCurrent();
};

struct ZBQueue : public LinkedList {
    ZBQueue() : LinkedList() {}
    ZBQueue(int t) : LinkedList(t) {}

    // Non-inline to force CALL (generates COMDAT per compilation unit)
    void* GetCurrentData();
    void InsertBeforeCurrent(void* data);
    void* Pop();  // 0x401710, 0x401790, 0x401810

    // 0x401560
    void ClearList();
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
