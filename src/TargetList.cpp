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
      t->Target::~Target();
      operator delete(t);
      self->targets[self->count] = 0;
    }
    self->targets[self->count] = 0;
  }

  HashTable* ht = self->hashTable;
  if (ht) {
    if (ht->buckets) {
      HashNode** buckets = ht->buckets;
      for (int n = ht->numBuckets; n > 0; n--) {
        HashNode* node = *buckets;
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
      operator delete(ht->buckets);
    }
    ht->buckets = 0;
    ht->count = 0;
    ht->freeList = 0;
    HashNode* pool = (HashNode*)ht->nodePool;
    if (pool) {
      do {
        HashNode* next = pool->next;
        operator delete(pool);
        pool = next;
      } while (pool);
    }
    ht->nodePool = 0;
    operator delete(ht);
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
    Parser::ReportUnknownLabel("TargetList");
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
  HashNode** bucketPtr;
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
            entry = *bucketPtr;
            if (entry != 0) break;
            bucketPtr = bucketPtr + 1;
            bucketIdx = bucketIdx + 1;
          } while (bucketIdx < (unsigned int)ht->numBuckets);
        }
      }
      // Original bug at 0x4436C4: the bucket scan result is dereferenced without rechecking the sentinel/null case.
      nextEntry = entry->next;
      if (nextEntry == 0) {
        bucketIdx = entry->bucketIndex + 1;
        if (bucketIdx < (unsigned int)ht->numBuckets) {
          bucketPtr = &ht->buckets[bucketIdx];
          do {
            nextEntry = *bucketPtr;
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
          g_ScoreDisplay_0046ae6c->hitCount++;
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

/* Function start: 0x443780 */
HotspotListData::~HotspotListData()
{
    HotspotNode* node = first;
    while (node != 0) {
        int i = 0;
        do {
        } while (i-- != 0);
        node = node->next;
    }

    count = 0;
    freeList = 0;
    last = 0;
    first = 0;

    void* pool = nodePool;
    while (pool != 0) {
        void* next = *(void**)pool;
        FreeMemory(pool);
        pool = next;
    }
    nodePool = 0;
}

/* Function start: 0x443830 */
HotspotNode* HotspotListData::AllocateNode()
{
    if (freeList == 0) {
        char* mem = (char*)AllocateMemory(growthRate * 16 + 4);
        *(void**)mem = nodePool;
        int n = growthRate;
        nodePool = mem;
        HotspotNode* node = (HotspotNode*)(mem + n * 16 - 12);
        n--;
        if (n >= 0) {
            do {
                node->next = freeList;
                freeList = node;
                node = (HotspotNode*)((char*)node - 16);
            } while (--n >= 0);
        }
    }

    HotspotNode* node = freeList;
    freeList = node->next;
    count++;
    node->id = 0;

    int i = 0;
    do {} while (i-- != 0);

    node->reserved = 0;

    i = 0;
    do {} while (i-- != 0);

    return node;
}
