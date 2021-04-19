#include <stdio.h>
#include <stdlib.h>

#include "hardware.h"
#include "lru.h"

int64_t getMMLRU(struct Hardware *hardware) {
    int64_t frameNumber;
    int64_t minLRU = __INT_MAX__;
    for (int i = 0; i < 65536; i++) {
        if (hardware->mainMemory->lru[i] <= minLRU) {
            minLRU = hardware->mainMemory->lru[i];
            frameNumber = i;
        }
    }
    printf("Frame Number to be evicted from MM is %ld\n", frameNumber);
    return frameNumber;
}

void shiftMMLRU(struct Hardware *hardware) {

    printf("Start Shifted MMLRU\n");
    fflush(stdout);
    for (int i = 0; i < 65536; i++) {
        hardware->mainMemory->lru[i] = hardware->mainMemory->lru[i] >> 1;
    }
    printf("Shifted MMLRU\n");
    fflush(stdout);
    return;
}

void updateMMLRU(struct Hardware *hardware, int64_t frameNumber) {
    hardware->mainMemory->lru[frameNumber] = hardware->mainMemory->lru[frameNumber] | 0x8000;
    return;
}