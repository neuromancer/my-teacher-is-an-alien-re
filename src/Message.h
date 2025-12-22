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
    int field_0x88;
    int field_0x8c;
    int field_0x90;
    int field_0x94;
    int field_0x98;
    int field_0x9c;
    int field_0xa0;
    int field_0xa4;
    int field_0xa8;
    int field_0xac;
    int field_0xb0;
    int field_0xb4;
    int field_0xb8;
    int field_0xbc;

    Message(int, int, int, int, int, int, int, int, int, int);
};

void SC_Message_Send(int param_1, int param_2, int param_3, int param_4,
    int param_5, int param_6, int param_7, int param_8,
    int param_9, int param_10);

#endif // MESSAGE_H
