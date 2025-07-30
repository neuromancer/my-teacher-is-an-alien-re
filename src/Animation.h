#ifndef ANIMATION_H
#define ANIMATION_H

#include "Sprite.h"

class Animation : public Sprite {
public:
    void Play(char* filename, unsigned int flags);
    void Open(char* filename, int, unsigned int);
};

#endif // ANIMATION_H
