#include "EngineSubsystems.h"
#include "Memory.h"
#include "globals.h"
#include "Sample.h"
#include "CursorState.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ScoreManager - g_ScoreManager_00435f20 points to this
class ScoreManager {
public:
  int field_0;
  int score;    // field_4
  void AdjustScore(int value); // 0x416ba0
};

/* Function start: 0x41E030 */ /* ~90% match */
TargetList::TargetList() : Parser() {
  memset(&count, 0, 80 * sizeof(void*));
}

/* Function start: 0x443360 */ /* ~85% match */
TargetList::~TargetList() {
  TargetList* self = this;
  while (self->count > 0) {
    self->count--;
    Target* t = self->targets[self->count];
    if (t) {
      delete t;
      self->targets[self->count] = 0;
    }
    self->targets[self->count] = 0;
  }

  HashTable* ht = self->hashTable;
  if (ht) {
    if (ht->buckets) {
      int* buckets = ht->buckets;
      for (int n = ht->numBuckets; n > 0; n--) {
        HashNode* node = (HashNode*)*buckets;
        if (node) {
          do {
            int i = 0;
            while (i--) {}
            i = 0;
            while (i--) {}
            node = node->next;
          } while (node);
        }
        buckets++;
      }
      free(ht->buckets);
    }
    ht->buckets = 0;
    ht->count = 0;
    ht->freeList = 0;
    HashNode* pool = (HashNode*)ht->nodePool;
    if (pool) {
      do {
        HashNode* next = pool->next;
        free(pool);
        pool = next;
      } while (pool);
    }
    ht->nodePool = 0;
    free(ht);
    self->hashTable = 0;
  }
}

/* Function start: 0x414FD0 */ /* DEMO ONLY - no full game match */
int TargetList::LBLParse(char* line) {
  int id = 0;
  char type[32];

  type[0] = 0;
  sscanf(line, "%s", type);

  if (type[0] == 'F') {
    sscanf(&type[1], "%d", &id);

    Target* pTarget = new Target();

    targets[count] = pTarget;
    targets[count]->id = count;
    count++;

    pTarget->Init(line + 2);
    ProcessFile(pTarget, this, NULL);
    return 0;
  } else {
    if (stricmp(type, "END") == 0) {
      return 1;
    }
    Parser::LBLParse("TargetList");
  }

  return 0;
}

/* Function start: 0x414F30 */ /* DEMO ONLY - no full game match */
void TargetList::OnProcessEnd() {
  HashTable* ht;

  if (TargetList::count != 0) {
    ht = new HashTable(TargetList::count);
    TargetList::hashTable = ht;
  }
}

/* Function start: 0x4150F0 */ /* DEMO ONLY - no full game match */
Target* TargetList::ProcessTargets() {
  Target* fallbackTarget;
  Target* target;
  HashNode* entry;
  HashNode* nextEntry;
  unsigned int bucketIdx;
  int* bucketPtr;
  HashTable* ht;

  fallbackTarget = 0;
  currentTarget = 0;
  ht = g_TargetList_00435f0c->hashTable;
  if (ht == 0) {
    return 0;
  }
  if ((unsigned int)ht->count < 1) {
    return fallbackTarget;
  }
  entry = (HashNode*)-1;
  do {
    if (entry == (HashNode*)-1) {
      bucketIdx = 0;
      if ((unsigned int)ht->numBuckets != 0) {
        bucketPtr = ht->buckets;
        do {
          entry = (HashNode*)*bucketPtr;
          if (entry != 0) break;
          bucketPtr = bucketPtr + 1;
          bucketIdx = bucketIdx + 1;
        } while (bucketIdx < (unsigned int)ht->numBuckets);
      }
    }
    nextEntry = entry->next;
    if (nextEntry == 0) {
      bucketIdx = entry->bucketIndex + 1;
      if (bucketIdx < (unsigned int)ht->numBuckets) {
        bucketPtr = (int*)(bucketIdx * 4 + (int)ht->buckets);
        do {
          nextEntry = (HashNode*)*bucketPtr;
          if (nextEntry != 0) break;
          bucketPtr = bucketPtr + 1;
          bucketIdx = bucketIdx + 1;
        } while (bucketIdx < (unsigned int)ht->numBuckets);
      }
    }
    target = (Target*)entry->reserved;
    entry = nextEntry;
    if (target != 0 && target->Update() == 0) {
      if (target->AdvanceHotspot() != 0) {
        currentTarget = target;
        g_ScoreManager_00435f20->hitCount = g_ScoreManager_00435f20->hitCount + 1;
        ((ScoreManager*)g_ScoreManager_00435f20)->AdjustScore(-target->scoreWeight.end);
        if (defaultSound != 0) {
          defaultSound->Play(100, 1);
        }
      }
      if (fallbackTarget == 0 && target->CheckTimeInRange() != 0) {
        fallbackTarget = target;
      }
    }
  } while (nextEntry != 0);
  return fallbackTarget;
}
