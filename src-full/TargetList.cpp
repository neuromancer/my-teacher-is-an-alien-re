#include "EngineSubsystems.h"
#include "Memory.h"
#include "globals.h"
#include "Sample.h"
#include "CursorState.h"
#include "ScoreDisplay.h"
#include "SC_CombatBase.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Function start: 0x4432F0 */
TargetList::TargetList() : Parser() {
  memset(&count, 0, 80 * sizeof(void*));
}

/* Function start: 0x443360 */
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

/* Function start: 0x443540 */
int TargetList::LBLParse(char* line) {
  char type[32];
  int id = 0;

  type[0] = 0;
  sscanf(line, " %s ", type);

  if (type[0] == 'F') {
    sscanf(&type[1], "%d", &id);

    targets[count] = new Target();
    Target* pTarget = targets[count];
    pTarget->id = count;
    count++;

    pTarget->Init(line + 2);
    ProcessFile(pTarget, this, 0);
  } else {
    if (stricmp(type, "END") == 0) {
      return 1;
    }
    Parser::LBLParse("TargetList");
  }

  return 0;
}

/* Function start: 0x4434A0 */
void TargetList::OnProcessEnd() {
  HashTable* ht;

  if (TargetList::count != 0) {
    ht = new HashTable(TargetList::count);
    TargetList::hashTable = ht;
  }
}

/* Function start: 0x443660 */
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
  ht = g_TargetList_0046ae58->hashTable;
  if (ht == 0) {
    return 0;
  }
  entry = (HashNode*)((unsigned int)ht->count < 1 ? 0 : -1);
  if (entry != 0) {
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
          ((int*)g_ScoreDisplay_0046ae6c)[5]++;
          g_ScoreDisplay_0046ae6c->AdjustScore(-(int)target->scoreWeight.end);
          if (target->sound3 != 0) {
            target->sound3->Play(100, 1);
          }
        }
        if (fallbackTarget == 0 && target->CheckTimeInRange() != 0) {
          fallbackTarget = target;
        }
      }
    } while (nextEntry != 0);
  }
  return fallbackTarget;
}
