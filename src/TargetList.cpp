#include "EngineSubsystems.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
