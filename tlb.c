#include<stdio.h>
#include<stdlib.h>
#include "hardware.h"
#include "tlb.h"

unsigned int fetchTLB(struct Hardware *hardware, unsigned int pageNumber, unsigned int pid) {
    for (int i=0;i<32;i++) {
        if(hardware->tlb->valid[i] && hardware->tlb->pid[i] == pid && hardware->tlb->pageNumber[i] == pageNumber) {
            // tlb hit
            return hardware->tlb->frameNumber[i];
        }
    }
    return -1; // tlb miss
}

void invalidateTLB(struct Hardware *hardware, unsigned int pid) {
    for(int i=0;i<32;i++) {
        if(hardware->tlb->pid[i] == pid)
            hardware->tlb->valid[i] = 0;
    }
}