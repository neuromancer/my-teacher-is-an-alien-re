#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "Timer.h"

class BaseObject {
public:
    BaseObject* BaseObject_Init();
    virtual ~BaseObject();
    void Destroy();
    void Copy(BaseObject* source);
    void Copy_2(BaseObject* source);
    void PureVirtualCall();
    virtual int VFunc5(BaseObject* other);
    void BaseObject_CreateFromQueue();
private:
    char pad_4[4];
    int field_4;
    int field_8;
    int field_c;
    char field_10[32];
    int field_30;
    char pad_38[4];
    int field_38;
    int field_3c;
    char field_40[64];
    int field_80;
    Timer timer;
    int field_88;
    int field_8c;
    int field_90;
    int field_94;
    int field_98;
    int object_type;
    int queue1;
    int queue2;
    int buffer;
    int field_ac;
    int field_b0;
    int field_b4;
    int field_b8;
    int field_bc;
    int field_c0;
    int field_c4;
    int field_c8;
    int field_cc;
    int field_d0;
    int field_d4;
    int field_d8;
    int field_dc;
    int field_e0;
    int field_e4;
    int field_e8;
    int field_ec;
    int field_f0;
    int field_f4;
    int field_f8;
    int field_fc;
};

#endif // BASEOBJECT_H
