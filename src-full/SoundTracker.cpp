#include <stdlib.h>

struct SoundPool {
    int* head;      // 0x00
    int* tail;      // 0x04
    int count;      // 0x08
    int* freeList;  // 0x0C
    int* blockList; // 0x10
    int blockSize;  // 0x14

    SoundPool(int param) {
        count = 0;
        freeList = 0;
        tail = 0;
        head = 0;
        blockList = 0;
        blockSize = param;
    }
    ~SoundPool();
};

extern int DAT_00469128;
extern SoundPool* DAT_00469134;

class SoundTracker {
public:
    char field_0;

    SoundTracker(int param);
    ~SoundTracker();
    void Init();     // 0x412480
    void Cleanup();  // 0x412730
};

/* Function start: 0x412000 */
SoundTracker::SoundTracker(int param) {
    field_0 = 0;
    DAT_00469128 = param;
    DAT_00469134 = new SoundPool(param);
    Init();
}

/* Function start: 0x4120A0 */
SoundTracker::~SoundTracker() {
    SoundPool* p;
    int* node;
    int* next;
    int* data;
    int i;

    Cleanup();
    if (DAT_00469134 != 0) {
        p = DAT_00469134;
        node = p->head;
        while (node != 0) {
            data = (int*)(node + 2);
            i = 0;
            do {
                if (*data != 0) {
                    operator delete((void*)*data);
                    *data = 0;
                }
                data = data + 1;
                int tmp = i;
                i = i - 1;
                if (tmp == 0) break;
            } while (1);
            node = (int*)node[0];
        }
        p->count = 0;
        p->freeList = 0;
        p->tail = 0;
        p->head = 0;
        next = p->blockList;
        while (next != 0) {
            int* tmp = (int*)next[0];
            operator delete(next);
            next = tmp;
        }
        p->blockList = 0;
        operator delete(p);
        DAT_00469134 = 0;
    }
}
