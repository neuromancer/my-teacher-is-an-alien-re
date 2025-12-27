#ifndef MANAGER_H
#define MANAGER_H

class Manager {
public:
    void* Init(const char* filename, int mode);
    void Cleanup();
    void LoadQuestions();
};

#endif // MANAGER_H
