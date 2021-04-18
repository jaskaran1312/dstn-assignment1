#ifndef FRAMETABLE
#define FRAMETABLE
#include "hardware.h"

int64_t checkFrameTable (int64_t pa, struct Process *process, struct Hardware *hardware);
int64_t allocateNewFrame(struct Process *process, struct Hardware *hardware);
#endif