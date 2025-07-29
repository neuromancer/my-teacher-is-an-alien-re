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
private:
    char pad[0x88];
    Sprite* sprite;
};

#endif // T_HOTSPOT_H
