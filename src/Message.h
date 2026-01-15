#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
    int field_0x0;
    int field_0x4;
    int field_0x8;
    int field_0xc;
    char data1[0x20];
    int field_0x30;
    int field_0x34;
    int field_0x38;
    int field_0x3c;
    char data2[0x40];
    int field_0x80;
    int field_0x84;
    int targetAddress;  // 0x88 - destination handler address
    int sourceAddress;  // 0x8c - source/from address
    int command;        // 0x90 - message type/command code
    int data;           // 0x94 - associated data
    int priority;       // 0x98 - queue priority
    int param1;         // 0x9c
    int param2;         // 0xa0
    int clickX;         // 0xa4 - mouse click X (param9)
    int clickY;         // 0xa8 - mouse click Y (param10)
    int mouseX;         // 0xac - current mouse X
    int mouseY;         // 0xb0 - current mouse Y
    int field_0xb4;
    int field_0xb8;
    int userPtr;        // 0xbc - user pointer (param8)
    // Note: Uses SC_Message::SC_Message at 0x4198C0 for initialization
};

void SC_Message_Send(int targetAddress, int sourceAddress, int command, int data,
    int priority, int param1, int param2, int userPtr,
    int param9, int param10);

#endif // MESSAGE_H
