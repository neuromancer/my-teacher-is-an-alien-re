#ifndef SCI_DIALOG_H
#define SCI_DIALOG_H

#include "SearchScreen.h"

class Sprite;
class Queue;

class SCI_Dialog : public SearchScreen
{
public:
    // Reusing SearchScreen fields or overlaying
    // SearchScreen has void* field_608[11];
    
    // Accessors or reinterpret_cast for ease?
    // We can define methods to access them, or unions if we modify SearchScreen.
    // For now, I'll rely on the fact they are pointers.
    
    // 0x608
    Sprite* GetButton() { return (Sprite*)this->field_608[0]; }
    void SetButton(Sprite* s) { this->field_608[0] = (void*)s; }
    
    // 0x60c
    Sprite* GetHilite() { return (Sprite*)this->field_608[1]; }
    void SetHilite(Sprite* s) { this->field_608[1] = (void*)s; }
    
    // 0x610
    Queue* GetQuestionQueue() { return (Queue*)this->field_608[2]; }
    void SetQuestionQueue(Queue* q) { this->field_608[2] = (void*)q; }
    
    // 0x618 is field_608[4]? (0x608 + 4*4 = 0x618)
    // AmbientsController array?
    
    /* Function start: 0x407EC6 */
    virtual int LBLParse(char* line);
};

#endif
