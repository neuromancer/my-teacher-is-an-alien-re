#ifndef T_HOTSPOT_H
#define T_HOTSPOT_H

class Sprite {
public:
    char Do(int x, int y, int z, float scale);
    void SetState(int state);
private:
    char pad[0x90];
public:
    int field_0x90;
    char pad2[0xa4-0x94];
    int field_0xa4;
    int field_0xa8;
};

class T_Hotspot {
public:
    void Do();
    int Set_State(int state);
    int Get_State();
    int Update(int, int, int);
private:
    char pad[0x88];
    Sprite* sprite;
    int field_0x8c;
    int field_0x90;
    int field_0x94;
    char pad2[0x11c - 0x98];
    int field_0x11c;
    int field_0x120;
    int field_0x124;
    int field_0x128;
};

#endif // T_HOTSPOT_H
