#ifndef AILDATA_H
#define AILDATA_H

class AILData {
public:
    AILData *ClearFields();
    ~AILData();
    int Load(char* filename);

private:
    char* data;
    int size;
    int field_8;
    int field_C;
};

#endif // AILDATA_H
