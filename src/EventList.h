#ifndef EVENTLIST_H
#define EVENTLIST_H

// Doubly linked list node structure for eventList
// field_0x00: next pointer
// field_0x04: prev pointer
// field_0x08: data pointer (to object with vtable)
struct EventNode {
    EventNode* next;    // 0x00
    EventNode* prev;    // 0x04
    void* data;         // 0x08 - pointer to object with vtable

    // Inline: no standalone function in binary
    EventNode() {}

    // Inline: body inlined into Cleanup at 0x4188A0
    ~EventNode() { data = 0; next = 0; prev = 0; }

    // Inline: inlined in InsertNode; compiler also emits out-of-line copy at 0x4189D0
    EventNode(void* d) { data = d; next = 0; prev = 0; }

    // Out-of-line: scalar deleting destructor at 0x4188A0
    void* Cleanup(int flag);
};

// List header structure for eventList
// field_0x00: head pointer
// field_0x04: tail pointer
// field_0x08: current pointer (iteration)
// field_0x0C: count or flags
struct EventList {
    EventNode* head;    // 0x00
    EventNode* tail;    // 0x04
    EventNode* current; // 0x08
    int field_0x0C;     // 0x0C

    EventList() {
        field_0x0C = 0;
        head = 0;
        tail = 0;
        current = head;
    }

    void InsertNode(void* data);  // 0x4188d0
};

#endif // EVENTLIST_H
