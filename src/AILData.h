#ifndef AILDATA_H
#define AILDATA_H

class AILData {
public:
    AILData();
    ~AILData();
    int Load(char* filename);

private:
    char* data;
    int size;
};

#endif // AILDATA_H
