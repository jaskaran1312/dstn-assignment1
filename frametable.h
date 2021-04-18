#ifndef FRAMETABLE
#define FRAMETABLE
#include "hardware.h"

int checkFrameTable (long pa, struct Process *process, struct Hardware *hardware);
int allocateNewFrame(struct Process *process, struct Hardware *hardware);
#endif