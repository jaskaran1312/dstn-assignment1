#ifndef INIT
#define INIT
#include "main.h"
void tlbInit(struct Hardware *hardware);
void l1CacheInit(struct Hardware *hardware);
void victimCacheInit(struct Hardware *hardware);
void l2CacheInit(struct Hardware *hardware);
void frameTableInit(struct Hardware *hardware);
void mainMemoryInit(struct Hardware *hardware);
struct Process *processInit(int64_t pid);
void statsInit(stats *s);
#endif