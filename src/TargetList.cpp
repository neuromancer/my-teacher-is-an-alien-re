#include "EngineSubsystems.h"
#include "Memory.h"
#include "globals.h"
#include "Sample.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern void FUN_00416ba0(int* scoreManager, int value);

/* Function start: 0x414D80 */
TargetList::TargetList() : Parser() {
  memset(&count, 0, 80 * sizeof(void*));
}

/* Function start: 0x414DF0 */
TargetList::~TargetList() {
  TargetList* self = this;
  while (self->count > 0) {
    self->count--;
    Target* t = self->targets[self->count];
    if (t) {
      delete t;
      self->targets[self->count] = (Target*)0;
    }
    self->targets[self->count] = (Target*)0;
  }

  HashTable* ht = self->hashTable;
  if (ht) {
    if (ht->buckets) {
      int* buckets = ht->buckets;
      int n = ht->numBuckets;
      if (n > 0) {
        do {
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
          n--;
        } while (n);
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

/* Function start: 0x414FD0 */
int TargetList::LBLParse(char* line) {
  int id = 0;
  char type[32];

  type[0] = 0;
  sscanf(line, "%s", type);

  if (type[0] == 'F') {
    sscanf(&type[1], "%d", &id);

    Target* pTarget = new Target();

    int currentCount = count;
    targets[currentCount] = pTarget;
    targets[currentCount]->id = currentCount;
    count++;

    targets[currentCount]->Init(line + 2);
    ProcessFile(targets[currentCount], this, NULL);
    return 0;
  } else {
    if (stricmp(type, "END") == 0) {
      return 1;
    }
    Parser::LBLParse((char*)"TargetList");
  }

  return 0;
}

/* Function start: 0x414F30 */
void TargetList::OnProcessEnd() {
  HashTable* ht;

  if (TargetList::count != 0) {
    ht = new HashTable(TargetList::count);
    TargetList::hashTable = ht;
  }
}

/* Function start: 0x4150F0 */
Target* TargetList::ProcessTargets() {
  HashTable* ht;
  int* entry;
  int* nextEntry;
  Target* target;
  Target* fallbackTarget;
  unsigned int bucketIdx;
  int* bucketPtr;

  fallbackTarget = 0;
  TargetList::currentTarget = 0;
  ht = ((TargetList*)DAT_00435f0c)->hashTable;
  if (ht == 0) {
    return 0;
  }
  entry = (int*)(((unsigned int)ht->count < 1) - 1);
  if (entry == 0) {
    return fallbackTarget;
  }
  do {
    if (entry == (int*)-1) {
      bucketIdx = 0;
      if (ht->numBuckets != 0) {
        bucketPtr = ht->buckets;
        do {
          entry = (int*)*bucketPtr;
          if (entry != 0) break;
          bucketPtr = bucketPtr + 1;
          bucketIdx = bucketIdx + 1;
        } while (bucketIdx < (unsigned int)ht->numBuckets);
      }
    }
    nextEntry = (int*)*entry;
    if (nextEntry == 0) {
      bucketIdx = *(entry + 1) + 1;
      if (bucketIdx < (unsigned int)ht->numBuckets) {
        bucketPtr = (int*)(bucketIdx * 4 + (int)ht->buckets);
        do {
          nextEntry = (int*)*bucketPtr;
          if (nextEntry != 0) break;
          bucketPtr = bucketPtr + 1;
          bucketIdx = bucketIdx + 1;
        } while (bucketIdx < (unsigned int)ht->numBuckets);
      }
    }
    target = (Target*)*(entry + 3);
    entry = nextEntry;
    if (target != 0 && target->Update() == 0) {
      if (target->AdvanceHotspot() != 0) {
        TargetList::currentTarget = target;
        g_ScoreManager[5] = g_ScoreManager[5] + 1;
        FUN_00416ba0(g_ScoreManager, -target->field_0x110);
        if (TargetList::field_0x1c0 != 0) {
          ((Sample*)TargetList::field_0x1c0)->Play(100, 1);
        }
      }
      if (fallbackTarget == 0 && target->CheckTimeInRange() != 0) {
        fallbackTarget = target;
      }
    }
  } while (nextEntry != 0);
  return fallbackTarget;
}
