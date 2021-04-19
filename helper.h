#ifndef HELPER
#define HELPER

#include "main.h"
void saveResult(stats *statistics);
int fetchData(int64_t pa, struct Hardware *hardware, int selector, int makeRead, stats *statistics);
int readWriteSelector(int selector);
#endif