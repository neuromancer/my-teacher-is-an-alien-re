#ifndef T_HOTSPOT_H
#define T_HOTSPOT_H

class Sprite {
public:
    char Do(int x, int y, int z, float scale);
private:
    char pad[0xa4];
public:
    int field_0xa4;
    int field_0xa8;
};

class T_Hotspot {
public:
    void Do();
private:
    char pad[0x88];
    Sprite* sprite;
};

#endif // T_HOTSPOT_H
